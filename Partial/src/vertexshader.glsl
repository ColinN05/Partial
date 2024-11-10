#version 330 core

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in float in_value;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec4 normal;
out float value;

void main()
{
    gl_Position = Projection * View * (Model * vec4(in_Position, 1.0) * vec4(1.0,3.0,1.0,1.0));

    normal = vec4(normalize(mat3(transpose(inverse(Model))) * in_Normal), 1.0);

    value = in_value;
}