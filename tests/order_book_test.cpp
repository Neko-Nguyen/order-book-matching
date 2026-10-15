#include "obm/order_book.hpp"

#include <gtest/gtest.h>

TEST(OrderBook, StartsEmpty) {
  obm::OrderBook book;
  EXPECT_FALSE(book.best_bid().has_value());
  EXPECT_FALSE(book.best_ask().has_value());
}
