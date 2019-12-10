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

SCENARIO("Run-length encoded character array")
{
  GIVEN("Encoded data with strategy type 6")
  {
    const auto encoded = std::vector<char>{
        // strategy
        '\x00', '\x00', '\x00', '\x06',
        // length
        '\x00', '\x00', '\x00', '\x06',
        // parameter
        '\x00', '\x00', '\x00', '\x00',
        // data
        '\x00', '\x00', '\x00', 'A',
        '\x00', '\x00', '\x00', '\x04',
        '\x00', '\x00', '\x00', 'Z',
        '\x00', '\x00', '\x00', '\x02',
        '\x00', '\x00', '\x00', 'F',
        '\x00', '\x00', '\x00', '\x00'};

    WHEN("Decode code header")
    {
      const auto header = mmtf::make_codec_header(encoded);
      const auto decoded = mmtf::decode_header_type_6(header);

      THEN("Get decoded data")
      {
        REQUIRE(decoded.size() == 6);
        REQUIRE(decoded[0] == 'A');
        REQUIRE(decoded[1] == 'A');
        REQUIRE(decoded[2] == 'A');
        REQUIRE(decoded[3] == 'A');
        REQUIRE(decoded[4] == 'Z');
        REQUIRE(decoded[5] == 'Z');
      }
    }
  }
}

SCENARIO("Run-length encoded 32-bit signed integer array")
{
  GIVEN("Encoded data with strategy type 7")
  {
    const auto encoded = std::vector<char>{
        // strategy
        '\x00', '\x00', '\x00', '\x07',
        // length
        '\x00', '\x00', '\x00', '\x05',
        // parameter
        '\x00', '\x00', '\x00', '\x00',
        // data
        '\x00', '\x00', '\x00', '\x01',
        '\x00', '\x00', '\x00', '\x03',
        '\x00', '\x00', '\x00', '\xaa',
        '\x00', '\x00', '\x00', '\x00',
        '\x00', '\x00', '\x00', '\x09',
        '\x00', '\x00', '\x00', '\x02'};

    WHEN("Decode code header")
    {
      const auto header = mmtf::make_codec_header(encoded);
      const auto decoded = mmtf::decode_header_type_7(header);

      THEN("Get decoded data")
      {
        REQUIRE(decoded.size() == 5);
        REQUIRE(decoded[0] == 1);
        REQUIRE(decoded[1] == 1);
        REQUIRE(decoded[2] == 1);
        REQUIRE(decoded[3] == 9);
        REQUIRE(decoded[4] == 9);
      }
    }
  }
}

SCENARIO("Delta & run-length encoded 32-bit signed integer array")
{
  GIVEN("Encoded data with strategy type 8")
  {
    const auto encoded = std::vector<char>{
        // strategy
        '\x00', '\x00', '\x00', '\x08',
        // length
        '\x00', '\x00', '\x00', '\x07',
        // parameter
        '\x00', '\x00', '\x00', '\x00',
        // data
        '\x00', '\x00', '\x00', '\x01',
        '\x00', '\x00', '\x00', '\x07'};

    WHEN("Decode code header")
    {
      const auto header = mmtf::make_codec_header(encoded);
      const auto decoded = mmtf::decode_header_type_8(header);

      THEN("Get decoded data")
      {
        REQUIRE(decoded.size() == 7);
        REQUIRE(decoded[0] == 1);
        REQUIRE(decoded[1] == 2);
        REQUIRE(decoded[2] == 3);
        REQUIRE(decoded[3] == 4);
        REQUIRE(decoded[4] == 5);
        REQUIRE(decoded[5] == 6);
        REQUIRE(decoded[6] == 7);
      }
    }
  }
}

SCENARIO("Integer & run-length encoded 32-bit floating-point number array")
{
  GIVEN("Encoded data with strategy type 9")
  {
    const auto encoded = std::vector<char>{
        // strategy
        '\x00', '\x00', '\x00', '\x09',
        // length
        '\x00', '\x00', '\x00', '\x05',
        // parameter
        '\x00', '\x00', '\x00', '\x0c',
        // data
        '\x00', '\x00', '\x00', '\x06',
        '\x00', '\x00', '\x00', '\x02',
        '\x00', '\x00', '\x00', '\x0c',
        '\x00', '\x00', '\x00', '\x03'};

    WHEN("Decode code header")
    {
      const auto header = mmtf::make_codec_header(encoded);
      const auto decoded = mmtf::decode_header_type_9(header);

      THEN("Get decoded data")
      {
        CHECK(decoded.size() == 5);
        CHECK(decoded[0] == .5f);
        CHECK(decoded[1] == .5f);
        CHECK(decoded[2] == 1.f);
        CHECK(decoded[3] == 1.f);
        CHECK(decoded[4] == 1.f);
      }
    }
  }
}

