#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 normal;
out vec3 fragPos;
out vec2 texCoords;
out vec3 lightDirection;

uniform vec3 inlightDirection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    fragPos = vec3(view * model * vec4(aPos, 1.0));
    normal = mat3(transpose(inverse(view * model))) * aNormal;
    texCoords = aTexCoords;
    lightDirection = vec3(view * vec4(inlightDirection, 0.0));
}
