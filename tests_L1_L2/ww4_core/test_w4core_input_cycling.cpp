/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file test_w4core_input_cycling.cpp
 * @brief Unit tests for homogeneous input cycling logic.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-21
 */

#include "ww4_core/w4core_hom_input.hpp"
#include "ww4_core/w4core_init.hpp"
#include "ww4_core/w4core_wave.hpp"
#include "ww4_utils/time_management.hpp"
#include <fstream>
#include <gtest/gtest.h>

namespace {

class W4CoreInputCyclingTest : public ::testing::Test {
protected:
  void SetUp() override {
    ww4_core::resetInternalState();
    ww4_utils::TimeManagement::reset();
  }

  void TearDown() override {
    ww4_core::resetInternalState();
    ww4_utils::TimeManagement::reset();
    std::remove("ww4_run_config.yml");
    std::remove("log.ww4");
  }

  void writeYaml(const std::string &content) {
    std::ofstream ofs("ww4_run_config.yml");
    ofs << content;
    ofs.close();
  }
};

TEST_F(W4CoreInputCyclingTest, CyclingBeforeData) {
  writeYaml(R"(
calendar_type: "Standard"
time_step: 3600.0
water_levels: "none"
currents: "none"
winds: "homogeneous"
- 20260101 120000 10.0 0.0
- 20260101 180000 15.0 0.0
ice_concentrations: "none"
bottom_depth: "none"
)");

  ww4_utils::DateTime startTime{20260101, 0.0};
  ww4_utils::DateTime endTime{20260101, 230000.0};
  std::stringstream ss;
  ww4_core::w4core_init(startTime, "test", ss);

  ww4_core::w4core_hom_winds(endTime);
  const auto &data = ww4_core::getWaveTimeData().winds;

  ASSERT_TRUE(data.time1.has_value());
  ASSERT_TRUE(data.time2.has_value());
  EXPECT_EQ(data.time1->ymd, 20260101);
  EXPECT_NEAR(data.time1->hms, 0.0, 1e-6);
  EXPECT_EQ(data.time2->ymd, 20260101);
  EXPECT_NEAR(data.time2->hms, 120000.0, 1e-6);
  EXPECT_NEAR(data.maxStep, 12.0 * 3600.0, 1e-6);
}

TEST_F(W4CoreInputCyclingTest, CyclingDuringData) {
  writeYaml(R"(
calendar_type: "Standard"
time_step: 3600.0
water_levels: "none"
currents: "none"
winds: "homogeneous"
- 20260101 000000 10.0 0.0
- 20260101 120000 15.0 0.0
- 20260101 180000 20.0 0.0
ice_concentrations: "none"
bottom_depth: "none"
)");

  ww4_utils::DateTime startTime{20260101, 60000.0}; // 06:00
  ww4_utils::DateTime endTime{20260101, 230000.0};
  std::stringstream ss;
  ww4_core::w4core_init(startTime, "test", ss);

  ww4_core::w4core_hom_winds(endTime);
  const auto &data = ww4_core::getWaveTimeData().winds;

  ASSERT_TRUE(data.time1.has_value());
  ASSERT_TRUE(data.time2.has_value());
  EXPECT_EQ(data.time1->ymd, 20260101);
  EXPECT_NEAR(data.time1->hms, 0.0, 1e-6);
  EXPECT_EQ(data.time2->ymd, 20260101);
  EXPECT_NEAR(data.time2->hms, 120000.0, 1e-6);
  EXPECT_NEAR(data.maxStep, 6.0 * 3600.0, 1e-6);
}

TEST_F(W4CoreInputCyclingTest, CyclingAfterData) {
  writeYaml(R"(
calendar_type: "Standard"
time_step: 3600.0
water_levels: "none"
currents: "none"
winds: "homogeneous"
- 20260101 000000 10.0 0.0
- 20260101 120000 15.0 0.0
ice_concentrations: "none"
bottom_depth: "none"
)");

  ww4_utils::DateTime startTime{20260101, 180000.0}; // 18:00
  ww4_utils::DateTime endTime{20260101, 230000.0};
  std::stringstream ss;
  ww4_core::w4core_init(startTime, "test", ss);

  ww4_core::w4core_hom_winds(endTime);
  const auto &data = ww4_core::getWaveTimeData().winds;

  ASSERT_TRUE(data.time1.has_value());
  ASSERT_TRUE(data.time2.has_value());
  EXPECT_EQ(data.time1->ymd, 20260101);
  EXPECT_NEAR(data.time1->hms, 120000.0, 1e-6);
  EXPECT_EQ(data.time2->ymd, 20260101);
  EXPECT_NEAR(data.time2->hms, 230000.0, 1e-6);
  EXPECT_NEAR(data.maxStep, 5.0 * 3600.0, 1e-6);
}

TEST_F(W4CoreInputCyclingTest, CyclingEmptyData) {
  // Note: w4core_hom_input fails if homogeneous is set but no data
  // But updateHomogeneousInputCycling should handle empty series if called.
  // We can't easily test empty series with w4core_init due to validation.
  // We can mock it if we could access processed data directly, but it's
  // private. Let's test the validation failure instead to ensure it's still
  // there.
  writeYaml(R"(
calendar_type: "Standard"
time_step: 3600.0
water_levels: "none"
currents: "none"
winds: "homogeneous"
ice_concentrations: "none"
bottom_depth: "none"
)");

  ww4_utils::DateTime startTime{20260101, 0.0};
  EXPECT_EXIT(ww4_core::w4core_init(startTime, "test", std::cerr),
              ::testing::ExitedWithCode(1),
              "No data provided for homogeneous field");
}

TEST_F(W4CoreInputCyclingTest, IntegrationWithWaveLoop) {
  writeYaml(R"(
calendar_type: "Standard"
time_step: 3600.0
water_levels: "none"
currents: "none"
winds: "homogeneous"
- 20260101 000000 10.0 0.0
- 20260101 003000 15.0 0.0
ice_concentrations: "none"
bottom_depth: "none"
)");

  ww4_utils::DateTime startTime{20260101, 0.0};
  ww4_utils::DateTime endTime{20260101, 10000.0}; // 01:00
  std::stringstream ss;
  ww4_core::w4core_init(startTime, "test", ss);

  // Run wave loop. First step should be 1800s (to reach 00:30:00) instead of
  // 3600s.
  ww4_core::w4core_wave(startTime, endTime, ss);

  // We can't easily check internal steps, but we can check the final model
  // time.
  EXPECT_EQ(ww4_core::getWaveTimeData().modelTime->ymd, 20260101);
  EXPECT_NEAR(ww4_core::getWaveTimeData().modelTime->hms, 10000.0, 1e-6);
}

} // namespace
