#include "utilities.hpp"
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace util {
    namespace time {
        std::string toString(const std::chrono::system_clock::time_point& timestamp, 
                           std::string_view format, Zone zone) {
            auto time_t = std::chrono::system_clock::to_time_t(timestamp);
            std::tm tm_time;
            
            if (zone == Zone::UTC) {
                gmtime_s(&tm_time, &time_t);
            } else {
                localtime_s(&tm_time, &time_t);
            }

            std::stringstream ss;
            ss << std::put_time(&tm_time, format.data());
            return ss.str();
        }

        std::chrono::system_clock::time_point fromString(std::string_view timeStr, 
                                                        std::string_view format, 
                                                        Zone zone) {
            std::tm tm = {};
            std::stringstream ss(timeStr.data());
            ss >> std::get_time(&tm, format.data());
            
            if (ss.fail()) {
                throw std::runtime_error("Failed to parse time string");
            }

            std::time_t time_t;
            if (zone == Zone::UTC) {
                time_t = _mkgmtime(&tm);
            } else {
                time_t = mktime(&tm);
            }

            return std::chrono::system_clock::from_time_t(time_t);
        }

        std::chrono::system_clock::time_point convertTimeZone(
            const std::chrono::system_clock::time_point& time,
            const std::string& from_zone,
            const std::string& to_zone) {
            // Uproszczona implementacja - zakładamy tylko konwersję między UTC i lokalnym czasem
            auto time_t = std::chrono::system_clock::to_time_t(time);
            std::tm tm_time;

            if (from_zone == "UTC") {
                gmtime_s(&tm_time, &time_t);
                time_t = mktime(&tm_time);
            } else if (to_zone == "UTC") {
                localtime_s(&tm_time, &time_t);
                time_t = _mkgmtime(&tm_time);
            }

            return std::chrono::system_clock::from_time_t(time_t);
        }

        bool isInRange(const std::chrono::system_clock::time_point& time,
                      const std::chrono::system_clock::time_point& start,
                      const std::chrono::system_clock::time_point& end) {
            return time >= start && time <= end;
        }

        std::chrono::system_clock::time_point roundToNearest(
            const std::chrono::system_clock::time_point& time,
            const std::chrono::minutes& resolution) {
            using namespace std::chrono;

            auto since_epoch = time.time_since_epoch();
            auto minutes_since_epoch = duration_cast<minutes>(since_epoch);
            
            auto remainder = minutes_since_epoch.count() % resolution.count();
            auto rounded_minutes = minutes_since_epoch.count() - remainder;
            
            if (remainder >= resolution.count() / 2) {
                rounded_minutes += resolution.count();
            }

            return system_clock::time_point(minutes(rounded_minutes));
        }
    }
}
