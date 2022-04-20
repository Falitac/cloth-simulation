#include "Plane.hpp"


Plane::Plane(
  float width,
  float height,
  unsigned int xdiv,
  unsigned int ydiv
) {
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  auto [vertices, indices] = generateVertices(width, height, xdiv, ydiv);

  glBindVertexArray(vao);
  
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(
    GL_ARRAY_BUFFER, 
    vertices.size() * sizeof(vertices[0]),
    vertices.data(),
    GL_STATIC_DRAW
  );

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    indices.size() * sizeof(indices[0]),
    indices.data(),
    GL_STATIC_DRAW
  );
  indicesCount = indices.size();
  std::printf("%d\n", indicesCount);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), nullptr);

  glBindVertexArray(0);
}

Plane::~Plane() {
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
}

void Plane::update(double dt) {

}

void Plane::draw() {
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
}

std::pair<
  std::vector<float>,
  std::vector<GLuint>
>
Plane::generateVertices(float width, float height, unsigned xdiv, unsigned ydiv) {
  std::vector<float> vertices;
  std::vector<GLuint> indices;
  vertices.reserve((1 + xdiv + 1) * (ydiv + 1) * 3);
  indices.reserve(xdiv * ydiv * 2);

  for(int j = 0; j <= ydiv; j++) {
    for(int i = 0; i <= xdiv; i++) {
      vertices.emplace_back(width * i / static_cast<float>(xdiv));
      vertices.emplace_back(height * j / static_cast<float>(ydiv));
      vertices.emplace_back(0.0);
    }
  }

  auto makeEvenFaceTopology = [&] (unsigned int index) {
    indices.emplace_back(index);
    indices.emplace_back(index + xdiv + 2);
    indices.emplace_back(index + xdiv + 1);

    indices.emplace_back(index);
    indices.emplace_back(index + 1);
    indices.emplace_back(index + xdiv + 2);
  };
  auto makeOddFaceTopology = [&] (unsigned int index) {
    indices.emplace_back(index + 1);
    indices.emplace_back(index + xdiv + 2);
    indices.emplace_back(index + xdiv + 1);

    indices.emplace_back(index + 1);
    indices.emplace_back(index + xdiv + 1);
    indices.emplace_back(index);
  };

  for(int j = 0; j < ydiv; j++) {
    for(int i = 0; i < xdiv; i++) {
      auto index = i + j * (xdiv + 1);
      if((i + j) % 2) {
        makeOddFaceTopology(index);
      } else {
        makeEvenFaceTopology(index);
      }
    }
  }

  return {vertices, indices};
}
