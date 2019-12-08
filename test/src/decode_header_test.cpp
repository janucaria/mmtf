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

SCENARIO("Pass-through: 8-bit signed integer array")
{
  GIVEN("Encoded data with strategy type 2")
  {
    const auto encoded = std::vector<char>{
        // strategy
        '\x00', '\x00', '\x00', '\x02',
        // length
        '\x00', '\x00', '\x00', '\x05',
        // parameter
        '\x00', '\x00', '\x00', '\x00',
        // data
        '\x07', '\x06', '\x06', '\x07',
        '\x07'};

    WHEN("Decode code header")
    {
      const auto header = mmtf::make_codec_header(encoded);
      const auto decoded = mmtf::decode_header_type_2(header);

      THEN("Get decoded data")
      {
        REQUIRE(decoded.size() == 5);
        REQUIRE(decoded[0] == 0x07);
        REQUIRE(decoded[1] == 0x06);
        REQUIRE(decoded[2] == 0x06);
        REQUIRE(decoded[3] == 0x07);
        REQUIRE(decoded[4] == 0x07);
      }
    }
  }
}

SCENARIO("Pass-through: 16-bit signed integer array")
{
  GIVEN("Encoded data with strategy type 3")
  {
    const auto encoded = std::vector<char>{
        // strategy
        '\x00', '\x00', '\x00', '\x03',
        // length
        '\x00', '\x00', '\x00', '\x04',
        // parameter
        '\x00', '\x00', '\x00', '\x00',
        // data
        '\x00', '\x06', '\x00', '\x07',
        '\x00', '\x01', '\x00', '\x09'};

    WHEN("Decode code header")
    {
      const auto header = mmtf::make_codec_header(encoded);
      const auto decoded = mmtf::decode_header_type_3(header);

      THEN("Get decoded data")
      {
        REQUIRE(decoded.size() == 4);
        REQUIRE(decoded[0] == 6);
        REQUIRE(decoded[1] == 7);
        REQUIRE(decoded[2] == 1);
        REQUIRE(decoded[3] == 9);
      }
    }
  }
}

SCENARIO("Pass-through: 32-bit signed integer array")
{
  GIVEN("Encoded data with strategy type 4")
  {
    const auto encoded = std::vector<char>{
        // strategy
        '\x00', '\x00', '\x00', '\x04',
        // length
        '\x00', '\x00', '\x00', '\x04',
        // parameter
        '\x00', '\x00', '\x00', '\x00',
        // data
        '\x00', '\x00', '\x00', '\x01',
        '\x00', '\x02', '\x00', '\x01',
        '\x00', '\x00', '\x00', '\x00',
        '\x00', '\x00', '\x00', '\x02'};

    WHEN("Decode code header")
    {
      const auto header = mmtf::make_codec_header(encoded);
      const auto decoded = mmtf::decode_header_type_4(header);

      THEN("Get decoded data")
      {
        REQUIRE(decoded.size() == 4);
        REQUIRE(decoded[0] == 1);
        REQUIRE(decoded[1] == 131073);
        REQUIRE(decoded[2] == 0);
        REQUIRE(decoded[3] == 2);
      }
    }
  }
}

SCENARIO("UTF8/ASCII fixed-length string array")
{
  GIVEN("Encoded data with strategy type 5")
  {
    const auto encoded = std::vector<char>{
        // strategy
        '\x00', '\x00', '\x00', '\x05',
        // length
        '\x00', '\x00', '\x00', '\x06',
        // parameter
        '\x00', '\x00', '\x00', '\x04',
        // data
        'A', '\x00', '\x00', '\x00',
        'B', '\x00', '\x00', '\x00',
        'C', '\x00', '\x00', '\x00',
        'D', '\x00', '\x00', '\x00',
        'E', '\x00', '\x00', '\x00',
        'F', '\x00', '\x00', '\x00'};

    WHEN("Decode code header")
    {
      const auto header = mmtf::make_codec_header(encoded);
      const auto decoded = mmtf::decode_header_type_5(header);

      THEN("Get decoded data")
      {
        REQUIRE(decoded.size() == 6);
        REQUIRE(decoded[0] == "A");
        REQUIRE(decoded[1] == "B");
        REQUIRE(decoded[2] == "C");
        REQUIRE(decoded[3] == "D");
        REQUIRE(decoded[4] == "E");
        REQUIRE(decoded[5] == "F");
      }
    }
  }
}
