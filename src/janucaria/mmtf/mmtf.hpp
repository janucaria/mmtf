#ifndef JANUCARIA_MMTF_MMTF_HPP
#define JANUCARIA_MMTF_MMTF_HPP

#include <cstdint>
#include <cstddef>
#include <cassert>
#include <exception>
#include <iterator>
#include <limits>

#include <boost/endian/conversion.hpp>
#include <fmt/format.h>
#include <gsl/gsl>

#include "codec_header.hpp"
#include "type_traits.hpp"

namespace janucaria::mmtf
{

template <typename TInIter, typename TOutIter>
constexpr auto run_length_decode(TInIter first, TInIter last, TOutIter output) -> TOutIter
{
  while (first != last)
  {
    const auto value = *first++;
    const auto count = *first++;
    std::fill_n(output, count, value);
  }

  return output;
}

template <typename TRange, typename TOutIter>
constexpr auto run_length_decode(TRange &&range, TOutIter output) -> decltype(auto)
{
  return run_length_decode(std::cbegin(std::forward<TRange>(range)), std::cend(std::forward<TRange>(range)), output);
}

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

template <typename T>
auto decode_header_type_5(CodecHeader<T> header) -> std::vector<std::string>
{
  if (header.strategy != 5)
  {
    const auto message = fmt::format("Wrong strategy type, expected 5 got {}.", header.strategy);
    throw std::invalid_argument{message};
  }

  const auto length_array = header.length;
  const auto chars_size = boost::endian::big_to_native(*reinterpret_cast<const std::int32_t *>(header.parameter.data()));
  const auto data_size = length_array * chars_size;

  if (header.encoded_data.size() != data_size)
  {
    const auto message = fmt::format("Data length is corrupt, {} expected, got {}.", data_size, header.encoded_data.size());
    throw std::invalid_argument{message};
  }

  auto output = std::vector<std::string>{};

  const auto data_view = header.encoded_data;

  output.reserve(data_size);
  for (auto it = std::cbegin(data_view); it != std::cend(data_view); std::advance(it, chars_size))
  {
    const auto str = std::string{it.operator->(), static_cast<std::size_t>(chars_size)} + '\0';
    output.push_back(str.c_str());
  }

  return output;
}

template <typename T>
auto decode_header_type_6(CodecHeader<T> header) -> std::vector<char>
{
  if (header.strategy != 6)
  {
    const auto message = fmt::format("Wrong strategy type, expected 6 got {}.", header.strategy);
    throw std::invalid_argument{message};
  }

  if (header.encoded_data.size() % 4 != 0)
  {
    const auto message = fmt::format("Data length {} is not a multiple of {}.", header.encoded_data.size(), 4);
    throw std::invalid_argument{message};
  }

  auto output = std::vector<char>{};

  const auto data_size = header.encoded_data.size() / 4;
  if (data_size % 2 != 0)
  {
    const auto message = "Data run length size is not even.";
    throw std::invalid_argument{message};
  }

  const auto data_view = gsl::span<const std::int32_t>{reinterpret_cast<const std::int32_t *>(header.encoded_data.data()), data_size};
  auto endian_data = std::vector<std::int32_t>{};
  std::transform(std::cbegin(data_view), std::cend(data_view), std::back_inserter(endian_data), [](auto val) noexcept {
    return boost::endian::big_to_native(val);
  });
  run_length_decode(endian_data, std::back_inserter(output));

  return output;
}

template <typename T>
auto decode_header_type_7(CodecHeader<T> header) -> std::vector<std::int32_t>
{
  if (header.strategy != 7)
  {
    const auto message = fmt::format("Wrong strategy type, expected 7 got {}.", header.strategy);
    throw std::invalid_argument{message};
  }

  if (header.encoded_data.size() % 4 != 0)
  {
    const auto message = fmt::format("Data length {} is not a multiple of {}.", header.encoded_data.size(), 4);
    throw std::invalid_argument{message};
  }

  auto output = std::vector<std::int32_t>{};

  const auto data_size = header.encoded_data.size() / 4;
  if (data_size % 2 != 0)
  {
    const auto message = "Data run length size is not even.";
    throw std::invalid_argument{message};
  }

  const auto data_view = gsl::span<const std::int32_t>{reinterpret_cast<const std::int32_t *>(header.encoded_data.data()), data_size};
  auto endian_data = std::vector<std::int32_t>{};
  std::transform(std::cbegin(data_view), std::cend(data_view), std::back_inserter(endian_data), [](auto val) noexcept {
    return boost::endian::big_to_native(val);
  });
  run_length_decode(endian_data, std::back_inserter(output));

  return output;
}

template <typename T>
auto decode_header_type_8(CodecHeader<T> header) -> std::vector<std::int32_t>
{
  if (header.strategy != 8)
  {
    const auto message = fmt::format("Wrong strategy type, expected 8 got {}.", header.strategy);
    throw std::invalid_argument{message};
  }

  if (header.encoded_data.size() % 4 != 0)
  {
    const auto message = fmt::format("Data length {} is not a multiple of {}.", header.encoded_data.size(), 4);
    throw std::invalid_argument{message};
  }

  const auto data_size = header.encoded_data.size() / 4;
  if (data_size % 2 != 0)
  {
    const auto message = "Data run length size is not even.";
    throw std::invalid_argument{message};
  }

  auto run_length = std::vector<std::int32_t>{};
  const auto data_view = gsl::span<const std::int32_t>{reinterpret_cast<const std::int32_t *>(header.encoded_data.data()), data_size};
  auto endian_data = std::vector<std::int32_t>{};
  std::transform(std::cbegin(data_view), std::cend(data_view), std::back_inserter(endian_data), [](auto val) noexcept {
    return boost::endian::big_to_native(val);
  });
  run_length_decode(endian_data, std::back_inserter(run_length));

  auto output = std::vector<std::int32_t>{};
  output.reserve(run_length.size());

  auto iter_begin = std::cbegin(run_length);
  auto iter_end = std::cend(run_length);
  auto output_inserter = std::back_inserter(output);

  if (iter_begin != iter_end)
  {
    auto prev_val = *iter_begin++;
    *output_inserter++ = prev_val;
    while (iter_begin != iter_end)
    {
      auto current_val = prev_val + *iter_begin++;
      *output_inserter++ = current_val;
      prev_val = current_val;
    }
  }

  return output;
}

template <typename T>
auto decode_header_type_9(CodecHeader<T> header) -> std::vector<float>
{
  if (header.strategy != 9)
  {
    const auto message = fmt::format("Wrong strategy type, expected 9 got {}.", header.strategy);
    throw std::invalid_argument{message};
  }

  if (header.encoded_data.size() % 4 != 0)
  {
    const auto message = fmt::format("Data length {} is not a multiple of {}.", header.encoded_data.size(), 4);
    throw std::invalid_argument{message};
  }

  const auto data_size = header.encoded_data.size() / 4;
  if (data_size % 2 != 0)
  {
    const auto message = "Data run length size is not even.";
    throw std::invalid_argument{message};
  }

  auto run_length = std::vector<std::int32_t>{};
  const auto data_view = gsl::span<const std::int32_t>{reinterpret_cast<const std::int32_t *>(header.encoded_data.data()), data_size};
  auto endian_data = std::vector<std::int32_t>{};
  std::transform(std::cbegin(data_view), std::cend(data_view), std::back_inserter(endian_data), [](auto val) noexcept {
    return boost::endian::big_to_native(val);
  });
  run_length_decode(endian_data, std::back_inserter(run_length));

  auto output = std::vector<float>{};
  output.reserve(run_length.size());

  const auto divisor = boost::endian::big_to_native(*reinterpret_cast<const std::uint32_t *>(header.parameter.data()));
  std::transform(std::cbegin(run_length), std::cend(run_length), std::back_inserter(output), [divisor](auto val) noexcept {
    return static_cast<const float>(val) / divisor;
  });

  return output;
}

template <typename T>
auto decode_header_type_10(CodecHeader<T> header) -> std::vector<float>
{
  constexpr auto strategy = 10;
  if (header.strategy != strategy)
  {
    const auto message = fmt::format("Wrong strategy type, expected {} got {}.", strategy, header.strategy);
    throw std::invalid_argument{message};
  }

  constexpr auto type_size = sizeof(std::uint16_t);
  if (header.encoded_data.size() % type_size != 0)
  {
    const auto message = fmt::format("Data length {} is not a multiple of {}.", header.encoded_data.size(), type_size);
    throw std::invalid_argument{message};
  }

  auto decoded_data = std::vector<std::int16_t>{};

  const auto data_size = header.encoded_data.size() / static_cast<decltype(header.encoded_data.size())>(type_size);
  const auto data_view = gsl::span<const std::int16_t>{reinterpret_cast<const std::int16_t *>(header.encoded_data.data()), data_size};

  decoded_data.reserve(data_size);
  std::transform(std::cbegin(data_view), std::cend(data_view), std::back_inserter(decoded_data), [](auto val) noexcept {
    return boost::endian::big_to_native(val);
  });

  constexpr auto min_int = std::numeric_limits<std::int16_t>::min();
  constexpr auto max_int = std::numeric_limits<std::int16_t>::max();

  auto decoded_recursive_indexing = std::vector<std::int32_t>{};
  decoded_recursive_indexing.reserve(decoded_data.size());

  auto cur_val = std::int32_t{};
  for (auto val : decoded_data)
  {
    cur_val += val;
    if (val != min_int && val != max_int)
    {
      decoded_recursive_indexing.push_back(cur_val);
      cur_val = 0;
    }
  }

  auto decoded_delta = std::vector<std::int32_t>{};
  decoded_delta.reserve(decoded_recursive_indexing.size());

  auto iter_begin = std::cbegin(decoded_recursive_indexing);
  auto iter_end = std::cend(decoded_recursive_indexing);
  auto iter_output = std::back_inserter(decoded_delta);

  if (iter_begin != iter_end)
  {
    auto prev_val = *iter_begin++;
    *iter_output++ = prev_val;
    while (iter_begin != iter_end)
    {
      auto current_val = prev_val + *iter_begin++;
      *iter_output++ = current_val;
      prev_val = current_val;
    }
  }

  auto output = std::vector<float>{};
  output.reserve(decoded_delta.size());

  const auto divisor = boost::endian::big_to_native(*reinterpret_cast<const std::int32_t *>(header.parameter.data()));
  std::transform(std::cbegin(decoded_delta), std::cend(decoded_delta), std::back_inserter(output), [divisor](auto val) noexcept {
    return static_cast<const float>(val) / divisor;
  });

  return output;
}

template <typename T>
auto decode_header_type_11(CodecHeader<T> header) -> std::vector<float>
{
  constexpr auto strategy = 11;
  if (header.strategy != strategy)
  {
    const auto message = fmt::format("Wrong strategy type, expected {} got {}.", strategy, header.strategy);
    throw std::invalid_argument{message};
  }

  constexpr auto type_size = sizeof(std::uint16_t);
  if (header.encoded_data.size() % type_size != 0)
  {
    const auto message = fmt::format("Data length {} is not a multiple of {}.", header.encoded_data.size(), type_size);
    throw std::invalid_argument{message};
  }

  auto decoded_data = std::vector<std::int16_t>{};

  const auto data_size = header.encoded_data.size() / static_cast<decltype(header.encoded_data.size())>(type_size);
  const auto data_view = gsl::span<const std::int16_t>{reinterpret_cast<const std::int16_t *>(header.encoded_data.data()), data_size};

  decoded_data.reserve(data_size);
  std::transform(std::cbegin(data_view), std::cend(data_view), std::back_inserter(decoded_data), [](auto val) noexcept {
    return boost::endian::big_to_native(val);
  });

  auto output = std::vector<float>{};
  output.reserve(decoded_data.size());

  const auto divisor = boost::endian::big_to_native(*reinterpret_cast<const std::int32_t *>(header.parameter.data()));
  std::transform(std::cbegin(decoded_data), std::cend(decoded_data), std::back_inserter(output), [divisor](auto val) noexcept {
    return static_cast<const float>(val) / divisor;
  });

  return output;
}

template <typename T>
auto decode_header_type_12(CodecHeader<T> header) -> std::vector<float>
{
  constexpr auto strategy = 12;
  if (header.strategy != strategy)
  {
    const auto message = fmt::format("Wrong strategy type, expected {} got {}.", strategy, header.strategy);
    throw std::invalid_argument{message};
  }

  constexpr auto type_size = sizeof(std::uint16_t);
  if (header.encoded_data.size() % type_size != 0)
  {
    const auto message = fmt::format("Data length {} is not a multiple of {}.", header.encoded_data.size(), type_size);
    throw std::invalid_argument{message};
  }

  auto decoded_data = std::vector<std::int16_t>{};

  const auto data_size = header.encoded_data.size() / static_cast<decltype(header.encoded_data.size())>(type_size);
  const auto data_view = gsl::span<const std::int16_t>{reinterpret_cast<const std::int16_t *>(header.encoded_data.data()), data_size};

  decoded_data.reserve(data_size);
  std::transform(std::cbegin(data_view), std::cend(data_view), std::back_inserter(decoded_data), [](auto val) noexcept {
    return boost::endian::big_to_native(val);
  });

  constexpr auto min_int = std::numeric_limits<std::int16_t>::min();
  constexpr auto max_int = std::numeric_limits<std::int16_t>::max();

  auto decoded_recursive_indexing = std::vector<std::int32_t>{};
  decoded_recursive_indexing.reserve(decoded_data.size());

  auto cur_val = std::int32_t{};
  for (auto val : decoded_data)
  {
    cur_val += val;
    if (val != min_int && val != max_int)
    {
      decoded_recursive_indexing.push_back(cur_val);
      cur_val = 0;
    }
  }

  auto output = std::vector<float>{};
  output.reserve(decoded_recursive_indexing.size());

  const auto divisor = boost::endian::big_to_native(*reinterpret_cast<const std::int32_t *>(header.parameter.data()));
  std::transform(std::cbegin(decoded_recursive_indexing), std::cend(decoded_recursive_indexing), std::back_inserter(output), [divisor](auto val) noexcept {
    return static_cast<const float>(val) / divisor;
  });

  return output;
}

template <typename T>
auto decode_header_type_13(CodecHeader<T> header) -> std::vector<float>
{
  constexpr auto strategy = 13;
  if (header.strategy != strategy)
  {
    const auto message = fmt::format("Wrong strategy type, expected {} got {}.", strategy, header.strategy);
    throw std::invalid_argument{message};
  }

  const auto data_size = header.encoded_data.size();
  const auto decoded_data = gsl::span<const std::int8_t>{reinterpret_cast<const std::int8_t *>(header.encoded_data.data()), data_size};

  constexpr auto min_int = std::numeric_limits<std::int8_t>::min();
  constexpr auto max_int = std::numeric_limits<std::int8_t>::max();

  auto decoded_recursive_indexing = std::vector<std::int32_t>{};
  decoded_recursive_indexing.reserve(decoded_data.size());

  auto cur_val = std::int32_t{};
  for (auto val : decoded_data)
  {
    cur_val += val;
    if (val != min_int && val != max_int)
    {
      decoded_recursive_indexing.push_back(cur_val);
      cur_val = 0;
    }
  }

  auto output = std::vector<float>{};
  output.reserve(decoded_recursive_indexing.size());

  const auto divisor = boost::endian::big_to_native(*reinterpret_cast<const std::int32_t *>(header.parameter.data()));
  std::transform(std::cbegin(decoded_recursive_indexing), std::cend(decoded_recursive_indexing), std::back_inserter(output), [divisor](auto val) noexcept {
    return static_cast<const float>(val) / divisor;
  });

  return output;
}

template <typename T>
auto decode_header_type_14(CodecHeader<T> header) -> std::vector<std::int32_t>
{
  constexpr auto strategy = 14;
  if (header.strategy != strategy)
  {
    const auto message = fmt::format("Wrong strategy type, expected {} got {}.", strategy, header.strategy);
    throw std::invalid_argument{message};
  }

  constexpr auto type_size = sizeof(std::int16_t);
  if (header.encoded_data.size() % type_size != 0)
  {
    const auto message = fmt::format("Data length {} is not a multiple of {}.", header.encoded_data.size(), type_size);
    throw std::invalid_argument{message};
  }

  auto decoded_data = std::vector<std::int16_t>{};

  const auto data_size = header.encoded_data.size() / static_cast<decltype(header.encoded_data.size())>(type_size);
  const auto data_view = gsl::span<const std::int16_t>{reinterpret_cast<const std::int16_t *>(header.encoded_data.data()), data_size};

  decoded_data.reserve(data_size);
  std::transform(std::cbegin(data_view), std::cend(data_view), std::back_inserter(decoded_data), [](auto val) noexcept {
    return boost::endian::big_to_native(val);
  });

  constexpr auto min_int = std::numeric_limits<std::int16_t>::min();
  constexpr auto max_int = std::numeric_limits<std::int16_t>::max();

  auto output = std::vector<std::int32_t>{};
  output.reserve(decoded_data.size());

  auto cur_val = std::int32_t{};
  for (auto val : decoded_data)
  {
    cur_val += val;
    if (val != min_int && val != max_int)
    {
      output.push_back(cur_val);
      cur_val = 0;
    }
  }

  return output;
}

template <typename T>
auto decode_header_type_15(CodecHeader<T> header) -> std::vector<std::int32_t>
{
  constexpr auto strategy = 15;
  if (header.strategy != strategy)
  {
    const auto message = fmt::format("Wrong strategy type, expected {} got {}.", strategy, header.strategy);
    throw std::invalid_argument{message};
  }

  const auto data_size = header.encoded_data.size();
  const auto decoded_data = gsl::span<const std::int8_t>{reinterpret_cast<const std::int8_t *>(header.encoded_data.data()), data_size};

  constexpr auto min_int = std::numeric_limits<std::int8_t>::min();
  constexpr auto max_int = std::numeric_limits<std::int8_t>::max();

  auto output = std::vector<std::int32_t>{};
  output.reserve(decoded_data.size());

  auto cur_val = std::int32_t{};
  for (auto val : decoded_data)
  {
    cur_val += val;
    if (val != min_int && val != max_int)
    {
      output.push_back(cur_val);
      cur_val = 0;
    }
  }

  return output;
}

} // namespace janucaria::mmtf

#endif
