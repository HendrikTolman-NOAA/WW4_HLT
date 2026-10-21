/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file spatial_grid.cpp
 * @brief Implementation of spatial grid utility methods in WAVEWATCH IV.
 * @details Implements memory reset routines for FullGrid and
 * LocalGridPartition.
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

#include "ww4_utils/spatial_grid.h"

namespace ww4_utils {

// --- FullGrid ---------------------------------------------------------------

void FullGrid::clear() noexcept {
  gridType = GridType::Undefined;
  name.clear();
  nx = 0;
  ny = 0;
  numNodes = 0;
  numElements = 0;
  xCoordinates.clear();
  yCoordinates.clear();
  topobathy.clear();
  landMask.clear();
  faceNodeConnectivity.clear();
}

// --- LocalGridPartition -----------------------------------------------------

void LocalGridPartition::clear() noexcept {
  gridType = GridType::Undefined;
  rankId = 0;
  localNx = 0;
  localNy = 0;
  localNumNodes = 0;
  haloSizeX = 0;
  haloSizeY = 0;
  xCoordinates.clear();
  yCoordinates.clear();
  topobathy.clear();
  landMask.clear();
  localToGlobalMap.clear();
}

} // namespace ww4_utils
