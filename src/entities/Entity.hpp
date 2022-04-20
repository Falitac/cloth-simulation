#pragma once
#include <glm/glm.hpp>
#include <memory>

class Entity {
public:
  virtual void update(double dt) = 0;
  virtual void draw() = 0;

protected:
  glm::vec3 position;
  glm::vec3 rotation;
};

using EntityPtr = std::unique_ptr<Entity>;