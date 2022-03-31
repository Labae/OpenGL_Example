#ifndef __SHADER_H_
#define __SHADER_H_

#include "common.h"

CLASS_PTR(Shader);
class Shader
{
public:
    static ShaderUPtr CreateFromFile(const std::string& fileName, GLenum shaderType);

    ~Shader();
    uint32_t Get() const { return m_Shader; }

private:
    Shader() {}
    bool LoadFile(const std::string& fileName, GLenum shaderType);
    uint32_t m_Shader { 0 };
}

#endif  //__SHADER_H_