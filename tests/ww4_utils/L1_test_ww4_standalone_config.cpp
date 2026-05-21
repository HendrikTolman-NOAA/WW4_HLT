/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file L1_test_standalone_config.cpp
 * @brief Unit tests for stand-alone configuration processing.
 * @details This file provides unit tests for the loadStandaloneConfig
 *          function, covering valid, invalid, and edge-case configurations.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-02
 * @date Last update : 2026-05-20
 */

#include "ww4_utils/ww4_standalone_config.h"
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>

using namespace ww4_utils;

TEST(StandaloneConfigTest, ValidConfig) {
  const std::string filename = "test_valid_config.yaml";
  std::ofstream file(filename);
  file << "start_time: \"19680606 060000\"\n";
  file << "end_time: \"19680606 180000\"\n";
  file.close();

  const auto config = loadStandaloneConfig(filename, std::cerr);
  ASSERT_TRUE(config.has_value());
  EXPECT_EQ(config->startTime.ymd, 19680606);
  EXPECT_NEAR(config->startTime.hms, 60000.0, 1e-6);
  EXPECT_EQ(config->endTime.ymd, 19680606);
  EXPECT_NEAR(config->endTime.hms, 180000.0, 1e-6);

  std::remove(filename.c_str());
}

TEST(StandaloneConfigTest, ParseDateTimeString) {
  // Valid case
  auto dt = parseDateTimeString("19680606 060000");
  ASSERT_TRUE(dt.has_value());
  EXPECT_EQ(dt->ymd, 19680606);
  EXPECT_NEAR(dt->hms, 60000.0, 1e-6);

  // Valid case with whitespace and quotes
  dt = parseDateTimeString("  \"19680606 060000\"  ");
  ASSERT_TRUE(dt.has_value());
  EXPECT_EQ(dt->ymd, 19680606);
  EXPECT_NEAR(dt->hms, 60000.0, 1e-6);

  // Invalid: Empty
  EXPECT_FALSE(parseDateTimeString("").has_value());
  EXPECT_FALSE(parseDateTimeString("   ").has_value());

  // Invalid: Short
  EXPECT_FALSE(parseDateTimeString("19680606").has_value());
  EXPECT_FALSE(parseDateTimeString("19680606 123").has_value());

  // Invalid: Malformed (wrong separator)
  EXPECT_FALSE(parseDateTimeString("19680606-060000").has_value());

  // Invalid: Non-numeric
  EXPECT_FALSE(parseDateTimeString("ABCDEFGH 060000").has_value());
  EXPECT_FALSE(parseDateTimeString("19680606 ABCDEF").has_value());
}

TEST(StandaloneConfigTest, EmptyDateTime) {
  const std::string filename = "test_empty_dt.yaml";
  std::ofstream file(filename);
  file << "start_time: \"\"\n";
  file << "end_time: \"19680606 180000\"\n";
  file.close();

  const auto config = loadStandaloneConfig(filename, std::cerr);
  EXPECT_FALSE(config.has_value());

  std::remove(filename.c_str());
}

TEST(StandaloneConfigTest, ShortDateTime) {
  const std::string filename = "test_short_dt.yaml";
  std::ofstream file(filename);
  file << "start_time: \"19680606\"\n";
  file << "end_time: \"19680606 180000\"\n";
  file.close();

  const auto config = loadStandaloneConfig(filename, std::cerr);
  EXPECT_FALSE(config.has_value());

  std::remove(filename.c_str());
}

TEST(StandaloneConfigTest, NonNumericDateTime) {
  const std::string filename = "test_non_numeric_dt.yaml";
  std::ofstream file(filename);
  file << "start_time: \"ABCDEFGH 060000\"\n";
  file << "end_time: \"19680606 180000\"\n";
  file.close();

  const auto config = loadStandaloneConfig(filename, std::cerr);
  EXPECT_FALSE(config.has_value());

  std::remove(filename.c_str());
}

