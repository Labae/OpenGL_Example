#include "shader.h"

ShaderUPtr Shader::CreateFromFile(const std::string &fileName,
                                  GLenum shaderType) {
  auto shader = ShaderUPtr(new Shader());

  if (!shader->LoadFile(fileName, shaderType)) {
    return nullptr;
  }

  return std::move(shader);
}

Shader::~Shader() {
  if (m_shader) {
    glDeleteShader(m_shader);
  }
}

bool Shader::LoadFile(const std::string &fileName, GLenum shaderType) {
  auto result = LoadTextFile(fileName);
  if (!result.has_value()) {
    return false;
  }

  auto &code = result.value();
  const char *codePtr = code.c_str();
  int32_t codeLength = static_cast<int32_t>(code.length());

  m_shader = glCreateShader(shaderType);
  glShaderSource(m_shader, 1, (const GLchar *const *)&codePtr, &codeLength);
  glCompileShader(m_shader);

  int success = 0;
  glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[1024];
    glGetShaderInfoLog(m_shader, 1024, nullptr, infoLog);
    SPDLOG_ERROR("Failed to compile shader: \"{}\"", fileName);
    SPDLOG_ERROR("Reason: {}", infoLog);
    return false;
  }

  return true;
}
