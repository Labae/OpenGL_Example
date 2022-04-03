#include "vertex_layout.h"

VertexLayoutUPtr VertexLayout::Create() {
  auto vertexLayout = VertexLayoutUPtr(new VertexLayout());
  vertexLayout->Init();
  return std::move(vertexLayout);
}

VertexLayout::~VertexLayout() {
  if (m_vertexArrayObject) {
    glDeleteVertexArrays(1, &m_vertexArrayObject);
  }
}

void VertexLayout::Bind() const { glBindVertexArray(m_vertexArrayObject); }

void VertexLayout::SetAttrib(uint32_t attribIndex, int count, uint32_t type,
                             bool normalized, size_t stride,
                             size_t offset) const {
  glEnableVertexAttribArray(attribIndex);
  glVertexAttribPointer(attribIndex, count, type, normalized, stride,
                        reinterpret_cast<const GLvoid *>(offset));
}

void VertexLayout::DisableAttrib(uint32_t attribIndex) {
  glDisableVertexAttribArray(attribIndex);
}

void VertexLayout::Init() {
  glGenVertexArrays(1, &m_vertexArrayObject);
  Bind();
}