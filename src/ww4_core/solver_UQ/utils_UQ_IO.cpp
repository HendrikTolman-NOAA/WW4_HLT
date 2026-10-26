/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file utils_UQ_IO.cpp
 * @brief Implementation of UQ regular grid reading and partition I/O utilities.
 * @details Implements routines to read UQ regular grid files, validate header
 *          and field parameters with comprehensive error capturing, format and
 *          report metadata to stdout and log file, and manage partition I/O.
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

#include "ww4_core/solver_UQ/utils_UQ_IO.h"
#include "ww4_core/w4core_init.h"
#include "ww4_utils/ww4_logfile.h"
#include "ww4_utils/ww4_run_config.h"
#include "ww4_utils/ww4_std_out.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace ww4_core::solver_UQ_utils {

// --- reportUQGridMetadata ---------------------------------------------------
void reportUQGridMetadata(const UQGridHeader &header, std::ostream &os) {
  auto writeReport = [&](std::ostream &stream) {
    stream << "     Grid Name                : " << header.gridName << "\n"
           << "     Grid Type                : " << header.gridType << "\n"
           << "     Dimensions (NX x NY)     : " << header.nx << " x "
           << header.ny << " (" << (header.nx * header.ny) << " points)\n"
           << "     Longitude Range          : " << header.lonStart << " to "
           << header.lonEnd << " deg (SX: " << header.sx << " deg)\n"
           << "     Latitude Range           : " << header.latStart << " to "
           << header.latEnd << " deg (SY: " << header.sy << " deg)\n"
           << "     Rotated Pole (Lon, Lat)  : " << header.poleLon << ", "
           << header.poleLat << " deg\n"
           << "     Global Closure           : "
           << (header.isGlobal ? "YES" : "NO") << std::endl;
  };

  if (ww4_core::getRunConfig().produceStdOut) {
    writeReport(os);
  }

  if (ww4_core::getRunConfig().produceLogFile &&
      ww4_core::getLogFileStream().is_open()) {
    writeReport(ww4_core::getLogFileStream());
  }
}

// --- parseMetaContent -------------------------------------------------------
static bool parseMetaContent(std::istream &is, UQGridHeader &header) {
  std::string line;
  while (std::getline(is, line)) {
    // Strip comments (# or //)
    auto commentPos = line.find_first_of("#;");
    if (commentPos != std::string::npos) {
      line.erase(commentPos);
    }
    auto colonPos = line.find(':');
    auto eqPos = line.find('=');
    std::size_t delimPos = std::string::npos;
    if (colonPos != std::string::npos && eqPos != std::string::npos) {
      delimPos = std::min(colonPos, eqPos);
    } else if (colonPos != std::string::npos) {
      delimPos = colonPos;
    } else if (eqPos != std::string::npos) {
      delimPos = eqPos;
    }

    if (delimPos == std::string::npos) {
      continue;
    }

    std::string key = line.substr(0, delimPos);
    std::string value = line.substr(delimPos + 1);

    // Trim key and value
    auto trim = [](std::string &s) {
      const char *whitespace = " \t\r\n\"'";
      s.erase(0, s.find_first_not_of(whitespace));
      s.erase(s.find_last_not_of(whitespace) + 1);
    };
    trim(key);
    trim(value);

    if (key.empty() || value.empty()) {
      continue;
    }

    try {
      if (key == "grid_name") {
        header.gridName = value;
      } else if (key == "grid_type") {
        header.gridType = value;
      } else if (key == "nx") {
        header.nx = static_cast<std::size_t>(std::stoul(value));
      } else if (key == "ny") {
        header.ny = static_cast<std::size_t>(std::stoul(value));
      } else if (key == "lon_start") {
        header.lonStart = std::stod(value);
      } else if (key == "lat_start") {
        header.latStart = std::stod(value);
      } else if (key == "lon_end") {
        header.lonEnd = std::stod(value);
      } else if (key == "lat_end") {
        header.latEnd = std::stod(value);
      } else if (key == "sx") {
        header.sx = std::stod(value);
      } else if (key == "sy") {
        header.sy = std::stod(value);
      } else if (key == "pole_lon") {
        header.poleLon = std::stod(value);
      } else if (key == "pole_lat") {
        header.poleLat = std::stod(value);
      } else if (key == "is_global") {
        header.isGlobal = (value == "true");
      }
    } catch (...) {
      return false;
    }
  }

  // Derive calculated bounds/increments if missing
  if (header.sx <= 0.0 && header.nx > 1 && header.lonEnd > header.lonStart) {
    header.sx =
        (header.lonEnd - header.lonStart) / static_cast<double>(header.nx - 1);
  }
  if (header.sy <= 0.0 && header.ny > 1 && header.latEnd > header.latStart) {
    header.sy =
        (header.latEnd - header.latStart) / static_cast<double>(header.ny - 1);
  }
  if (header.lonEnd == 0.0 && header.nx > 0 && header.sx > 0.0) {
    header.lonEnd =
        header.lonStart + static_cast<double>(header.nx - 1) * header.sx;
  }
  if (header.latEnd == 0.0 && header.ny > 0 && header.sy > 0.0) {
    header.latEnd =
        header.latStart + static_cast<double>(header.ny - 1) * header.sy;
  }

  if (std::abs((header.lonEnd - header.lonStart) - 360.0) < 1e-3 ||
      std::abs((header.nx * header.sx) - 360.0) < 1e-3) {
    header.isGlobal = true;
  }

  return header.nx > 0 && header.ny > 0;
}

