#include "MatrixStack.h"
#include "GL/freeglut.h"

MatrixStack::MatrixStack() {
  // Initialize the matrix stack with the identity matrix.
  m_matrices.push_back(Matrix4f::identity());
  glLoadMatrixf(m_matrices.back());
}

void MatrixStack::clear() {
  // Revert to just containing the identity matrix.
  m_matrices.clear();
  m_matrices.push_back(Matrix4f::identity());
  glLoadMatrixf(m_matrices.back());
}

Matrix4f MatrixStack::top() {
  // Return the top of the stack
  return m_matrices.back();
}

void MatrixStack::push(const Matrix4f &m) {
  // Push m onto the stack.
  // Your stack should have OpenGL semantics:
  // the new top should be the old top multiplied by m
  m_matrices.push_back(m_matrices.back() * m);
  glLoadMatrixf(m_matrices.back());
}

void MatrixStack::pop() {
  // Remove the top element from the stack
  m_matrices.pop_back();
  glLoadMatrixf(m_matrices.back());
}
