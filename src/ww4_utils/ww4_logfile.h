/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_logfile.h
 * @brief Routines for log file output.
 * @details This header defines routines for managing log file output,
 *          duplicating the formats from WAVEWATCH III, updated for
 *          WAVEWATCH IV.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-01
 * @date Last update : 2026-09-28
 * @note Converted from WAVEWATCH III (ww3_shel.F90 and ww3_multi.F90).
 *       Original author: Hendrik L. Tolman.
 */

#pragma once

#include "ww4_utils/memory_utils.h"
#include "ww4_utils/time_management.h"
#include <iostream>
#include <optional>
#include <string>

namespace ww4_utils {

namespace ww4_logfile {

// --- LogTableData -----------------------------------------------------------
/**
 * @struct LogTableData
 * @brief Data structure for tabular log output tracking.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-01
 * @date Last update : 2026-09-28
 * @var LogTableData::wlUpdated
 * @brief Water level update flag.
 * @var LogTableData::cuUpdated
 * @brief Currents update flag.
 * @var LogTableData::wiUpdated
 * @brief Winds update flag.
 * @var LogTableData::icUpdated
 * @brief Ice concentrations update flag.
 * @var LogTableData::bdUpdated
 * @brief Bottom depth update flag.
 * @var LogTableData::fieldsPerformed
 * @brief Gridded fields output flag.
 * @var LogTableData::pointsPerformed
 * @brief Point output flag.
 * @var LogTableData::restartPerformed
 * @brief Restart file output flag.
 * @var LogTableData::apiPerformed
 * @brief API output flag.
 */
struct LogTableData {
  bool wlUpdated = false;
  bool cuUpdated = false;
  bool wiUpdated = false;
  bool icUpdated = false;
  bool bdUpdated = false;
  bool fieldsPerformed = false;
  bool pointsPerformed = false;
  bool restartPerformed = false;
  bool apiPerformed = false;

  /**
   * @brief Checks if any action (input update or output) occurred.
   * @return True if any flag is set.
   */
  bool anyAction() const;

  /**
   * @brief Resets all flags to false.
   */
  void reset();
};

// --- writeInitialOutput -----------------------------------------------------
/**
 * @brief Writes the initial log entry to the provided output stream.
 * @param os Output stream to write to.
 * @param programName Name of the executable program.
 */
void writeInitialOutput(std::ostream &os, std::string_view programName);

// --- writeFinalOutput -------------------------------------------------------
/**
 * @brief Writes the final log entry to the provided output stream.
 * @param os Output stream to write to.
 * @param programName Name of the executable program.
 * @param initTime Optional initialization time in seconds.
 * @param elapsedTotal Optional total elapsed time in seconds.
 */
void writeFinalOutput(std::ostream &os, std::string_view programName,
                      std::optional<double> initTime = std::nullopt,
                      std::optional<double> elapsedTotal = std::nullopt);

// --- writeUpdatingField -----------------------------------------------------
/**
 * @brief Writes a message identifying that an input field is being updated.
 * @param os Output stream to write to.
 * @param fieldName Name of the field being updated.
 */
void writeUpdatingField(std::ostream &os, std::string_view fieldName);

// --- writeInterpolationInfo -------------------------------------------------
/**
 * @brief Writes interpolation interval information for an input field.
 * @param os Output stream to write to.
 * @param time1 First interpolation time tag.
 * @param time2 Second interpolation time tag.
 */
void writeInterpolationInfo(std::ostream &os, const DateTime &time1,
                            const DateTime &time2);

// --- writeLogTableHeader ----------------------------------------------------
/**
 * @brief Writes the header of the tabular log output.
 * @param os Output stream to write to.
 */
void writeLogTableHeader(std::ostream &os);

// --- writeLogTableLine ------------------------------------------------------
/**
 * @brief Adds a data line to the tabular log output.
 * @param os Output stream to write to.
 * @param time Time stamp for the end of the interval.
 * @param data Data flags for the line.
 */
void writeLogTableLine(std::ostream &os, const DateTime &time,
                       const LogTableData &data);

// --- writeLogTableFooter ----------------------------------------------------
/**
 * @brief Writes the footer of the tabular log output.
 * @param os Output stream to write to.
 */
void writeLogTableFooter(std::ostream &os);

} // namespace ww4_logfile
} // namespace ww4_utils
