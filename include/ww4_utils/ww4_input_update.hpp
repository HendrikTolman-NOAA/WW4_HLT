/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_input_update.hpp
 * @brief Input data processing for WAVEWATCH IV.
 * @details This header defines the structures and routines for processing
 *          input data (initially homogeneous) in WW4.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-21
 */

#ifndef WW4_UTILS_WW4_INPUT_UPDATE_HPP
#define WW4_UTILS_WW4_INPUT_UPDATE_HPP

#include "ww4_utils/time_management.hpp"
#include "ww4_utils/ww4_input_utils.hpp"
#include "ww4_utils/ww4_run_config.hpp"
#include <iostream>
#include <vector>

namespace ww4_utils {

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

} // namespace ww4_utils

#endif // WW4_UTILS_WW4_INPUT_UPDATE_HPP
