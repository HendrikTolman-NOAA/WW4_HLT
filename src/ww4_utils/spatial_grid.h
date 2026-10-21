/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file spatial_grid.h
 * @brief Data structures for spatial grid representation in WAVEWATCH IV.
 * @details Defines FullGrid and LocalGridPartition data structures
 *          for representing full domain and local partitioned grid data.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-10-21
 * @date Last update : 2026-10-21
 */

#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <vector>

namespace ww4_utils {

// --- GridType ---------------------------------------------------------------
/**
 * @enum GridType
 * @brief Spatial grid topology classifications in WAVEWATCH IV.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-10-21
 * @date Last update : 2026-10-21
 * @var GridType::Undefined
 * @brief Grid type not yet set or defined.
 * @var GridType::Rectangular
 * @brief Structured 2D rectilinear grid.
 * @var GridType::Triangular
 * @brief Unstructured triangular mesh conforming to UGRID conventions.
 * @var GridType::SMC
 * @brief Spherical Multiple-Cell (SMC) grid.
 */
enum class GridType { Undefined, Rectangular, Triangular, SMC };

// --- FullGrid ---------------------------------------------------------------
/**
 * @struct FullGrid
 * @brief Full model domain spatial grid metadata and static data buffers.
 * @details Holds global spatial dimensions, longitudes, latitudes,
 *          unified topobathy elevation field, land/sea mask, and cell
 * topologies.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-10-21
 * @date Last update : 2026-10-21
 */
struct FullGrid {
  GridType gridType = GridType::Undefined;
  std::string name;

  std::size_t nx = 0;
  std::size_t ny = 0;
  std::size_t numNodes = 0;
  std::size_t numElements = 0;

  std::vector<double> xCoordinates;
  std::vector<double> yCoordinates;
  std::vector<double> topobathy;
  std::vector<int> landMask;
  std::vector<int> faceNodeConnectivity;

  /**
   * @brief Access non-owning span of longitude/x coordinates.
   * @return std::span<const double>
   */
  [[nodiscard]] std::span<const double> getXCoordinates() const noexcept {
    return xCoordinates;
  }

  /**
   * @brief Access non-owning span of latitude/y coordinates.
   * @return std::span<const double>
   */
  [[nodiscard]] std::span<const double> getYCoordinates() const noexcept {
    return yCoordinates;
  }

  /**
   * @brief Access non-owning span of the unified topobathy elevation field.
   * @return std::span<const double>
   */
  [[nodiscard]] std::span<const double> getTopobathy() const noexcept {
    return topobathy;
  }

  /**
   * @brief Access non-owning span of the land/sea mask array.
   * @return std::span<const int>
   */
  [[nodiscard]] std::span<const int> getLandMask() const noexcept {
    return landMask;
  }

  /**
   * @brief Access non-owning span of element face-node connectivity (for
   * UGRID).
   * @return std::span<const int>
   */
  [[nodiscard]] std::span<const int> getFaceNodeConnectivity() const noexcept {
    return faceNodeConnectivity;
  }

  /**
   * @brief Resets all full grid fields and arrays to default empty state.
   */
  void clear() noexcept;
};

// --- LocalGridPartition -----------------------------------------------------
/**
 * @struct LocalGridPartition
 * @brief Sub-domain local compute grid for MPI execution or local partitions.
 * @details Encapsulates local sub-mesh dimensions, local coordinate vectors,
 *          contiguous local topobathy buffers, local land/sea mask, halo cell
 *          counts, and local-to-global cell index mappings.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-10-21
 * @date Last update : 2026-10-21
 */
struct LocalGridPartition {
  GridType gridType = GridType::Undefined;
  int rankId = 0;

  std::size_t localNx = 0;
  std::size_t localNy = 0;
  std::size_t localNumNodes = 0;

  std::size_t haloSizeX = 0;
  std::size_t haloSizeY = 0;

  std::vector<double> xCoordinates;
  std::vector<double> yCoordinates;
  std::vector<double> topobathy;
  std::vector<int> landMask;
  std::vector<std::size_t> localToGlobalMap;

  /**
   * @brief Access non-owning span of local longitude/x coordinates.
   * @return std::span<const double>
   */
  [[nodiscard]] std::span<const double> getXCoordinates() const noexcept {
    return xCoordinates;
  }

  /**
   * @brief Access non-owning span of local latitude/y coordinates.
   * @return std::span<const double>
   */
  [[nodiscard]] std::span<const double> getYCoordinates() const noexcept {
    return yCoordinates;
  }

  /**
   * @brief Access non-owning span of local topobathy elevation field.
   * @return std::span<const double>
   */
  [[nodiscard]] std::span<const double> getTopobathy() const noexcept {
    return topobathy;
  }

  /**
   * @brief Access non-owning span of local land/sea mask array.
   * @return std::span<const int>
   */
  [[nodiscard]] std::span<const int> getLandMask() const noexcept {
    return landMask;
  }

  /**
   * @brief Access non-owning span of local-to-global cell index mapping.
   * @return std::span<const std::size_t>
   */
  [[nodiscard]] std::span<const std::size_t>
  getLocalToGlobalMap() const noexcept {
    return localToGlobalMap;
  }

  /**
   * @brief Resets all local partition fields and arrays to default empty state.
   */
  void clear() noexcept;
};

} // namespace ww4_utils
