/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_run_config.hpp
 * @brief Service routine for processing run-time configuration YAML file.
 * @details This header defines the RunConfig structure and the
 *          loadRunConfig function, which reads and processes the
 *          run-time configuration for WAVEWATCH IV.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-03
 * @date Last update, 2026-04-22
 */

#ifndef WW4_UTILS_WW4_RUN_CONFIG_HPP
#define WW4_UTILS_WW4_RUN_CONFIG_HPP

#include "ww4_utils/time_management.hpp"
#include "ww4_utils/ww4_output_utils.hpp"
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>

namespace ww4_utils {

/**
 * @enum InputFieldOption
 * @brief Options for model input fields.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 */
enum class InputFieldOption {
  Undefined,    ///< Mandatory field not yet defined.
  None,         ///< No input, data set to zero.
  FromFile,     ///< Input from file.
  FromCoupling, ///< Input from coupling.
  Homogeneous,  ///< Homogeneous field.
  FromGrid      ///< Static data read as part of the grid data.
};

/**
 * @enum ScreenOutputLevel
 * @brief Options for the level of output to standard output during time
 * stepping.
 * @author Main Author(s): Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 */
enum class ScreenOutputLevel {
  None,    ///< No output in the time stepping loop.
  Summary, ///< Output if input field updated or output produced.
  Full     ///< Standard full output for every step.
};

/**
 * @enum EchoOption
 * @brief Options for echoing input data to standard output and log files.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 */
enum class EchoOption {
  None,    ///< No echoing of input data.
  Summary, ///< Produce a summary of the input data.
  Full     ///< Full reproduction of the user input data.
};

/**
 * @struct HomogeneousDataPoint
 * @brief Data point for a homogeneous input field.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 */
struct HomogeneousDataPoint {
  DateTime time;              ///< Time of the data point.
  std::vector<double> values; ///< Values of the data point.
};

/**
 * @struct RunConfig
 * @brief Configuration for the run-time environment.
 * @details Stores the calendar type, output preferences, and input options.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 */
struct RunConfig {
  TimeManagement::CalendarType calendarType =
      TimeManagement::CalendarType::Standard; ///< Calendar type.
  bool produceStdOut = true;                  ///< Screen output flag.
  bool produceLogFile = true;                 ///< Log file flag.
  bool dryRun = false;                        ///< Dry run flag.
  bool propagateX = true;     ///< Propagation flag in x-direction.
  bool propagateY = true;     ///< Propagation flag in y-direction.
  bool propagateTheta = true; ///< Propagation flag in theta-direction.
  bool propagateK = true;     ///< Propagation flag in k-direction.
  bool sourceTerms = true;    ///< Source terms flag.

  InputFieldOption waterLevels =
      InputFieldOption::Undefined; ///< Water levels option.
  InputFieldOption currents = InputFieldOption::Undefined; ///< Currents option.
  InputFieldOption winds = InputFieldOption::Undefined;    ///< Winds option.
  InputFieldOption iceConcentrations =
      InputFieldOption::Undefined; ///< Ice concentrations option.
  InputFieldOption bottomDepth =
      InputFieldOption::FromGrid; ///< Bottom depth option.

  EchoOption echoHomInput = EchoOption::Summary; ///< Echo input flag.

  ScreenOutputLevel screenOutputLevel =
      ScreenOutputLevel::Full; ///< Screen output level.

  std::vector<HomogeneousDataPoint>
      homogeneousWaterLevels; ///< Homogeneous water levels data.
  std::vector<HomogeneousDataPoint>
      homogeneousCurrents; ///< Homogeneous currents data.
  std::vector<HomogeneousDataPoint>
      homogeneousWinds; ///< Homogeneous winds data.
  std::vector<HomogeneousDataPoint>
      homogeneousIceConcentrations; ///< Homogeneous ice concentrations data.
  std::vector<HomogeneousDataPoint>
      homogeneousBottomDepth; ///< Homogeneous bottom depth data.

  double timeStep = -1.0; ///< Model time step in seconds.

  OutputConfig outputApi; ///< API output configuration.

  OutputConfig outputFields;  ///< Gridded fields output configuration.
  OutputConfig outputPoints;  ///< Point output configuration.
  OutputConfig outputNesting; ///< Nesting data output configuration.
  OutputConfig outputTracks;  ///< Track output configuration.
  OutputConfig outputRestart; ///< Restart file output configuration.
};

/**
 * @brief Internal helper to trim whitespace and quotes from a string.
 * @param s The string view to clean.
 * @return A cleaned string view.
 */
std::string_view cleanValue(std::string_view s);

/**
 * @brief Loads the run-time configuration from a YAML file.
 * @details Reads the specified YAML file from the current directory,
 *          extracts the configuration settings, and updates the
 *          TimeManagement calendar type.
 * @param filename The name of the YAML file to load.
 * @return A RunConfig structure containing the loaded (or default) settings,
 *         or std::nullopt if the file could not be opened.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 */
std::optional<RunConfig> loadRunConfig(std::string_view filename,
                                       std::ostream &os) noexcept;

/**
 * @brief Reports the current configuration to the provided output stream.
 * @param config The RunConfig structure to report.
 * @param os The output stream to write to (default: std::cout).
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 */
void reportRunConfig(const RunConfig &config, std::ostream &os);

} // namespace ww4_utils

#endif // WW4_UTILS_WW4_RUN_CONFIG_HPP
