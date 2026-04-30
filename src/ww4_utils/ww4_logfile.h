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
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-01
 * @date Last update : 2026-04-30
 * @note Converted from WAVEWATCH III (ww3_shel.F90 and ww3_multi.F90).
 *       Original author: Hendrik L. Tolman.
 */

#pragma once

#include "ww4_utils/memory_utils.h"
#include "ww4_utils/time_management.h"
#include <iostream>
#include <optional>
#include <string>

/**
 * @namespace ww4_utils
 * @brief Utilities for WAVEWATCH IV.
 */
namespace ww4_utils {

/**
 * @namespace ww4_logfile
 * @brief Routines for log file output.
 */
namespace ww4_logfile {

/**
 * @struct LogTableData
 * @brief Data structure for tabular log output tracking.
 */
struct LogTableData {
  bool wlUpdated = false;        ///< Water level update flag.
  bool cuUpdated = false;        ///< Currents update flag.
  bool wiUpdated = false;        ///< Winds update flag.
  bool icUpdated = false;        ///< Ice concentrations update flag.
  bool bdUpdated = false;        ///< Bottom depth update flag.
  bool fieldsPerformed = false;  ///< Gridded fields output flag.
  bool pointsPerformed = false;  ///< Point output flag.
  bool nestingPerformed = false; ///< Nesting data output flag.
  bool tracksPerformed = false;  ///< Track output flag.
  bool restartPerformed = false; ///< Restart file output flag.
  bool apiPerformed = false;     ///< API output flag.

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

void writeInitialOutput(std::ostream &os, std::string_view programName);

void writeFinalOutput(std::ostream &os, std::string_view programName,
                      std::optional<double> initTime = std::nullopt,
                      std::optional<double> elapsedTotal = std::nullopt);

void writeUpdatingField(std::ostream &os, std::string_view fieldName);

void writeInterpolationInfo(std::ostream &os, const DateTime &time1,
                            const DateTime &time2);

void writeLogTableHeader(std::ostream &os);

void writeLogTableLine(std::ostream &os, const DateTime &time,
                       const LogTableData &data);

void writeLogTableFooter(std::ostream &os);

} // namespace ww4_logfile
} // namespace ww4_utils
