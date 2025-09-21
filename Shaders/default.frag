#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;
uniform int trimmingOption;

uniform vec4 color;

const float eps = 0.01;
const vec3 red = vec3(1.0, 0.0, 0.0);
const vec3 black = vec3(0.0, 0.0, 0.0);
const vec3 white = vec3(1.0, 1.0, 1.0);

void main()
{
    if (trimmingOption == 0)
    {
        FragColor = color;
        return;
    }

    bool hideBlack = (trimmingOption == 1 || trimmingOption == 3);
    bool hideRed = (trimmingOption == 2 || trimmingOption == 3);
    vec4 texColor = texture(tex, TexCoord);

    if (hideRed && (
        all(lessThan(abs(texColor.rgb - red), vec3(eps))) ||
        all(lessThan(abs(texColor.rgb - white), vec3(eps)))
    )) discard;

    if (hideBlack && (
        all(lessThan(abs(texColor.rgb - black), vec3(eps))) ||
        all(lessThan(abs(texColor.rgb - white), vec3(eps)))
    )) discard;

    FragColor = color;
}