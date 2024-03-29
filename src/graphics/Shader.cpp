#include "Shader.hpp"

#include <string>
#include <iostream>
#include <array>
#include <stack>
#include <memory>

Shader::Shader(const std::string& shaderName) {
  compile(shaderName);
}

std::string Shader::loadFromFileContent(const std::string& fileName) {
  std::ifstream file(fileName);
  std::stringstream s;

  if(file.good()) {
    s << file.rdbuf();
  } else {
    std::printf("%s\n", fileName.c_str());
    throw FileNotFound(fileName);
  }

  return s.str();
}

void Shader::compile(const std::string& shaderName) {
  programID = glCreateProgram();
  const static std::array<GLenum, 3> SHADER_TYPES = {
    GL_VERTEX_SHADER,
    GL_FRAGMENT_SHADER,
    GL_GEOMETRY_SHADER
  };
  std::stack<GLuint> shadersToDestroy;

  for(auto& shaderType : SHADER_TYPES) {
    try {
      auto shader = compileShader(shaderName, shaderType);
      glAttachShader(programID, shader);
      shadersToDestroy.push(shader);
    } catch(FileNotFound& e) {
      std::printf("File not found %s\n", e.what());
    }
  }

  glLinkProgram(programID);

  GLint compilationResult, errorLogSize;
  glGetProgramiv(programID, GL_LINK_STATUS, &compilationResult);
  glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &errorLogSize);

  if(errorLogSize > 0) {
    char errorMessage[2048];
    glGetProgramInfoLog(programID, 2048 - 1, NULL, errorMessage);
    std::fprintf(stderr, "Error compiling program %s\n", errorMessage);
  }

  while(!shadersToDestroy.empty()) {
    glDeleteShader(shadersToDestroy.top());
    shadersToDestroy.pop();
  }
}

GLuint Shader::compileShader(const std::string& shaderName, GLenum shaderType) {
  const static std::array<std::string, 3> EXTENSIONS = {".vert", ".frag", ".geom"};
  std::size_t fileExtensionID = 0;

  switch (shaderType) {
    case GL_VERTEX_SHADER:   fileExtensionID = 0; break;
    case GL_FRAGMENT_SHADER: fileExtensionID = 1; break;
    case GL_GEOMETRY_SHADER: fileExtensionID = 2; break;
    default: throw WrongShaderType(); break;
  }
  auto shaderSource = loadFromFileContent(shaderName + EXTENSIONS[fileExtensionID]);
  const char* shaderSourceCasted  = shaderSource.c_str();

  auto resultShader = glCreateShader(shaderType);
  glShaderSource(resultShader, 1, &shaderSourceCasted, NULL);
  glCompileShader(resultShader);

  GLint compilationResult, errorLogSize;
  glGetShaderiv(resultShader, GL_COMPILE_STATUS, &compilationResult);
  glGetShaderiv(resultShader, GL_INFO_LOG_LENGTH, &errorLogSize);

  if(errorLogSize > 0) {
    char errorMessage[2048];
    glGetShaderInfoLog(resultShader, errorLogSize, NULL, errorMessage);

    std::fprintf(stderr, "Compiling %s error\n", (shaderName + EXTENSIONS[fileExtensionID]).c_str());
    std::fprintf(stderr, "%s\n", errorMessage);
  }

  return resultShader;
}

GLint Shader::findVarLocation(const std::string& varName) {
  if(!variableLocations.contains(varName)) {
    auto id = glGetAttribLocation(programID, varName.c_str());
    variableLocations[varName] = id;
  }
  return variableLocations[varName];
}

GLint Shader::findUniformLocation(const std::string& uniformName) {
  if(!uniformLocations.contains(uniformName)) {
    auto id = glGetUniformLocation(programID, uniformName.c_str());
    variableLocations[uniformName] = id;
  }
  return variableLocations[uniformName];
}

void Shader::use() {
  glUseProgram(programID);
}