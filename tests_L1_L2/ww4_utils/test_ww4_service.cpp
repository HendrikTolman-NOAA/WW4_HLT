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
 * @date Initial, 2026-04-10
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

/**
 * @test VerifyJonswap5p
 * @brief Ensures the 5-parameter JONSWAP spectrum is correctly calculated.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 */
TEST(WW4ServiceTest, VerifyJonswap5p) {
  const double f = 0.1;
  const double fp = 0.1;
  const double alpha = 0.0081;
  const double gamma = 3.3;
  const double siga = 0.07;
  const double sigb = 0.09;

  // At f = fp, the formula simplifies:
  // r = exp(-0.5 * ((0.1 - 0.1) / (0.07 * 0.1))^2) = exp(0) = 1.0
  // f_ratio = 0.1 / 0.1 = 1.0
  // E(f) = 0.06175 * 0.0081 * (0.1)^-5 * exp(-1.25 * (1.0)^4) * (3.3)^1.0
  // E(f) = 0.06175 * 0.0081 * 100000 * exp(-1.25) * 3.3
  // E(f) = 50.0175 * 0.28650479686 * 3.3
  // E(f) = 47.28913...

  double expected = 0.06175 * alpha * std::pow(f, -5) * std::exp(-1.25) * gamma;
  double result = ww4_service::JONSWAP_5p(f, fp, alpha, gamma, siga, sigb);

  EXPECT_NEAR(result, expected, 1e-7);

  // Test zero/negative frequency
  EXPECT_DOUBLE_EQ(ww4_service::JONSWAP_5p(0.0, fp, alpha, gamma, siga, sigb),
                   0.0);
  EXPECT_DOUBLE_EQ(ww4_service::JONSWAP_5p(-0.1, fp, alpha, gamma, siga, sigb),
                   0.0);
}

/**
 * @test VerifyDistHaversine
 * @brief Ensures the haversine distance is correctly calculated.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 */
TEST(WW4ServiceTest, VerifyDistHaversine) {
  // Test distance between same points
  EXPECT_NEAR(ww4_service::dist_Haversine(10.0, 20.0, 10.0, 20.0), 0.0, 1e-9);

  // Test distance of 1 degree along the equator
  // lon1=0, lat1=0, lon2=1, lat2=0 -> distance should be 1 degree in radians
  EXPECT_NEAR(ww4_service::dist_Haversine(0.0, 0.0, 1.0, 0.0), DERA, 1e-9);

  // Test distance of 1 degree along a meridian
  // lon1=0, lat1=0, lon2=0, lat2=1 -> distance should be 1 degree in radians
  EXPECT_NEAR(ww4_service::dist_Haversine(0.0, 0.0, 0.0, 1.0), DERA, 1e-9);

  // Test distance of 180 degrees (antipodal points)
  // lon1=0, lat1=0, lon2=180, lat2=0 -> distance should be PI radians
  EXPECT_NEAR(ww4_service::dist_Haversine(0.0, 0.0, 180.0, 0.0), PI, 1e-9);

  // Test distance between (0, 45) and (1, 45)
  // dlat = 0
  // a = cos(45)^2 * sin(0.5)^2
  // c = 2 * atan2(sqrt(a), sqrt(1-a))
  double dlon_rad = 1.0 * DERA;
  double lat_rad = 45.0 * DERA;
  double a =
      std::pow(std::cos(lat_rad), 2) * std::pow(std::sin(dlon_rad / 2.0), 2);
  double expected_c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));

  EXPECT_NEAR(ww4_service::dist_Haversine(0.0, 45.0, 1.0, 45.0), expected_c,
              1e-9);
}

/**
 * @test VerifyDistOnSphere
 * @brief Ensures the spherical distance in meters is correctly calculated.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 */
TEST(WW4ServiceTest, VerifyDistOnSphere) {
  // Test distance between same points
  EXPECT_NEAR(ww4_service::dist_on_sphere(10.0, 20.0, 10.0, 20.0), 0.0, 1e-3);

  // Test distance of 1 degree along the equator
  // 1 degree in radians * RADIUS should be the distance in meters
  double expected_m = DERA * RADIUS;
  EXPECT_NEAR(ww4_service::dist_on_sphere(0.0, 0.0, 1.0, 0.0), expected_m,
              1e-3);

  // According to definition of RADIUS = 4.0e7 / TPI,
  // 360 degrees (TPI radians) = 4.0e7 meters.
  // 1 degree = 4.0e7 / 360 = 111111.111... meters.
  EXPECT_NEAR(ww4_service::dist_on_sphere(0.0, 0.0, 1.0, 0.0), 4.0e7 / 360.0,
              1e-3);
}

/**
 * @test VerifyWavenumberBeji
 * @brief Ensures the wavenumber calculation (Beji 2013) is accurate.
 * @details Validates the approximate wavenumber against the exact dispersion
 *          relation (omega^2 = g*k*tanh(k*h)) across different regimes.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 */
TEST(WW4ServiceTest, VerifyWavenumberBeji) {
  // Test cases: {omega, h}
  struct TestCase {
    double omega;
    double h;
    std::string label;
  };

  std::vector<TestCase> test_cases = {{1.0, 1000.0, "Deep water"},
                                      {1.0, 10.0, "Intermediate water"},
                                      {0.1, 1.0, "Shallow water"},
                                      {2.0, 0.5, "Very shallow/high freq"}};

  for (const auto &tc : test_cases) {
    auto result = ww4_service::wavenumber_Beji(tc.omega, tc.h);

    // Back-calculate omega^2 from exact dispersion relation: omega^2 = g * k *
    // tanh(k * h)
    double omega_sq_exact =
        constants::GRAV * result.k * std::tanh(result.k * tc.h);
    double omega_exact = std::sqrt(omega_sq_exact);

    // Beji (2013) is an approximation, so we expect some error but it should be
    // small.
    EXPECT_NEAR(omega_exact, tc.omega, 1e-3)
        << "Failed for " << tc.label << " (omega=" << tc.omega << ", h=" << tc.h
        << ")";

    // Verify group velocity relation: cg = 0.5 * (1 + 2kh/sinh(2kh)) * omega/k
    double kh = result.k * tc.h;
    double expected_cg;
    if (kh > 20.0) {
      expected_cg = 0.5 * tc.omega / result.k;
    } else {
      expected_cg =
          0.5 * (1.0 + (2.0 * kh / std::sinh(2.0 * kh))) * tc.omega / result.k;
    }
    EXPECT_NEAR(result.cg, expected_cg, 1e-5)
        << "Group velocity mismatch for " << tc.label;
  }

  // Test zero/negative inputs
  auto zero_h = ww4_service::wavenumber_Beji(1.0, 0.0);
  EXPECT_DOUBLE_EQ(zero_h.k, 0.0);
  EXPECT_DOUBLE_EQ(zero_h.cg, 0.0);

  auto zero_omega = ww4_service::wavenumber_Beji(0.0, 10.0);
  EXPECT_DOUBLE_EQ(zero_omega.k, 0.0);
  EXPECT_DOUBLE_EQ(zero_omega.cg, 0.0);
}

} // namespace testing
} // namespace constants
} // namespace ww4_utils
