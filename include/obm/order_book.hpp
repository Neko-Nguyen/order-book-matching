#pragma once

#include <cstddef>
#include <optional>
#include <vector>

#include "obm/types.hpp"

namespace obm {

// A single-symbol limit order book with price-time priority.
//
// Rules (see the primer for worked examples):
// - Better price trades first; at the same price, the earlier order trades first.
// - A trade happens at the resting order's price, never the incoming order's.
// - An incoming limit order trades with everything it crosses, then rests the remainder.
// - A market order trades until filled or the opposite side is empty, and never rests.
// - Trades are returned in the order they happen.
class OrderBook {
 public:
  // Throws std::invalid_argument if quantity is 0 or the id is already resting in the book.
  std::vector<Trade> add_limit(OrderId id, Side side, Price price, Quantity quantity);

  // Throws std::invalid_argument if quantity is 0.
  std::vector<Trade> add_market(OrderId id, Side side, Quantity quantity);

  // Removes a resting order. Returns false if no resting order has this id.
  bool cancel(OrderId id);

  std::optional<Price> best_bid() const;
  std::optional<Price> best_ask() const;

  // Total resting quantity at one price on one side; 0 if the level is empty.
  Quantity volume_at(Side side, Price price) const;

  // Number of orders currently resting in the book, both sides.
  std::size_t order_count() const;
};

}  // namespace obm
