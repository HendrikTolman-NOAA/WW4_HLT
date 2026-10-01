/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file memory_utils.h
 * @brief Utilities for capturing memory usage of the current process.
 * @details This header defines the MemoryUsage structure and MemoryUtils class,
 *          providing functionality to read process memory metrics from the
 *          operating system.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-02-27
 * @date Last update : 2026-09-28
 */

#pragma once

#include <cstdint>
#include <optional>

namespace ww4_utils {

// --- setMemoryStatusPathForTesting ------------------------------------------
/**
 * @brief Sets the path to the process status file for testing purposes.
 * @param path The path to the mock status file.
 * @note This is for internal testing only and should not be used in production.
 */
void setMemoryStatusPathForTesting(const char *path);

// --- resetMemoryStatusPath --------------------------------------------------
/**
 * @brief Resets the memory status path to the default "/proc/self/status".
 */
void resetMemoryStatusPath() noexcept;

// --- MemoryUsage ------------------------------------------------------------
/**
 * @struct MemoryUsage
 * @brief Represents various memory usage metrics of a process.
 * @details Values are typically in kilobytes (kB).
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-02-27
 * @date Last update : 2026-09-28
 * @var MemoryUsage::vmPeak
 * @brief Peak virtual memory size.
 * @var MemoryUsage::vmSize
 * @brief Virtual memory size.
 * @var MemoryUsage::vmHWM
 * @brief Peak resident set size ("High Water Mark").
 * @var MemoryUsage::vmRSS
 * @brief Resident set size.
 */
struct MemoryUsage {
  std::uint64_t vmPeak{0};
  std::uint64_t vmSize{0};
  std::uint64_t vmHWM{0};
  std::uint64_t vmRSS{0};
};

// --- MemoryUtils ------------------------------------------------------------
/**
 * @class MemoryUtils
 * @brief Utility class for memory-related operations.
 * @details Provides static methods to query memory usage from the system.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-02-27
 * @date Last update : 2026-09-28
 */
class MemoryUtils {
public:
  /**
   * @brief Captures the current memory usage of the calling process.
   * @details Reads metrics from /proc/self/status on Linux systems.
   * @return A MemoryUsage struct containing the captured metrics, or
   *         std::nullopt if capture fails.
   * @pre The operating system must provide /proc/self/status (Linux).
   */
  [[nodiscard]] static std::optional<MemoryUsage> captureMemoryUsage() noexcept;

  /**
   * @brief Captures the memory high water mark (HWM) of the calling process.
   * @details Reads the vmHWM metric from /proc/self/status on Linux systems.
   * @return The peak resident set size in kB, or std::nullopt if capture fails.
   * @pre The operating system must provide /proc/self/status (Linux).
   */
  [[nodiscard]] static std::optional<std::uint64_t> captureMemoryHWM() noexcept;
};

} // namespace ww4_utils
