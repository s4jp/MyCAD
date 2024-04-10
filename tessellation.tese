#version 460 core

layout (isolines) in;

uniform mat4 proj;
uniform int cpCount;

void main()
{
    float t = gl_TessCoord.x;
    float minT = (1.0 - t);

    vec3 result;

    if (cpCount == 4){
        vec3 p0 = gl_in[0].gl_Position.xyz;
        vec3 p1 = gl_in[1].gl_Position.xyz;
        vec3 p2 = gl_in[2].gl_Position.xyz;
        vec3 p3 = gl_in[3].gl_Position.xyz;

        float b3_3 = t * t * t;
        float b3_2 = 3.0 * minT * t * t;
        float b3_1 = 3.0 * minT * minT * t;
        float b3_0 = minT * minT * minT;

        result = b3_0 * p0 + b3_1 * p1 + b3_2 * p2 + b3_3 * p3;
    } else if (cpCount == 3) {
        vec3 p0 = gl_in[0].gl_Position.xyz;
        vec3 p1 = gl_in[1].gl_Position.xyz;
        vec3 p2 = gl_in[2].gl_Position.xyz;

        float b2_2 = t * t;
        float b2_1 = 2.0 * minT * t;
        float b2_0 = minT * minT;

        result = b2_0 * p0 + b2_1 * p1 + b2_2 * p2;
    } else if (cpCount == 2) {
        vec3 p0 = gl_in[0].gl_Position.xyz;
        vec3 p1 = gl_in[1].gl_Position.xyz;

        float b1_1 = t;
        float b1_0 = minT;

        result = b1_0 * p0 + b1_1 * p1;
    }

    gl_Position = proj * vec4(result, 1.0);
}