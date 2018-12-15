#version 330
layout(location = 0) in vec3 position;

uniform mat4 translation;
uniform mat4 projection;
uniform mat4 camera;

void main()
{
  gl_Position = projection * camera * translation * vec4(position, 1.0);
}
