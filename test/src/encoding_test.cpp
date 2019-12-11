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
