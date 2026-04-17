/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file test_w4core_hom_input.cpp
 * @brief Unit tests for w4core_hom_input.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-17
 */

#include "ww4_core/w4core_hom_input.hpp"
#include "ww4_core/w4core_init.hpp"
#include "ww4_utils/time_management.hpp"
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>

namespace {

class W4CoreHomInputTest : public ::testing::Test {
protected:
  void SetUp() override {
    ww4_core::resetInternalState();
    ww4_utils::TimeManagement::reset();
  }

  void TearDown() override {
    ww4_core::resetInternalState();
    ww4_utils::TimeManagement::reset();
    std::remove("ww4_run_config.yml");
  }

  void writeYaml(const std::string &content) {
    std::ofstream ofs("ww4_run_config.yml");
    ofs << content;
    ofs.close();
  }
};

TEST_F(W4CoreHomInputTest, ValidHomogeneousInput) {
  writeYaml(R"(
calendar_type: "Standard"
time_step: 3600.0
water_levels: "homogeneous"
- 20260101 000000 0.5
- 20260101 120000 0.6
currents: "none"
winds: "none"
ice_concentrations: "none"
bottom_depth: "from_grid"
echo_hom_input: "full"
)");

  ww4_utils::DateTime startTime{20260101, 0.0};
  std::stringstream ss;
  ww4_core::w4core_init(startTime, "test_input", ss);

  const auto &wl = ww4_core::getHomogeneousWaterLevels();
  ASSERT_EQ(wl.size(), 2);
  EXPECT_EQ(wl[0].time.ymd, 20260101);
  EXPECT_NEAR(wl[0].time.hms, 0.0, 1e-6);
  EXPECT_EQ(wl[0].values.size(), 1);
  EXPECT_NEAR(wl[0].values[0], 0.5, 1e-6);

  EXPECT_EQ(wl[1].time.ymd, 20260101);
  EXPECT_NEAR(wl[1].time.hms, 120000.0, 1e-6);
  EXPECT_EQ(wl[1].values.size(), 1);
  EXPECT_NEAR(wl[1].values[0], 0.6, 1e-6);

  std::string output = ss.str();
  EXPECT_EQ(output.find("Input data (w4core_hom_input) processing:"),
            std::string::npos);
  EXPECT_NE(output.find("Water levels         : homogeneous"),
            std::string::npos);
  EXPECT_NE(output.find("2026/01/01 00:00:00 UTC : 0.5"), std::string::npos);
  EXPECT_NE(output.find("2026/01/01 12:00:00 UTC : 0.6"), std::string::npos);
}

TEST_F(W4CoreHomInputTest, SummaryEcho) {
  writeYaml(R"(
calendar_type: "Standard"
time_step: 3600.0
water_levels: "homogeneous"
- 20260101 000000 0.5
- 20260101 120000 0.6
currents: "none"
winds: "none"
ice_concentrations: "none"
bottom_depth: "from_grid"
echo_hom_input: "summary"
)");

  ww4_utils::DateTime startTime{20260101, 0.0};
  std::stringstream ss;
  ww4_core::w4core_init(startTime, "test_input", ss);

  std::string output = ss.str();
  EXPECT_EQ(output.find("Input data (w4core_hom_input) processing:"),
            std::string::npos);
  EXPECT_NE(output.find("Number of data points: 2"), std::string::npos);
  EXPECT_EQ(output.find("2026/01/01 00:00:00 UTC : 0.5"), std::string::npos);
}

TEST_F(W4CoreHomInputTest, MultipleFields) {
  writeYaml(R"(
calendar_type: "Standard"
time_step: 3600.0
water_levels: "homogeneous"
- 20260101 000000 0.5
winds: "homogeneous"
- 20260101 000000 10.0 5.0
currents: "none"
ice_concentrations: "none"
bottom_depth: "from_grid"
echo_hom_input: "full"
)");

  ww4_utils::DateTime startTime{20260101, 0.0};
  std::stringstream ss;
  ww4_core::w4core_init(startTime, "test_input", ss);

  const auto &wl = ww4_core::getHomogeneousWaterLevels();
  ASSERT_EQ(wl.size(), 1);
  EXPECT_NEAR(wl[0].values[0], 0.5, 1e-6);

  const auto &wi = ww4_core::getHomogeneousWinds();
  ASSERT_EQ(wi.size(), 1);
  ASSERT_EQ(wi[0].values.size(), 2);
  EXPECT_NEAR(wi[0].values[0], 10.0, 1e-6);
  EXPECT_NEAR(wi[0].values[1], 5.0, 1e-6);

  std::string output = ss.str();
  EXPECT_NE(output.find("Water levels         : homogeneous"),
            std::string::npos);
  EXPECT_NE(output.find("Winds                : homogeneous"),
            std::string::npos);
}

TEST_F(W4CoreHomInputTest, BackwardTimeStamps) {
  writeYaml(R"(
calendar_type: "Standard"
time_step: 3600.0
water_levels: "homogeneous"
- 20260101 120000 0.5
- 20260101 000000 0.6
currents: "none"
winds: "none"
ice_concentrations: "none"
bottom_depth: "from_grid"
)");

  ww4_utils::DateTime startTime{20260101, 0.0};
  EXPECT_EXIT(ww4_core::w4core_init(startTime, "test_input", std::cerr),
              ::testing::ExitedWithCode(1), "Time stamps go backward");
}

TEST_F(W4CoreHomInputTest, MissingDataForHomogeneousField) {
  writeYaml(R"(
calendar_type: "Standard"
time_step: 3600.0
water_levels: "homogeneous"
currents: "none"
winds: "none"
ice_concentrations: "none"
bottom_depth: "from_grid"
)");

  ww4_utils::DateTime startTime{20260101, 0.0};
  EXPECT_EXIT(ww4_core::w4core_init(startTime, "test_input", std::cerr),
              ::testing::ExitedWithCode(1),
              "No data provided for homogeneous field");
}

TEST_F(W4CoreHomInputTest, IceConcentrationRange) {
  writeYaml(R"(
calendar_type: "Standard"
time_step: 3600.0
water_levels: "none"
currents: "none"
winds: "none"
ice_concentrations: "homogeneous"
- 20260101 000000 1.5
bottom_depth: "from_grid"
)");

  ww4_utils::DateTime startTime{20260101, 0.0};
  EXPECT_EXIT(ww4_core::w4core_init(startTime, "test_input", std::cerr),
              ::testing::ExitedWithCode(1),
              "Ice concentration must be between 0.0 and 1.0");
}

TEST_F(W4CoreHomInputTest, WindsParameterCount) {
  writeYaml(R"(
calendar_type: "Standard"
time_step: 3600.0
water_levels: "none"
currents: "none"
winds: "homogeneous"
- 20260101 000000 10.0
ice_concentrations: "none"
bottom_depth: "from_grid"
)");

  ww4_utils::DateTime startTime{20260101, 0.0};
  EXPECT_EXIT(ww4_core::w4core_init(startTime, "test_input", std::cerr),
              ::testing::ExitedWithCode(1),
              "Homogeneous winds requires 2 or 3 values");
}

} // namespace
