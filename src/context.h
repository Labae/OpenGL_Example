#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "mesh.h"
#include "model.h"
#include "program.h"
#include "shader.h"
#include "texture.h"
#include "framebuffer.h"

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
  ProgramUPtr m_textureProgram;
  ProgramUPtr m_postProgram;

  int m_width{WINDOW_WIDTH};
  int m_height{WINDOW_HEIGHT};

  MeshUPtr m_box;
  MeshUPtr m_plane;

  // camera parameters
  bool m_animaiton{true};
  bool m_cameraControl{false};
  glm::vec2 m_prevMousePos{glm::vec2(0.0f)};

  glm::vec3 m_cameraPos{glm::vec3(0.0f, 2.5f, 8.0f)};
  glm::vec3 m_cameraFront{glm::vec3(0.0f, 0.0f, -1.0f)};
  glm::vec3 m_cameraUp{glm::vec3(0.0f, 1.0f, 0.0f)};

  float m_cameraPitch{-20.0f};
  float m_cameraYaw{0.0f};

  FramebufferUPtr m_framebuffer;
  float m_gamma{1.0f};

  glm::vec4 m_clearColor{glm::vec4(0.1f, 0.2f, 0.3f, 0.0f)};

  struct Light {
    glm::vec3 position{glm::vec3(1.0f, 4.0f, 4.0f)};
    glm::vec3 direction{glm::vec3(-1.0f, -1.0f, -1.0f)};
    glm::vec2 cutoff{glm::vec2(120.0f, 5.0f)};
    float distance{128.0f};
    glm::vec3 ambient{glm::vec3(0.1f, 0.1f, 0.1f)};
    glm::vec3 diffuse{glm::vec3(0.8f, 0.8f, 0.8f)};
    glm::vec3 specular{glm::vec3(1.0f, 1.0f, 1.0f)};
  };
  Light m_light;

  MaterialPtr m_planeMaterial;
  MaterialPtr m_box1Material;
  MaterialPtr m_box2Material;
  TexturePtr m_windowTexture;
};

#endif