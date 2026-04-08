/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file memory_utils.hpp
 * @brief Utilities for capturing memory usage of the current process.
 * @details This header defines the MemoryUsage structure and MemoryUtils class,
 *          providing functionality to read process memory metrics from the
 * operating system.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-02-27
 * @date Last Update, 2026-04-08
 */

#ifndef WW4_UTILS_MEMORY_UTILS_HPP
#define WW4_UTILS_MEMORY_UTILS_HPP

#include <cstdint>
#include <optional>

/**
 * @namespace ww4_utils
 * @brief Utilities for WAVEWATCH IV.
 */
namespace ww4_utils {

/**
 * @brief Sets the path to the process status file for testing purposes.
 * @param path The path to the mock status file.
 * @note This is for internal testing only and should not be used in production.
 */
void setMemoryStatusPathForTesting(const char *path);

/**
 * @struct MemoryUsage
 * @brief Represents various memory usage metrics of a process.
 * @details Values are typically in kilobytes (kB).
 */
struct MemoryUsage {
  std::uint64_t vmPeak{0}; ///< Peak virtual memory size.
  std::uint64_t vmSize{0}; ///< Virtual memory size.
  std::uint64_t vmHWM{0};  ///< Peak resident set size ("High Water Mark").
  std::uint64_t vmRSS{0};  ///< Resident set size.
};

/**
 * @class MemoryUtils
 * @brief Utility class for memory-related operations.
 * @details Provides static methods to query memory usage from the system.
 */
class MemoryUtils {
public:
  /**
   * @brief Captures the current memory usage of the calling process.
   * @details Reads metrics from /proc/self/status on Linux systems.
   * @return A MemoryUsage struct containing the captured metrics, or
   * std::nullopt if capture fails.
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

#endif // WW4_UTILS_MEMORY_UTILS_HPP
