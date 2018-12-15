#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 color;
layout(location = 3) in int rotating;

uniform mat4 translation;
uniform mat4 projection;
uniform mat4 rotation;
uniform mat4 camera;

void main()
{
  gl_Position = projection * camera * rotation * translation * vec4(position, 1.0);
}
