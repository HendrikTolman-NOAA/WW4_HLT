/**
 * @file test_scheme_factory.cpp
 * @brief Unit tests for the SchemeFactory class.
 */

#include "ww4_core/scheme_factory.hpp"
#include <gtest/gtest.h>

namespace ww4_core {

TEST(SchemeFactoryTest, CreateSolverPR3) {
  auto solver = SchemeFactory::createSolver("PR3");
  ASSERT_NE(solver, nullptr);
  EXPECT_EQ(solver->getName(), "PR3");
}

TEST(SchemeFactoryTest, CreateSourceST4) {
  auto scheme = SchemeFactory::createSourceTerm("ST4");
  ASSERT_NE(scheme, nullptr);
  EXPECT_EQ(scheme->getName(), "ST4");
}

TEST(SchemeFactoryTest, CreateUnknownSolver) {
  EXPECT_THROW(SchemeFactory::createSolver("UNKNOWN"), std::invalid_argument);
}

TEST(SchemeFactoryTest, CreateUnknownSource) {
  EXPECT_THROW(SchemeFactory::createSourceTerm("UNKNOWN"),
               std::invalid_argument);
}

} // namespace ww4_core
