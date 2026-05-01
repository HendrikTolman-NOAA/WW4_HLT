/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_std_out.cpp
 * @brief Implementation of standard screen output routines.
 * @details This file implements the routines for managing screen output,
 *          matching the formats from WAVEWATCH III, updated for WAVEWATCH IV.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-01
 * @date Last update : 2026-05-01
 * @note Converted from WAVEWATCH III (ww3_shel.F90, ww3_multi.F90, and
 *       w3servmd.F90).
 *       Original author: Hendrik L. Tolman.
 */

#include "ww4_utils/ww4_std_out.h"
#include <cstdlib>
#include <iomanip>

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
 *        banner.
 */
void writeInitialOutput(std::ostream &os, std::string_view programName) {
  const std::string mid =
      "*** WAVEWATCH IV program " + std::string(programName) + " ***";

  // Generic banner format with 15X (15 spaces) and / (newline)
  const std::string bannerLine = "      " + mid + "      ";
  const std::string separator(bannerLine.length(), '=');

  os << "\n"
     << "               " << bannerLine << "\n"
     << "               " << separator << "\n"
     << std::endl;
}

/**
 * @brief Writes the final footer to the provided output stream.
 * @details Duplicates the final Fortran FORMAT statements from WW3.
 *          Identified as format 999 in ww3_shel.F90 and ww3_multi.F90.
 *          Optionally includes execution times and memory usage.
 * @param os The output stream to write to (e.g., std::cout).
 * @param programName The name of the executable program to identify in the
 *        footer.
 * @param initTime Optional initialization time in seconds.
 * @param elapsedTotal Optional total elapsed time in seconds.
 */
void writeFinalOutput(std::ostream &os, std::string_view programName,
                      std::optional<double> initTime,
                      std::optional<double> elapsedTotal) {
  // Matches FORMAT 997, 998 in log but adapted for modularity
  if (initTime.has_value()) {
    os << "\n  Initialization time :" << std::fixed << std::setprecision(2)
       << std::setw(10) << *initTime << " s\n";
  }
  if (elapsedTotal.has_value()) {
    os << "  Elapsed time " << std::fixed << std::setprecision(2)
       << std::setw(10) << *elapsedTotal << " s\n";
  }

  os << "\n  End of program \n"
     << " ==================================================\n"
     << "          WAVEWATCH IV program " << programName << " \n"
     << std::endl;
}

/**
 * @brief Writes an error message to the provided output stream in the standard
 *        WAVEWATCH format.
 * @details Heritage from EXTCDE in WAVEWATCH III w3servmd.F90.
 * @param os The output stream to write to.
 * @param msg Optional error message to report.
 * @param file Optional source file name where the error occurred.
 * @param line Optional line number in the source file.
 */
void writeExtcdeOutput(std::ostream &os, std::optional<std::string_view> msg,
                       std::optional<std::string_view> file,
                       std::optional<int> line) {
  const std::string prefix = "WW4 ERROR:";

  if (msg.has_value()) {
    os << prefix << " " << *msg << "\n";
  }

  std::string context;
  if (file.has_value()) {
    context += " FILE=";
    context += *file;
  }
  if (line.has_value()) {
    context += " LINE=";
    context += std::to_string(*line);
  }

  if (!context.empty()) {
    os << prefix << context << "\n";
  }
  os << std::flush;
}

/**
 * @brief Writes a warning message to the provided output stream in the standard
 *        WAVEWATCH format.
 * @details Heritage from WARNNG in WAVEWATCH III w3servmd.F90.
 * @param os The output stream to write to.
 * @param msg The warning message to report.
 * @param file Optional source file name where the warning occurred.
 * @param line Optional line number in the source file.
 */
void writeWarnngOutput(std::ostream &os, std::string_view msg,
                       std::optional<std::string_view> file,
                       std::optional<int> line) {
  const std::string prefix = "WW4 WARNING:";

  os << prefix << " " << msg << "\n";

  std::string context;
  if (file.has_value()) {
    context += " FILE=";
    context += *file;
  }
  if (line.has_value()) {
    context += " LINE=";
    context += std::to_string(*line);
  }

  if (!context.empty()) {
    os << prefix << context << "\n";
  }
  os << std::flush;
}

/**
 * @brief Performs a program stop with an exit code.
 * @details Heritage from EXTCDE in WAVEWATCH III w3servmd.F90.
 *          Calls writeExtcdeOutput and then std::exit.
 * @param exitCode The exit code to return to the environment.
 * @param os The output stream to write to (defaults to std::cerr).
 * @param msg Optional error message to report.
 * @param file Optional source file name where the error occurred.
 * @param line Optional line number in the source file.
 */
[[noreturn]] void extcde(int exitCode, std::ostream &os,
                         std::optional<std::string_view> msg,
                         std::optional<std::string_view> file,
                         std::optional<int> line) {
  writeExtcdeOutput(os, msg, file, line);
  std::exit(exitCode);
}

/**
 * @brief Reports a warning and continues execution.
 * @details Heritage from WARNNG in WAVEWATCH III w3servmd.F90.
 *          Calls writeWarnngOutput.
 * @param os The output stream to write to (defaults to std::cout).
 * @param msg The warning message to report.
 * @param file Optional source file name where the warning occurred.
 * @param line Optional line number in the source file.
 */
void warnng(std::ostream &os, std::string_view msg,
            std::optional<std::string_view> file, std::optional<int> line) {
  writeWarnngOutput(os, msg, file, line);
}

/**
 * @brief Writes a message identifying that an input field is being updated.
 * @param os The output stream to write to.
 * @param fieldName The name of the field being updated.
 */
void writeUpdatingField(std::ostream &os, std::string_view fieldName) {
  os << "    Updating " << fieldName << std::endl;
}

/**
 * @brief Writes interpolation interval information for an input field.
 * @param os The output stream to write to.
 * @param time1 First interpolation time tag.
 * @param time2 Second interpolation time tag.
 */
void writeInterpolationInfo(std::ostream &os, const DateTime &time1,
                            const DateTime &time2) {
  os << "      Interpolation from " << TimeManagement::toFormattedString(time1)
     << " to " << TimeManagement::toFormattedString(time2) << std::endl;
}

} // namespace ww4_std_out
} // namespace ww4_utils
