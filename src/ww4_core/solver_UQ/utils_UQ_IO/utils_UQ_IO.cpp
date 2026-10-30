/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file utils_UQ_IO.cpp
 * @brief Implementation of UQ regular grid reading and partition I/O utilities.
 * @details Implements routines to read UQ regular grid files using netcdf-c and
 *          ASCII metadata, validate header and field parameters with error
 *          capturing, format and report metadata to stdout and log file, and
 *          manage partition I/O.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the
 * National Weather Service. NWS often uses Generative AI (GenAI) for code
 * development and refactoring. Whenever GenAI is used, NWS requires a full
 * human review of code before it is added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-10-26
 * @date Last update : 2026-10-30
 */

#include "ww4_core/solver_UQ/utils_UQ_IO/utils_UQ_IO.h"
#include "ww4_core/w4core_init.h"
#include "ww4_utils/ww4_logfile.h"
#include "ww4_utils/ww4_run_config.h"
#include "ww4_utils/ww4_std_out.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <netcdf.h>
#include <sstream>
#include <type_traits>

namespace ww4_core::solver_UQ_utils {

namespace {

/**
 * @class NcHandle
 * @brief RAII wrapper for NetCDF handle (ncid).
 */
class NcHandle {
public:
  explicit NcHandle(int id) noexcept : ncid_(id) {}
  ~NcHandle() {
    if (ncid_ >= 0) {
      nc_close(ncid_);
    }
  }
  NcHandle(const NcHandle &) = delete;
  NcHandle &operator=(const NcHandle &) = delete;
  NcHandle(NcHandle &&other) noexcept : ncid_(other.ncid_) { other.ncid_ = -1; }
  NcHandle &operator=(NcHandle &&other) noexcept {
    if (this != &other) {
      if (ncid_ >= 0) {
        nc_close(ncid_);
      }
      ncid_ = other.ncid_;
      other.ncid_ = -1;
    }
    return *this;
  }
  [[nodiscard]] int get() const noexcept { return ncid_; }

private:
  int ncid_{-1};
};

/**
 * @brief Utility function to read a global string attribute.
 */
static std::string readGlobalAttributeNC(int ncid, const char *attname) {
  std::size_t len = 0;
  if (nc_inq_attlen(ncid, NC_GLOBAL, attname, &len) == NC_NOERR) {
    std::string val(len, '\0');
    if (nc_get_att_text(ncid, NC_GLOBAL, attname, val.data()) == NC_NOERR) {
      while (!val.empty() && (val.back() == '\0' || val.back() == ' ')) {
        val.pop_back();
      }
      return val;
    }
  }
  return "";
}

/**
 * @brief Reads data from a NetCDF variable into a std::vector with dynamic
 * buffer sizing and type conversion.
 */
template <typename T>
static bool readVariableNC(int ncid, const char *varname, std::vector<T> &vec) {
  int varid = -1;
  if (nc_inq_varid(ncid, varname, &varid) != NC_NOERR) {
    return false;
  }
  int ndims = 0;
  if (nc_inq_varndims(ncid, varid, &ndims) != NC_NOERR || ndims < 0) {
    return false;
  }
  std::vector<int> dimids(static_cast<std::size_t>(ndims));
  if (nc_inq_vardimid(ncid, varid, dimids.data()) != NC_NOERR) {
    return false;
  }
  std::size_t totalLen = 1;
  for (int i = 0; i < ndims; ++i) {
    std::size_t dlen = 0;
    if (nc_inq_dimlen(ncid, dimids[i], &dlen) != NC_NOERR) {
      return false;
    }
    totalLen *= dlen;
  }
  if (totalLen == 0) {
    return false;
  }
  vec.resize(totalLen);

  nc_type xtype = NC_NAT;
  if (nc_inq_vartype(ncid, varid, &xtype) != NC_NOERR) {
    return false;
  }

  if constexpr (std::is_same_v<T, double>) {
    if (xtype == NC_FLOAT) {
      std::vector<float> tmp(totalLen);
      if (nc_get_var_float(ncid, varid, tmp.data()) != NC_NOERR) {
        return false;
      }
      for (std::size_t i = 0; i < totalLen; ++i) {
        vec[i] = static_cast<double>(tmp[i]);
      }
      return true;
    }
    return nc_get_var_double(ncid, varid, vec.data()) == NC_NOERR;
  } else if constexpr (std::is_same_v<T, float>) {
    if (xtype == NC_DOUBLE) {
      std::vector<double> tmp(totalLen);
      if (nc_get_var_double(ncid, varid, tmp.data()) != NC_NOERR) {
        return false;
      }
      for (std::size_t i = 0; i < totalLen; ++i) {
        vec[i] = static_cast<float>(tmp[i]);
      }
      return true;
    }
    return nc_get_var_float(ncid, varid, vec.data()) == NC_NOERR;
  } else if constexpr (std::is_same_v<T, int>) {
    if (xtype == NC_INT64) {
      std::vector<long long> tmp64(totalLen);
      if (nc_get_var_longlong(ncid, varid, tmp64.data()) != NC_NOERR) {
        return false;
      }
      for (std::size_t i = 0; i < totalLen; ++i) {
        vec[i] = static_cast<int>(tmp64[i]);
      }
      return true;
    }
    return nc_get_var_int(ncid, varid, vec.data()) == NC_NOERR;
  }

  return false;
}

/**
 * @brief Reports an error to screen and log file before calling extcde.
 */
static void reportErrorAndExit(int exitCode, std::ostream &os,
                               const std::string &msg, const char *file,
                               int line) {
  if (ww4_core::getRunConfig().produceLogFile &&
      ww4_core::getLogFileStream().is_open()) {
    ww4_utils::ww4_std_out::writeExtcdeOutput(ww4_core::getLogFileStream(), msg,
                                              file, line);
  }
  ww4_utils::ww4_std_out::extcde(exitCode, os, msg, file, line);
}

/**
 * @brief Attempts to read UQ regular grid header and fields directly from
 * NetCDF file using netcdf-c library following gridgen UGRID reader logic.
 */
static bool tryParseNetCDFGrid(const std::string &fname, UQGridHeader &header,
                               std::vector<double> &topobathy,
                               std::vector<int> &mask) {
  int raw_ncid = -1;
  if (nc_open(fname.c_str(), NC_NOWRITE, &raw_ncid) != NC_NOERR) {
    return false;
  }
  NcHandle handle(raw_ncid);
  int ncid = handle.get();

  auto getDimLen = [ncid](const char *dimname) -> std::size_t {
    int dimid = -1;
    if (nc_inq_dimid(ncid, dimname, &dimid) != NC_NOERR) {
      return 0;
    }
    std::size_t len = 0;
    if (nc_inq_dimlen(ncid, dimid, &len) != NC_NOERR) {
      return 0;
    }
    return len;
  };

  std::size_t ny = getDimLen("ny");
  if (ny == 0) {
    ny = getDimLen("nlat");
  }
  std::size_t nx = getDimLen("nx");
  if (nx == 0) {
    nx = getDimLen("nlon");
  }

  if (nx == 0 || ny == 0) {
    return false;
  }

  header.nx = nx;
  header.ny = ny;

  std::string title = readGlobalAttributeNC(ncid, "title");
  header.gridName = title.empty() ? fname : title;

  std::size_t numPoints = nx * ny;
  std::vector<double> lon;
  std::vector<double> lat;

  bool hasGridLon = readVariableNC(ncid, "grid_lon", lon) ||
                    readVariableNC(ncid, "face_lon", lon) ||
                    readVariableNC(ncid, "longitude", lon) ||
                    readVariableNC(ncid, "lon", lon);
  bool hasGridLat = readVariableNC(ncid, "grid_lat", lat) ||
                    readVariableNC(ncid, "face_lat", lat) ||
                    readVariableNC(ncid, "latitude", lat) ||
                    readVariableNC(ncid, "lat", lat);

  if (hasGridLon && !lon.empty()) {
    header.lonStart = lon[0];
    if (lon.size() == nx) {
      header.lonEnd = lon[nx - 1];
    } else if (lon.size() >= numPoints) {
      header.lonEnd = lon[nx - 1];
    } else {
      header.lonEnd = lon.back();
    }
    if (nx > 1) {
      header.sx =
          (header.lonEnd - header.lonStart) / static_cast<double>(nx - 1);
      if (header.sx < 0.0) {
        header.sx = std::abs(header.sx);
      }
    }
  }

  if (hasGridLat && !lat.empty()) {
    header.latStart = lat[0];
    if (lat.size() == ny) {
      header.latEnd = lat[ny - 1];
    } else if (lat.size() >= numPoints) {
      header.latEnd = lat[(ny - 1) * nx];
    } else {
      header.latEnd = lat.back();
    }
    if (ny > 1) {
      header.sy =
          (header.latEnd - header.latStart) / static_cast<double>(ny - 1);
      if (header.sy < 0.0) {
        header.sy = std::abs(header.sy);
      }
    }
  }

  topobathy.clear();
  bool hasDepth = readVariableNC(ncid, "grid_depth", topobathy) ||
                  readVariableNC(ncid, "depth", topobathy) ||
                  readVariableNC(ncid, "elevation", topobathy);
  if (!hasDepth || topobathy.empty()) {
    topobathy.assign(numPoints, 100.0);
  } else if (topobathy.size() < numPoints) {
    topobathy.resize(numPoints, 100.0);
  }

  mask.clear();
  bool hasMask = readVariableNC(ncid, "grid_mask", mask) ||
                 readVariableNC(ncid, "mask", mask);
  if (!hasMask || mask.empty()) {
    mask.assign(numPoints, 1);
  } else if (mask.size() < numPoints) {
    mask.resize(numPoints, 1);
  }

  if (header.sx > 0.0 &&
      (std::abs((header.lonEnd - header.lonStart) - 360.0) < 1e-3 ||
       std::abs((header.nx * header.sx) - 360.0) < 1e-3)) {
    header.isGlobal = true;
  }

  return header.nx > 0 && header.ny > 0;
}

} // namespace

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
  if (!std::filesystem::exists(filename)) {
    std::vector<std::string> candidates;
    if (filename.length() >= 9 &&
        filename.substr(filename.length() - 9) == "_ugrid.nc") {
      candidates.push_back(filename.substr(0, filename.length() - 9) + ".meta");
      candidates.push_back(filename.substr(0, filename.length() - 3) + ".meta");
    }
    if (filename.length() >= 3 &&
        filename.substr(filename.length() - 3) == ".nc") {
      candidates.push_back(filename.substr(0, filename.length() - 3) + ".meta");
      candidates.push_back(filename + ".meta");
    }
    bool anyExists = std::any_of(
        candidates.begin(), candidates.end(),
        [](const std::string &cand) { return std::filesystem::exists(cand); });
    if (!anyExists) {
      // Explanatory comment preceding first use of __FILE__ and __LINE__
      // Pass source location for error tracing
      reportErrorAndExit(1, os, "Grid file does not exist: " + filename,
                         __FILE__, __LINE__);
    }
  }

