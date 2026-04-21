/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_output_utils.cpp
 * @brief Implementation of model output utility routines.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-21
 */

#include "ww4_utils/ww4_output_utils.hpp"
#include <algorithm>

namespace ww4_utils {

void assessOutputConfig(OutputConfig &oc, const DateTime &modelTime, bool isApi,
                        const std::optional<DateTime> &waveEndTime) {
  if (!oc.requested) {
    return;
  }

  // 1. For API output, set actualTime to waveEndTime
  if (isApi && waveEndTime.has_value()) {
    oc.actualTime = *waveEndTime;
    return;
  }

  // 2. If startTime is not set, set it to modelTime
  if (!oc.startTime.has_value()) {
    oc.startTime = modelTime;
  }

  // 3. If actualTime is not set, set it to startTime
  if (!oc.actualTime.has_value()) {
    oc.actualTime = oc.startTime;
  }

  // 4. If actualTime is before present modelTime, increment it
  while (oc.actualTime.has_value() && TimeManagement::differenceInSeconds(
                                          *oc.actualTime, modelTime) > 0.001) {
    TimeManagement::incrementDateTime(*oc.actualTime, oc.interval);
  }

  // 5. If modelTime is past endTime, switch output off
  if (oc.endTime.has_value() &&
      TimeManagement::differenceInSeconds(*oc.endTime, modelTime) > 0.001) {
    oc.requested = false;
  }
}

double computeNextOutputStep(const OutputConfig &oc,
                             const DateTime &modelTime) {
  if (!oc.requested || !oc.actualTime.has_value()) {
    return 1.0e10; // Large value
  }

  double dt = TimeManagement::differenceInSeconds(modelTime, *oc.actualTime);
  return std::max(0.0, dt);
}

bool isOutputDue(const OutputConfig &oc, const DateTime &modelTime) {
  if (!oc.requested || !oc.actualTime.has_value()) {
    return false;
  }

  return std::abs(TimeManagement::differenceInSeconds(modelTime,
                                                      *oc.actualTime)) < 0.001;
}

void updateOutputTime(OutputConfig &oc, const DateTime &modelTime) {
  if (!oc.requested || !oc.actualTime.has_value()) {
    return;
  }

  if (std::abs(TimeManagement::differenceInSeconds(modelTime, *oc.actualTime)) <
      0.001) {
    TimeManagement::incrementDateTime(*oc.actualTime, oc.interval);
  }
}

} // namespace ww4_utils
