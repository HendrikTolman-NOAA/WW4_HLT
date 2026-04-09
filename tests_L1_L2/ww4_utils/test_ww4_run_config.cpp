/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file test_run_config.cpp
 * @brief Unit tests for run-time configuration processing.
 * @details This file provides unit tests for the loadRunConfig function,
 *          covering various configuration options and defaults.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-03
 * @date Last Update, 2026-04-08
 */

#include "ww4_utils/ww4_run_config.hpp"
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>

using namespace ww4_utils;

/**
 * @test Verify loading a configuration with all non-default values.
 */
TEST(RunConfigTest, NonDefaultConfig) {
  const std::string filename = "test_run_nondefault.yml";
  std::ofstream file(filename);
  file << "calendar_type: \"NoLeap\"\n";
  file << "produce_std_out: \"no\"\n";
  file << "produce_log_file: \"no\"\n";
  file.close();

  const auto config = loadRunConfig(filename);
  ASSERT_TRUE(config.has_value());
  EXPECT_EQ(config->calendarType, TimeManagement::CalendarType::NoLeap);
  EXPECT_FALSE(config->produceStdOut);
  EXPECT_FALSE(config->produceLogFile);
  EXPECT_EQ(TimeManagement::getCalendarType(),
            TimeManagement::CalendarType::NoLeap);

  std::remove(filename.c_str());
}

/**
 * @test Verify loading configuration with comments, blank lines, and
 * whitespace.
 */
TEST(RunConfigTest, RobustParsingConfig) {
  const std::string filename = "test_run_robust.yml";
  std::ofstream file(filename);
  file << "\n";
  file << "  # This is a comment\n";
  file << "calendar_type   :   \"NoLeap\"   # End of line comment\n";
  file << "produce_std_out: yes\n";
  file << "\n";
  file << "produce_log_file : \"no\"\n";
  file.close();

  const auto config = loadRunConfig(filename);
  ASSERT_TRUE(config.has_value());
  EXPECT_EQ(config->calendarType, TimeManagement::CalendarType::NoLeap);
  EXPECT_TRUE(config->produceStdOut);
  EXPECT_FALSE(config->produceLogFile);

  std::remove(filename.c_str());
}

/**
 * @test Verify reportRunConfig formatting for Standard calendar.
 */
TEST(RunConfigTest, ReportConfigStandard) {
  RunConfig config;
  config.calendarType = TimeManagement::CalendarType::Standard;
  config.produceStdOut = true;
  config.produceLogFile = true;

  std::stringstream ss;
  reportRunConfig(config, ss);
  std::string output = ss.str();

  EXPECT_NE(output.find("Configuration settings :"), std::string::npos);
  EXPECT_NE(output.find("Calendar type      : Standard"), std::string::npos);
  EXPECT_NE(output.find("Screen output      : yes"), std::string::npos);
  EXPECT_NE(output.find("Log file           : yes"), std::string::npos);
}

/**
 * @test Verify reportRunConfig formatting for NoLeap calendar.
 */
TEST(RunConfigTest, ReportConfigNoLeap) {
  RunConfig config;
  config.calendarType = TimeManagement::CalendarType::NoLeap;
  config.produceStdOut = false;
  config.produceLogFile = false;

  std::stringstream ss;
  reportRunConfig(config, ss);
  std::string output = ss.str();

  EXPECT_NE(output.find("Calendar type      : NoLeap"), std::string::npos);
  EXPECT_NE(output.find("Screen output      : no"), std::string::npos);
  EXPECT_NE(output.find("Log file           : no"), std::string::npos);
}

/**
 * @test Verify reportRunConfig formatting for ThreeSixtyDay calendar.
 */
TEST(RunConfigTest, ReportConfigThreeSixtyDay) {
  RunConfig config;
  config.calendarType = TimeManagement::CalendarType::ThreeSixtyDay;

  std::stringstream ss;
  reportRunConfig(config, ss);
  std::string output = ss.str();

  EXPECT_NE(output.find("Calendar type      : ThreeSixtyDay"),
            std::string::npos);
}

/**
 * @test Verify default values when file is missing.
 */
TEST(RunConfigTest, MissingFileDefaults) {
  // Ensure calendar is something else before test
  TimeManagement::setCalendarType(TimeManagement::CalendarType::ThreeSixtyDay);

  const auto config = loadRunConfig("non_existent_run_config.yml");
  EXPECT_FALSE(config.has_value());

  // Manually reset for other tests
  TimeManagement::setCalendarType(TimeManagement::CalendarType::Standard);
}

/**
 * @test Verify ThreeSixtyDay calendar type.
 */
TEST(RunConfigTest, ThreeSixtyDayConfig) {
  const std::string filename = "test_run_360.yml";
  std::ofstream file(filename);
  file << "calendar_type: \"ThreeSixtyDay\"\n";
  file.close();

  const auto config = loadRunConfig(filename);
  ASSERT_TRUE(config.has_value());
  EXPECT_EQ(config->calendarType, TimeManagement::CalendarType::ThreeSixtyDay);
  EXPECT_TRUE(config->produceStdOut);
  EXPECT_TRUE(config->produceLogFile);
  EXPECT_EQ(TimeManagement::getCalendarType(),
            TimeManagement::CalendarType::ThreeSixtyDay);

  std::remove(filename.c_str());
}

/**
 * @test Verify partial configuration with some defaults.
 */
TEST(RunConfigTest, PartialConfig) {
  const std::string filename = "test_run_partial.yml";
  std::ofstream file(filename);
  file << "produce_std_out: \"no\"\n";
  file.close();

  const auto config = loadRunConfig(filename);
  ASSERT_TRUE(config.has_value());
  EXPECT_EQ(config->calendarType, TimeManagement::CalendarType::Standard);
  EXPECT_FALSE(config->produceStdOut);
  EXPECT_TRUE(config->produceLogFile);

  std::remove(filename.c_str());
}
