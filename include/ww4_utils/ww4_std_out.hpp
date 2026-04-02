/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_std_out.hpp
 * @brief Routines for standard screen output.
 * @details This header defines routines for managing screen output to std_out,
 *          duplicating the formats from WAVEWATCH III.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Aldgisl (AI Persona) (Initial, 2026-04-01)
 * @author Hendrik L. Tolman, Jules (Agentic AI) (Last Update, 2026-04-01)
 * @date Initial, 2026-04-01
 * @date Last Update, 2026-04-01
 * @note Converted from WAVEWATCH III (ww3_shel.F90 and ww3_multi.F90).
 *       Original author: Hendrik L. Tolman.
 */

#ifndef WW4_UTILS_WW4_STD_OUT_HPP
#define WW4_UTILS_WW4_STD_OUT_HPP

#include <iostream>
#include <string>

/**
 * @namespace ww4_utils
 * @brief Utilities for WAVEWATCH IV.
 */
namespace ww4_utils {

/**
 * @namespace ww4_std_out
 * @brief Routines for standard screen output.
 */
namespace ww4_std_out {

/**
 * @brief Writes the initial banner to the provided output stream.
 * @details Duplicates the initial Fortran FORMAT statements from WW3.
 *          Identified as format 900 in ww3_shel.F90 and ww3_multi.F90.
 * @param os The output stream to write to (e.g., std::cout).
 * @param programName The name of the executable program to identify in the
 *        banner (e.g., "Program shell" or "Multi-grid shell").
 */
void writeInitialOutput(std::ostream &os, const std::string &programName);

/**
 * @brief Writes the final footer to the provided output stream.
 * @details Duplicates the final Fortran FORMAT statements from WW3.
 *          Identified as format 999 in ww3_shel.F90 and ww3_multi.F90.
 * @param os The output stream to write to (e.g., std::cout).
 * @param programName The name of the executable program to identify in the
 *        footer.
 */
void writeFinalOutput(std::ostream &os, const std::string &programName);

} // namespace ww4_std_out
} // namespace ww4_utils

#endif // WW4_UTILS_WW4_STD_OUT_HPP
