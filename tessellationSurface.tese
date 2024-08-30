#version 460 core

layout (isolines) in;

uniform mat4 proj;
uniform int segmentCount;
uniform int segmentIdx;
uniform int division;
uniform bool otherAxis;
uniform bool bspline;

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

    float W3_u = u * u * u;
    float W2_u = bspline ? minU * (u + 1) * (u + 1) + u * (2 - u) * (u + 1) + (3 - u) * u * u : 3.0 * minU * u * u;
    float W1_u = bspline ? minU * minU * (u + 2) + minU * (u + 1) * (2 - u) + u * (2 - u) * (2 - u) : 3.0 * minU * minU * u;
    float W0_u = minU * minU * minU;
    if (bspline)
    {
	    W3_u /= 6.0;
        W2_u /= 6.0;
        W1_u /= 6.0;
        W0_u /= 6.0;
    }

    float W3_v = v * v * v;
    float W2_v = bspline ? minV * (v + 1) * (v + 1) + v * (2 - v) * (v + 1) + (3 - v) * v * v : 3.0 * minV * v * v;
    float W1_v = bspline ? minV * minV * (v + 2) + minV * (v + 1) * (2 - v) + v * (2 - v) * (2 - v) : 3.0 * minV * minV * v;
    float W0_v = minV * minV * minV;
    if (bspline)
    {
	    W3_v /= 6.0;
        W2_v /= 6.0;
        W1_v /= 6.0;
        W0_v /= 6.0;
    }

    result = W0_u * W0_v * p0 +
             W1_u * W0_v * p1 + 
             W2_u * W0_v * p2 + 
             W3_u * W0_v * p3 +
			 W0_u * W1_v * p4 + 
             W1_u * W1_v * p5 + 
             W2_u * W1_v * p6 + 
             W3_u * W1_v * p7 +
			 W0_u * W2_v * p8 + 
             W1_u * W2_v * p9 + 
             W2_u * W2_v * p10 + 
             W3_u * W2_v * p11 +
			 W0_u * W3_v * p12 + 
             W1_u * W3_v * p13 + 
             W2_u * W3_v * p14 + 
             W3_u * W3_v * p15;

    gl_Position = proj * vec4(result, 1.0);
}