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

 private:
  Context() {}
  bool Init();
  ProgramUPtr m_program;

  VertexLayoutUPtr m_vertexLayout;
  BufferUPtr m_vertexBuffer;
  BufferUPtr m_indexBuffer;

  TextureUPtr m_texture;
};

#endif