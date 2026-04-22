/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_output_utils.hpp
 * @brief Utility structures and routines for model output processing.
 * @details This header defines the OutputConfig structure and routines for
 *          managing model output timing and orchestration.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-21
 */

#ifndef WW4_UTILS_WW4_OUTPUT_UTILS_HPP
#define WW4_UTILS_WW4_OUTPUT_UTILS_HPP

#include "ww4_utils/time_management.hpp"
#include <optional>

namespace ww4_utils {

struct RunConfig;

/**
 * @struct OutputConfig
 * @brief Configuration for a specific output type.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 */
struct OutputConfig {
  bool requested = false;             ///< Is this output requested?
  std::optional<DateTime> startTime;  ///< Start time for output.
  std::optional<DateTime> endTime;    ///< End time for output.
  double interval = -1.0;             ///< Output interval in seconds.
  bool atFirstTime = true;            ///< Output at first time?
  std::optional<DateTime> actualTime; ///< Actual next output time.
};

/**
 * @brief Assesses and updates the output configuration.
 * @details Initializes startTime and actualTime if not set, handles API output
 *          timing, and ensures actualTime is synced with modelTime.
 * @param[in,out] oc The output configuration to update.
 * @param[in] modelTime The current model time.
 * @param[in] isApi Flag indicating if this is API output.
 * @param[in] waveEndTime The end time of the current w4core_wave call.
 * @date 2026-04-21
 */
void assessOutputConfig(
    OutputConfig &oc, const DateTime &modelTime, bool isApi,
    const std::optional<DateTime> &waveEndTime = std::nullopt);

/**
 * @brief Computes the next output time step.
 * @details Calculates the number of seconds until the next output is due.
 * @param[in] oc The output configuration.
 * @param[in] modelTime The current model time.
 * @return The time in seconds until the next output, or a large value if no
 * output is scheduled.
 * @date 2026-04-21
 */
double computeNextOutputStep(const OutputConfig &oc, const DateTime &modelTime);

/**
 * @brief Checks if output is due at the present model time.
 * @param[in] oc The output configuration.
 * @param[in] modelTime The current model time.
 * @return true if output should be performed now.
 * @date 2026-04-21
 */
bool isOutputDue(const OutputConfig &oc, const DateTime &modelTime);

/**
 * @brief Updates the actual output time after execution.
 * @details Increments actualTime by the interval if it matches modelTime.
 * @param[in,out] oc The output configuration to update.
 * @param[in] modelTime The current model time.
 * @date 2026-04-21
 */
void updateOutputTime(OutputConfig &oc, const DateTime &modelTime);

/**
 * @brief Computes the minimum next output time step across all output types.
 * @param[in] config The run configuration containing all output settings.
 * @param[in] modelTime The current model time.
 * @param[in] endTime The simulation end time.
 * @return The minimum time in seconds until the next output is due.
 * @date 2026-04-21
 */
double computeMinOutputStep(const RunConfig &config, const OutputConfig &apiConfig,
                            const DateTime &modelTime, const DateTime &endTime);

} // namespace ww4_utils

#endif // WW4_UTILS_WW4_OUTPUT_UTILS_HPP
