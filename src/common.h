#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef WIN32
#include <windows.h>
#endif
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

#define CLASS_PTR(klassName)                          \
  class klassName;                                    \
  using klassName##UPtr = std::unique_ptr<klassName>; \
  using klassName##Ptr = std::shared_ptr<klassName>;  \
  using klassName##WPtr = std::weak_ptr<klassName>;

std::optional<std::string> LoadTextFile(const std::string &filename);

#endif  // __COMMON_H__