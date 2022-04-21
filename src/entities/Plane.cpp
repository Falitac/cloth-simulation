#include "Plane.hpp"


Plane::Plane(
  float width,
  float height,
  unsigned int xdiv,
  unsigned int ydiv
)
: xdiv(xdiv)
, ydiv(ydiv)
{
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
    GL_DYNAMIC_DRAW
  );

  points = std::move(vertices);
  pointForces = pointVelocities = std::vector<glm::vec3>(points.size(), {0.0f, 0.0f, 0.0f});

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
  glDeleteVertexArrays(1, &vao);
}

void Plane::update(double dt) {
  std::printf("%4lf\n", dt);
  dt /= 1000.f;
  constexpr auto gravity = glm::vec3(0.0f, 9.8, 0.0);

  float xlen = 1.0 / static_cast<float>(xdiv);
  float ylen = 1.0 / static_cast<float>(ydiv);

  float k = 10.1;
  auto calcForce = [&] (int index, int offset) {
    try {
      auto& p1 = points.at(index);
      auto& p2 = points.at(index - offset);
      auto dist = glm::distance(p1, p2);
      float forceVal = k * (xlen - dist);
      pointForces[index] += forceVal * glm::normalize(p1 - p2);
    } catch(...) {
    }
  };

  for(int j = 0; j < ydiv; j++) {
    for(int i = 0; i <= xdiv; i++) {
      auto index = j * (xdiv + 1) + i;

      pointForces[index] = {};
      calcForce(index, -1);
      calcForce(index,  1);
      calcForce(index,  xdiv + 1);
      calcForce(index, -xdiv - 1);


      pointVelocities[index] += pointForces[index];
      //pointVelocities[index] -= gravity;

      auto velocity = pointVelocities[index];
      points[index] +=
      static_cast<float>(dt) * velocity;
    }
  }
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferSubData(
    GL_ARRAY_BUFFER,
    0,
    points.size() * sizeof(points[0]),
    points.data()
  );

}

void Plane::draw() {
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
}

std::pair<
  std::vector<glm::vec3>,
  std::vector<GLuint>
>
Plane::generateVertices(float width, float height, unsigned xdiv, unsigned ydiv) {
  std::vector<glm::vec3> vertices;
  std::vector<GLuint> indices;
  vertices.reserve((1 + xdiv + 1) * (ydiv + 1) * 3);
  indices.reserve(xdiv * ydiv * 2);

  for(int j = 0; j <= ydiv; j++) {
    for(int i = 0; i <= xdiv; i++) {
      vertices.emplace_back(
        width * i / static_cast<float>(xdiv),
        height * j / static_cast<float>(ydiv),
        0.0
      );
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