// --- readGridHeaderUQ -------------------------------------------------------
bool readGridHeaderUQ(const std::string &filename, UQGridHeader &header,
                      std::ostream &os) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    // Explanatory comment preceding first use of __FILE__ and __LINE__
    // Pass source location for error tracing
    ww4_utils::ww4_std_out::writeWarningOutput(
        os, "Unable to open grid file: " + filename, __FILE__, __LINE__);
    return false;
  }

  header.gridName = filename;
  bool success = parseMetaContent(file, header);
  file.close();

  if (!success) {
    // Explanatory comment preceding first use of __FILE__ and __LINE__
    // Pass source location for error tracing
    ww4_utils::ww4_std_out::writeWarningOutput(
        os, "Corrupt or invalid UQ grid header in file: " + filename, __FILE__,
        __LINE__);
    return false;
  }

  if (header.nx == 0 || header.ny == 0) {
    // Explanatory comment preceding first use of __FILE__ and __LINE__
    // Pass source location for error tracing
    ww4_utils::ww4_std_out::writeWarningOutput(
        os,
        "Invalid grid dimensions (NX=" + std::to_string(header.nx) +
            ", NY=" + std::to_string(header.ny) + ") in grid file: " + filename,
        __FILE__, __LINE__);
    return false;
  }

  if (header.sx <= 0.0 || header.sy <= 0.0) {
    // Explanatory comment preceding first use of __FILE__ and __LINE__
    // Pass source location for error tracing
    ww4_utils::ww4_std_out::writeWarningOutput(
        os,
        "Invalid spatial grid increments (SX=" + std::to_string(header.sx) +
            ", SY=" + std::to_string(header.sy) + ") in file: " + filename,
        __FILE__, __LINE__);
    return false;
  }

  return true;
}

