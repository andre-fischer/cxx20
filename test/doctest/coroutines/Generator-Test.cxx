#include <doctest.h>
#include <iostream>

#include "coroutines/Generator.hxx"


TEST_CASE("Generator, fibonacci_sequence")
{
    SUBCASE("one integer")
    {
        auto generator = fibonacci_sequence(40);

        for (int j=0; generator; j++)
            std::cout << "fib(" << j << ")=" << generator() << '\n';

    }
}
