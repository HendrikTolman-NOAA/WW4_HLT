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
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-21
 */

#include "ww4_utils/ww4_output_utils.hpp"
#include "ww4_utils/ww4_run_config.hpp"
#include <gtest/gtest.h>

using namespace ww4_utils;

TEST(WW4OutputUtilsTest, AssessOutputConfigInitialization) {
  OutputConfig oc;
  oc.requested = true;
  oc.interval = 3600.0;
  DateTime modelTime = {20260421, 0.0};

  assessOutputConfig(oc, modelTime, false);

  EXPECT_TRUE(oc.startTime.has_value());
  EXPECT_EQ(oc.startTime->ymd, 20260421);
  EXPECT_EQ(oc.startTime->hms, 0.0);
  EXPECT_TRUE(oc.actualTime.has_value());
  EXPECT_EQ(oc.actualTime->ymd, 20260421);
  EXPECT_EQ(oc.actualTime->hms, 0.0);
}

TEST(WW4OutputUtilsTest, AssessOutputConfigSyncsWithModelTime) {
  OutputConfig oc;
  oc.requested = true;
  oc.interval = 3600.0;
  oc.startTime = {20260421, 0.0};
  oc.actualTime = {20260421, 0.0};

  DateTime modelTime = {20260421, 0.0};
  TimeManagement::incrementDateTime(modelTime, 7200.0); // 2 hours later

  assessOutputConfig(oc, modelTime, false);

  EXPECT_TRUE(oc.actualTime.has_value());
  EXPECT_EQ(oc.actualTime->ymd, modelTime.ymd);
  EXPECT_DOUBLE_EQ(oc.actualTime->hms, modelTime.hms);
}

TEST(WW4OutputUtilsTest, AssessOutputConfigHandlesApi) {
  OutputConfig oc;
  oc.requested = true;
  DateTime modelTime = {20260421, 0.0};
  DateTime waveEndTime = {20260421, 10800.0};

  assessOutputConfig(oc, modelTime, true, waveEndTime);

  EXPECT_TRUE(oc.actualTime.has_value());
  EXPECT_EQ(oc.actualTime->ymd, 20260421);
  EXPECT_EQ(oc.actualTime->hms, 10800.0);
}

TEST(WW4OutputUtilsTest, ComputeNextOutputStep) {
  OutputConfig oc;
  oc.requested = true;
  oc.actualTime = {20260421, 0.0};
  TimeManagement::incrementDateTime(*oc.actualTime, 3600.0);
  DateTime modelTime = {20260421, 0.0};

  double step = computeNextOutputStep(oc, modelTime);
  EXPECT_DOUBLE_EQ(step, 3600.0);

  TimeManagement::incrementDateTime(modelTime, 3600.0);
  step = computeNextOutputStep(oc, modelTime);
  EXPECT_DOUBLE_EQ(step, 0.0);
}

TEST(WW4OutputUtilsTest, IsOutputDue) {
  OutputConfig oc;
  oc.requested = true;
  oc.actualTime = {20260421, 0.0};
  TimeManagement::incrementDateTime(*oc.actualTime, 3600.0);

  DateTime modelTime = {20260421, 0.0};
  EXPECT_FALSE(isOutputDue(oc, modelTime));

  TimeManagement::incrementDateTime(modelTime, 3600.0);
  EXPECT_TRUE(isOutputDue(oc, modelTime));
}

TEST(WW4OutputUtilsTest, UpdateOutputTime) {
  OutputConfig oc;
  oc.requested = true;
  oc.interval = 3600.0;
  oc.actualTime = {20260421, 0.0};
  TimeManagement::incrementDateTime(*oc.actualTime, 3600.0);

  DateTime modelTime = {20260421, 0.0};
  TimeManagement::incrementDateTime(modelTime, 3600.0);
  updateOutputTime(oc, modelTime);

  DateTime expectedTime = {20260421, 0.0};
  TimeManagement::incrementDateTime(expectedTime, 7200.0);

  EXPECT_EQ(oc.actualTime->ymd, expectedTime.ymd);
  EXPECT_DOUBLE_EQ(oc.actualTime->hms, expectedTime.hms);
}

TEST(WW4OutputUtilsTest, AssessOutputConfigHandlesEndTime) {
  OutputConfig oc;
  oc.requested = true;
  oc.interval = 3600.0;
  oc.endTime = {20260421, 0.0};
  TimeManagement::incrementDateTime(*oc.endTime, 3600.0);

  DateTime modelTime = {20260421, 0.0};
  TimeManagement::incrementDateTime(modelTime, 7200.0); // Past end time

  assessOutputConfig(oc, modelTime, false);

  EXPECT_FALSE(oc.requested);
}

TEST(WW4OutputUtilsTest, ComputeMinOutputStep) {
  RunConfig config;
  config.outputFields.requested = true;
  config.outputFields.actualTime = {20260421, 0.0};
  TimeManagement::incrementDateTime(*config.outputFields.actualTime, 3600.0);

  config.outputPoints.requested = true;
  config.outputPoints.actualTime = {20260421, 0.0};
  TimeManagement::incrementDateTime(*config.outputPoints.actualTime, 1800.0);

  DateTime modelTime = {20260421, 0.0};
  DateTime endTime = {20260421, 0.0};
  TimeManagement::incrementDateTime(endTime, 7200.0);

  double step = computeMinOutputStep(config, modelTime, endTime);
  EXPECT_DOUBLE_EQ(step, 1800.0);
}
