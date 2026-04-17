/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file w4core_input.hpp
 * @brief Input data processing for the WAVEWATCH IV core.
 * @details This header defines the structures and routines for processing
 *          homogeneous and other input data in WW4.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-16
 */

#ifndef WW4_CORE_W4CORE_INPUT_HPP
#define WW4_CORE_W4CORE_INPUT_HPP

#include "ww4_utils/time_management.hpp"
#include <iostream>
#include <vector>

namespace ww4_core {

/**
 * @struct HomogeneousDataPoint
 * @brief Data point for a homogeneous input field.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 */
struct HomogeneousDataPoint {
  ww4_utils::DateTime time;  ///< Time of the data point.
  std::vector<double> values; ///< Values of the data point.
};

/**
 * @brief Processes and validates all input data for the model.
 * @details Parses homogeneous input data from the run configuration,
 *          validates its availability and temporal consistency,
 *          and echoes it according to user preference.
 * @param os Output stream for reporting and echoing.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-16
 */
void w4core_input(std::ostream &os);

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
const std::vector<HomogeneousDataPoint> &getHomogeneousIceConcentrations() noexcept;

/**
 * @brief Accessor for processed homogeneous bottom depth.
 * @return Reference to the vector of data points.
 */
const std::vector<HomogeneousDataPoint> &getHomogeneousBottomDepth() noexcept;

} // namespace ww4_core

#endif // WW4_CORE_W4CORE_INPUT_HPP
