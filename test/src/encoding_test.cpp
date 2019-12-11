#include <string>

#include <catch2/catch.hpp>

#include <janucaria/mmtf/mmtf.hpp>

#include <iostream>

using namespace janucaria;

SCENARIO("Run-length encoding")
{
  GIVEN("Data to encode")
  {
    const auto data = std::vector<int>{1, 10, 2, 1, 1, 4};

    WHEN("Encode data with Run-length encoding")
    {
      auto output = std::vector<int>{};
      mmtf::run_length_decode(data, std::back_inserter(output));

      THEN("Data encoded")
      {
        REQUIRE(output == std::vector<int>{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1});
      }
    }
  }
}

SCENARIO("Delta encoding")
{
  GIVEN("Data to encode")
  {
    const auto data = std::vector<int>{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1};

    WHEN("Encode data with Delta encoding")
    {
      auto output = std::vector<int>{};
      mmtf::delta_decode(data, std::back_inserter(output));

      THEN("Data encoded")
      {
        REQUIRE(output == std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 14, 15, 16});
      }
    }
  }
}
