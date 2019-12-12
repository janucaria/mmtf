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

SCENARIO("Recursive indexing encoding")
{
  GIVEN("Data to encode")
  {
    const auto data = std::vector<std::int8_t>{127, 41, 34, 1, 0, -50, -128, 0, 7, 127, 0, 127, 127, 14};

    WHEN("Encode data with Recursive indexing encoding")
    {
      auto output = std::vector<int>{};
      mmtf::recursive_indexing_decode(data, std::back_inserter(output));

      THEN("Data encoded")
      {
        REQUIRE(output == std::vector<int>{168, 34, 1, 0, -50, -128, 7, 127, 268});
      }
    }
  }
}

SCENARIO("Integer encoding")
{
  GIVEN("Data to encode")
  {
    const auto data = std::vector<int>{100, 100, 100, 100, 50, 50};

    WHEN("Encode data with Integer encoding encoding")
    {
      auto output = std::vector<float>{};
      mmtf::integer_decode(data, std::back_inserter(output), 100);

      THEN("Data encoded")
      {
        REQUIRE(output == std::vector<float>{1.00, 1.00, 1.00, 1.00, 0.50, 0.50});
      }
    }
  }
}
