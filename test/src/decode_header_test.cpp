#include <string>

#include <catch2/catch.hpp>

#include <janucaria/mmtf/mmtf.hpp>

using namespace janucaria;

SCENARIO("Pass-through: 32-bit floating-point number array")
{
  GIVEN("Encoded data with strategy type 1")
  {
    const auto encoded = std::vector<char>{
        // strategy
        '\x00', '\x00', '\x00', '\x01',
        // length
        '\x00', '\x00', '\x00', '\x04',
        // parameter
        '\x00', '\x00', '\x00', '\x00',
        // data
        '\x40', '\x20', '\x00', '\x00',
        '\x3e', '\x99', '\x99', '\x9a',
        '\xc0', '\xb9', '\x99', '\x9a',
        '\x41', '\x1e', '\x66', '\x66'};

    WHEN("Decode code header")
    {
      const auto header = mmtf::make_codec_header(encoded);
      const auto decoded = mmtf::decode_header_type_1(header);

      THEN("Get decoded data")
      {
        REQUIRE(decoded.size() == 4);
        REQUIRE(decoded[0] == Approx(2.5f));
        REQUIRE(decoded[1] == Approx(0.3f));
        REQUIRE(decoded[2] == Approx(-5.8f));
        REQUIRE(decoded[3] == Approx(9.9f));
      }
    }
  }
}