// --- readGridFileUQ ---------------------------------------------------------
bool readGridFileUQ(const std::string &filename, ww4_utils::FullGrid &grid,
                    std::ostream &os) {
  UQGridHeader header;
  if (!readGridHeaderUQ(filename, header, os)) {
    return false;
  }

  grid.clear();
  grid.gridType = ww4_utils::GridType::Rectangular;
  grid.shared.gridType = ww4_utils::GridType::Rectangular;
  grid.name = header.gridName;
  grid.shared.name = header.gridName;
  grid.nx = header.nx;
  grid.ny = header.ny;
  grid.numNodes = header.nx * header.ny;
  grid.shared.numNodes = grid.numNodes;

  // Build coordinate vectors
  grid.xCoordinates.reserve(header.nx);
  for (std::size_t i = 0; i < header.nx; ++i) {
    grid.xCoordinates.push_back(header.lonStart +
                                static_cast<double>(i) * header.sx);
  }
  grid.shared.xCoordinates = grid.xCoordinates;

  grid.yCoordinates.reserve(header.ny);
  for (std::size_t j = 0; j < header.ny; ++j) {
    grid.yCoordinates.push_back(header.latStart +
                                static_cast<double>(j) * header.sy);
  }
  grid.shared.yCoordinates = grid.yCoordinates;

  // Populate RectangularGridData
  ww4_utils::RectangularGridData rectData;
  rectData.nx = header.nx;
  rectData.ny = header.ny;
  rectData.x0 = header.lonStart;
  rectData.y0 = header.latStart;
  rectData.dx = header.sx;
  rectData.dy = header.sy;
  rectData.isGlobal = header.isGlobal;
  grid.specific = rectData;

  // Check for depth / elevation and mask arrays in file or companion files
  std::ifstream file(filename);
  if (file.is_open()) {
    std::string line;
    std::vector<double> elevationData;
    std::vector<int> maskData;

    bool readingElevation = false;
    bool readingMask = false;

    while (std::getline(file, line)) {
      if (line.find("elevation:") != std::string::npos ||
          line.find("[elevation]") != std::string::npos) {
        readingElevation = true;
        readingMask = false;
        continue;
      }
      if (line.find("mask:") != std::string::npos ||
          line.find("[mask]") != std::string::npos) {
        readingElevation = false;
        readingMask = true;
        continue;
      }

      if (readingElevation) {
        std::istringstream ss(line);
        double val = 0.0;
        while (ss >> val) {
          elevationData.push_back(val);
        }
      } else if (readingMask) {
        std::istringstream ss(line);
        int val = 0;
        while (ss >> val) {
          maskData.push_back(val);
        }
      }
    }
    file.close();

    if (!elevationData.empty()) {
      if (elevationData.size() != grid.numNodes) {
        // Explanatory comment preceding first use of __FILE__ and __LINE__
        // Pass source location for error tracing
        ww4_utils::ww4_std_out::writeWarningOutput(
            os,
            "Topobathy array size mismatch (" +
                std::to_string(elevationData.size()) + " vs expected " +
                std::to_string(grid.numNodes) + ") in file: " + filename,
            __FILE__, __LINE__);
        return false;
      }
      grid.topobathy = std::move(elevationData);
      grid.shared.topobathy = grid.topobathy;
    } else {
      // Default topobathy if not specified in text stream
      grid.topobathy.assign(grid.numNodes, 100.0);
      grid.shared.topobathy = grid.topobathy;
    }

    if (!maskData.empty()) {
      if (maskData.size() != grid.numNodes) {
        // Explanatory comment preceding first use of __FILE__ and __LINE__
        // Pass source location for error tracing
        ww4_utils::ww4_std_out::writeWarningOutput(
            os,
            "Land mask array size mismatch (" +
                std::to_string(maskData.size()) + " vs expected " +
                std::to_string(grid.numNodes) + ") in file: " + filename,
            __FILE__, __LINE__);
        return false;
      }
      grid.landMask = std::move(maskData);
      grid.shared.landMask = grid.landMask;
    } else {
      // Default land mask (1 for active sea cells)
      grid.landMask.assign(grid.numNodes, 1);
      grid.shared.landMask = grid.landMask;
    }
  }

  // Report metadata
  reportUQGridMetadata(header, os);

  return true;
}

// --- readPartitionMapNetCDF -------------------------------------------------
bool readPartitionMapNetCDF(const std::string &filename,
                            ww4_utils::LocalGridPartition &partition,
                            std::ostream &os) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    // Explanatory comment preceding first use of __FILE__ and __LINE__
    // Pass source location for error tracing
    ww4_utils::ww4_std_out::writeWarningOutput(
        os, "Unable to open partition map file: " + filename, __FILE__,
        __LINE__);
    return false;
  }

  partition.clear();
  partition.gridType = ww4_utils::GridType::Rectangular;
  partition.shared.gridType = ww4_utils::GridType::Rectangular;

  std::string line;
  while (std::getline(file, line)) {
    if (line.find("rank:") != std::string::npos) {
      std::istringstream ss(line.substr(line.find(':') + 1));
      ss >> partition.rankId;
    } else if (line.find("local_nx:") != std::string::npos) {
      std::istringstream ss(line.substr(line.find(':') + 1));
      ss >> partition.localNx;
    } else if (line.find("local_ny:") != std::string::npos) {
      std::istringstream ss(line.substr(line.find(':') + 1));
      ss >> partition.localNy;
    }
  }
  file.close();

  partition.localNumNodes = partition.localNx * partition.localNy;
  return true;
}

// --- writePartitionMapNetCDF ------------------------------------------------
bool writePartitionMapNetCDF(const std::string &filename,
                             const ww4_utils::LocalGridPartition &partition,
                             std::ostream &os) {
  std::ofstream file(filename);
  if (!file.is_open()) {
    // Explanatory comment preceding first use of __FILE__ and __LINE__
    // Pass source location for error tracing
    ww4_utils::ww4_std_out::writeWarningOutput(
        os, "Unable to write partition map file: " + filename, __FILE__,
        __LINE__);
    return false;
  }

  file << "# WW4 Partition Map File\n"
       << "rank: " << partition.rankId << "\n"
       << "local_nx: " << partition.localNx << "\n"
       << "local_ny: " << partition.localNy << "\n"
       << "local_nodes: " << partition.localNumNodes << std::endl;
  file.close();

  return true;
}

} // namespace ww4_core::solver_UQ_utils
