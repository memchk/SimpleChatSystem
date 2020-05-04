#pragma once
#include <cstring>
#include <gsl/gsl>
#include <string>
#include <vector>

#include "Serialization.hpp"

namespace scs::shared::packet {

// CRC table calculation
constexpr std::array<uint16_t, 256> CalculateCRC16Table(
    uint16_t lsb_polynomial) {
  std::array<uint16_t, 256> crc_table{};
  for (uint16_t i = 0; i < 256; i++) {
    auto byte = i;
    for (auto bit = 0; bit < 8; bit++) {
      if (byte & 0x01) {
        byte >>= 1;
        byte ^= lsb_polynomial;
      } else {
        byte >>= 1;
      }
    }
    crc_table[i] = byte;
  }
  return crc_table;
}

const std::array<uint16_t, 256> CRC_16_CCITT_TABLE =
    CalculateCRC16Table(0x8408);
constexpr uint16_t CalculateCRC16CCITT(ser::const_span data) {
  uint16_t crc = 0;
  for (auto &i : data) {
    auto idx = (crc ^ i) & 0xFF;
    crc = (crc >> 8) ^ CRC_16_CCITT_TABLE[idx];
  }

  return crc;
}

// template <typename T>
// class Frame {
//   private:
//     T packet_;
//   public:
//     void WriteToStream(std::ostream &os) const
//     {
//       std::vector<unsigned char> packet_data;
//       packet_data.resize()
//     }
// };

struct Packet {
 public:
  virtual ser::result ReadFromSpan(ser::const_span &data) = 0;
  virtual ser::result Serialize(ser::span &data) const = 0;
  virtual unsigned char opcode() const = 0;
};

struct HELOPacket final : public Packet {
  std::string user_name, init_channel;

  HELOPacket(std::string user_name, std::string init_channel = "")
      : user_name(user_name), init_channel(init_channel) {}

  ser::result ReadFromSpan(ser::const_span &data) override {
    ser::optional opt_init_channel = init_channel;
    return ser::DeserializeElements(data, user_name, opt_init_channel);
  }

  ser::result Serialize(ser::span &data) const override {
    return ser::SerializeElements(data, user_name, init_channel);
  }

  unsigned char opcode() const override { return 0x08; }
};

namespace {
template <unsigned char OpCode>
class NoDataPacketImpl final : public Packet {
  ser::result ReadFromSpan(gsl::span<unsigned char> _data) override { return {true, 0}; };
  ser::result Serialize(gsl::span<unsigned char> &data) const override {
    return {true, 0};
  };
  unsigned char opcode() const override { return OpCode; }
};
}  // namespace

using ACKPacket = NoDataPacketImpl<0x00>;
using InvalidFCSPacket = NoDataPacketImpl<0x02>;

}  // namespace scs::shared::packet
