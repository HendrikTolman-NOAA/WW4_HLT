/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file utils_UQ_IO.h
 * @brief Header for UQ regular grid reading and partition I/O utilities.
 * @details Provides data structures and routines to read UQ regular grid files,
 *          capture grid header metadata, write metadata reports to stdout and
 *          log file, and support eventual reading and writing of partition
 * maps.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the
 * National Weather Service. NWS often uses Generative AI (GenAI) for code
 * development and refactoring. Whenever GenAI is used, NWS requires a full
 * human review of code before it is added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-10-26
 * @date Last update : 2026-10-26
 */

#pragma once

#include "ww4_utils/spatial_grid.h"
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

namespace ww4_core::solver_UQ_utils {

// --- UQGridHeader -----------------------------------------------------------
/**
 * @struct UQGridHeader
 * @brief Header metadata structure for UQ regular spatial grids.
 * @details Stores regular grid dimensions, domain bounds, increments,
 * coordinate projection, and closure status extracted from grid file.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-10-26
 * @date Last update : 2026-10-26
 */
struct UQGridHeader {
  std::string gridName;
  std::string gridType = "regular";
  std::size_t nx = 0;
  std::size_t ny = 0;
  double lonStart = 0.0;
  double latStart = 0.0;
  double lonEnd = 0.0;
  double latEnd = 0.0;
  double sx = 0.0;
  double sy = 0.0;
  double poleLon = 0.0;
  double poleLat = 90.0;
  bool isSpherical = true;
  bool isGlobal = false;
};

// --- readGridHeaderUQ -------------------------------------------------------
/**
 * @brief Reads UQ regular grid header metadata from a grid file.
 * @param[in] filename Path to the grid file.
 * @param[out] header Output header metadata structure.
 * @param[in,out] os Output stream for logging and error reporting.
 * @return True if header was successfully read, false otherwise.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-10-26
 * @date Last update : 2026-10-26
 */
std::string readGridHeaderUQ(const std::string &filename, UQGridHeader &header,
                             std::ostream &os = std::cout);

// --- readGridFileUQ ---------------------------------------------------------
/**
 * @brief Reads UQ regular grid dataset including header metadata and physical
 * fields.
 * @param[in] filename Path to the grid file.
 * @param[out] grid Output FullGrid structure populated with grid geometry and
 * fields.
 * @param[in,out] os Output stream for logging and error reporting.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-10-26
 * @date Last update : 2026-10-26
 */
void readGridFileUQ(const std::string &filename, ww4_utils::FullGrid &grid,
                    std::ostream &os = std::cout);

// --- reportUQGridMetadata ---------------------------------------------------
/**
 * @brief Writes extracted UQ grid metadata to stdout and log file.
 * @param[in] header UQ grid header metadata structure.
 * @param[in,out] os Output stream for writing metadata report.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-10-26
 * @date Last update : 2026-10-26
 */
void reportUQGridMetadata(const UQGridHeader &header,
                          std::ostream &os = std::cout);

// --- readPartitionMapNetCDF -------------------------------------------------
/**
 * @brief Reads partition mapping information from a NetCDF partition file.
 * @param[in] filename Path to the partition map NetCDF file.
 * @param[out] partition Map or partition data structure to populate.
 * @param[in,out] os Output stream for error reporting.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-10-26
 * @date Last update : 2026-10-26
 */
void readPartitionMapNetCDF(const std::string &filename,
                            ww4_utils::LocalGridPartition &partition,
                            std::ostream &os = std::cout);

// --- writePartitionMapNetCDF ------------------------------------------------
/**
 * @brief Writes domain partition mapping information to a NetCDF file.
 * @param[in] filename Path to the target partition map NetCDF file.
 * @param[in] partition Local grid partition structure to export.
 * @param[in,out] os Output stream for error reporting.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-10-26
 * @date Last update : 2026-10-26
 */
void writePartitionMapNetCDF(const std::string &filename,
                             const ww4_utils::LocalGridPartition &partition,
                             std::ostream &os = std::cout);

} // namespace ww4_core::solver_UQ_utils
