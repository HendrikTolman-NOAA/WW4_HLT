/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file test_ww4_service.cpp
 * @brief Unit tests for Ww4Service.
 * @details This file contains unit tests for core constants and the
 *          JONSWAP_5p static method of the Ww4Service class.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-09
 */

#include "ww4_utils/ww4_service.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace ww4_utils {

/**
 * @test Verify mathematical and physical constants.
 */
TEST(Ww4ServiceTest, ConstantsVerification) {
  EXPECT_NEAR(Ww4Service::PI, 3.14159265358979323846, 1.0e-15);
  EXPECT_NEAR(Ww4Service::TPI, 2.0 * Ww4Service::PI, 1.0e-15);
  EXPECT_NEAR(Ww4Service::GRAV, 9.806, 1.0e-10);
  EXPECT_NEAR(Ww4Service::DWAT, 1000.0, 1.0e-10);
  EXPECT_NEAR(Ww4Service::DAIR, 1.225, 1.0e-10);
  EXPECT_NEAR(Ww4Service::UNDEF, -999.9, 1.0e-10);
}

/**
 * @test Verify the JONSWAP_5p JONSWAP spectrum routine.
 */
TEST(Ww4ServiceTest, JONSWAP_5pVerification) {
  // Test case 1: Zero frequency should return zero.
  EXPECT_DOUBLE_EQ(Ww4Service::JONSWAP_5p(0.0, 1.0, 0.1, 1.0, 0.07, 0.09), 0.0);
  EXPECT_DOUBLE_EQ(Ww4Service::JONSWAP_5p(1.0e-5, 1.0, 0.1, 1.0, 0.07, 0.09),
                   0.0);

  // Test case 2: Typical JONSWAP parameters.
  // Using parameters where we can estimate behavior.
  const double f = 0.1;
  const double alfa = 0.0081;
  const double fp = 0.1;
  const double yln = std::log(3.3);
  const double siga = 0.07;
  const double sigb = 0.09;

  // At f = fp, B = -1.25, C = 0.
  // A = alfa * 0.06175 / fp**5 = 0.0081 * 0.06175 / 0.1**5 = 0.0081 * 0.06175 *
  // 100000 = 50.0175 J = A * exp(B + exp(C) * yln) = 50.0175 * exp(-1.25 + 1.0
  // * ln(3.3)) = 50.0175 * exp(ln(3.3) - 1.25) = 50.0175 * 3.3 * exp(-1.25) J =
  // 165.05775 * 0.28650479... approx 47.289...

  const double expected = 50.0175 * 3.3 * std::exp(-1.25);
  const double result = Ww4Service::JONSWAP_5p(f, alfa, fp, yln, siga, sigb);
  EXPECT_NEAR(result, expected, 1.0e-7);

  // Test case 3: Low overshoot (PM-like)
  const double result_pm = Ww4Service::JONSWAP_5p(f, alfa, fp, 0.0, siga, sigb);
  EXPECT_NEAR(result_pm, 50.0175 * std::exp(-1.25), 1.0e-7);
}

} // namespace ww4_utils
