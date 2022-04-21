#version 330 core

layout(location = 0) in vec3 vertex;

uniform mat4 MVP;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 vertexColor;

void main() {
    float t = gl_VertexID / 400.0;
    vertexColor = vec3(sin(t) * sin(t * t), cos((t+5)*(t+3)), 0.5);
    gl_Position = MVP * vec4(vertex, 1.0);
}