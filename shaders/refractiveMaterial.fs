#version 430 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPosition;
uniform samplerCube skybox;

void main()
{
    // float ratio = 1.00 / 1.33;  // Water refraction
    float ratio = 1.00 / 1.52;  // Glass refraction
    // float ratio = 1.00 / 2.42;  // Diamond refraction

    vec3 I = normalize(Position - cameraPosition);
    vec3 R = refract( I, normalize(Normal), ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}