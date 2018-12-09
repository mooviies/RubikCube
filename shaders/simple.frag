#version 330
in vec4 vColor;
in vec2 uvPos;
in float bw;

out vec4 fColor;

void main()
{
    float min = bw, max = 1.0 - bw;

    if(uvPos.x < min || uvPos.x >= max || uvPos.y < min || uvPos.y >= max)
    {
        fColor.rgb = vec3(0.0);
    }
    else
    {
        fColor.rgb = vColor.rgb;
    }

    fColor.a = 1.0;
}
