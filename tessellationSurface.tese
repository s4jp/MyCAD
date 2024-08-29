#version 460 core

layout (isolines) in;

uniform mat4 proj;
uniform int segmentCount;
uniform int segmentIdx;
uniform int division;
uniform bool otherAxis;

void main()
{
    float u, v;

    if (!otherAxis)
	{
		u = (gl_TessCoord.x * (1 + segmentIdx)) / segmentCount;
		v = gl_TessCoord.y * division / (division - 1.f);
	}
	else
	{
        u = gl_TessCoord.y * division / (division - 1.f);
        // BUG, FIX MAYBE
        // u = ((gl_TessCoord.y * division / (division - 1.f)) * (1 + segmentIdx)) / segmentCount;
		v = gl_TessCoord.x;
	}

    float minU = (1.0 - u);
    float minV = (1.0 - v);

    vec3 result;

    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;

    vec3 p4 = gl_in[4].gl_Position.xyz;
    vec3 p5 = gl_in[5].gl_Position.xyz;
    vec3 p6 = gl_in[6].gl_Position.xyz;
    vec3 p7 = gl_in[7].gl_Position.xyz;

    vec3 p8 = gl_in[8].gl_Position.xyz;
    vec3 p9 = gl_in[9].gl_Position.xyz;
    vec3 p10 = gl_in[10].gl_Position.xyz;
    vec3 p11 = gl_in[11].gl_Position.xyz;

    vec3 p12 = gl_in[12].gl_Position.xyz;
    vec3 p13 = gl_in[13].gl_Position.xyz;
    vec3 p14 = gl_in[14].gl_Position.xyz;
    vec3 p15 = gl_in[15].gl_Position.xyz;

    float b3_3_u = u * u * u;
    float b3_2_u = 3.0 * minU * u * u;
    float b3_1_u = 3.0 * minU * minU * u;
    float b3_0_u = minU * minU * minU;

    float b3_3_v = v * v * v;
    float b3_2_v = 3.0 * minV * v * v;
    float b3_1_v = 3.0 * minV * minV * v;
    float b3_0_v = minV * minV * minV;

    result = b3_0_u * b3_0_v * p0 +
             b3_1_u * b3_0_v * p1 + 
             b3_2_u * b3_0_v * p2 + 
             b3_3_u * b3_0_v * p3 +
			 b3_0_u * b3_1_v * p4 + 
             b3_1_u * b3_1_v * p5 + 
             b3_2_u * b3_1_v * p6 + 
             b3_3_u * b3_1_v * p7 +
			 b3_0_u * b3_2_v * p8 + 
             b3_1_u * b3_2_v * p9 + 
             b3_2_u * b3_2_v * p10 + 
             b3_3_u * b3_2_v * p11 +
			 b3_0_u * b3_3_v * p12 + 
             b3_1_u * b3_3_v * p13 + 
             b3_2_u * b3_3_v * p14 + 
             b3_3_u * b3_3_v * p15;

    gl_Position = proj * vec4(result, 1.0);
}