/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file test_ww4_output_utils.cpp
 * @brief Unit tests for model output utilities.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-21
 */

#include "ww4_utils/ww4_output_utils.h"
#include "ww4_utils/ww4_run_config.h"
#include <gtest/gtest.h>

using namespace ww4_utils;

TEST(WW4OutputUtilsTest, AssessInitializesStartTime) {
  DateTime modelTime = {20260101, 0.0};
  DateTime endTime = {20260102, 0.0};
  RunConfig config;

  config.outputFields.requested = true;
  config.outputFields.interval = 3600.0;
  config.outputFields.atFirstTime = true;

  assessOutputConfig(modelTime, endTime, config);

  ASSERT_TRUE(config.outputFields.startTime.has_value());
  EXPECT_EQ(config.outputFields.startTime->ymd, 20260101);
  EXPECT_EQ(config.outputFields.startTime->hms, 0.0);

  ASSERT_TRUE(config.outputFields.actualTime.has_value());
  EXPECT_EQ(config.outputFields.actualTime->ymd, 20260101);
  EXPECT_EQ(config.outputFields.actualTime->hms, 0.0);
}

TEST(WW4OutputUtilsTest, AssessInitializesActualTimeWithoutFirstTime) {
  DateTime modelTime = {20260101, 0.0};
  DateTime endTime = {20260102, 0.0};
  RunConfig config;

  config.outputFields.requested = true;
  config.outputFields.interval = 3600.0;
  config.outputFields.atFirstTime = false;

  assessOutputConfig(modelTime, endTime, config);

  ASSERT_TRUE(config.outputFields.actualTime.has_value());
  EXPECT_EQ(config.outputFields.actualTime->ymd, 20260101);
  EXPECT_EQ(config.outputFields.actualTime->hms, 10000.0); // 3600s = 1h
}

TEST(WW4OutputUtilsTest, AssessApiOutput) {
  DateTime modelTime = {20260101, 0.0};
  DateTime endTime = {20260102, 0.0};
  RunConfig config;

  config.outputApi.requested = true;

  assessOutputConfig(modelTime, endTime, config);

  ASSERT_TRUE(config.outputApi.actualTime.has_value());
  EXPECT_EQ(config.outputApi.actualTime->ymd, 20260102);
  EXPECT_EQ(config.outputApi.actualTime->hms, 0.0);
}

TEST(WW4OutputUtilsTest, ComputeOutputTimeStep) {
  DateTime modelTime = {20260101, 0.0};
  DateTime endTime = {20260101, 7200.0}; // 2 hours later
  RunConfig config;

  config.outputFields.requested = true;
  config.outputFields.interval = 3600.0;
  config.outputFields.actualTime = DateTime{20260101, 10000.0}; // 1 hour later

  double step = computeOutputTimeStep(modelTime, endTime, config);
  EXPECT_NEAR(step, 3600.0, 0.001);

  config.outputPoints.requested = true;
  config.outputPoints.actualTime = DateTime{20260101, 0.0}; // Immediate
  step = computeOutputTimeStep(modelTime, endTime, config);
  EXPECT_NEAR(step, 0.0, 0.001);
}

TEST(WW4OutputUtilsTest, UpdateOutputActualTimes) {
  DateTime modelTime = {20260101, 10000.0};
  DateTime endTime = {20260102, 0.0};
  RunConfig config;

  config.outputFields.requested = true;
  config.outputFields.interval = 3600.0;
  config.outputFields.actualTime = modelTime;

  updateOutputActualTimes(modelTime, endTime, config);

  ASSERT_TRUE(config.outputFields.actualTime.has_value());
  EXPECT_EQ(config.outputFields.actualTime->ymd, 20260101);
  EXPECT_EQ(config.outputFields.actualTime->hms, 20000.0);
}

TEST(WW4OutputUtilsTest, DeactivatesOutputPastEndTime) {
  DateTime modelTime = {20260102, 0.0};
  DateTime endTime = {20260102, 0.0};
  RunConfig config;

  config.outputFields.requested = true;
  config.outputFields.interval = 3600.0;
  config.outputFields.actualTime = modelTime;

  updateOutputActualTimes(modelTime, endTime, config);

  // After updating at endTime, next scheduled time would be past endTime
  EXPECT_FALSE(config.outputFields.requested);
}
