#include "obm/order_book.hpp"

namespace obm {

// TODO(step 2): replace these stubs with a real implementation until the tests pass.

std::vector<Trade> OrderBook::add_limit(OrderId, Side, Price, Quantity) { return {}; }

std::vector<Trade> OrderBook::add_market(OrderId, Side, Quantity) { return {}; }

bool OrderBook::cancel(OrderId) { return false; }

std::optional<Price> OrderBook::best_bid() const { return std::nullopt; }

std::optional<Price> OrderBook::best_ask() const { return std::nullopt; }

Quantity OrderBook::volume_at(Side, Price) const { return 0; }

std::size_t OrderBook::order_count() const { return 0; }

}  // namespace obm
