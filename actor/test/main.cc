#define CATCH_CONFIG_MAIN // Have Catch provide a main(); must only be done once
#include "catch.hpp"

// sample function used in sample test
unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}

// test example that also tests that tests work
//         test name                  test group
TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}
