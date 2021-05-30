#include "Mesh.h"
#include "util.h"

using namespace std;

void Mesh::load(const char *filename) {
  // 2.1.1. load() should populate bindVertices, currentVertices, and faces

  // Add your code here.
  ifstream in(filename);
  if (!in) {
    cerr << filename << " not found\a" << endl;
    exit(0);
  }

  // Store latest obj.
  vector<Vector3f> vecn;
  vector<vector<unsigned>> vecf;

  string token = "";
  float v0, v1, v2;
  unsigned a, b, c;
  while (in >> token) {
    if (token == "v") {
      in >> v0 >> v1 >> v2;
      bindVertices.emplace_back(v0, v1, v2);
    } else if (token == "f") {
      in >> a >> b >> c;
      faces.push_back(Tuple3u(a, b, c));
    }
  }

  // make a copy of the bind vertices as the current vertices
  currentVertices = bindVertices;
}

void Mesh::draw() {
  // Since these meshes don't have normals
  // be sure to generate a normal per triangle.
  // Notice that since we have per-triangle normals
  // rather than the analytical normals from
  // assignment 1, the appearance is "faceted".
  GLuint index = glGenLists(1);
  // compile the display list
  glNewList(index, GL_COMPILE);

  glBegin(GL_TRIANGLES);
  for (auto &face : faces) {
    auto a = face[0];
    auto b = face[1];
    auto c = face[2];
    const auto& v1 = currentVertices[a - 1];
    const auto& v2 = currentVertices[b - 1];
    const auto& v3 = currentVertices[c - 1];
    const auto norm = Vector3f::cross(v2 - v1, v3 - v1).normalized();
    glNormal3d(norm[0], norm[1], norm[2]);
    glVertex3d(v1[0], v1[1], v1[2]);
    glVertex3d(v2[0], v2[1], v2[2]);
    glVertex3d(v3[0], v3[1], v3[2]);
  }
  glEnd();

  // End list.
  glEndList();

  glCallList(index);
}

void Mesh::loadAttachments(const char *filename, int numJoints) {
  // 2.2. Implement this method to load the per-vertex attachment weights
  // this method should update m_mesh.attachments

  ifstream in(filename);
  if (!in) {
    cerr << filename << " not found\a" << endl;
    exit(0);
  }

  float w;
  for (int i = 0; i < currentVertices.size(); ++i) {
    vector<float> attachment;
    for (int j = 0; j < numJoints; ++j) {
      in >> w;
      attachment.push_back(w);
    }
    attachments.push_back(attachment);
  }
}
