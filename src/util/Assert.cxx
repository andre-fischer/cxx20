#include "util/Assert.hxx"

#include <stdexcept>


void Assert::that (bool expressionValue)
{
    if ( ! expressionValue)
        throw std::runtime_error("assertion not fulfilled");
}


void Assert::fail ()
{
    throw std::runtime_error("failure");
}
