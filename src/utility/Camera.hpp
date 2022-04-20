#pragma once
#include <glm/glm.hpp>
#include <utility>

class Camera {
public:
  Camera(
    glm::vec3 position = {},
    float vAngle = 0.0,
    float hAngle = 0.0,
    float fov = 45.f
  );

  std::pair<glm::mat4, glm::mat4>
  calcViewProjection(float aspect);

  glm::vec3 position;
  float vAngle;
  float hAngle;
  float fov;

private:
};

