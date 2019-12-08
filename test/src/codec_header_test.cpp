#include <string>

#include <catch2/catch.hpp>

#include <janucaria/mmtf/mmtf.hpp>

using namespace janucaria;

SCENARIO("Extract Codec from mutable data")
{
  GIVEN("Encoded mutable vector data")
  {
    auto encoded = std::vector<char>{
        '\x00', '\x00', '\x00', '\x08',
        '\x00', '\x00', '\x00', '\x07',
        '\x00', '\x00', '\x00', '\x00',
        '\x00', '\x00', '\x00', '\x01',
        '\x00', '\x00', '\x00', '\x07'};

    WHEN("Extract codec header from iterator")
    {
      const auto header = mmtf::decode_header(encoded.begin(), encoded.end());

      THEN("Get codec header")
      {
        REQUIRE(header.strategy == 8);
        REQUIRE(header.length == 7);
        REQUIRE((header.parameter == std::array{
                                         static_cast<std::byte>(0),
                                         static_cast<std::byte>(0),
                                         static_cast<std::byte>(0),
                                         static_cast<std::byte>(0)}));
        REQUIRE(header.encoded_data.size() == 8);
        REQUIRE(reinterpret_cast<const void *>(header.encoded_data.data()) == reinterpret_cast<const void *>(encoded.data() + 12));
      }
    }
  }
}

SCENARIO("Extract Codec from immutable data")
{
  GIVEN("Encoded immutable array data")
  {
    const auto encoded = std::array{
        '\x00', '\x00', '\x00', '\x08',
        '\x00', '\x00', '\x00', '\x07',
        '\x00', '\x00', '\x00', '\x00',
        '\x00', '\x00', '\x00', '\x01',
        '\x00', '\x00', '\x00', '\x07'};

    WHEN("Extract codec header from container")
    {
      const auto header = mmtf::decode_header(encoded);

      THEN("Get codec header")
      {
        REQUIRE(header.strategy == 8);
        REQUIRE(header.length == 7);
        REQUIRE((header.parameter == std::array{
                                         static_cast<std::byte>(0),
                                         static_cast<std::byte>(0),
                                         static_cast<std::byte>(0),
                                         static_cast<std::byte>(0)}));
        REQUIRE(header.encoded_data.size() == 8);
        REQUIRE(reinterpret_cast<const void *>(header.encoded_data.data()) == reinterpret_cast<const void *>(encoded.data() + 12));
      }
    }
  }
}
