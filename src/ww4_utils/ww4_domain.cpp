//+ -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
//-- -- -- + | WAVEWATCH IV, open source, code management by NOAA / NWS |
//+ -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
//-- -- -- +
//
// @file ww4_domain.cpp
// @brief Domain partitioning and NetCDF I/O routines for WAVEWATCH IV (WW4).
// @details Provides functions to partition global UQ structured grids into
// sub-domains based on active sea points (derived from WW3 ww3_gspl) and
// routines to write and read partitioning maps in NetCDF format.
//
// @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
// Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks
// of the National Weather Service.
// NWS often uses Generative AI (GenAI) for code development and refactoring.
// Whenever GenAI is used, NWS requires a full human review of code before it is
// added to its repositories.
// @author Main Author(s) : Aldgisl (AI Persona), Hendrik L. Tolman
// @author Contributors : Jules (Agentic AI), Kit Stokes, Jessica Meixner
// @date Initial : 2026-10-14
// @date Last update : 2026-10-14

#include "ww4_utils/ww4_domain.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace ww4_utils {

namespace {

// NetCDF constants
constexpr uint32_t NC_DIMENSION_TAG = 0x0000000A;
constexpr uint32_t NC_VARIABLE_TAG = 0x0000000B;
constexpr uint32_t NC_ATTRIBUTE_TAG = 0x0000000C;
constexpr uint32_t NC_INT_TYPE = 4;
constexpr uint32_t NC_CHAR_TYPE = 2;

inline uint32_t toBigEndian32(uint32_t val) {
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  return val;
#else
  return ((val & 0x000000FFu) << 24) | ((val & 0x0000FF00u) << 8) |
         ((val & 0x00FF0000u) >> 8) | ((val & 0xFF000000u) >> 24);
#endif
}

inline uint32_t fromBigEndian32(uint32_t val) { return toBigEndian32(val); }

void writeU32(std::ostream &os, uint32_t val) {
  uint32_t be = toBigEndian32(val);
  os.write(reinterpret_cast<const char *>(&be), sizeof(be));
}

void writeNcString(std::ostream &os, std::string_view str) {
  uint32_t len = static_cast<uint32_t>(str.size());
  writeU32(os, len);
  if (len > 0) {
    os.write(str.data(), len);
  }
  size_t pad = (4 - (len % 4)) % 4;
  for (size_t i = 0; i < pad; ++i) {
    char zero = 0;
    os.write(&zero, 1);
  }
}

void writeNcCharAttribute(std::ostream &os, std::string_view name,
                          std::string_view val) {
  writeNcString(os, name);
  writeU32(os, NC_CHAR_TYPE);
  uint32_t len = static_cast<uint32_t>(val.size());
  writeU32(os, len);
  if (len > 0) {
    os.write(val.data(), len);
  }
  size_t pad = (4 - (len % 4)) % 4;
  for (size_t i = 0; i < pad; ++i) {
    char zero = 0;
    os.write(&zero, 1);
  }
}

void writeNcIntAttribute(std::ostream &os, std::string_view name,
                         std::span<const int> vals) {
  writeNcString(os, name);
  writeU32(os, NC_INT_TYPE);
  uint32_t len = static_cast<uint32_t>(vals.size());
  writeU32(os, len);
  for (int v : vals) {
    writeU32(os, static_cast<uint32_t>(v));
  }
}

bool readU32(std::istream &is, uint32_t &val) {
  uint32_t be = 0;
  if (!is.read(reinterpret_cast<char *>(&be), sizeof(be))) {
    return false;
  }
  val = fromBigEndian32(be);
  return true;
}

bool readNcString(std::istream &is, std::string &str) {
  uint32_t len = 0;
  if (!readU32(is, len)) {
    return false;
  }
  str.resize(len);
  if (len > 0) {
    if (!is.read(&str[0], len)) {
      return false;
    }
  }
  size_t pad = (4 - (len % 4)) % 4;
  if (pad > 0) {
    is.ignore(pad);
  }
  return true;
}

int countSeaPointsInRect(int x_min, int x_max, int y_min, int y_max, int nx,
                         std::span<const int> land_sea_map) {
  int count = 0;
  for (int y = y_min; y <= y_max; ++y) {
    for (int x = x_min; x <= x_max; ++x) {
      if (land_sea_map[y * nx + x] > 0) {
        count++;
      }
    }
  }
  return count;
}

void recursiveBisectDomain(int x_min, int x_max, int y_min, int y_max, int nx,
                           std::span<const int> land_sea_map,
                           int target_partitions, int partition_start_id,
                           std::vector<int> &partition_map) {
  int total_sea =
      countSeaPointsInRect(x_min, x_max, y_min, y_max, nx, land_sea_map);
  if (total_sea == 0) {
    return;
  }

  if (target_partitions <= 1) {
    for (int y = y_min; y <= y_max; ++y) {
      for (int x = x_min; x <= x_max; ++x) {
        int idx = y * nx + x;
        if (land_sea_map[idx] > 0) {
          partition_map[idx] = partition_start_id;
        }
      }
    }
    return;
  }

  int p1 = target_partitions / 2;
  int p2 = target_partitions - p1;

  int target_s1 = static_cast<int>(
      std::round(static_cast<double>(total_sea) * static_cast<double>(p1) /
                 static_cast<double>(target_partitions)));
  target_s1 = std::clamp(target_s1, 1, total_sea - 1);

  bool split_x = (x_max - x_min >= y_max - y_min);

  if (split_x && x_max > x_min) {
    int current_sea = 0;
    int best_x = x_min;
    int best_diff = total_sea + 1;

    for (int x = x_min; x < x_max; ++x) {
      int sea_in_col =
          countSeaPointsInRect(x, x, y_min, y_max, nx, land_sea_map);
      current_sea += sea_in_col;
      int diff = std::abs(current_sea - target_s1);
      if (diff < best_diff) {
        best_diff = diff;
        best_x = x;
      }
    }

    recursiveBisectDomain(x_min, best_x, y_min, y_max, nx, land_sea_map, p1,
                          partition_start_id, partition_map);
    recursiveBisectDomain(best_x + 1, x_max, y_min, y_max, nx, land_sea_map, p2,
                          partition_start_id + p1, partition_map);
  } else if (y_max > y_min) {
    int current_sea = 0;
    int best_y = y_min;
    int best_diff = total_sea + 1;

    for (int y = y_min; y < y_max; ++y) {
      int sea_in_row =
          countSeaPointsInRect(x_min, x_max, y, y, nx, land_sea_map);
      current_sea += sea_in_row;
      int diff = std::abs(current_sea - target_s1);
      if (diff < best_diff) {
        best_diff = diff;
        best_y = y;
      }
    }

    recursiveBisectDomain(x_min, x_max, y_min, best_y, nx, land_sea_map, p1,
                          partition_start_id, partition_map);
    recursiveBisectDomain(x_min, x_max, best_y + 1, y_max, nx, land_sea_map, p2,
                          partition_start_id + p1, partition_map);
  } else {
    for (int y = y_min; y <= y_max; ++y) {
      for (int x = x_min; x <= x_max; ++x) {
        int idx = y * nx + x;
        if (land_sea_map[idx] > 0) {
          partition_map[idx] = partition_start_id;
        }
      }
    }
  }
}

} // namespace

