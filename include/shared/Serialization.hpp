#pragma once
#include <algorithm>
#include <gsl/gsl>

namespace scs::shared::ser {

using span = gsl::span<unsigned char>;
using const_span = gsl::span<const unsigned char>;

inline long SerializeElement(span data, const std::string &str) {
  auto needed_size = str.size() + 4;
  Expects(needed_size <= data.size());

  if (str.empty()) {
    return 0;
  }

  if (str.size() > std::numeric_limits<unsigned int>::max()) {
    return -1;
  }

  auto length = static_cast<unsigned int>(str.size());
  // FIXME: Make endian independent.
  *data.data() = length;
  std::copy(str.begin(), str.end(), data.subspan(sizeof(length)).begin());
  return needed_size;
}

inline long DeserializeElement(const_span data, std::string &str) {
  if (data.size() < 4) {
    return -1;
  }

  auto length = *(reinterpret_cast<const unsigned int *>(&data[0]));
  auto needed_size = length + sizeof(unsigned int);
  if (data.size() < needed_size) {
    return -1;
  }
  auto str_span = data.subspan(sizeof(unsigned int), length);
  str = std::string(str_span.begin(), str_span.end());

  return needed_size;
}

template <typename T>
struct optional {
  T t;
  optional(T t_) : t(t_) {}
};

template <typename Subject>
inline long SerializeElement(span data, const optional<Subject> &opt) {
  auto len = SerializeElement(data, opt.t);
  return std::max(0, len);
}

template <typename Subject>
inline long DeserializeElement(const_span data, optional<Subject> &opt) {
  auto len = DeserializeElement(data, opt.t);
  return std::max(0L, len);
}


template <typename Subject>
long SerializeElements(span data, const Subject &sub) {
  return SerializeElement(data, sub);
}

template <typename Subject, typename... Args>
long SerializeElements(span data, const Subject &sub, const Args &... args) {
  auto used = SerializeElement(data, sub);
  if (used >= 0) {
    auto rused = SerializeElements(data.subspan(used), args...);
    if (used >= 0) {
      return used + rused;
    }
  }
  return -1;
}

template <typename Subject>
long DeserializeElements(const_span data, Subject &sub) {
  return DeserializeElement(data, sub);
}

template <typename Subject, typename... Args>
long DeserializeElements(const_span data, Subject &sub, Args &... args) {
  auto used = DeserializeElement(data, sub);
  if (used >= 0) {
    auto rused = DeserializeElements(data.subspan(used), args...);
    if (used >= 0) {
      return used + rused;
    }
  }
  return -1;
}

}  // namespace scs::shared::ser