#include "App.hpp"
#include "utility/Timer.hpp"
#include <chrono>

#include <glm/gtc/type_ptr.hpp>


static App* app;

static void checkError(int line) {
  GLenum err;
  while((err = glGetError()) != GL_NO_ERROR) {
    std::printf("Line: %d, glerror %x\n", line, err);
  }
}

static void GLAPIENTRY
glMessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam ) {
                   /*
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
            */
}

void glErrorCallback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
};

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  if(action == GLFW_PRESS) {
    app->pressedKeys[key] = true;
  }
  if(action == GLFW_RELEASE) {
    app->pressedKeys[key] = false;
  }

  /*
  if(key == GLFW_KEY_F && action == GLFW_PRESS) {
    const auto mode = glfwGetVideoMode(app->monitors[0]);
    if(app->isFullScreen) {
      glfwSetWindowMonitor(app->window, nullptr, 0, 0, app->width, app->height, mode->refreshRate);
      app->isFullScreen = false;
    } else {
      glfwSetWindowMonitor(app->window, app->monitors[0], 0, 0, mode->width, mode->height, mode->refreshRate);
      app->isFullScreen = true;
    }
  }
  */
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
  app->WIDTH = width;
  app->HEIGHT = height;
  app->aspectRatio = width / static_cast<float>(height);

  glViewport(0, 0, width, height);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  //app->camRadius += yoffset * app->dt * 40.f;
  //std::printf("Scroll: %lf %lf\n", xoffset, yoffset);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  if(button == GLFW_MOUSE_BUTTON_LEFT) {
    if(action == GLFW_PRESS) {
      glfwSetCursorPos(window, app->WIDTH / 2.0, app->HEIGHT / 2.0);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
    if(action == GLFW_PRESS) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
  }

}

App::App()
: running(true)
, WIDTH(1920)
, HEIGHT(1080)
{
  app = this;
  if(!glfwInit()) {
    std::cerr << "could not load glfw\n";
    throw std::exception();
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  window = glfwCreateWindow(WIDTH, HEIGHT, "Cloth simulation", nullptr, nullptr);

  if(!window) {
    std::cerr << "could create window\n";
    glfwTerminate();
    throw std::exception();
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  glewExperimental = true;
  GLenum glewStatus = glewInit();
  if(glewStatus != GLEW_OK) {
    std::cerr << "could not load glew\n";
    std::fprintf(stderr, "GLEW error: %s", glewGetErrorString(glewStatus));
    throw std::exception();
  }

  glfwSetErrorCallback(glErrorCallback);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetMouseButtonCallback(window, mouseButtonCallback);
  glfwSetFramebufferSizeCallback(window, windowResizeCallback);
  glfwSetScrollCallback(window, scrollCallback);

  windowResizeCallback(window, WIDTH, HEIGHT);

  std::printf("GLEW version: %s\n", glewGetString(GLEW_VERSION));
  glViewport(0, 0, WIDTH, HEIGHT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(glMessageCallback, 0);

  shader = {"shaders/shader"};

  vertices = {
    -0.5f, -0.5f, 0.0f,
     0.5f,  0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f,

    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.5f,  0.5f, 0.0f,
  };

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vertexBuff);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuff);
  glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);

  auto vertexLoc = shader.findVarLocation("vertex");
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  auto plane = std::make_unique<Plane>(1.0, 1.0, 20, 20);
  
  entities.push_back(std::move(plane));

  camera.position = {0.0, 0.0, 1.3};
  camera.fov = glm::radians(45.f);
  camera.hAngle = glm::radians(-90.f);
  checkError(__LINE__);
}

App::~App() {
   glDeleteBuffers(1, &vertexBuff);
   glDeleteBuffers(1, &vao);
   glfwTerminate();
}

void App::run() {
  tickCounter = FPS = 0;
  double accumulator {}, lastFrameTime {};
  Timer timer;
  Timer fpsTimer;
  while(running) {
    if(glfwWindowShouldClose(window)) {
      running = false;
    }

    FPS++;
    double currentTime = timer();
    accumulator += currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    handleInput();

    while(accumulator >= dt) {
      tickCounter++;
      update(dt);
      accumulator -= dt;
    }
    clearWindow();
    display();
    glfwSwapBuffers(window);
    glfwPollEvents();

    if(fpsTimer() > 1.0) {
      fpsTimer.restart();
      char title[128];
      snprintf(title, 127, "Cloth simulation (FPS: %lu, ticks: %lu)", FPS, tickCounter);
      glfwSetWindowTitle(window, title);
      FPS = tickCounter = 0;
    }
  }
}

void App::handleInput() {
  if(pressedKeys[GLFW_KEY_X]) {
    if(!shaderReloaded) {
      shaderReloaded = true;
      shader = {"shaders/shader"};
    }
  } else {
    shaderReloaded = false;
  }

}

void App::update(double elapsedTime) {
  for(auto& entity : entities) {
    entity->update(elapsedTime);
  }
  float maxSpeed = 4.0;
  glm::vec3 cameraVelocity = {};
  if(pressedKeys[GLFW_KEY_W]) {
    cameraVelocity.z = -maxSpeed * elapsedTime;
  }
  if(pressedKeys[GLFW_KEY_S]) {
    cameraVelocity.z = +maxSpeed * elapsedTime;
  }
  if(pressedKeys[GLFW_KEY_A]) {
    cameraVelocity.x = -maxSpeed * elapsedTime;
  }
  if(pressedKeys[GLFW_KEY_D]) {
    cameraVelocity.x = maxSpeed * elapsedTime;
  }
  if(pressedKeys[GLFW_KEY_R]) {
    cameraVelocity.y = maxSpeed * elapsedTime;
  }
  if(pressedKeys[GLFW_KEY_F]) {
    cameraVelocity.y = -maxSpeed * elapsedTime;
  }

  float vAngle = 0.0;
  float hAngle = 0.0;
  float maxCamSpeed = 1.0;
  if(pressedKeys[GLFW_KEY_KP_4]) {
    hAngle = -maxCamSpeed;
  }
  if(pressedKeys[GLFW_KEY_KP_6]) {
    hAngle = maxCamSpeed;
  }
  if(pressedKeys[GLFW_KEY_KP_2]) {
    vAngle = -maxCamSpeed;
  }
  if(pressedKeys[GLFW_KEY_KP_8]) {
    vAngle = maxCamSpeed;
  }

  vAngle *= elapsedTime;
  hAngle *= elapsedTime;
  cameraVelocity *= elapsedTime;
  camera.position += cameraVelocity;
  camera.vAngle += vAngle;
  camera.hAngle += hAngle;
}

void App::display() {
  shader.use();

  auto [view, projection] = camera.calcViewProjection(aspectRatio);
  auto MVP = projection * view;
  auto mvpLoc = shader.findUniformLocation("MVP");
  glUniformMatrix4fv(mvpLocation, 1, false, &MVP[0][0]);

  for(auto& entity : entities) {
    entity->draw();
  }
}

void App::clearWindow() {
   glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
