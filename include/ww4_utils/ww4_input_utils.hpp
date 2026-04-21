/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_input_utils.hpp
 * @brief Utility structures and routines for model input processing.
 * @details This header defines structures and routines for managing model
 *          input time data and orchestration of input updates.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-21
 */

#ifndef WW4_UTILS_WW4_INPUT_UTILS_HPP
#define WW4_UTILS_WW4_INPUT_UTILS_HPP

#include "ww4_utils/time_management.hpp"
#include "ww4_utils/ww4_run_config.hpp"
#include <iostream>
#include <optional>
#include <vector>

/**
 * @namespace ww4_utils
 * @brief Utilities for WAVEWATCH IV.
 */
namespace ww4_utils {

/**
 * @enum InputType
 * @brief Types of model inputs for time management.
 */
enum class InputType {
  WaterLevels,       ///< Water levels input.
  Currents,          ///< Currents input.
  Winds,             ///< Winds input.
  IceConcentrations, ///< Ice concentrations input.
  BottomDepth,       ///< Bottom depth input.
};

/**
 * @struct intTimeData
 * @brief Structure to hold time tags for model inputs.
 */
struct intTimeData {
  std::optional<DateTime> time1; ///< First time tag.
  std::optional<DateTime> time2; ///< Second time tag.
  double maxStep = -1.0;         ///< Maximum model time step.
};

/**
 * @struct waveTimeData
 * @brief Structure to hold model time and time step information.
 */
struct waveTimeData {
  double timeStep = -1.0;            ///< Model time step.
  std::optional<DateTime> modelTime; ///< Current model time.
  intTimeData waterLevels;           ///< Time data for water levels.
  intTimeData currents;              ///< Time data for currents.
  intTimeData winds;                 ///< Time data for winds.
  intTimeData iceConcentrations;     ///< Time data for ice concentrations.
  intTimeData bottomDepth;           ///< Time data for bottom depth.
};

/**
 * @struct InputUpdateState
 * @brief Tracking state for input interpolation reporting.
 */
struct InputUpdateState {
  std::optional<DateTime> lastWlTime1, lastWlTime2;
  std::optional<DateTime> lastCuTime1, lastCuTime2;
  std::optional<DateTime> lastWiTime1, lastWiTime2;
  std::optional<DateTime> lastIcTime1, lastIcTime2;
  std::optional<DateTime> lastBdTime1, lastBdTime2;
};

/**
 * @brief Processes and validates input data for the model.
 * @details Parses input data from the run configuration and
 *          validates its availability and temporal consistency.
 * @param config The run configuration.
 * @param os Output stream for reporting.
 * @date 2026-04-21
 */
void ww4_input_update(const RunConfig &config, std::ostream &os);

/**
 * @brief Resets all internal input data storage.
 * @details Clears vectors containing processed homogeneous data.
 */
void resetInputData() noexcept;

/**
 * @brief Accessor for processed homogeneous water levels.
 * @return Reference to the vector of data points.
 */
const std::vector<HomogeneousDataPoint> &getHomogeneousWaterLevels() noexcept;

/**
 * @brief Accessor for processed homogeneous currents.
 * @return Reference to the vector of data points.
 */
const std::vector<HomogeneousDataPoint> &getHomogeneousCurrents() noexcept;

/**
 * @brief Accessor for processed homogeneous winds.
 * @return Reference to the vector of data points.
 */
const std::vector<HomogeneousDataPoint> &getHomogeneousWinds() noexcept;

/**
 * @brief Accessor for processed homogeneous ice concentrations.
 * @return Reference to the vector of data points.
 */
const std::vector<HomogeneousDataPoint> &
getHomogeneousIceConcentrations() noexcept;

/**
 * @brief Accessor for processed homogeneous bottom depth.
 * @return Reference to the vector of data points.
 */
const std::vector<HomogeneousDataPoint> &getHomogeneousBottomDepth() noexcept;

/**
 * @brief Cycle through homogeneous water levels to find interpolation interval.
 * @param modelTime Current model time.
 * @param endTime Simulation end time for capping max step.
 * @param data Output structure to store interpolation interval and max step.
 */
void ww4_hom_water_levels(const DateTime &modelTime, const DateTime &endTime,
                          intTimeData &data);

/**
 * @brief Cycle through homogeneous currents to find interpolation interval.
 * @param modelTime Current model time.
 * @param endTime Simulation end time for capping max step.
 * @param data Output structure to store interpolation interval and max step.
 */
void ww4_hom_currents(const DateTime &modelTime, const DateTime &endTime,
                      intTimeData &data);

/**
 * @brief Cycle through homogeneous winds to find interpolation interval.
 * @param modelTime Current model time.
 * @param endTime Simulation end time for capping max step.
 * @param data Output structure to store interpolation interval and max step.
 */
void ww4_hom_winds(const DateTime &modelTime, const DateTime &endTime,
                   intTimeData &data);

/**
 * @brief Cycle through homogeneous ice concentrations to find interpolation
 * interval.
 * @param modelTime Current model time.
 * @param endTime Simulation end time for capping max step.
 * @param data Output structure to store interpolation interval and max step.
 */
void ww4_hom_ice(const DateTime &modelTime, const DateTime &endTime,
                 intTimeData &data);

/**
 * @brief Cycle through homogeneous bottom depth to find interpolation interval.
 * @param modelTime Current model time.
 * @param endTime Simulation end time for capping max step.
 * @param data Output structure to store interpolation interval and max step.
 */
void ww4_hom_bottom_depth(const DateTime &modelTime, const DateTime &endTime,
                          intTimeData &data);

/**
 * @brief Processes all input fields and calculates next time step.
 * @details Orchestrates the update of all active input fields and returns
 *          the time interval to the next required update.
 * @param[in] modelTime Current model time.
 * @param[in] endTime Simulation end time.
 * @param[in,out] waveTime Global wave time data to update.
 * @param[in,out] state Persistent state for reporting interpolation intervals.
 * @param[in] config The run configuration.
 * @param[in] os Output stream for reporting.
 * @param[in] logStream Output stream for log file.
 * @return The time step (seconds) from the present model time to the next
 * update.
 */
double updateAllInputs(const DateTime &modelTime, const DateTime &endTime,
                       waveTimeData &waveTime, InputUpdateState &state,
                       const RunConfig &config, std::ostream &os,
                       std::ostream &logStream);

/**
 * @brief Computes the minimum input time step based on active fields.
 * @param modelTime Current model time.
 * @param endTime Simulation end time.
 * @param waveTime Global wave time data.
 * @param config The run configuration.
 * @return The minimum required time step (seconds).
 */
double computeInputTimeStep(const DateTime &modelTime, const DateTime &endTime,
                            const waveTimeData &waveTime,
                            const RunConfig &config);

} // namespace ww4_utils

#endif // WW4_UTILS_WW4_INPUT_UTILS_HPP
