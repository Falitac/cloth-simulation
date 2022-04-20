#version 330 core

layout(location = 0) in vec3 vertex;

uniform mat4 MVP;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 vertexColor;

void main() {
    vertexColor = vec3(gl_VertexID / 10.0);
    gl_Position = MVP * vec4(vertex, 1.0);
}