#pragma once

#include <optional>

#include "obm/types.hpp"

namespace obm {

// A single-symbol limit order book with price-time priority.
// The matching logic is step 2 of the plan; for now the book only
// reports its top of book, which is empty.
class OrderBook {
 public:
  std::optional<Price> best_bid() const;
  std::optional<Price> best_ask() const;
};

}  // namespace obm
