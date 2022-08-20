#include "doctest.h"

#include "concepts/printF.hxx"


TEST_CASE("printF, C Types")
{
    SUBCASE("one integer")
    {
        CHECK(printF("%d", 1) == "1");
    }

    SUBCASE("multiple arguments")
    {
        CHECK(printF("integer %d, double %f, string %s, pointer %p", 1, 1.2, "abc", 0x1234) == "integer 1, double 1.200000, string abc, pointer 0x1234");
    }

    SUBCASE("nontrivial specifiers")
    {
        SUBCASE("hex")
        {
            CHECK(printF("%04x", 10) == "000a");
        }

        SUBCASE("double")
        {
            CHECK(printF("%.1f", 1.2) == "1.2");
        }
    }
}


TEST_CASE("printF, C++ Types")
{
    using std::operator""s;
    using std::operator""sv;

    SUBCASE("std::string")
    {
        CHECK(printF("%s", "abc"s) == "abc");
    }

    SUBCASE("std::string_view")
    {
        CHECK(printF("%s", "abc"sv) == "abc");
    }

   SUBCASE("bool")
   {
       CHECK(printF("%s", true) == "true");
   }
}


class ClassWithToString
{
public:
    ClassWithToString (const int v) : value(v) {}

    std::string toString () const
    {
        return "[" + std::to_string(value) + "]";
    }

private:
    const int value;
};
class ClassWithTo_String
{
public:
    ClassWithTo_String (const int v) : value(v) {}

    // This method is not supposed to be called directly by printF
    std::string serialize () const
    {
        return "<" + std::to_string(value) + ">";
    }

private:
    const int value;
};
std::string to_string (const ClassWithTo_String& value)
{
    return value.serialize();
}


TEST_CASE("printF, custom types")
{
    SUBCASE("custom class with toString()")
    {
        CHECK(printF("%s", ClassWithToString(4)) == "[4]");
    }

    SUBCASE("custom class with external to_string()")
    {
        CHECK(printF("%s", ClassWithTo_String(5)) == "<5>");
    }
}