// --- partitionGlobalGrid -------------------------------------------------

std::vector<int> partitionGlobalGrid(int nx, int ny,
                                     std::span<const int> land_sea_map,
                                     int num_partitions) {
  if (nx <= 0 || ny <= 0 || num_partitions <= 0 ||
      land_sea_map.size() != static_cast<size_t>(nx * ny)) {
    return {};
  }

  std::vector<int> partition_map(nx * ny, 0);

  recursiveBisectDomain(0, nx - 1, 0, ny - 1, nx, land_sea_map, num_partitions,
                        1, partition_map);

  return partition_map;
}

// --- writePartitionMapNetCDF ---------------------------------------------

bool writePartitionMapNetCDF(std::string_view filename, int nx, int ny,
                             std::span<const int> partition_map,
                             int num_partitions,
                             std::span<const int> land_sea_map) {
  if (nx <= 0 || ny <= 0 || num_partitions <= 0 ||
      partition_map.size() != static_cast<size_t>(nx * ny)) {
    return false;
  }

  std::ofstream os(std::string(filename), std::ios::binary);
  if (!os.is_open()) {
    return false;
  }

  // 1. Magic
  os.write("CDF\x01", 4);

  // 2. numrecs
  writeU32(os, 0);

  // 3. dim_list
  writeU32(os, NC_DIMENSION_TAG);
  writeU32(os, 2);
  writeNcString(os, "y");
  writeU32(os, static_cast<uint32_t>(ny));
  writeNcString(os, "x");
  writeU32(os, static_cast<uint32_t>(nx));

  // 4. att_list (Global attributes)
  writeU32(os, NC_ATTRIBUTE_TAG);
  writeU32(os, 2);

  writeNcCharAttribute(os, "title", "WW4 Grid Partitioning Map File");
  const int npart_val = num_partitions;
  writeNcIntAttribute(os, "num_partitions",
                      std::span<const int>(&npart_val, 1));

  // 5. var_list
  bool has_ls_map = (!land_sea_map.empty() &&
                     land_sea_map.size() == static_cast<size_t>(nx * ny));
  uint32_t var_count = has_ls_map ? 2 : 1;

  writeU32(os, NC_VARIABLE_TAG);
  writeU32(os, var_count);

  uint32_t var_bytes = static_cast<uint32_t>(nx * ny * 4);

  std::ostringstream var0_ss(std::ios::binary);
  writeNcString(var0_ss, "partition_map");
  writeU32(var0_ss, 2);
  writeU32(var0_ss, 0);
  writeU32(var0_ss, 1);
  writeU32(var0_ss, NC_ATTRIBUTE_TAG);
  writeU32(var0_ss, 2);
  writeNcCharAttribute(var0_ss, "long_name", "Domain partitioning map");
  const int fill_val = 0;
  writeNcIntAttribute(var0_ss, "_FillValue",
                      std::span<const int>(&fill_val, 1));
  writeU32(var0_ss, NC_INT_TYPE);
  writeU32(var0_ss, var_bytes);

  std::string var0_meta = var0_ss.str();

  std::string var1_meta;
  if (has_ls_map) {
    std::ostringstream var1_ss(std::ios::binary);
    writeNcString(var1_ss, "land_sea_map");
    writeU32(var1_ss, 2);
    writeU32(var1_ss, 0);
    writeU32(var1_ss, 1);
    writeU32(var1_ss, NC_ATTRIBUTE_TAG);
    writeU32(var1_ss, 1);
    writeNcCharAttribute(var1_ss, "long_name", "Land sea mask map");
    writeU32(var1_ss, NC_INT_TYPE);
    writeU32(var1_ss, var_bytes);
    var1_meta = var1_ss.str();
  }

  uint32_t current_pos = static_cast<uint32_t>(os.tellp());
  uint32_t var0_offset =
      current_pos + static_cast<uint32_t>(var0_meta.size()) + 4;
  if (has_ls_map) {
    var0_offset += static_cast<uint32_t>(var1_meta.size()) + 4;
  }

  // Write var 0 metadata and offset
  os.write(var0_meta.data(), var0_meta.size());
  writeU32(os, var0_offset);

  if (has_ls_map) {
    uint32_t var1_offset = var0_offset + var_bytes;
    os.write(var1_meta.data(), var1_meta.size());
    writeU32(os, var1_offset);
  }

  // 6. Data Section
  for (int val : partition_map) {
    writeU32(os, static_cast<uint32_t>(val));
  }

  if (has_ls_map) {
    for (int val : land_sea_map) {
      writeU32(os, static_cast<uint32_t>(val));
    }
  }

  return os.good();
}

