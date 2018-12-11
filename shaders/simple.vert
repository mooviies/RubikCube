#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 color;
layout(location = 3) in int rotating;

uniform mat4 projection;
uniform mat4 camera;
uniform mat4 rotation;
uniform float borderWidth;

out vec4 vColor;
out vec2 uvPos;
out float bw;

void main()
{
  bw = borderWidth;
  uvPos = uv;

  if(rotating > 0.5)
    gl_Position = projection * camera * rotation * vec4(position, 1.0);
  else
    gl_Position = projection * camera * vec4(position, 1.0);

  vColor = vec4(color, 1.0);
}
