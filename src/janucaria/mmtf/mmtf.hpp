#ifndef JANUCARIA_MMTF_MMTF_HPP
#define JANUCARIA_MMTF_MMTF_HPP

#include <cstdint>
#include <cstddef>
#include <cassert>
#include <exception>
#include <iterator>

#include <boost/endian/conversion.hpp>
#include <fmt/format.h>
#include <gsl/gsl>

#include "codec_header.hpp"
#include "type_traits.hpp"

namespace janucaria::mmtf
{

template <typename TInIter,
          std::enable_if_t<is_iter_value_1byte_integral_v<TInIter>> * = nullptr>
auto make_codec_header(TInIter iter, TInIter iter_end) noexcept
    -> CodecHeader<std::add_const_t<typename std::iterator_traits<TInIter>::value_type>>
{
  using DataByte = std::add_const_t<typename std::iterator_traits<TInIter>::value_type>;

  auto header = CodecHeader<DataByte>{};

  auto strategy = std::int32_t{};
  std::copy_n(iter, 4, reinterpret_cast<std::uint8_t *>(&strategy));
  header.strategy = boost::endian::big_to_native(strategy);

  auto length = std::int32_t{};
  std::copy_n(std::next(iter, 4), 4, reinterpret_cast<std::uint8_t *>(&length));
  header.length = boost::endian::big_to_native(length);

  const auto data_begin = std::next(iter, 12);
  auto parameter = std::array<std::byte, 4>{};
  std::copy(std::next(iter, 8), data_begin, reinterpret_cast<std::uint8_t *>(parameter.data()));
  header.parameter = parameter;

  if constexpr(std::is_pointer_v<TInIter>) {
    header.encoded_data = gsl::span<DataByte>{data_begin, iter_end};
  } else {
    header.encoded_data = gsl::span<DataByte>{data_begin.operator->(), iter_end.operator->()};
  }

  return header;
}

template <typename TRange>
auto make_codec_header(TRange &&range) noexcept -> decltype(auto)
{
  return make_codec_header(std::cbegin(std::forward<TRange>(range)), std::cend(std::forward<TRange>(range)));
}

template <typename T>
auto decode_header_type_1(CodecHeader<T> header) -> std::vector<float>
{
  if(header.strategy != 1) {
    const auto message = fmt::format("Wrong strategy type, expected 1 got {}.", header.strategy);
    throw std::invalid_argument{message};
  }

  if (header.encoded_data.size() % 4 != 0)
  {
    const auto message = fmt::format("Data length {} is not a multiple of {}.", header.encoded_data.size(), 4);
    throw std::invalid_argument{message};
  }

  auto output = std::vector<float>{};

  const auto data_size = header.encoded_data.size() / 4;
  const auto floats_view = gsl::span<const std::uint32_t>{reinterpret_cast<const std::uint32_t *>(header.encoded_data.data()), data_size};

  output.reserve(data_size);
  std::transform(std::cbegin(floats_view), std::cend(floats_view), std::back_inserter(output), [](auto val) noexcept {
    const auto native_val = boost::endian::big_to_native(val);
    return *reinterpret_cast<const float *>(&native_val);
    ;
  });

  return output;
}

template <typename T>
auto decode_header_type_2(CodecHeader<T> header) -> std::vector<std::int8_t>
{
  if (header.strategy != 2)
  {
    const auto message = fmt::format("Wrong strategy type, expected 2 got {}.", header.strategy);
    throw std::invalid_argument{message};
  }

  auto output = std::vector<std::int8_t>{};

  const auto data_size = header.encoded_data.size();
  const auto data_view = header.encoded_data;

  output.reserve(data_size);
  std::transform(std::cbegin(data_view), std::cend(data_view), std::back_inserter(output), [](auto val) noexcept {
    return std::int8_t{boost::endian::big_to_native(val)};
  });

  return output;
}

template <typename T>
auto decode_header_type_3(CodecHeader<T> header) -> std::vector<std::int16_t>
{
  if (header.strategy != 3)
  {
    const auto message = fmt::format("Wrong strategy type, expected 3 got {}.", header.strategy);
    throw std::invalid_argument{message};
  }

  if (header.encoded_data.size() % 2 != 0)
  {
    const auto message = fmt::format("Data length {} is not a multiple of {}.", header.encoded_data.size(), 2);
    throw std::invalid_argument{message};
  }

  auto output = std::vector<std::int16_t>{};

  const auto data_size = header.encoded_data.size() / 2;
  const auto data_view = gsl::span<const std::int16_t>{reinterpret_cast<const std::int16_t *>(header.encoded_data.data()), data_size};

  output.reserve(data_size);
  std::transform(std::cbegin(data_view), std::cend(data_view), std::back_inserter(output), [](auto val) noexcept {
    return boost::endian::big_to_native(val);
  });

  return output;
}

template <typename T>
auto decode_header_type_4(CodecHeader<T> header) -> std::vector<std::int32_t>
{
  if (header.strategy != 4)
  {
    const auto message = fmt::format("Wrong strategy type, expected 4 got {}.", header.strategy);
    throw std::invalid_argument{message};
  }

  if (header.encoded_data.size() % 4 != 0)
  {
    const auto message = fmt::format("Data length {} is not a multiple of {}.", header.encoded_data.size(), 4);
    throw std::invalid_argument{message};
  }

  auto output = std::vector<std::int32_t>{};

  const auto data_size = header.encoded_data.size() / 4;
  const auto data_view = gsl::span<const std::int32_t>{reinterpret_cast<const std::int32_t *>(header.encoded_data.data()), data_size};

  output.reserve(data_size);
  std::transform(std::cbegin(data_view), std::cend(data_view), std::back_inserter(output), [](auto val) noexcept {
    return boost::endian::big_to_native(val);
  });

  return output;
}

} // namespace janucaria::mmtf

#endif
