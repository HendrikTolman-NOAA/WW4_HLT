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
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-03
 * @date Last update, 2026-04-21
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
  file << "water_levels: \"from_file\"\n";
  file << "currents: \"from_coupling\"\n";
  file << "winds: \"none\"\n";
  file << "ice_concentrations: \"none\"\n";
  file << "bottom_depth: \"from_grid\"\n";
  file << "time_step: 1800.0\n";
  file.close();

  const auto config = loadRunConfig(filename, std::cerr);
  ASSERT_TRUE(config.has_value());
  EXPECT_DOUBLE_EQ(config->timeStep, 1800.0);
  EXPECT_EQ(config->calendarType, TimeManagement::CalendarType::NoLeap);
  EXPECT_FALSE(config->produceStdOut);
  EXPECT_FALSE(config->produceLogFile);
  EXPECT_EQ(config->waterLevels, InputFieldOption::FromFile);
  EXPECT_EQ(config->currents, InputFieldOption::FromCoupling);
  EXPECT_EQ(config->winds, InputFieldOption::None);
  EXPECT_EQ(config->iceConcentrations, InputFieldOption::None);
  EXPECT_EQ(config->bottomDepth, InputFieldOption::FromGrid);
  EXPECT_EQ(TimeManagement::getCalendarType(),
            TimeManagement::CalendarType::NoLeap);

  std::remove(filename.c_str());
}

/**
 * @test Verify screen_output_level parsing and reporting.
 */
TEST(RunConfigTest, ScreenOutputLevelConfig) {
  const std::string filename = "test_run_screen_level.yml";
  std::ofstream file(filename);
  file << "water_levels: none\n";
  file << "currents: none\n";
  file << "winds: none\n";
  file << "ice_concentrations: none\n";
  file << "time_step: 3600.0\n";
  file << "screen_output_level: summary\n";
  file.close();

  const auto config = loadRunConfig(filename, std::cerr);
  ASSERT_TRUE(config.has_value());
  EXPECT_EQ(config->screenOutputLevel, ScreenOutputLevel::Summary);

  std::stringstream ss;
  reportRunConfig(*config, ss);
  std::string output = ss.str();
  EXPECT_NE(output.find("Screen output level  : summary"), std::string::npos);

  std::remove(filename.c_str());
}

/**
 * @test Verify data output configuration parsing.
 */
TEST(RunConfigTest, OutputConfigParsing) {
  const std::string filename = "test_output_parsing.yml";
  std::ofstream file(filename);
  file << "water_levels: none\n";
  file << "currents: none\n";
  file << "winds: none\n";
  file << "ice_concentrations: none\n";
  file << "time_step: 3600.0\n";

  file << "output_fields_requested: yes\n";
  file << "output_fields_interval: 3600\n";
  file << "output_fields_start: \"20260101 000000\"\n";
  file << "output_fields_end: \"20260101 120000\"\n";
  file << "output_fields_at_first: no\n";

  file << "output_restart_requested: yes\n";
  file << "output_restart_interval: 86400\n";
  file.close();

  const auto config = loadRunConfig(filename, std::cerr);
  ASSERT_TRUE(config.has_value());

  EXPECT_TRUE(config->outputFields.requested);
  EXPECT_DOUBLE_EQ(config->outputFields.interval, 3600.0);
  ASSERT_TRUE(config->outputFields.startTime.has_value());
  EXPECT_EQ(config->outputFields.startTime->ymd, 20260101);
  EXPECT_DOUBLE_EQ(config->outputFields.startTime->hms, 0.0);
  ASSERT_TRUE(config->outputFields.endTime.has_value());
  EXPECT_EQ(config->outputFields.endTime->ymd, 20260101);
  EXPECT_DOUBLE_EQ(config->outputFields.endTime->hms, 120000.0);
  EXPECT_FALSE(config->outputFields.atFirstTime);

  EXPECT_TRUE(config->outputRestart.requested);
  EXPECT_DOUBLE_EQ(config->outputRestart.interval, 86400.0);
  EXPECT_FALSE(config->outputRestart.startTime.has_value());
  EXPECT_TRUE(config->outputRestart.atFirstTime); // Default

  EXPECT_FALSE(config->outputPoints.requested); // Default

  std::remove(filename.c_str());
}

/**
 * @test Verify mandatory output interval failure.
 */
