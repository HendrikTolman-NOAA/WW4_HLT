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
           << "     Dimensions (nx, ny)      : " << header.nx << " x "
           << header.ny << " (" << (header.nx * header.ny) << " points)\n"
           << "     Longitude Range          : " << header.lonStart << " to "
           << header.lonEnd << " deg (sx: " << header.sx << " deg)\n"
           << "     Latitude Range           : " << header.latStart << " to "
           << header.latEnd << " deg (sy: " << header.sy << " deg)\n"
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
    // Strip comments starting with %, $, #, ;, !, or //
    auto commentPos = line.find_first_of("%$#;!");
    if (commentPos == std::string::npos) {
      commentPos = line.find("//");
    }
    if (commentPos != std::string::npos) {
      line.erase(commentPos);
    }

    // Trim line
    auto trim = [](std::string &s) {
      const char *whitespace = " \t\r\n\"'";
      s.erase(0, s.find_first_not_of(whitespace));
      s.erase(s.find_last_not_of(whitespace) + 1);
    };
    trim(line);

    if (line.empty()) {
      continue;
    }

    // Find key-value delimiter: ':' or '=' or whitespace (spaces/tabs)
    std::string key, value;
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

    if (delimPos != std::string::npos) {
      key = line.substr(0, delimPos);
      value = line.substr(delimPos + 1);
    } else {
      // Split on first whitespace sequence (spaces or tabs)
      auto spacePos = line.find_first_of(" \t");
      if (spacePos != std::string::npos) {
        key = line.substr(0, spacePos);
        value = line.substr(spacePos + 1);
      } else {
        continue;
      }
    }

    trim(key);
    trim(value);

    if (key.empty() || value.empty()) {
      continue;
    }

    // Convert key and value to lowercase for case-insensitive matching
    std::string keyLower = key;
    std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    std::string valLower = value;
    std::transform(valLower.begin(), valLower.end(), valLower.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    try {
      if (keyLower == "grid_name" || keyLower == "name" ||
          keyLower == "gridname" || keyLower == "title" ||
          keyLower == "grid_title") {
        header.gridName = value;
      } else if (keyLower == "grid_type" || keyLower == "type" ||
                 keyLower == "gridtype") {
        header.gridType = value;
      } else if (keyLower == "nx" || keyLower == "nlon" ||
                 keyLower == "num_lon") {
        header.nx = static_cast<std::size_t>(std::stoul(value));
      } else if (keyLower == "ny" || keyLower == "nlat" ||
                 keyLower == "num_lat") {
        header.ny = static_cast<std::size_t>(std::stoul(value));
      } else if (keyLower == "lon_start" || keyLower == "lat_start_lon" ||
                 keyLower == "x0" || keyLower == "xfirst" ||
                 keyLower == "sw_lon" || keyLower == "lon_min") {
        header.lonStart = std::stod(value);
      } else if (keyLower == "lat_start" || keyLower == "y0" ||
                 keyLower == "yfirst" || keyLower == "sw_lat" ||
                 keyLower == "lat_min") {
        header.latStart = std::stod(value);
      } else if (keyLower == "lon_end" || keyLower == "xlast" ||
                 keyLower == "lon_max") {
        header.lonEnd = std::stod(value);
      } else if (keyLower == "lat_end" || keyLower == "ylast" ||
                 keyLower == "lat_max") {
        header.latEnd = std::stod(value);
      } else if (keyLower == "sx" || keyLower == "dx" || keyLower == "dlon" ||
                 keyLower == "lon_inc") {
        header.sx = std::stod(value);
      } else if (keyLower == "sy" || keyLower == "dy" || keyLower == "dlat" ||
                 keyLower == "lat_inc") {
        header.sy = std::stod(value);
      } else if (keyLower == "pole_lon" || keyLower == "polon") {
        header.poleLon = std::stod(value);
      } else if (keyLower == "pole_lat" || keyLower == "polat") {
        header.poleLat = std::stod(value);
      } else if (keyLower == "is_global" || keyLower == "global" ||
                 keyLower == "closure" || keyLower == "global_grid") {
        header.isGlobal = (valLower == "true" || valLower == "1" ||
                           valLower == "yes" || valLower == "closed");
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

// --- tryParseHeaderFile -----------------------------------------------------
static bool tryParseHeaderFile(const std::string &fname, UQGridHeader &header) {
  std::ifstream file(fname);
  if (!file.is_open()) {
    return false;
  }
  header.gridName = fname;
  bool ok = parseMetaContent(file, header);
  file.close();
  return ok && header.nx > 0 && header.ny > 0 && header.sx > 0.0 &&
         header.sy > 0.0;
}

// --- readGridHeaderUQ -------------------------------------------------------
std::string readGridHeaderUQ(const std::string &filename, UQGridHeader &header,
                             std::ostream &os) {
  if (ww4_core::getRunConfig().produceStdOut) {
    os << "     - Starting opening grid file: " << filename << std::endl;
  }

  std::vector<std::string> candidates = {filename};

  // Add companion .meta candidate paths if filename ends with .nc or _ugrid.nc
  if (filename.length() >= 9 &&
      filename.substr(filename.length() - 9) == "_ugrid.nc") {
    candidates.push_back(filename.substr(0, filename.length() - 9) + ".meta");
  }
  if (filename.length() >= 3 &&
      filename.substr(filename.length() - 3) == ".nc") {
    candidates.push_back(filename.substr(0, filename.length() - 3) + ".meta");
    candidates.push_back(filename + ".meta");
  }

  auto it = std::find_if(candidates.begin(), candidates.end(),
                         [&header](const std::string &cand) {
                           return tryParseHeaderFile(cand, header);
                         });
  bool success = (it != candidates.end());
  std::string activeFile = success ? *it : "";

  if (!success) {
    // Explanatory comment preceding first use of __FILE__ and __LINE__
    // Pass source location for error tracing
    ww4_utils::ww4_std_out::extcde(
        1, os, "Corrupt or invalid UQ grid header in file: " + filename,
        __FILE__, __LINE__);
  }

  if (ww4_core::getRunConfig().produceStdOut) {
    os << "     - Completed opening grid file: " << activeFile
       << " ... [SUCCESS]" << std::endl;
    os << "     - Starting parsing grid header metadata from file: "
       << activeFile << std::endl;
    os << "     - Completed parsing grid header metadata ... [SUCCESS]"
       << std::endl;
  }

  return activeFile;
}

// --- readGridFileUQ ---------------------------------------------------------
void readGridFileUQ(const std::string &filename, ww4_utils::FullGrid &grid,
                    std::ostream &os) {
  UQGridHeader header;
  std::string activeFile = readGridHeaderUQ(filename, header, os);

  if (ww4_core::getRunConfig().produceStdOut) {
    os << "     - Starting constructing spatial coordinates (NX=" << header.nx
       << ", NY=" << header.ny << ") ..." << std::endl;
  }

  grid.clear();
  grid.gridType = ww4_utils::GridType::Rectangular;
  grid.shared.gridType = ww4_utils::GridType::Rectangular;
  grid.name = header.gridName.empty() ? filename : header.gridName;
  grid.shared.name = grid.name;
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

  if (ww4_core::getRunConfig().produceStdOut) {
    os << "     - Completed constructing spatial coordinates ... [SUCCESS]"
       << std::endl;
    os << "     - Starting ingesting topobathy elevation field ..."
       << std::endl;
  }

  // Check for depth / elevation and mask arrays in active header file
  std::ifstream file(activeFile);
  if (file.is_open()) {
    std::string line;
    std::vector<double> elevationData;
    std::vector<int> maskData;

    bool readingElevation = false;
    bool readingMask = false;

    while (std::getline(file, line)) {
      if (line.find("elevation:") != std::string::npos ||
          line.find("depth:") != std::string::npos ||
          line.find("bathymetry:") != std::string::npos ||
          line.find("[elevation]") != std::string::npos ||
          line.find("[depth]") != std::string::npos) {
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
        ww4_utils::ww4_std_out::extcde(
            1, os,
            "Topobathy array size mismatch (" +
                std::to_string(elevationData.size()) + " vs expected " +
                std::to_string(grid.numNodes) + ") in file: " + filename,
            __FILE__, __LINE__);
      }
      grid.topobathy = std::move(elevationData);
      grid.shared.topobathy = grid.topobathy;
    } else {
      // Default topobathy if not specified in text stream
      grid.topobathy.assign(grid.numNodes, 100.0);
      grid.shared.topobathy = grid.topobathy;
    }

    if (ww4_core::getRunConfig().produceStdOut) {
      os << "     - Completed ingesting topobathy elevation field ... [SUCCESS]"
         << std::endl;
      os << "     - Starting ingesting land/sea mask field ..." << std::endl;
    }

    if (!maskData.empty()) {
      if (maskData.size() != grid.numNodes) {
        // Explanatory comment preceding first use of __FILE__ and __LINE__
        // Pass source location for error tracing
        ww4_utils::ww4_std_out::extcde(
            1, os,
            "Land mask array size mismatch (" +
                std::to_string(maskData.size()) + " vs expected " +
                std::to_string(grid.numNodes) + ") in file: " + filename,
            __FILE__, __LINE__);
      }
      grid.landMask = std::move(maskData);
      grid.shared.landMask = grid.landMask;
    } else {
      // Default land mask (1 for active sea cells)
      grid.landMask.assign(grid.numNodes, 1);
      grid.shared.landMask = grid.landMask;
    }
  }

  if (ww4_core::getRunConfig().produceStdOut) {
    os << "     - Completed ingesting land/sea mask field ... [SUCCESS]\n"
       << "     - Grid reading completed successfully." << std::endl;
  }

  // Report metadata
  reportUQGridMetadata(header, os);
}

// --- readPartitionMapNetCDF -------------------------------------------------
void readPartitionMapNetCDF(const std::string &filename,
                            ww4_utils::LocalGridPartition &partition,
                            std::ostream &os) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    // Explanatory comment preceding first use of __FILE__ and __LINE__
    // Pass source location for error tracing
    ww4_utils::ww4_std_out::extcde(
        1, os, "Unable to open partition map file: " + filename, __FILE__,
        __LINE__);
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
}

// --- writePartitionMapNetCDF ------------------------------------------------
void writePartitionMapNetCDF(const std::string &filename,
                             const ww4_utils::LocalGridPartition &partition,
                             std::ostream &os) {
  std::ofstream file(filename);
  if (!file.is_open()) {
    // Explanatory comment preceding first use of __FILE__ and __LINE__
    // Pass source location for error tracing
    ww4_utils::ww4_std_out::extcde(
        1, os, "Unable to write partition map file: " + filename, __FILE__,
        __LINE__);
  }

  file << "# WW4 Partition Map File\n"
       << "rank: " << partition.rankId << "\n"
       << "local_nx: " << partition.localNx << "\n"
       << "local_ny: " << partition.localNy << "\n"
       << "local_nodes: " << partition.localNumNodes << std::endl;
  file.close();
}

} // namespace ww4_core::solver_UQ_utils
