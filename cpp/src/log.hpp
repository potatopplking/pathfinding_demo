#pragma once

#include <iostream>

#define LOG_CRITICAL(...) Log::critical(__PRETTY_FUNCTION__, ": ", __VA_ARGS__)
#define LOG_ERROR(...) Log::error(__PRETTY_FUNCTION__, ": ", __VA_ARGS__)
#define LOG_WARNING(...) Log::warning(__PRETTY_FUNCTION__, ": ", __VA_ARGS__)
#define LOG_INFO(...) Log::info(__PRETTY_FUNCTION__, ": ", __VA_ARGS__)
#define LOG_DEBUG(...) Log::debug(__PRETTY_FUNCTION__, ": ", __VA_ARGS__)
#define LOG_PROFILING_DEBUG(...)                                               \
  Log::profiling_debug(__PRETTY_FUNCTION__, ": ", __VA_ARGS__)

namespace Log {
enum class LevelTypes {
  CRITICAL = 0,    // Things that crash
  ERROR,           // Bad stuff, but we can go on
  WARNING,         // Minor inconvenience
  INFO,            // Normal stuff
  DEBUG,           // Everything. Will slow down execution
  PROFILING_DEBUG, // including constructors etc.
};

// Set logging level here
constexpr LevelTypes Level = LevelTypes::DEBUG;

template <LevelTypes FUNC_LEVEL, typename... Args>
void _print(const Args &...args) {
  if constexpr (Level < FUNC_LEVEL) {
    return;
  }
  (std::cout << ... << args) << std::endl;
}

template <typename... Args> void critical(const Args &...args) {
  _print<LevelTypes::CRITICAL>("CRITICAL: ", args...);
}

template <typename... Args> void error(const Args &...args) {
  _print<LevelTypes::ERROR>("ERROR: ", args...);
}

template <typename... Args> void warning(const Args &...args) {
  _print<LevelTypes::WARNING>("WARNING: ", args...);
}

template <typename... Args> void info(const Args &...args) {
  _print<LevelTypes::INFO>("INFO: ", args...);
}

template <typename... Args> void debug(const Args &...args) {
  _print<LevelTypes::DEBUG>("DEBUG: ", args...);
}

template <typename... Args> void profiling_debug(const Args &...args) {
  _print<LevelTypes::PROFILING_DEBUG>("PROFILING_DEBUG: ", args...);
}
} // namespace Log
