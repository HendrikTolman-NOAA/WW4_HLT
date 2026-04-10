/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file test_ww4_service.cpp
 * @brief Unit tests for WW4 constants in ww4_service.hpp.
 * @details Verifies the values of mathematical and physical constants.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @date Initial, 2026-04-09
 */

#include "ww4_utils/ww4_service.hpp"
#include <gtest/gtest.h>

namespace ww4_utils {
namespace constants {
namespace testing {

/**
 * @test VerifyMathematicalConstants
 * @brief Ensures all mathematical constants are correctly defined.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 */
TEST(WW4ServiceTest, VerifyMathematicalConstants) {
  EXPECT_DOUBLE_EQ(PI, 3.14159265358979323846);
  EXPECT_DOUBLE_EQ(TPI, 2.0 * PI);
  EXPECT_DOUBLE_EQ(HPI, 0.5 * PI);
  EXPECT_DOUBLE_EQ(TPIINV, 1.0 / TPI);
  EXPECT_DOUBLE_EQ(HPIINV, 1.0 / HPI);
  EXPECT_DOUBLE_EQ(RADE, 180.0 / PI);
  EXPECT_DOUBLE_EQ(DERA, PI / 180.0);
}

/**
 * @test VerifyPhysicalConstants
 * @brief Ensures all physical constants match WW3 values.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 */
TEST(WW4ServiceTest, VerifyPhysicalConstants) {
  EXPECT_DOUBLE_EQ(GRAV, 9.806);
  EXPECT_DOUBLE_EQ(DWAT, 1000.0);
  EXPECT_DOUBLE_EQ(DAIR, 1.225);
  EXPECT_DOUBLE_EQ(NU_AIR, 1.4e-5);
  EXPECT_DOUBLE_EQ(NU_WATER, 1.31e-6);
  EXPECT_DOUBLE_EQ(SED_SG, 2.65);
  EXPECT_DOUBLE_EQ(KAPPA, 0.40);
  EXPECT_DOUBLE_EQ(RADIUS, 4.0e7 / TPI);
}

/**
 * @test VerifyDerivedConstants
 * @brief Ensures derived constants are correctly calculated.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 */
TEST(WW4ServiceTest, VerifyDerivedConstants) {
  EXPECT_DOUBLE_EQ(G2PI3I, 1.0 / (GRAV * GRAV * TPI * TPI * TPI));
  EXPECT_DOUBLE_EQ(G1PI1I, 1.0 / (GRAV * TPI));
}

/**
 * @test VerifyModelConstants
 * @brief Ensures model-specific constants are correctly defined.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 */
TEST(WW4ServiceTest, VerifyModelConstants) {
  EXPECT_DOUBLE_EQ(UNDEF, -999.9);
  EXPECT_DOUBLE_EQ(ABMIN, -1.0);
  EXPECT_DOUBLE_EQ(ABMAX, 8.0);
}

} // namespace testing
} // namespace constants
} // namespace ww4_utils
