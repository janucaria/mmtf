#ifndef JANUCARIA_MMTF_MMTF_HPP
#define JANUCARIA_MMTF_MMTF_HPP

#include <cstdint>
#include <cstddef>
#include <cassert>
#include <iterator>

#include <boost/endian/conversion.hpp>
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

} // namespace janucaria::mmtf

#endif
