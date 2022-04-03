#include "context.h"

#include "image.h"

ContextUPtr Context::Create() {
  auto context = ContextUPtr(new Context());
  if (!context->Init()) {
    return nullptr;
  }

  return std::move(context);
}

bool Context::Init() {
  float vertices[] = {// x,  y,  z,  r,  g,  b,  s,  t
                      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                      -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f};

  uint32_t indices[] = {0, 1, 3, 1, 2, 3};

  m_vertexLayout = VertexLayout::Create();

  m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW,
                                          vertices, sizeof(float) * 32);

  m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
  m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8,
                            sizeof(float) * 3);
  m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8,
                            sizeof(float) * 6);

  m_indexBuffer = Buffer::CreateWithData(
      GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 6);

  ShaderPtr vertexShader =
      Shader::CreateFromFile("./shader/texture.vs", GL_VERTEX_SHADER);
  ShaderPtr fragmentShader =
      Shader::CreateFromFile("./shader/texture.fs", GL_FRAGMENT_SHADER);
  if (!vertexShader || !fragmentShader) {
    return false;
  }

  SPDLOG_INFO("Vertex shader id : {}", vertexShader->Get());
  SPDLOG_INFO("Fragment shader id : {}", fragmentShader->Get());
  m_program = Program::Create({fragmentShader, vertexShader});
  if (!m_program) {
    return false;
  }
  SPDLOG_INFO("Program id : {}", m_program->Get());

  glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

  auto image = Image::Load("./image/container.jpg");
  if (!image) {
    return false;
  }

  SPDLOG_INFO("Image : {}x{}, {} channels", image->GetWidth(),
              image->GetHeight(), image->GetChannelCount());

  m_texture = Texture::CreateFromImage(image.get());

  return true;
}

void Context::Render() {
  glClear(GL_COLOR_BUFFER_BIT);
  m_program->Use();
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}