#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 color;
layout(location = 3) in float borderWidth;

uniform mat4 projection;
uniform mat4 camera;

out vec4 vColor;
out vec2 uvPos;
out float bw;

void main()
{
  bw = borderWidth;
  uvPos = uv;
  gl_Position = projection * camera * vec4(position, 1.0);
  vColor = vec4(color, 1.0);
}
