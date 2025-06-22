#pragma once

#include <chrono>
#include <string>
#include <string_view>

namespace util {
    namespace time {
        /// @brief Specifies the time zone for time operations
        enum class Zone {
            Local,  ///< System's local time zone
            UTC     ///< Coordinated Universal Time (UTC)
        };

        /// @brief Converts a time point to a string using specified format and time zone
        /// @param timestamp Time point to convert
        /// @param format Output string format (default: "%Y-%m-%d %H:%M:%S")
        /// @param zone Time zone for conversion (default: Local)
        /// @return Formatted string representing the given time
        /// @throws std::runtime_error if formatting fails
        std::string toString(
            const std::chrono::system_clock::time_point& timestamp, 
            std::string_view format = "%Y-%m-%d %H:%M:%S",
            Zone zone = Zone::Local);

        /// @brief Parses a string to a time point using specified format and time zone
        /// @param timeStr String containing time representation to parse
        /// @param format Input string format (default: "%Y-%m-%d %H:%M:%S")
        /// @param zone Time zone for parsing (default: Local)
        /// @return Parsed time point
        /// @throws std::runtime_error if parsing fails
        std::chrono::system_clock::time_point fromString(
            std::string_view timeStr, 
            std::string_view format = "%Y-%m-%d %H:%M:%S",
            Zone zone = Zone::Local);

        /// @brief Converts time between different time zones
        /// @param time Time point to convert
        /// @param from_zone Source time zone name (e.g., "UTC", "Europe/Warsaw")
        /// @param to_zone Target time zone name
        /// @return Time point converted to target time zone
        /// @throws std::runtime_error if time zone name is invalid
        std::chrono::system_clock::time_point convertTimeZone(
            const std::chrono::system_clock::time_point& time,
            const std::string& from_zone,
            const std::string& to_zone);

        /// @brief Checks if given time point is within specified range
        /// @param time Time point to check
        /// @param start Range start time (inclusive)
        /// @param end Range end time (inclusive)
        /// @return true if time is within range, false otherwise
        bool isInRange(
            const std::chrono::system_clock::time_point& time,
            const std::chrono::system_clock::time_point& start,
            const std::chrono::system_clock::time_point& end);

        /// @brief Rounds time point to nearest multiple of specified resolution
        /// @param time Time point to round
        /// @param resolution Rounding resolution in minutes
        /// @return Rounded time point
        /// @example roundToNearest(time, 15min) will round time to nearest 15 minutes
        std::chrono::system_clock::time_point roundToNearest(
            const std::chrono::system_clock::time_point& time,
            const std::chrono::minutes& resolution);

        /// @brief Predefined time formats
        namespace format {
            /// @brief ISO 8601 format (YYYY-MM-DDThh:mm:ss)
            constexpr std::string_view ISO8601 = "%Y-%m-%dT%H:%M:%S";
            
            /// @brief Date only format (YYYY-MM-DD)
            constexpr std::string_view DATE_ONLY = "%Y-%m-%d";
            
            /// @brief Time only format (hh:mm:ss)
            constexpr std::string_view TIME_ONLY = "%H:%M:%S";
            
            /// @brief Standard date and time format (YYYY-MM-DD hh:mm:ss)
            constexpr std::string_view DATETIME = "%Y-%m-%d %H:%M:%S";
            
            /// @brief RFC 3339 format (YYYY-MM-DDThh:mm:ss+zz:zz)
            constexpr std::string_view RFC3339 = "%Y-%m-%dT%H:%M:%S%z";
        }
    }
}