// --- readPartitionMapNetCDF ----------------------------------------------

bool readPartitionMapNetCDF(std::string_view filename, int &nx, int &ny,
                            int &num_partitions,
                            std::vector<int> &partition_map) {
  std::ifstream is(std::string(filename), std::ios::binary);
  if (!is.is_open()) {
    return false;
  }

  char magic[4];
  if (!is.read(magic, 4)) {
    return false;
  }

  if (magic[0] != 'C' || magic[1] != 'D' || magic[2] != 'F' ||
      (magic[3] != 1 && magic[3] != 2)) {
    return false;
  }

  uint32_t numrecs = 0;
  if (!readU32(is, numrecs)) {
    return false;
  }

  // Read dimensions
  uint32_t dim_tag = 0;
  if (!readU32(is, dim_tag)) {
    return false;
  }

  nx = 0;
  ny = 0;

  if (dim_tag == NC_DIMENSION_TAG) {
    uint32_t dim_cnt = 0;
    if (!readU32(is, dim_cnt)) {
      return false;
    }
    for (uint32_t i = 0; i < dim_cnt; ++i) {
      std::string dname;
      uint32_t dlen = 0;
      if (!readNcString(is, dname) || !readU32(is, dlen)) {
        return false;
      }
      if (dname == "x") {
        nx = static_cast<int>(dlen);
      } else if (dname == "y") {
        ny = static_cast<int>(dlen);
      }
    }
  } else if (dim_tag == 0) {
    uint32_t dummy = 0;
    readU32(is, dummy);
  }

  // Read attributes
  uint32_t att_tag = 0;
  if (!readU32(is, att_tag)) {
    return false;
  }

  num_partitions = 1;

  if (att_tag == NC_ATTRIBUTE_TAG) {
    uint32_t att_cnt = 0;
    if (!readU32(is, att_cnt)) {
      return false;
    }
    for (uint32_t i = 0; i < att_cnt; ++i) {
      std::string aname;
      uint32_t atype = 0;
      uint32_t alen = 0;
      if (!readNcString(is, aname) || !readU32(is, atype) ||
          !readU32(is, alen)) {
        return false;
      }
      if (atype == NC_CHAR_TYPE) {
        if (alen > 0) {
          is.ignore(alen);
        }
        size_t pad = (4 - (alen % 4)) % 4;
        if (pad > 0) {
          is.ignore(pad);
        }
      } else if (atype == NC_INT_TYPE) {
        for (uint32_t k = 0; k < alen; ++k) {
          uint32_t ival = 0;
          readU32(is, ival);
          if (aname == "num_partitions" && k == 0) {
            num_partitions = static_cast<int>(ival);
          }
        }
      }
    }
  } else if (att_tag == 0) {
    uint32_t dummy = 0;
    readU32(is, dummy);
  }

  // Read variables
  uint32_t var_tag = 0;
  if (!readU32(is, var_tag)) {
    return false;
  }

  uint32_t pm_offset = 0;

  if (var_tag == NC_VARIABLE_TAG) {
    uint32_t var_cnt = 0;
    if (!readU32(is, var_cnt)) {
      return false;
    }
    for (uint32_t i = 0; i < var_cnt; ++i) {
      std::string vname;
      uint32_t ndims = 0;
      if (!readNcString(is, vname) || !readU32(is, ndims)) {
        return false;
      }
      for (uint32_t d = 0; d < ndims; ++d) {
        uint32_t did = 0;
        readU32(is, did);
      }
      uint32_t var_att_tag = 0;
      readU32(is, var_att_tag);
      if (var_att_tag == NC_ATTRIBUTE_TAG) {
        uint32_t vacnt = 0;
        readU32(is, vacnt);
        for (uint32_t k = 0; k < vacnt; ++k) {
          std::string vaname;
          uint32_t vatype = 0, valen = 0;
          readNcString(is, vaname);
          readU32(is, vatype);
          readU32(is, valen);
          if (vatype == NC_CHAR_TYPE) {
            if (valen > 0) {
              is.ignore(valen);
            }
            size_t pad = (4 - (valen % 4)) % 4;
            if (pad > 0) {
              is.ignore(pad);
            }
          } else if (vatype == NC_INT_TYPE) {
            for (uint32_t x = 0; x < valen; ++x) {
              uint32_t ival = 0;
              readU32(is, ival);
            }
          }
        }
      } else if (var_att_tag == 0) {
        uint32_t dummy = 0;
        readU32(is, dummy);
      }

      uint32_t vtype = 0, vsize = 0, voffset = 0;
      if (!readU32(is, vtype) || !readU32(is, vsize) || !readU32(is, voffset)) {
        return false;
      }

      if (vname == "partition_map") {
        pm_offset = voffset;
      }
    }
  }

  if (nx <= 0 || ny <= 0 || pm_offset == 0) {
    return false;
  }

  is.seekg(pm_offset);
  partition_map.resize(nx * ny);

  for (int i = 0; i < nx * ny; ++i) {
    uint32_t val = 0;
    if (!readU32(is, val)) {
      return false;
    }
    partition_map[i] = static_cast<int>(val);
  }

  return true;
}

} // namespace ww4_utils