TEST(RunConfigTest, OutputIntervalFailure) {
  const std::string filename = "test_output_interval_failure.yml";
  std::ofstream file(filename);
  file << "water_levels: none\n";
  file << "currents: none\n";
  file << "winds: none\n";
  file << "ice_concentrations: none\n";
  file << "time_step: 3600.0\n";
  file << "output_fields_requested: yes\n";
  // Missing interval
  file.close();

  EXPECT_DEATH(loadRunConfig(filename, std::cerr),
               "Missing or invalid mandatory output interval\\(s\\).");

  std::remove(filename.c_str());
}

/**
 * @test Verify reportRunConfig with outputs.
 */
TEST(RunConfigTest, ReportConfigWithOutputs) {
  RunConfig config;
  config.waterLevels = InputFieldOption::None;
  config.currents = InputFieldOption::None;
  config.winds = InputFieldOption::None;
  config.iceConcentrations = InputFieldOption::None;
  config.bottomDepth = InputFieldOption::FromGrid;
  config.timeStep = 3600.0;

  config.outputFields.requested = true;
  config.outputFields.interval = 3600.0;
  config.outputFields.atFirstTime = false;
  config.outputFields.startTime = DateTime{20260101, 0.0};

  std::stringstream ss;
  reportRunConfig(config, ss);
  std::string output = ss.str();

  EXPECT_NE(output.find("Gridded fields output"), std::string::npos);
  EXPECT_NE(output.find("Interval          : 3600 s"), std::string::npos);
  EXPECT_NE(output.find("At first time     : no"), std::string::npos);
  EXPECT_NE(output.find("Start time        : 2026/01/01 00:00:00 UTC"),
            std::string::npos);
  EXPECT_NE(output.find("Time step            : 3600 s"), std::string::npos);
  EXPECT_NE(output.find("Point output not requested"), std::string::npos);
}

/**
 * @test Verify loading configuration with new runtime flags.
 */
TEST(RunConfigTest, NewFlagsConfig) {
  const std::string filename = "test_run_new_flags.yml";
  std::ofstream file(filename);
  file << "dry_run: yes\n";
  file << "propagate_x: no\n";
  file << "propagate_y: no\n";
  file << "propagate_theta: no\n";
  file << "propagate_k: no\n";
  file << "source_terms: no\n";
  file << "water_levels: none\n";
  file << "currents: none\n";
  file << "winds: none\n";
  file << "ice_concentrations: none\n";
  file << "bottom_depth: from_grid\n";
  file << "time_step: 3600.0\n";
  file.close();

  const auto config = loadRunConfig(filename, std::cerr);
  ASSERT_TRUE(config.has_value());
  EXPECT_TRUE(config->dryRun);
  EXPECT_FALSE(config->propagateX);
  EXPECT_FALSE(config->propagateY);
  EXPECT_FALSE(config->propagateTheta);
  EXPECT_FALSE(config->propagateK);
  EXPECT_FALSE(config->sourceTerms);

  std::remove(filename.c_str());
}

/**
 * @test Verify the cleanValue helper function for trimming and quote removal.
 */
TEST(RunConfigTest, CleanValue) {
  // Test whitespace trimming
  EXPECT_EQ(cleanValue("  hello  "), "hello");
  EXPECT_EQ(cleanValue("\thello\t"), "hello");

  // Test quote removal
  EXPECT_EQ(cleanValue("\"hello\""), "hello");
  EXPECT_EQ(cleanValue("  \"hello\"  "), "hello");

  // Test mixed whitespace and quotes
  EXPECT_EQ(cleanValue(" \t\"hello\" \t"), "hello");

  // Test empty and whitespace-only strings
  EXPECT_EQ(cleanValue(""), "");
  EXPECT_EQ(cleanValue("   "), "");
  EXPECT_EQ(cleanValue("\t\t"), "");

  // Test only quotes
  EXPECT_EQ(cleanValue("\"\""), "");
  EXPECT_EQ(cleanValue(" \"\" "), "");

  // Test string with internal spaces
  EXPECT_EQ(cleanValue("  hello world  "), "hello world");
  EXPECT_EQ(cleanValue("\"hello world\""), "hello world");
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
  file << "water_levels : \"none\"\n";
  file << "currents : none # inline comment\n";
  file << "winds : \"none\"\n";
  file << "ice_concentrations : \"none\"\n";
  file << "bottom_depth : \"from_grid\"\n";
  file << "time_step : 3600.0\n";
  file.close();

  const auto config = loadRunConfig(filename, std::cerr);
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
  config.waterLevels = InputFieldOption::None;
  config.currents = InputFieldOption::None;
  config.winds = InputFieldOption::None;
  config.iceConcentrations = InputFieldOption::None;
  config.bottomDepth = InputFieldOption::FromGrid;
  config.timeStep = 3600.0;

  std::stringstream ss;
  reportRunConfig(config, ss);
  std::string output = ss.str();

  EXPECT_NE(output.find("Configuration settings :"), std::string::npos);
  EXPECT_NE(output.find("Calendar type        : Standard"), std::string::npos);
  EXPECT_NE(output.find("Screen output        : yes"), std::string::npos);
  EXPECT_NE(output.find("Log file             : yes"), std::string::npos);
  EXPECT_NE(output.find("Conventional model run"), std::string::npos);
  // Ensure no detailed flag reporting when conventional
  EXPECT_EQ(output.find("Dry run"), std::string::npos);

  EXPECT_NE(output.find("Water levels         : none"), std::string::npos);
  EXPECT_NE(output.find("Currents             : none"), std::string::npos);
  EXPECT_NE(output.find("Winds                : none"), std::string::npos);
  EXPECT_NE(output.find("Ice concentrations   : none"), std::string::npos);
  EXPECT_NE(output.find("Bottom depth         : from_grid"), std::string::npos);
  EXPECT_NE(output.find("Time step            : 3600 s"), std::string::npos);
}

