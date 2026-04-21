/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file w4core_hom_input.hpp
 * @brief Homogeneous input data processing for the WAVEWATCH IV core.
 * @details This header defines the structures and routines for processing
 *          homogeneous input data in WW4.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-17
 * @date Last update, 2026-04-21
 */

#ifndef WW4_CORE_W4CORE_HOM_INPUT_HPP
#define WW4_CORE_W4CORE_HOM_INPUT_HPP

#include "ww4_utils/time_management.hpp"
#include "ww4_utils/ww4_run_config.hpp"
#include <iostream>
#include <vector>

namespace ww4_core {

/**
 * @brief Processes and validates homogeneous input data for the model.
 * @details Parses homogeneous input data from the run configuration,
 *          validates its availability and temporal consistency,
 *          and echoes it according to user preference.
 * @param os Output stream for reporting and echoing.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-17
 * @date Last update, 2026-04-21
 */
void w4core_hom_input(std::ostream &os);

/**
 * @brief Resets all internal input data storage.
 * @details Clears vectors containing processed homogeneous data.
 */
void resetInputData() noexcept;

/**
 * @brief Accessor for processed homogeneous water levels.
 * @return Reference to the vector of data points.
 */
const std::vector<ww4_utils::HomogeneousDataPoint> &
getHomogeneousWaterLevels() noexcept;

/**
 * @brief Accessor for processed homogeneous currents.
 * @return Reference to the vector of data points.
 */
const std::vector<ww4_utils::HomogeneousDataPoint> &
getHomogeneousCurrents() noexcept;

/**
 * @brief Accessor for processed homogeneous winds.
 * @return Reference to the vector of data points.
 */
const std::vector<ww4_utils::HomogeneousDataPoint> &
getHomogeneousWinds() noexcept;

/**
 * @brief Accessor for processed homogeneous ice concentrations.
 * @return Reference to the vector of data points.
 */
const std::vector<ww4_utils::HomogeneousDataPoint> &
getHomogeneousIceConcentrations() noexcept;

/**
 * @brief Accessor for processed homogeneous bottom depth.
 * @return Reference to the vector of data points.
 */
const std::vector<ww4_utils::HomogeneousDataPoint> &
getHomogeneousBottomDepth() noexcept;

/**
 * @brief Cycle through homogeneous water levels to find interpolation interval.
 * @param endTime Simulation end time for capping max step.
 */
void w4core_hom_water_levels(const ww4_utils::DateTime &endTime);

/**
 * @brief Cycle through homogeneous currents to find interpolation interval.
 * @param endTime Simulation end time for capping max step.
 */
void w4core_hom_currents(const ww4_utils::DateTime &endTime);

/**
 * @brief Cycle through homogeneous winds to find interpolation interval.
 * @param endTime Simulation end time for capping max step.
 */
void w4core_hom_winds(const ww4_utils::DateTime &endTime);

/**
 * @brief Cycle through homogeneous ice concentrations to find interpolation
 * interval.
 * @param endTime Simulation end time for capping max step.
 */
void w4core_hom_ice(const ww4_utils::DateTime &endTime);

} // namespace ww4_core

#endif // WW4_CORE_W4CORE_HOM_INPUT_HPP
