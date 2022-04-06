#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "buffer.h"
#include "common.h"
#include "program.h"
#include "shader.h"
#include "texture.h"
#include "vertex_layout.h"

CLASS_PTR(Context)
class Context {
  public:
    static ContextUPtr Create();
    void Render();
    void ProcessInput(GLFWwindow *window);
    void MouseButton(int button, int action, double x, double y);
    void MouseMove(double x, double y);
    void Reshape(const int width, const int height);

  private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;
    ProgramUPtr m_simpleProgram;

    int m_width{WINDOW_WIDTH};
    int m_height{WINDOW_HEIGHT};

    VertexLayoutUPtr m_vertexLayout;
    BufferUPtr m_vertexBuffer;
    BufferUPtr m_indexBuffer;

    // camera parameters
    bool m_animaiton{true};
    bool m_cameraControl{false};
    glm::vec2 m_prevMousePos{glm::vec2(0.0f)};

    glm::vec3 m_cameraPos{glm::vec3(0.0f, 0.0f, 3.0f)};
    glm::vec3 m_cameraFront{glm::vec3(0.0f, 0.0f, -1.0f)};
    glm::vec3 m_cameraUp{glm::vec3(0.0f, 1.0f, 0.0f)};

    float m_cameraPitch{0.0f};
    float m_cameraYaw{0.0f};

    glm::vec4 m_clearColor{glm::vec4(0.1f, 0.2f, 0.3f, 0.0f)};

    struct Light {
        glm::vec3 position{glm::vec3(3.0f, 3.0f, 3.0f)};
        glm::vec3 ambient{glm::vec3(0.1f, 0.1f, 0.1f)};
        glm::vec3 diffuse{glm::vec3(0.5f, 0.5f, 0.5f)};
        glm::vec3 specular{glm::vec3(1.0f, 1.0f, 1.0f)};
    };
    Light m_light;

    // material parameter
    struct Material {
        TextureUPtr diffuse;
        TextureUPtr specular;
        float shininess{32.0f};
    };
    Material m_material;
};

#endif