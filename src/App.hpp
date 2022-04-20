#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <list>

#include "graphics/Shader.hpp"
#include "entities/Plane.hpp"
#include "utility/Camera.hpp"

class App {
public:
    App();
    ~App();

    void run();
private:
    bool running;
    unsigned int FPS = 0;
    unsigned int tickCounter = 0;

    GLFWwindow* window;

    unsigned int WIDTH;
    unsigned int HEIGHT;
    float aspectRatio;

    Camera camera;

    const double dt = 1. / 60.;

    std::map<unsigned int, bool> pressedKeys;

    Shader shader;
    bool shaderReloaded = true;

    GLint mvpLocation;

    std::vector<GLfloat> vertices;
    unsigned int vertexBuff;
    unsigned int vao;
    unsigned int colorBuff;

    std::list<EntityPtr> entities;

    
private:

    void update(double elapsedTime);
    void handleInput();
    void display();
    void clearWindow();

    friend void glErrorCallback(int, const char*);
    friend void keyCallback(GLFWwindow*, int, int, int, int);
    friend void windowResizeCallback(GLFWwindow* window, int width, int height);
    friend void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    friend void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
};