/**
 * @test Verify API output configuration parsing.
 */
TEST(RunConfigTest, ApiOutputConfig) {
  const std::string filename = "test_api_output.yml";
  std::ofstream file(filename);
  file << "water_levels: none\n";
  file << "currents: none\n";
  file << "winds: none\n";
  file << "ice_concentrations: none\n";
  file << "time_step: 3600.0\n";
  file << "output_api: yes\n";
  file.close();

  const auto config = loadRunConfig(filename, std::cerr);
  ASSERT_TRUE(config.has_value());
  EXPECT_TRUE(config->outputApi.requested);

  std::stringstream ss;
  reportRunConfig(*config, ss);
  std::string output = ss.str();
  EXPECT_NE(output.find("API output"), std::string::npos);
  EXPECT_EQ(output.find("Interval", output.find("API")), std::string::npos);
  EXPECT_EQ(output.find("At first time", output.find("API")),
            std::string::npos);

  std::remove(filename.c_str());
}

/**
 * @test Verify reportRunConfig for non-conventional run.
 */
TEST(RunConfigTest, ReportConfigNonConventional) {
  RunConfig config;
  config.dryRun = true;
  config.propagateX = false;
  config.waterLevels = InputFieldOption::None;
  config.currents = InputFieldOption::None;
  config.winds = InputFieldOption::None;
  config.iceConcentrations = InputFieldOption::None;
  config.bottomDepth = InputFieldOption::FromGrid;
  config.timeStep = 3600.0;

  std::stringstream ss;
  reportRunConfig(config, ss);
  std::string output = ss.str();

  EXPECT_NE(output.find("Unconventional model run"), std::string::npos);
  EXPECT_NE(output.find("Dry run"), std::string::npos);
  EXPECT_EQ(output.find("Propagate X"), std::string::npos);
  EXPECT_NE(output.find("Time step            : 3600 s"), std::string::npos);
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

  EXPECT_NE(output.find("Calendar type        : NoLeap"), std::string::npos);
  EXPECT_NE(output.find("Screen output        : no"), std::string::npos);
  EXPECT_NE(output.find("Log file             : no"), std::string::npos);
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

  EXPECT_NE(output.find("Calendar type        : ThreeSixtyDay"),
            std::string::npos);
}

/**
 * @test Verify default values when file is missing.
 */
TEST(RunConfigTest, MissingFileDefaults) {
  // Ensure calendar is something else before test
  TimeManagement::setCalendarType(TimeManagement::CalendarType::ThreeSixtyDay);

  const auto config = loadRunConfig("non_existent_run_config.yml", std::cerr);
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
  file << "water_levels: none\n";
  file << "currents: none\n";
  file << "winds: none\n";
  file << "ice_concentrations: none\n";
  file << "bottom_depth: from_grid\n";
  file << "time_step: 3600.0\n";
  file.close();

  const auto config = loadRunConfig(filename, std::cerr);
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
  file << "water_levels: none\n";
  file << "currents: none\n";
  file << "winds: none\n";
  file << "ice_concentrations: none\n";
  file << "bottom_depth: from_grid\n";
  file << "time_step: 3600.0\n";
  file.close();

  const auto config = loadRunConfig(filename, std::cerr);
  ASSERT_TRUE(config.has_value());
  EXPECT_EQ(config->calendarType, TimeManagement::CalendarType::Standard);
  EXPECT_FALSE(config->produceStdOut);
  EXPECT_TRUE(config->produceLogFile);

  std::remove(filename.c_str());
}

