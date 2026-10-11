#include "obm/order_book.hpp"

namespace obm {

std::optional<Price> OrderBook::best_bid() const { return std::nullopt; }

std::optional<Price> OrderBook::best_ask() const { return std::nullopt; }

}  // namespace obm
