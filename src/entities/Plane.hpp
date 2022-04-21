#pragma once
#include <GL/glew.h>
#include <vector>
#include "Entity.hpp"



class Plane : public Entity {
public:
  Plane(
    float width = 1.f,
    float height = 1.f,
    unsigned int xdiv = 1,
    unsigned int ydiv = 1
  );
  ~Plane();

  virtual void update(double dt) override;
  virtual void draw() override;
private:
  GLuint vao;
  GLuint vbo;
  GLuint ebo;

  unsigned int xdiv;
  unsigned int ydiv;

  std::vector<glm::vec3> points;
  std::vector<glm::vec3> pointVelocities;
  std::vector<glm::vec3> pointForces;
  GLuint indicesCount;

  std::pair<
    std::vector<glm::vec3>,
    std::vector<GLuint>
  >
  generateVertices(float width, float height, unsigned int xdiv, unsigned int ydiv);
};

