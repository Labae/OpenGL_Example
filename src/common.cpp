#include "common.h"

#include <fstream>
#include <sstream>

std::optional<std::string> LoadTextFile(const std::string &fileName) {
  std::ifstream fin(fileName);
  if (!fin.is_open()) {
    SPDLOG_INFO("Failed to open file : {}", fileName);
    return {};
  }

  std::stringstream ss;
  ss << fin.rdbuf();
  return ss.str();
}