TEST(StandaloneConfigTest, NonNumericTimePart) {
  const std::string filename = "test_non_numeric_time.yaml";
  std::ofstream file(filename);
  file << "start_time: \"19680606 ABCDEF\"\n";
  file << "end_time: \"19680606 180000\"\n";
  file.close();

  const auto config = loadStandaloneConfig(filename, std::cerr);
  EXPECT_FALSE(config.has_value());

  std::remove(filename.c_str());
}

TEST(StandaloneConfigTest, EqualTimes) {
  const std::string filename = "test_equal_times.yaml";
  std::ofstream file(filename);
  file << "start_time: \"19680606 060000\"\n";
  file << "end_time: \"19680606 060000\"\n";
  file.close();

  const auto config = loadStandaloneConfig(filename, std::cerr);
  ASSERT_TRUE(config.has_value());
  EXPECT_EQ(config->startTime.ymd, config->endTime.ymd);
  EXPECT_NEAR(config->startTime.hms, config->endTime.hms, 1e-6);

  std::remove(filename.c_str());
}

TEST(StandaloneConfigTest, InvalidTimeOrder) {
  const std::string filename = "test_invalid_order.yaml";
  std::ofstream file(filename);
  file << "start_time: \"19680606 060000\"\n";
  file << "end_time: \"19680605 180000\"\n";
  file.close();

  const auto config = loadStandaloneConfig(filename, std::cerr);
  EXPECT_FALSE(config.has_value());

  std::remove(filename.c_str());
}

TEST(StandaloneConfigTest, MissingFields) {
  const std::string filename = "test_missing_fields.yaml";
  std::ofstream file(filename);
  file << "start_time: \"19680606 060000\"\n";
  file.close();

  const auto config = loadStandaloneConfig(filename, std::cerr);
  EXPECT_FALSE(config.has_value());

  std::remove(filename.c_str());
}

TEST(StandaloneConfigTest, MalformedDateTime) {
  const std::string filename = "test_malformed.yaml";
  std::ofstream file(filename);
  file << "start_time: \"19680606-060000\"\n"; // Missing space
  file << "end_time: \"19680606 180000\"\n";
  file.close();

  const auto config = loadStandaloneConfig(filename, std::cerr);
  EXPECT_FALSE(config.has_value());

  std::remove(filename.c_str());
}

TEST(StandaloneConfigTest, NonExistentFile) {
  const auto config = loadStandaloneConfig("non_existent.yaml", std::cerr);
  EXPECT_FALSE(config.has_value());
}

TEST(StandaloneConfigTest, ReportConfig) {
  StandaloneConfig config;
  config.startTime = {19680606, 60000.0};
  config.endTime = {19680606, 180000.0};

  std::stringstream ss;
  reportStandaloneConfig(config, ss);
  std::string output = ss.str();

  EXPECT_NE(output.find("Stand-alone configuration settings :"),
            std::string::npos);
  EXPECT_NE(output.find("Start time         : 1968/06/06 06:00:00 UTC"),
            std::string::npos);
  EXPECT_NE(output.find("End time           : 1968/06/06 18:00:00 UTC"),
            std::string::npos);
}

TEST(StandaloneConfigTest, RobustParsingConfig) {
  const std::string filename = "test_standalone_robust.yaml";
  std::ofstream file(filename);
  file << "\n";
  file << "  # This is a comment\n";
  file << "start_time   :   \"19680606 060000\"   # End of line comment\n";
  file << "\n";
  file << "end_time : \"19680606 180000\"\n";
  file.close();

  const auto config = loadStandaloneConfig(filename, std::cerr);
  ASSERT_TRUE(config.has_value());
  EXPECT_EQ(config->startTime.ymd, 19680606);
  EXPECT_NEAR(config->startTime.hms, 60000.0, 1e-6);
  EXPECT_EQ(config->endTime.ymd, 19680606);
  EXPECT_NEAR(config->endTime.hms, 180000.0, 1e-6);

  std::remove(filename.c_str());
}
