#version 330
in vec4 vColor;
in vec2 uvPos;
in float bw;

/*float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return 6.0 / (30.1 - z * 29.9);
}*/

void main()
{
    if(!gl_FrontFacing)
        gl_FragColor.rgb = vec3(0.0);
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
                    gl_FragColor.rgb = vec3(0.0);
                else
                    gl_FragColor.rgb = vColor.rgb / (nbLoop - i);

                border = true;
                break;
            }

            min += 0.002;
            max -= 0.002;
        }

        if(!border)
            gl_FragColor.rgb = vColor.rgb;
    }

    gl_FragColor.a = 1.0;

    //float depth = LinearizeDepth(gl_FragCoord.z) / 30.0;
    //gl_FragColor = vec4(vec3(depth), 1.0);
}
