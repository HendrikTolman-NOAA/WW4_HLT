/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_input_utils.h
 * @brief Utility structures and routines for model input processing.
 * @details This header defines structures and routines for managing model
 *          input time data and orchestration of input updates.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-21
 * @date Last update : 2026-09-28
 */

#pragma once

#include "ww4_utils/time_management.h"
#include "ww4_utils/ww4_logfile.h"
#include "ww4_utils/ww4_run_config.h"
#include <iostream>
#include <optional>
#include <vector>

namespace ww4_utils {

// --- InputType --------------------------------------------------------------
/**
 * @enum InputType
 * @brief Types of model inputs for time management.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-21
 * @date Last update : 2026-09-28
 * @var InputType::WaterLevels
 * @brief Water levels input.
 * @var InputType::Currents
 * @brief Currents input.
 * @var InputType::Winds
 * @brief Winds input.
 * @var InputType::IceConcentrations
 * @brief Ice concentrations input.
 * @var InputType::BottomDepth
 * @brief Bottom depth input.
 */
enum class InputType {
  WaterLevels,
  Currents,
  Winds,
  IceConcentrations,
  BottomDepth,
};

// --- intTimeData ------------------------------------------------------------
/**
 * @struct intTimeData
 * @brief Structure to hold time tags for model inputs.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-21
 * @date Last update : 2026-09-28
 * @var intTimeData::time1
 * @brief First time tag.
 * @var intTimeData::time2
 * @brief Second time tag.
 * @var intTimeData::maxStep
 * @brief Maximum model time step.
 */
struct intTimeData {
  std::optional<DateTime> time1;
  std::optional<DateTime> time2;
  double maxStep = -1.0;
};

// --- waveTimeData -----------------------------------------------------------
/**
 * @struct waveTimeData
 * @brief Structure to hold model time and time step information.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-21
 * @date Last update : 2026-09-28
 * @var waveTimeData::timeStep
 * @brief Model time step.
 * @var waveTimeData::modelTime
 * @brief Current model time.
 * @var waveTimeData::waterLevels
 * @brief Time data for water levels.
 * @var waveTimeData::currents
 * @brief Time data for currents.
 * @var waveTimeData::winds
 * @brief Time data for winds.
 * @var waveTimeData::iceConcentrations
 * @brief Time data for ice concentrations.
 * @var waveTimeData::bottomDepth
 * @brief Time data for bottom depth.
 */
struct waveTimeData {
  double timeStep = -1.0;
  std::optional<DateTime> modelTime;
  intTimeData waterLevels;
  intTimeData currents;
  intTimeData winds;
  intTimeData iceConcentrations;
  intTimeData bottomDepth;
};

// --- InputUpdateState -------------------------------------------------------
/**
 * @struct InputUpdateState
 * @brief Tracking state for input interpolation reporting.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-21
 * @date Last update : 2026-09-28
 * @var InputUpdateState::lastWlTime1
 * @brief Last reported time1 for water levels.
 * @var InputUpdateState::lastWlTime2
 * @brief Last reported time2 for water levels.
 * @var InputUpdateState::lastCuTime1
 * @brief Last reported time1 for currents.
 * @var InputUpdateState::lastCuTime2
 * @brief Last reported time2 for currents.
 * @var InputUpdateState::lastWiTime1
 * @brief Last reported time1 for winds.
 * @var InputUpdateState::lastWiTime2
 * @brief Last reported time2 for winds.
 * @var InputUpdateState::lastIcTime1
 * @brief Last reported time1 for ice concentrations.
 * @var InputUpdateState::lastIcTime2
 * @brief Last reported time2 for ice concentrations.
 * @var InputUpdateState::lastBdTime1
 * @brief Last reported time1 for bottom depth.
 * @var InputUpdateState::lastBdTime2
 * @brief Last reported time2 for bottom depth.
 */
struct InputUpdateState {
  std::optional<DateTime> lastWlTime1, lastWlTime2;
  std::optional<DateTime> lastCuTime1, lastCuTime2;
  std::optional<DateTime> lastWiTime1, lastWiTime2;
  std::optional<DateTime> lastIcTime1, lastIcTime2;
  std::optional<DateTime> lastBdTime1, lastBdTime2;
};

// --- ww4_input_update -------------------------------------------------------
/**
 * @brief Processes and validates input data for the model.
 * @param config The run configuration.
 * @param os Output stream for reporting.
 */
void ww4_input_update(const RunConfig &config, std::ostream &os);

// --- resetInputData ---------------------------------------------------------
/**
 * @brief Resets all internal input data storage.
 */
void resetInputData() noexcept;

// --- getHomogeneousWaterLevels ----------------------------------------------
/**
 * @brief Accessor for processed homogeneous water levels.
 * @return Reference to the vector of data points.
 */
const std::vector<HomogeneousDataPoint> &getHomogeneousWaterLevels() noexcept;

// --- getHomogeneousCurrents -------------------------------------------------
/**
 * @brief Accessor for processed homogeneous currents.
 * @return Reference to the vector of data points.
 */
const std::vector<HomogeneousDataPoint> &getHomogeneousCurrents() noexcept;

// --- getHomogeneousWinds ----------------------------------------------------
/**
 * @brief Accessor for processed homogeneous winds.
 * @return Reference to the vector of data points.
 */
const std::vector<HomogeneousDataPoint> &getHomogeneousWinds() noexcept;

// --- getHomogeneousIceConcentrations ----------------------------------------
/**
 * @brief Accessor for processed homogeneous ice concentrations.
 * @return Reference to the vector of data points.
 */
const std::vector<HomogeneousDataPoint> &
getHomogeneousIceConcentrations() noexcept;

// --- getHomogeneousBottomDepth ----------------------------------------------
/**
 * @brief Accessor for processed homogeneous bottom depth.
 * @return Reference to the vector of data points.
 */
const std::vector<HomogeneousDataPoint> &getHomogeneousBottomDepth() noexcept;

// --- ww4_hom_water_levels ---------------------------------------------------
/**
 * @brief Cycle through homogeneous water levels to find interpolation
 * interval.
 * @param modelTime Current model time.
 * @param endTime Simulation end time for capping max step.
 * @param data Output structure to store interpolation interval and max step.
 */
void ww4_hom_water_levels(const DateTime &modelTime, const DateTime &endTime,
                          intTimeData &data);

// --- ww4_hom_currents -------------------------------------------------------
/**
 * @brief Cycle through homogeneous currents to find interpolation interval.
 * @param modelTime Current model time.
 * @param endTime Simulation end time for capping max step.
 * @param data Output structure to store interpolation interval and max step.
 */
void ww4_hom_currents(const DateTime &modelTime, const DateTime &endTime,
                      intTimeData &data);

// --- ww4_hom_winds ----------------------------------------------------------
/**
 * @brief Cycle through homogeneous winds to find interpolation interval.
 * @param modelTime Current model time.
 * @param endTime Simulation end time for capping max step.
 * @param data Output structure to store interpolation interval and max step.
 */
void ww4_hom_winds(const DateTime &modelTime, const DateTime &endTime,
                   intTimeData &data);

// --- ww4_hom_ice ------------------------------------------------------------
/**
 * @brief Cycle through homogeneous ice concentrations to find interpolation
 * interval.
 * @param modelTime Current model time.
 * @param endTime Simulation end time for capping max step.
 * @param data Output structure to store interpolation interval and max step.
 */
void ww4_hom_ice(const DateTime &modelTime, const DateTime &endTime,
                 intTimeData &data);

// --- ww4_hom_bottom_depth ---------------------------------------------------
/**
 * @brief Cycle through homogeneous bottom depth to find interpolation
 * interval.
 * @param modelTime Current model time.
 * @param endTime Simulation end time for capping max step.
 * @param data Output structure to store interpolation interval and max step.
 */
void ww4_hom_bottom_depth(const DateTime &modelTime, const DateTime &endTime,
                          intTimeData &data);

// --- updateAllInputs --------------------------------------------------------
/**
 * @brief Processes all input fields and calculates next time step.
 * @param[in] modelTime Current model time.
 * @param[in] endTime Simulation end time.
 * @param[in,out] waveTime Global wave time data to update.
 * @param[in,out] state Persistent state for reporting interpolation intervals.
 * @param[in] config The run configuration.
 * @param[in,out] headerPrinted Flag to track if step header was printed.
 * @param[in] os Output stream for reporting.
 * @param[in,out] logData Data for tabular log output.
 * @return Time step (seconds) from present model time to next update.
 */
double updateAllInputs(const DateTime &modelTime, const DateTime &endTime,
                       waveTimeData &waveTime, InputUpdateState &state,
                       const RunConfig &config, bool &headerPrinted,
                       std::ostream &os, ww4_logfile::LogTableData &logData);

// --- computeInputTimeStep ---------------------------------------------------
/**
 * @brief Computes the minimum input time step based on active fields.
 * @param modelTime Current model time.
 * @param endTime Simulation end time.
 * @param waveTime Global wave time data.
 * @param config The run configuration.
 * @return Minimum required time step in seconds.
 */
double computeInputTimeStep(const DateTime &modelTime, const DateTime &endTime,
                            const waveTimeData &waveTime,
                            const RunConfig &config);

} // namespace ww4_utils
