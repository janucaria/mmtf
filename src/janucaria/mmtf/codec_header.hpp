#ifndef JANUCARIA_MMTF_CODEC_HEADER_HPP
#define JANUCARIA_MMTF_CODEC_HEADER_HPP

#include <array>
#include <cstdint>

#include <gsl/gsl>

namespace janucaria::mmtf {

template<typename TByte>
class CodecHeader {
public:
  std::int32_t strategy{};

  std::int32_t length{};

  std::array<std::byte, 4> parameter{};

  gsl::span<const TByte> encoded_data{};
};

} // namespace janucaria::mmtf

#endif
