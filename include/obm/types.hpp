#pragma once

#include <cstdint>

namespace obm {

// Prices are integer ticks (1002 means $10.02), never floating point.
using Price = std::int64_t;
using Quantity = std::uint64_t;
using OrderId = std::uint64_t;

enum class Side : std::uint8_t { Buy, Sell };

struct Trade {
  OrderId buyer;
  OrderId seller;
  Quantity quantity;
  Price price;
};

}  // namespace obm
