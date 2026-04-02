/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file test_stand_alone_config.cpp
 * @brief Unit tests for stand-alone configuration processing.
 * @details This file provides unit tests for the loadStandAloneConfig
 *          function, covering valid, invalid, and edge-case configurations.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-02
 * @date Last Update, 2026-04-02
 */

#include "ww4_utils/ww4_stand_alone_config.hpp"
#include <fstream>
#include <gtest/gtest.h>

using namespace ww4_utils;

/**
 * @test Verify loading a valid configuration.
 */
TEST(StandAloneConfigTest, ValidConfig) {
  const std::string filename = "test_valid_config.yml";
  std::ofstream file(filename);
  file << "start_time: \"19680606 060000\"\n";
  file << "end_time: \"19680606 180000\"\n";
  file.close();

  const auto config = loadStandAloneConfig(filename);
  ASSERT_TRUE(config.has_value());
  EXPECT_EQ(config->startTime.ymd, 19680606);
  EXPECT_NEAR(config->startTime.hms, 60000.0, 1e-6);
  EXPECT_EQ(config->endTime.ymd, 19680606);
  EXPECT_NEAR(config->endTime.hms, 180000.0, 1e-6);

  std::remove(filename.c_str());
}

/**
 * @test Verify that equal start and end times are valid.
 */
TEST(StandAloneConfigTest, EqualTimes) {
  const std::string filename = "test_equal_times.yml";
  std::ofstream file(filename);
  file << "start_time: \"19680606 060000\"\n";
  file << "end_time: \"19680606 060000\"\n";
  file.close();

  const auto config = loadStandAloneConfig(filename);
  ASSERT_TRUE(config.has_value());
  EXPECT_EQ(config->startTime.ymd, config->endTime.ymd);
  EXPECT_NEAR(config->startTime.hms, config->endTime.hms, 1e-6);

  std::remove(filename.c_str());
}

/**
 * @test Verify that end time before start time is invalid.
 */
TEST(StandAloneConfigTest, InvalidTimeOrder) {
  const std::string filename = "test_invalid_order.yml";
  std::ofstream file(filename);
  file << "start_time: \"19680606 060000\"\n";
  file << "end_time: \"19680605 180000\"\n";
  file.close();

  const auto config = loadStandAloneConfig(filename);
  EXPECT_FALSE(config.has_value());

  std::remove(filename.c_str());
}

/**
 * @test Verify handling of missing required fields.
 */
TEST(StandAloneConfigTest, MissingFields) {
  const std::string filename = "test_missing_fields.yml";
  std::ofstream file(filename);
  file << "start_time: \"19680606 060000\"\n";
  file.close();

  const auto config = loadStandAloneConfig(filename);
  EXPECT_FALSE(config.has_value());

  std::remove(filename.c_str());
}

/**
 * @test Verify handling of malformed date-time strings.
 */
TEST(StandAloneConfigTest, MalformedDateTime) {
  const std::string filename = "test_malformed.yml";
  std::ofstream file(filename);
  file << "start_time: \"19680606-060000\"\n"; // Missing space
  file << "end_time: \"19680606 180000\"\n";
  file.close();

  const auto config = loadStandAloneConfig(filename);
  EXPECT_FALSE(config.has_value());

  std::remove(filename.c_str());
}

/**
 * @test Verify behavior with non-existent file.
 */
TEST(StandAloneConfigTest, NonExistentFile) {
  const auto config = loadStandAloneConfig("non_existent.yml");
  EXPECT_FALSE(config.has_value());
}