/**
 * @test Verify mandatory time step failure.
 */
TEST(RunConfigTest, TimeStepFailure) {
  const std::string filename = "test_run_time_step_failure.yml";
  std::ofstream file(filename);
  file << "water_levels: none\n";
  file << "currents: none\n";
  file << "winds: none\n";
  file << "ice_concentrations: none\n";
  // time_step is missing or invalid
  file << "time_step: -1.0\n";
  file.close();

  // Should abort program
  EXPECT_DEATH(loadRunConfig(filename, std::cerr),
               "Missing or invalid mandatory time step.");

  std::remove(filename.c_str());
}

/**
 * @test Verify mandatory fields failure.
 */
TEST(RunConfigTest, MandatoryFieldsFailure) {
  const std::string filename = "test_run_mandatory_failure.yml";
  std::ofstream file(filename);
  file << "produce_std_out: \"no\"\n";
  // Missing other fields
  file.close();

  // Should abort program
  EXPECT_DEATH(loadRunConfig(filename, std::cerr),
               "Missing or invalid mandatory fields.");

  std::remove(filename.c_str());
}

/**
 * @test Verify that bottom_depth is optional and defaults to from_grid.
 */
TEST(RunConfigTest, BottomDepthDefault) {
  const std::string filename = "test_run_bottom_depth_default.yml";
  std::ofstream file(filename);
  file << "water_levels: none\n";
  file << "currents: none\n";
  file << "winds: none\n";
  file << "ice_concentrations: none\n";
  file << "time_step: 3600.0\n";
  // bottom_depth is missing
  file.close();

  const auto config = loadRunConfig(filename, std::cerr);
  ASSERT_TRUE(config.has_value());
  EXPECT_EQ(config->bottomDepth, InputFieldOption::FromGrid);

  std::remove(filename.c_str());
}

/**
 * @test Verify that bottom_depth can take other options.
 */
TEST(RunConfigTest, BottomDepthOtherOptions) {
  const std::string filename = "test_run_bottom_depth_none.yml";
  std::ofstream file(filename);
  file << "water_levels: none\n";
  file << "currents: none\n";
  file << "winds: none\n";
  file << "ice_concentrations: none\n";
  file << "bottom_depth: none\n";
  file << "time_step: 3600.0\n";
  file.close();

  const auto config = loadRunConfig(filename, std::cerr);
  ASSERT_TRUE(config.has_value());
  EXPECT_EQ(config->bottomDepth, InputFieldOption::None);

  std::remove(filename.c_str());
}

/**
 * @test Verify that from_grid is rejected for other fields.
 */
TEST(RunConfigTest, FromGridRejection) {
  const std::string filename = "test_run_from_grid_rejection.yml";
  std::ofstream file(filename);
  file << "water_levels: from_grid\n";
  file << "currents: none\n";
  file << "winds: none\n";
  file << "ice_concentrations: none\n";
  file << "bottom_depth: from_grid\n";
  file << "time_step: 3600.0\n";
  file.close();

  // Should abort because water_levels: from_grid is invalid
  EXPECT_DEATH(loadRunConfig(filename, std::cerr),
               "Missing or invalid mandatory fields.");

  std::remove(filename.c_str());
}

/**
 * @test Verify all InputFieldOption parsing.
 */
TEST(RunConfigTest, InputFieldOptionParsing) {
  const std::string filename = "test_input_parsing.yml";
  std::ofstream file(filename);
  file << "water_levels: \"none\"\n";
  file << "currents: \"from_file\"\n";
  file << "winds: \"from_coupling\"\n";
  file << "ice_concentrations: \"none\"\n";
  file << "bottom_depth: \"from_grid\"\n";
  file << "time_step: 3600.0\n";
  file.close();

  const auto config = loadRunConfig(filename, std::cerr);
  ASSERT_TRUE(config.has_value());
  EXPECT_EQ(config->waterLevels, InputFieldOption::None);
  EXPECT_EQ(config->currents, InputFieldOption::FromFile);
  EXPECT_EQ(config->winds, InputFieldOption::FromCoupling);
  EXPECT_EQ(config->iceConcentrations, InputFieldOption::None);
  EXPECT_EQ(config->bottomDepth, InputFieldOption::FromGrid);

  std::remove(filename.c_str());
}
