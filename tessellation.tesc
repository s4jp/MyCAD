#version 460 core

layout (vertices = 4) out;

uniform mat4 proj;
uniform int cpCount;
uniform ivec2 resolution;

float CalcTessLevel();

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    gl_TessLevelOuter[0] = 1.0;
    gl_TessLevelOuter[1] = CalcTessLevel();
}


float CalcTessLevel()
{
	float x_min = 1.0;
	float x_max = -1.0;
	float y_min = 1.0;
	float y_max = -1.0;

	for (int i = 0; i < cpCount; i++)
	{
		vec4 clipped = proj * gl_in[i].gl_Position;
		clipped /= clipped.w;

		if (!isnan(clipped.x) && !isinf(clipped.x))
		{
			if (x_min > clipped.x)
				x_min = clipped.x;
			if (x_max < clipped.x)
				x_max = clipped.x;
		}
		if (!isnan(clipped.y) && !isinf(clipped.y))
		{
			if (y_min > clipped.y)
				y_min = clipped.y;
			if (y_max < clipped.y)
				y_max = clipped.y;
		}
	}

	float clipbox_width = x_max - x_min;
	float clipbox_height = y_max - y_min;

	float width_scale = min(1.0, clipbox_width / 2.0);
	float height_scale = min(1.0, clipbox_height / 2.0);
	
	return max(width_scale * resolution.x, height_scale * resolution.y);
}