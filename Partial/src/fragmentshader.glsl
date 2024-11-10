#version 330 core

in vec4 normal;
in float value;

out vec4 FragColor;

uniform sampler2D Texture;

void main()
{
    vec4 norm = normalize(normal);

    vec4 diffuse = max(0.0, dot(norm, normalize(vec4(1.0,-1.0,1.0,1.0)))) * vec4(1.0,1.0,1.0,1.0);

    vec4 ambient = texture(Texture, vec2(value * 10.0, 0.0));

    FragColor = ambient + diffuse * 0.3;
}