/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file L1_test_ww4_constants.cpp
 * @brief Unit tests for WW4 constants in ww4_constants.h.
 * @details Verifies the values of mathematical and physical constants.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @date 2026-05-21
 */

#include "ww4_utils/ww4_constants.h"
#include <gtest/gtest.h>
#include <numbers>

namespace ww4_constants {
namespace testing {

TEST(WW4ConstantsTest, VerifyMathematicalConstants) {
  static_assert(Radians2Degrees == 180.0 / std::numbers::pi);
  static_assert(Degrees2Radians == std::numbers::pi / 180.0);

  EXPECT_NEAR(Radians2Degrees, 57.29577951308232, 1e-14);
  EXPECT_NEAR(Degrees2Radians, 0.017453292519943295, 1e-17);
}

TEST(WW4ConstantsTest, VerifyPhysicalConstants) {
  static_assert(GRAV == 9.806);
  static_assert(DWAT == 1000.0);
  static_assert(DAIR == 1.225);
  static_assert(NU_AIR == 1.4e-5);
  static_assert(NU_WATER == 1.31e-6);
  static_assert(SED_SG == 2.65);
  static_assert(KAPPA == 0.40);
  static_assert(RADIUS == 4.0e7 / (2.0 * std::numbers::pi));

  EXPECT_DOUBLE_EQ(GRAV, 9.806);
  EXPECT_DOUBLE_EQ(DWAT, 1000.0);
  EXPECT_DOUBLE_EQ(DAIR, 1.225);
  EXPECT_DOUBLE_EQ(NU_AIR, 1.4e-5);
  EXPECT_DOUBLE_EQ(NU_WATER, 1.31e-6);
  EXPECT_DOUBLE_EQ(SED_SG, 2.65);
  EXPECT_DOUBLE_EQ(KAPPA, 0.40);
  EXPECT_NEAR(RADIUS, 6366197.723675813, 1e-8);
}

TEST(WW4ConstantsTest, VerifyModelConstants) {
  static_assert(UNDEF == -999.9);
  static_assert(ABMIN == -1.0);
  static_assert(ABMAX == 8.0);
  static_assert(KDMAX == 20.0);
  static_assert(JONSWAP_FACTOR == 0.06175);

  EXPECT_DOUBLE_EQ(UNDEF, -999.9);
  EXPECT_DOUBLE_EQ(ABMIN, -1.0);
  EXPECT_DOUBLE_EQ(ABMAX, 8.0);
  EXPECT_DOUBLE_EQ(KDMAX, 20.0);
  EXPECT_DOUBLE_EQ(JONSWAP_FACTOR, 0.06175);
}

TEST(WW4ConstantsTest, VerifyConstantsConsistency) {
  EXPECT_NEAR(Degrees2Radians * Radians2Degrees, 1.0, 1e-15);
}

} // namespace testing
} // namespace ww4_constants
