#include <gtest/gtest.h>

#include "obm/order_book.hpp"

TEST(OrderBook, StartsEmpty) {
  obm::OrderBook book;
  EXPECT_FALSE(book.best_bid().has_value());
  EXPECT_FALSE(book.best_ask().has_value());
}
