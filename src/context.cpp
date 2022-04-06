#include "context.h"

#include <imgui.h>

#include "image.h"

ContextUPtr Context::Create() {
  auto context = ContextUPtr(new Context());
  if (!context->Init()) {
    return nullptr;
  }

  return std::move(context);
}

bool Context::Init() {
  m_box = Mesh::CreateBox();
  m_model = Model::Load("./model/backpack.obj");
  if (!m_model) return false;

  m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
  if (!m_simpleProgram) return false;

  m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
  if (!m_program) return false;

  SPDLOG_INFO("Program id : {}", m_program->Get());

  m_material.diffuse = Texture::CreateFromImage(
      Image::CreateSingleImage(4, 4, glm::vec4(1.0f)).get());
  if (!m_material.diffuse) return false;

  m_material.specular = Texture::CreateFromImage(
      Image::CreateSingleImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());
  if (!m_material.specular) return false;

  glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

  glEnable(GL_DEPTH_TEST);

  return true;
}

void Context::Render() {
  if (ImGui::Begin("UI Window")) {
    if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
      ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction),
                        0.01f);
      ImGui::DragFloat2("l.cutoff", glm::value_ptr(m_light.cutoff), 0.5f, 0.0f,
                        180.0f);
      ImGui::DragFloat("l.distance", &m_light.distance, 0.5f, 0.0f, 3000.0f);
      ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
      ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
      ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
    }

    if (ImGui::CollapsingHeader("material", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::DragFloat("m.shininess", &m_material.shininess, 1.0f, 1.0f,
                       256.0f);
    }

    ImGui::Separator();
    ImGui::Checkbox("animation", &m_animaiton);

    ImGui::Separator();
    if (ImGui::ColorEdit4("Clear color", glm::value_ptr(m_clearColor))) {
      glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b,
                   m_clearColor.w);
    }

    ImGui::Separator();
    ImGui::DragFloat3("Camera pos", glm::value_ptr(m_cameraPos), 0.01f);
    ImGui::DragFloat("Camera yaw", &m_cameraYaw, 0.5f);
    ImGui::DragFloat("Camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);

    ImGui::Separator();
    if (ImGui::Button("Reset camera")) {
      m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
      m_cameraYaw = 0.0f;
      m_cameraPitch = 0.0f;
    }
  }
  ImGui::End();

  std::vector<glm::vec3> cubePositions = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f),
  };

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_cameraFront = glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw),
                              glm::vec3(0.0f, 1.0f, 0.0f)) *
                  glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch),
                              glm::vec3(1.0f, 0.0f, 0.0f)) *
                  glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

  // m_light.position = m_cameraPos;
  // m_light.direction = m_cameraFront;

  auto projection = glm::perspective(
      glm::radians(45.0f), (float)m_width / (float)m_height, 0.01f, 100.0f);

  auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

  auto lightModelTransform = glm::translate(glm::mat4(1.0), m_light.position) *
                             glm::scale(glm::mat4(1.0), glm::vec3(0.1f));

  m_simpleProgram->Use();
  m_simpleProgram->SetUniform(
      "color", glm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
  m_simpleProgram->SetUniform("transform",
                              projection * view * lightModelTransform);
  m_box->Draw(m_simpleProgram.get());

  m_program->Use();
  m_program->SetUniform("viewPos", m_cameraPos);
  m_program->SetUniform("light.position", m_light.position);
  m_program->SetUniform("light.direction", m_light.direction);
  m_program->SetUniform(
      "light.cutoff",
      glm::vec2(cosf(glm::radians(m_light.cutoff[0])),
                cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
  m_program->SetUniform("light.attenuation",
                        GetAttenuationCoeff(m_light.distance));
  m_program->SetUniform("light.ambient", m_light.ambient);
  m_program->SetUniform("light.diffuse", m_light.diffuse);
  m_program->SetUniform("light.specular", m_light.specular);

  m_program->SetUniform("material.diffuse", 0);
  m_program->SetUniform("material.specular", 1);
  m_program->SetUniform("material.shininess", m_material.shininess);
  glActiveTexture(GL_TEXTURE0);
  m_material.diffuse->Bind();
  glActiveTexture(GL_TEXTURE1);
  m_material.specular->Bind();

  auto modelTransform = glm::mat4(1.0f);
  auto transform = projection * view * modelTransform;
  m_program->SetUniform("transform", transform);
  m_program->SetUniform("modelTransform", modelTransform);
  m_model->Draw(m_program.get());
}

void Context::ProcessInput(GLFWwindow *window) {
  if (!m_cameraControl) return;

  const float cameraSpeed = 0.05f;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    m_cameraPos += cameraSpeed * m_cameraFront;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    m_cameraPos -= cameraSpeed * m_cameraFront;

  auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    m_cameraPos += cameraSpeed * cameraRight;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    m_cameraPos -= cameraSpeed * cameraRight;

  auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    m_cameraPos += cameraSpeed * cameraUp;
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    m_cameraPos -= cameraSpeed * cameraUp;
}

void Context::MouseButton(int button, int action, double x, double y) {
  if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    if (action == GLFW_PRESS) {
      m_prevMousePos = glm::vec2((float)x, (float)y);
      m_cameraControl = true;
    } else if (action == GLFW_RELEASE) {
      m_cameraControl = false;
    }
  }
}

void Context::MouseMove(double x, double y) {
  if (!m_cameraControl) return;

  auto pos = glm::vec2((float)x, float(y));
  auto deltaPos = pos - m_prevMousePos;

  const float cameraRotSpeed = 0.4f;
  m_cameraYaw -= deltaPos.x * cameraRotSpeed;
  m_cameraPitch -= deltaPos.y * cameraRotSpeed;

  if (m_cameraYaw < 0.0f) m_cameraYaw += 360.0f;
  if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

  if (m_cameraPitch < -89.0f) m_cameraPitch = 89.0f;
  if (m_cameraPitch > 89.0f) m_cameraPitch = 89.0f;

  m_prevMousePos = pos;
}

void Context::Reshape(const int width, const int height) {
  m_width = width;
  m_height = height;
  glViewport(0, 0, m_width, m_height);
}