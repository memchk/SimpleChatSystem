#pragma once
#include <algorithm>
#include <gsl/gsl>

namespace scs::shared::ser {

using span = gsl::span<unsigned char>;
using const_span = gsl::span<const unsigned char>;

using result = std::pair<bool, std::size_t>;

inline std::size_t RequiredSize(const std::string &str) {
  return sizeof(unsigned int) + str.size();
}

inline result SerializeElement(span data, const std::string &str) {
  if (str.empty()) {
    return {true, 0};
  }

  if (RequiredSize(str) > data.size()) {
    return {false, RequiredSize(str)};
  }

  if (str.size() > std::numeric_limits<unsigned int>::max()) {
    return {false, RequiredSize(str)};
  }

  auto length = static_cast<unsigned int>(str.size());
  // FIXME: Make endian independent.
  *data.data() = length;
  std::copy(str.begin(), str.end(), data.subspan(sizeof(length)).begin());
  return {true, RequiredSize(str)};
}

inline result DeserializeElement(const_span data, std::string &str) {
  if (data.size() < sizeof(unsigned char)) {
    return {false, sizeof(unsigned char)};
  }

  auto length = *(reinterpret_cast<const unsigned int *>(&data[0]));
  if (data.size() < length + sizeof(unsigned char)) {
    return {false, length + sizeof(unsigned char)};
  }

  auto str_span = data.subspan(sizeof(unsigned int), length);
  str = std::string(str_span.begin(), str_span.end());

  return {true, RequiredSize(str)};
}

template <typename T>
struct optional {
  T t;
  optional(T t_) : t(t_) {}
};

template <typename Subject>
inline result SerializeElement(span data, const optional<Subject> &opt) {
  auto [ok, len] = SerializeElement(data, opt.t);
  (void)ok;
  return {true, len};
}

template <typename Subject>
inline result DeserializeElement(const_span data, optional<Subject> &opt) {
  auto [ok, len] = DeserializeElement(data, opt.t);
  (void)ok;
  return {true, len};
}


template <typename Subject>
result SerializeElements(span data, const Subject &sub) {
  return SerializeElement(data, sub);
}

template <typename Subject, typename... Args>
result SerializeElements(span data, const Subject &sub, const Args &... args) {
  auto [ok, used] = SerializeElement(data, sub);
  if(ok) {
    data = data.subspan(used);
  }
  auto [rok, rused] = SerializeElements(data, args...);
  return {ok && rok, used + rused};
}

template <typename Subject>
result DeserializeElements(const_span data, Subject &sub) {
  return DeserializeElement(data, sub);
}

template <typename Subject, typename... Args>
result DeserializeElements(const_span data, Subject &sub, Args &... args) {
  auto [ok, used] = DeserializeElement(data, sub);
  if(ok) {
    data = data.subspan(used);
  }
  auto [rok, rused] = DeserializeElements(data, args...);
  return {rok, used + rused};
}

}  // namespace scs::shared::ser