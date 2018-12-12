#version 330
in vec4 vColor;
in vec2 uvPos;
in float bw;

out vec4 fColor;

void main()
{
    if(!gl_FrontFacing)
        fColor.rgb = vec3(0.0);
    else
    {
        float min = bw, max = 1.0 - bw;
        int nbLoop = 10;
        bool border = false;
        for(int i = 0; i < nbLoop; i++)
        {
            if(uvPos.x <= min || uvPos.x >= max || uvPos.y <= min || uvPos.y >= max)
            {
                if(i == 0)
                    fColor.rgb = vec3(0.0);
                else
                    fColor.rgb = vColor.rgb / (nbLoop - i);

                border = true;
                break;
            }

            min += 0.002;
            max -= 0.002;
        }

        if(!border)
            fColor.rgb = vColor.rgb;
    }

    fColor.a = 1.0;
}