  if (ww4_core::getRunConfig().produceStdOut) {
    os << "     - Starting opening grid file: " << filename << std::endl;
  }

  std::vector<double> dummyElev;
  std::vector<int> dummyMask;
  if (tryParseNetCDFGrid(filename, header, dummyElev, dummyMask)) {
    if (ww4_core::getRunConfig().produceStdOut) {
      os << "     - Completed opening grid file: " << filename
         << " ... [SUCCESS]" << std::endl;
      os << "     - Starting parsing grid header metadata from file: "
         << filename << std::endl;
      os << "     - Completed parsing grid header metadata ... [SUCCESS]"
         << std::endl;
    }
    return filename;
  }

  std::vector<std::string> candidates = {filename};

  // Add companion candidate paths if filename ends with .nc or _ugrid.nc
  if (filename.length() >= 9 &&
      filename.substr(filename.length() - 9) == "_ugrid.nc") {
    candidates.push_back(filename.substr(0, filename.length() - 9) + ".meta");
    candidates.push_back(filename.substr(0, filename.length() - 3) + ".meta");
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
    reportErrorAndExit(1, os,
                       "Corrupt or invalid UQ grid header in file: " + filename,
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

  std::vector<double> ncTopobathy;
  std::vector<int> ncMask;
  bool isNetCDF = tryParseNetCDFGrid(activeFile, header, ncTopobathy, ncMask);

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

  if (isNetCDF) {
    int raw_ncid = -1;
    if (nc_open(activeFile.c_str(), NC_NOWRITE, &raw_ncid) == NC_NOERR) {
      NcHandle handle(raw_ncid);
      int ncid = handle.get();
      std::vector<double> obsX;
      std::vector<double> obsY;
      if (readVariableNC(ncid, "grid_ObsX", obsX) ||
          readVariableNC(ncid, "ObsX", obsX)) {
        grid.shared.obstructionX = std::move(obsX);
      }
      if (readVariableNC(ncid, "grid_ObsY", obsY) ||
          readVariableNC(ncid, "ObsY", obsY)) {
        grid.shared.obstructionY = std::move(obsY);
      }
    }
  }

  if (isNetCDF && !ncTopobathy.empty()) {
    grid.topobathy = std::move(ncTopobathy);
    grid.shared.topobathy = grid.topobathy;
  } else {
    // Check for depth / elevation and mask arrays in active header file
    std::ifstream file(activeFile);
    if (file.is_open()) {
      std::string line;
      std::vector<double> elevationData;
      std::vector<int> maskData;

      bool readingElevation = false;
      bool readingMask = false;

      while (std::getline(file, line)) {
        // Strip comments starting with %, $, #, ;, !, or //
        auto commentPos = line.find_first_of("%$#;!");
        if (commentPos == std::string::npos) {
          commentPos = line.find("//");
        }
        if (commentPos != std::string::npos) {
          line.erase(commentPos);
        }

        std::string lineLower = line;
        std::transform(lineLower.begin(), lineLower.end(), lineLower.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        if (lineLower.find("[elevation]") != std::string::npos ||
            lineLower.find("[depth]") != std::string::npos ||
            lineLower.find("[bathymetry]") != std::string::npos ||
            lineLower.find("[grid_depth]") != std::string::npos) {
          readingElevation = true;
          readingMask = false;
          continue;
        }
        if (lineLower.find("[mask]") != std::string::npos ||
            lineLower.find("[grid_mask]") != std::string::npos) {
          readingElevation = false;
          readingMask = true;
          continue;
        }

        auto posElev = lineLower.find("elevation:");
        if (posElev == std::string::npos) {
          posElev = lineLower.find("depth:");
        }
        if (posElev == std::string::npos) {
          posElev = lineLower.find("bathymetry:");
        }
        if (posElev == std::string::npos) {
          posElev = lineLower.find("grid_depth:");
        }

        if (posElev != std::string::npos) {
          readingElevation = true;
          readingMask = false;
          line = line.substr(posElev + lineLower.substr(posElev).find(':') + 1);
        } else {
          auto posMask = lineLower.find("mask:");
          if (posMask == std::string::npos) {
            posMask = lineLower.find("grid_mask:");
          }
          if (posMask != std::string::npos) {
            readingElevation = false;
            readingMask = true;
            line =
                line.substr(posMask + lineLower.substr(posMask).find(':') + 1);
          }
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
          reportErrorAndExit(
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

      if (!maskData.empty()) {
        ncMask = std::move(maskData);
      }
    }
  }

  if (ww4_core::getRunConfig().produceStdOut) {
    os << "     - Completed ingesting topobathy elevation field ... [SUCCESS]"
       << std::endl;
    os << "     - Starting ingesting land/sea mask field ..." << std::endl;
  }

  if (isNetCDF && !ncMask.empty()) {
    grid.landMask = std::move(ncMask);
    grid.shared.landMask = grid.landMask;
  } else if (!ncMask.empty()) {
    if (ncMask.size() != grid.numNodes) {
      // Explanatory comment preceding first use of __FILE__ and __LINE__
      // Pass source location for error tracing
      reportErrorAndExit(1, os,
                         "Land mask array size mismatch (" +
                             std::to_string(ncMask.size()) + " vs expected " +
                             std::to_string(grid.numNodes) +
                             ") in file: " + filename,
                         __FILE__, __LINE__);
    }
    grid.landMask = std::move(ncMask);
    grid.shared.landMask = grid.landMask;
  } else {
    // Default land mask (1 for active sea cells)
    grid.landMask.assign(grid.numNodes, 1);
    grid.shared.landMask = grid.landMask;
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
