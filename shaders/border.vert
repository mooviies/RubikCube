#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 barycentric;
layout(location = 2) in vec3 color;

uniform mat4 projection;
uniform mat4 camera;

out vec4 vColor;
out vec3 vBC;

void main()
{
  vBC = barycentric;
  gl_Position = projection * camera * vec4(position, 1.0);
  vColor = vec4(color, 1.0);
}
