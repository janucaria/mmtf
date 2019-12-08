#ifndef JANUCARIA_MMTF_TYPE_TRAITS_HPP
#define JANUCARIA_MMTF_TYPE_TRAITS_HPP

#include <iterator>
#include <type_traits>

#include <boost/type_traits.hpp>

namespace janucaria::mmtf
{

template <typename T>
struct is_1byte_integral : std::bool_constant<
                               std::is_integral_v<T> and sizeof(T) == 1>
{
};

template <typename T>
constexpr auto is_1byte_integral_v = is_1byte_integral<T>::value;

template <typename T, typename = void>
struct is_iter_value_1byte_integral : std::false_type
{
};

template <typename T>
struct is_iter_value_1byte_integral<T, std::void_t<typename std::iterator_traits<T>::value_type>>
    : is_1byte_integral<typename std::iterator_traits<T>::value_type>
{
};

template <typename T>
constexpr auto is_iter_value_1byte_integral_v = is_iter_value_1byte_integral<T>::value;

} // namespace janucaria::mmtf

#endif