SCENARIO("Integer & delta encoded & two-byte-packed 32-bit floating-point number array")
{
  GIVEN("Encoded data with strategy type 10")
  {
    const auto encoded = std::vector<char>{
        // strategy
        '\x00', '\x00', '\x00', '\x0a',
        // length
        '\x00', '\x00', '\x00', '\x03',
        // parameter
        '\x00', '\x00', '\x03', '\xe8',
        // data
        '\x7f', '\xff', '\x44', '\xab',
        '\x01', '\x8f', '\xff', '\xca'};

    WHEN("Decode code header")
    {
      const auto header = mmtf::make_codec_header(encoded);
      const auto decoded = mmtf::decode_header_type_10(header);

      THEN("Get decoded data")
      {
        CHECK(decoded.size() == 3);
        CHECK(decoded[0] == Approx(50.346f));
        CHECK(decoded[1] == Approx(50.745f));
        CHECK(decoded[2] == Approx(50.691f));
      }
    }
  }
}

SCENARIO("Integer encoded 32-bit floating-point number array")
{
  GIVEN("Encoded data with strategy type 11")
  {
    const auto encoded = std::vector<char>{
        // strategy
        '\x00', '\x00', '\x00', '\x0b',
        // length
        '\x00', '\x00', '\x00', '\x04',
        // parameter
        '\x00', '\x00', '\x00', '\x04',
        // data
        '\x00', '\x01', '\x00', '\x02',
        '\x00', '\x04', '\x00', '\x10'};

    WHEN("Decode code header")
    {
      const auto header = mmtf::make_codec_header(encoded);
      const auto decoded = mmtf::decode_header_type_11(header);

      THEN("Get decoded data")
      {
        REQUIRE(decoded.size() == 4);
        CHECK(decoded[0] == Approx(0.25f));
        CHECK(decoded[1] == Approx(0.50f));
        CHECK(decoded[2] == Approx(1.00f));
        CHECK(decoded[3] == Approx(4.00f));
      }
    }
  }
}

SCENARIO("Integer & two-byte-packed 32-bit floating-point number array")
{
  GIVEN("Encoded data with strategy type 12")
  {
    const auto encoded = std::vector<char>{
        // strategy
        '\x00', '\x00', '\x00', '\x0c',
        // length
        '\x00', '\x00', '\x00', '\x03',
        // parameter
        '\x00', '\x00', '\x03', '\xe8',
        // data
        '\x7f', '\xff', '\x44', '\xab',
        '\x01', '\x8f', '\xff', '\xca'};

    WHEN("Decode code header")
    {
      const auto header = mmtf::make_codec_header(encoded);
      const auto decoded = mmtf::decode_header_type_12(header);

      THEN("Get decoded data")
      {
        REQUIRE(decoded.size() == 3);
        CHECK(decoded[0] == Approx(50.346f));
        CHECK(decoded[1] == Approx(0.399f));
        CHECK(decoded[2] == Approx(-0.054f));
      }
    }
  }
}

SCENARIO("Integer & one-byte-packed 32-bit floating-point number array")
{
  GIVEN("Encoded data with strategy type 13")
  {
    const auto encoded = std::vector<char>{
        // strategy
        '\x00', '\x00', '\x00', '\x0d',
        // length
        '\x00', '\x00', '\x00', '\x06',
        // parameter
        '\x00', '\x00', '\x03', '\xe8',
        // data
        '\x7f', '\xff', '\x44', '\xab',
        '\x01', '\x80', '\xff', '\xca'};

    WHEN("Decode code header")
    {
      const auto header = mmtf::make_codec_header(encoded);
      const auto decoded = mmtf::decode_header_type_13(header);

      THEN("Get decoded data")
      {
        REQUIRE(decoded.size() == 6);
        CHECK(decoded[0] == Approx(0.126f));
        CHECK(decoded[1] == Approx(0.068f));
        CHECK(decoded[2] == Approx(-0.085f));
        CHECK(decoded[3] == Approx(0.001f));
        CHECK(decoded[4] == Approx(-0.129f));
        CHECK(decoded[5] == Approx(-0.054f));
      }
    }
  }
}

SCENARIO("Two-byte-packed 32-bit signed integer array")
{
  GIVEN("Encoded data with strategy type 14")
  {
    const auto encoded = std::vector<char>{
        // strategy
        '\x00', '\x00', '\x00', '\x0e',
        // length
        '\x00', '\x00', '\x00', '\x03',
        // parameter
        '\x00', '\x00', '\x03', '\xe8',
        // data
        '\x7f', '\xff', '\x44', '\xab',
        '\x01', '\x8f', '\xff', '\xca'};

    WHEN("Decode code header")
    {
      const auto header = mmtf::make_codec_header(encoded);
      const auto decoded = mmtf::decode_header_type_14(header);

      THEN("Get decoded data")
      {
        REQUIRE(decoded.size() == 3);
        CHECK(decoded[0] == 50346);
        CHECK(decoded[1] == 399);
        CHECK(decoded[2] == -54);
      }
    }
  }
}
