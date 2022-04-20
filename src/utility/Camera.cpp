#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(
  glm::vec3 position,
  float vAngle,
  float hAngle,
  float fov
)
: position(position)
, vAngle(vAngle)
, hAngle(hAngle)
, fov(fov)
{
}

std::pair<glm::mat4, glm::mat4>
Camera::calcViewProjection(float aspect) {
  glm::vec3 up{0.0, 1.0, 0.0};
  auto vCos = glm::cos(vAngle);
  glm::vec3 direction = {
    glm::cos(hAngle) * vCos,
    glm::sin(vAngle),
    glm::sin(hAngle) * vCos,
  };

  auto view = glm::lookAt(position, position + direction, up);

  auto projection = glm::perspective(fov, aspect, 0.05f, 500.f);

  return {view, projection};
}