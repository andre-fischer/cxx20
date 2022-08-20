#ifndef CXX20_CONCEPTS_PRINTF_HEADER
#define CXX20_CONCEPTS_PRINTF_HEADER

#include <string>
#include <sstream>
#include <tuple>
#include "util/Assert.hxx"


template<typename SpecifierPolicy>
class Formatter
{
public:
    template<typename First, typename ... Rest>
    static void format (std::ostream& s, std::string_view format, First&& first, Rest&& ... rest);

    template<typename First>
    static void format (std::ostream& s, std::string_view format, First&& first);

private:
    template<typename Argument>
    static std::string_view formatOne (std::ostream& s, std::string_view format, Argument&& argument);

};


template<typename T>
concept HasCString = requires(T a)
{
    a.c_str();
};


template<typename T>
concept IsBuffer = requires(T a)
{
    a.data();
    a.size();

    // std::string also has data() and size(). To avoid ambiguity, require T not to be fulfill HasToString<>.
    requires !HasCString<T>;
};

template<typename T>
concept ConvertableStandardType = std::is_same_v<T,bool>;

template<typename T>
concept HasToString = requires(T x)
{
    { x.toString() };
};

template<typename T>
concept HasTo_String = requires(T x)
{
    { to_string(x) };
};



class PrintfSpecifierPolicy
{
public:
    static std::tuple<std::string_view, std::string_view, std::string_view> split (std::string_view format);

    template<typename Argument>
    static void format (std::ostream& s, std::string_view specifier, Argument&& argument);

private:
    template<typename Argument>
    static void formatInteger (std::ostream& s, std::string specifier, Argument&& argument);
    template<typename Argument>
    static void formatFloat (std::ostream& s, std::string specifier, Argument&& argument);
    template<typename Argument>
    static void formatPointer (std::ostream& s, std::string specifier, Argument&& argument);

    template<HasCString T>
    static void formatString (std::ostream& s, std::string specifier, T&& argument);
    template<IsBuffer T>
    static void formatString (std::ostream& s, std::string specifier, T&& argument);
    template<ConvertableStandardType T>
    static void formatString (std::ostream& s, std::string specifier, T&& argument);
    template<typename T> requires HasToString<T> || HasTo_String<T>
    static void formatString (std::ostream& s, std::string specifier, T&& argument);
    template<typename Argument>
    static void formatString (std::ostream& s, std::string specifier, Argument&& argument);

    template<typename ... Arguments>
    static void appendSprintf (std::ostream& s, const char* specifier, Arguments&& ... arguments);
};


template<class ... Arguments>
std::string printF (std::string_view format, Arguments&& ... arguments)
{
    std::stringstream s;
    Formatter<PrintfSpecifierPolicy>::format(s, format, std::forward<Arguments>(arguments)...);
    return s.str();
}


template<class SpecifierPolicy>
template<typename First, typename ... Rest>
void Formatter<SpecifierPolicy>::format (std::ostream& s, std::string_view format, First&& first, Rest&& ... rest)
{
    const std::string_view formatRest = formatOne(s, format, std::forward<First>(first));
    Formatter<PrintfSpecifierPolicy>::format(s, formatRest, std::forward<Rest>(rest)...);
}


template<class SpecifierPolicy>
template<typename First>
void Formatter<SpecifierPolicy>::format (std::ostream& s, std::string_view format, First&& first)
{
    const std::string_view formatRest = formatOne(s, format, std::forward<First>(first));
    s << formatRest;
}


template<class SpecifierPolicy>
template<typename Argument>
std::string_view Formatter<SpecifierPolicy>::formatOne (std::ostream& s, std::string_view format, Argument&& argument)
{
    const auto [head, specifier, rest] = SpecifierPolicy::split(format);
    s << head;
    SpecifierPolicy::format(s, specifier, std::forward<Argument>(argument));
    return rest;
}


std::tuple<std::string_view, std::string_view, std::string_view> PrintfSpecifierPolicy::split (std::string_view format)
{
    const auto percent = format.find('%');
    Assert::that(percent>=0);
    auto end = percent+1;
    while (end != format.size())
    {
        switch(format[end])
        {
            case 's':
            case 'd':
            case 'f':
            case 'p':
            case 'x':
                ++end;
                return {
                    format.substr(0, percent),
                    format.substr(percent, end-percent),
                    format.substr(end)
                };

            default:
                ++end;
                break;
        }
    }
    Assert::fail();
}


template<typename Argument>
void PrintfSpecifierPolicy::format (std::ostream& s, std::string_view specifier, Argument&& argument)
{
    switch(specifier.back())
    {
        case 'd':
        case 'x':
            formatInteger(s, std::string(specifier), std::forward<Argument>(argument));
            break;
        case 'f':
            formatFloat(s, std::string(specifier), std::forward<Argument>(argument));
            break;
        case 'p':
            formatPointer(s, std::string(specifier), std::forward<Argument>(argument));
            break;
        case 's':
            formatString(s, std::string(specifier), std::forward<Argument>(argument));
            break;
        default:
            // Not supported.
            Assert::fail();
    }
}


template<typename Argument>
void PrintfSpecifierPolicy::formatInteger (std::ostream& s, std::string specifier, Argument&& argument)
{
    appendSprintf(s, specifier.c_str(), std::forward<Argument>(argument));
}


template<typename Argument>
void PrintfSpecifierPolicy::formatFloat (std::ostream& s, std::string specifier, Argument&& argument)
{
    appendSprintf(s, specifier.c_str(), std::forward<Argument>(argument));
}


template<typename Argument>
void PrintfSpecifierPolicy::formatPointer (std::ostream& s, std::string specifier, Argument&& argument)
{
    appendSprintf(s, specifier.c_str(), std::forward<Argument>(argument));
}


template<HasCString Argument>
void PrintfSpecifierPolicy::formatString (std::ostream& s, std::string specifier, Argument&& argument)
{
    appendSprintf(s, specifier.c_str(), argument.c_str());
}


template<IsBuffer Argument>
void PrintfSpecifierPolicy::formatString (std::ostream& s, std::string specifier, Argument&& argument)
{
    appendSprintf(s, "%.*s", static_cast<int>(argument.size()), argument.data());
}


template<ConvertableStandardType Argument>
void PrintfSpecifierPolicy::formatString (std::ostream& s, std::string specifier, Argument&& argument)
{
    if constexpr (std::is_same_v<Argument, bool>)
    {
        if (argument)
            appendSprintf(s, specifier.c_str(), "true");
        else
            appendSprintf(s, specifier.c_str(), "false");
    }
}


template<typename Argument> requires HasToString<Argument> || HasTo_String<Argument>
void PrintfSpecifierPolicy::formatString (std::ostream& s, std::string specifier, Argument&& argument)
{
    if constexpr (HasToString<Argument>)
        appendSprintf(s, specifier.c_str(), argument.toString().c_str());
    else if constexpr (HasTo_String<Argument>)
        appendSprintf(s, specifier.c_str(), to_string(argument).c_str());
}


template<typename Argument>
void PrintfSpecifierPolicy::formatString (std::ostream& s, std::string specifier, Argument&& argument)
{
    appendSprintf(s, specifier.c_str(), std::forward<Argument>(argument));
}


template<typename ... Arguments>
void PrintfSpecifierPolicy::appendSprintf (std::ostream& s, const char* specifier, Arguments&& ... arguments)
{
    char buffer[32];
    const int n = snprintf(buffer, sizeof(buffer), specifier, std::forward<Arguments>(arguments)...);
    Assert::that(n>=0);
    s << std::string_view(buffer, n);
}

#endif
