#include "obm/order_book.hpp"

#include <gtest/gtest.h>

#include <ostream>
#include <stdexcept>
#include <vector>

namespace obm {

// Lets GoogleTest print trades readably when a comparison fails.
void PrintTo(const Trade& t, std::ostream* os) {
  *os << "{buyer " << t.buyer << ", seller " << t.seller << ", qty " << t.quantity << " @ "
      << t.price << "}";
}

}  // namespace obm

namespace {

using obm::OrderBook;
using obm::Side;
using obm::Trade;
using Trades = std::vector<Trade>;

// Prices are in cents: 1002 means $10.02.

TEST(OrderBook, StartsEmpty) {
  OrderBook book;
  EXPECT_FALSE(book.best_bid().has_value());
  EXPECT_FALSE(book.best_ask().has_value());
  EXPECT_EQ(book.order_count(), 0u);
}

TEST(OrderBook, LimitOrderRestsWhenNothingCrosses) {
  OrderBook book;
  EXPECT_TRUE(book.add_limit(1, Side::Buy, 1000, 100).empty());
  EXPECT_TRUE(book.add_limit(2, Side::Sell, 1002, 50).empty());

  EXPECT_EQ(book.best_bid(), 1000);
  EXPECT_EQ(book.best_ask(), 1002);
  EXPECT_EQ(book.volume_at(Side::Buy, 1000), 100u);
  EXPECT_EQ(book.volume_at(Side::Sell, 1002), 50u);
  EXPECT_EQ(book.order_count(), 2u);
}

TEST(OrderBook, BestPricesTrackTheTopOfEachSide) {
  OrderBook book;
  book.add_limit(1, Side::Buy, 999, 10);
  book.add_limit(2, Side::Buy, 1000, 10);
  book.add_limit(3, Side::Buy, 998, 10);
  book.add_limit(4, Side::Sell, 1005, 10);
  book.add_limit(5, Side::Sell, 1003, 10);
  book.add_limit(6, Side::Sell, 1004, 10);

  EXPECT_EQ(book.best_bid(), 1000);
  EXPECT_EQ(book.best_ask(), 1003);
}

TEST(OrderBook, VolumeAddsUpAcrossOrdersAtOneLevel) {
  OrderBook book;
  book.add_limit(1, Side::Buy, 1000, 200);
  book.add_limit(2, Side::Buy, 1000, 100);
  EXPECT_EQ(book.volume_at(Side::Buy, 1000), 300u);
  EXPECT_EQ(book.volume_at(Side::Buy, 999), 0u);
  EXPECT_EQ(book.volume_at(Side::Sell, 1000), 0u);
}

TEST(OrderBook, TradesAtTheRestingOrdersPrice) {
  OrderBook book;
  book.add_limit(1, Side::Sell, 1002, 100);
  // The buyer is willing to pay 10.05 but gets the seller's 10.02.
  EXPECT_EQ(book.add_limit(2, Side::Buy, 1005, 100), (Trades{{2, 1, 100, 1002}}));
  EXPECT_EQ(book.order_count(), 0u);
}

TEST(OrderBook, EarlierOrderAtTheSamePriceTradesFirst) {
  OrderBook book;
  book.add_limit(1, Side::Buy, 1000, 50);
  book.add_limit(2, Side::Buy, 1000, 50);
  EXPECT_EQ(book.add_limit(3, Side::Sell, 1000, 70), (Trades{{1, 3, 50, 1000}, {2, 3, 20, 1000}}));
  EXPECT_EQ(book.volume_at(Side::Buy, 1000), 30u);
}

TEST(OrderBook, PartiallyFilledLimitOrderRestsTheRemainder) {
  OrderBook book;
  book.add_limit(1, Side::Sell, 1002, 40);
  EXPECT_EQ(book.add_limit(2, Side::Buy, 1002, 100), (Trades{{2, 1, 40, 1002}}));
  EXPECT_EQ(book.best_bid(), 1002);
  EXPECT_EQ(book.volume_at(Side::Buy, 1002), 60u);
  EXPECT_FALSE(book.best_ask().has_value());
}

TEST(OrderBook, MarketOrderNeverRests) {
  OrderBook book;
  book.add_limit(1, Side::Sell, 1002, 30);
  EXPECT_EQ(book.add_market(2, Side::Buy, 100), (Trades{{2, 1, 30, 1002}}));
  EXPECT_FALSE(book.best_bid().has_value());
  EXPECT_FALSE(book.best_ask().has_value());
  EXPECT_EQ(book.order_count(), 0u);
}

TEST(OrderBook, MarketOrderIntoAnEmptyBookDoesNothing) {
  OrderBook book;
  EXPECT_TRUE(book.add_market(1, Side::Sell, 100).empty());
  EXPECT_EQ(book.order_count(), 0u);
}

TEST(OrderBook, CancelRemovesARestingOrder) {
  OrderBook book;
  book.add_limit(1, Side::Buy, 1000, 100);
  book.add_limit(2, Side::Buy, 1000, 50);
  EXPECT_TRUE(book.cancel(1));
  EXPECT_EQ(book.volume_at(Side::Buy, 1000), 50u);
  EXPECT_EQ(book.order_count(), 1u);

  EXPECT_TRUE(book.cancel(2));
  EXPECT_FALSE(book.best_bid().has_value());
}

TEST(OrderBook, CancelOfUnknownOrFilledOrderReturnsFalse) {
  OrderBook book;
  EXPECT_FALSE(book.cancel(42));

  book.add_limit(1, Side::Sell, 1002, 10);
  book.add_market(2, Side::Buy, 10);
  EXPECT_FALSE(book.cancel(1));
}

TEST(OrderBook, CancelledOrderNeverTrades) {
  OrderBook book;
  book.add_limit(1, Side::Sell, 1002, 10);
  book.add_limit(2, Side::Sell, 1002, 10);
  book.cancel(1);
  EXPECT_EQ(book.add_market(3, Side::Buy, 10), (Trades{{3, 2, 10, 1002}}));
}

TEST(OrderBook, RejectsZeroQuantityAndDuplicateIds) {
  OrderBook book;
  EXPECT_THROW(book.add_limit(1, Side::Buy, 1000, 0), std::invalid_argument);
  EXPECT_THROW(book.add_market(2, Side::Buy, 0), std::invalid_argument);

  book.add_limit(3, Side::Buy, 1000, 10);
  EXPECT_THROW(book.add_limit(3, Side::Sell, 1005, 10), std::invalid_argument);
}

// The worked example from the primer, step by step.
TEST(OrderBook, PrimerWorkedExample) {
  // Ids: A=1 B=2 C=3 D=4 E=5 F=6 G=7 H=8 I=9 J=10.
  OrderBook book;
  book.add_limit(10, Side::Sell, 1005, 700);  // J
  book.add_limit(3, Side::Sell, 1003, 400);   // C
  book.add_limit(1, Side::Sell, 1002, 120);   // A
  book.add_limit(2, Side::Sell, 1002, 80);    // B
  book.add_limit(4, Side::Buy, 1000, 200);    // D
  book.add_limit(5, Side::Buy, 1000, 100);    // E
  book.add_limit(6, Side::Buy, 999, 500);     // F

  // Step 1: limit buy G 250 @ 10.03 sweeps 10.02, then takes 50 from C.
  EXPECT_EQ(book.add_limit(7, Side::Buy, 1003, 250),
            (Trades{{7, 1, 120, 1002}, {7, 2, 80, 1002}, {7, 3, 50, 1003}}));
  EXPECT_EQ(book.volume_at(Side::Sell, 1003), 350u);

  // Step 2: market sell H 250 fills D, then 50 of E.
  EXPECT_EQ(book.add_market(8, Side::Sell, 250), (Trades{{4, 8, 200, 1000}, {5, 8, 50, 1000}}));

  // Step 3: limit sell I 100 @ 10.01 does not cross and rests.
  EXPECT_TRUE(book.add_limit(9, Side::Sell, 1001, 100).empty());

  // Step 4: cancel C.
  EXPECT_TRUE(book.cancel(3));

  EXPECT_EQ(book.best_bid(), 1000);
  EXPECT_EQ(book.best_ask(), 1001);
  EXPECT_EQ(book.volume_at(Side::Buy, 1000), 50u);
  EXPECT_EQ(book.volume_at(Side::Buy, 999), 500u);
  EXPECT_EQ(book.volume_at(Side::Sell, 1001), 100u);
  EXPECT_EQ(book.volume_at(Side::Sell, 1005), 700u);
  EXPECT_EQ(book.volume_at(Side::Sell, 1003), 0u);
  EXPECT_EQ(book.order_count(), 4u);
}

}  // namespace
