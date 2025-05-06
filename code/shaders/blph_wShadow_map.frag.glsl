#version 330 core

in VS_OUT {
    vec3 fragNormal;
    vec3 fragPos;
    vec2 texCoords;
    vec4 fragPosLightSpace;
} fs_in;

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};


struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
const int NUM_POINT_LIGHTS = 16;

uniform sampler2D shadowMap;
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform Material material;
uniform int numPointLights;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadow);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace);

void main()
{
    // properties
    vec3 norm = normalize(fs_in.fragNormal);
    vec3 viewDir = normalize(viewPos - fs_in.fragPos);
    float shadow = ShadowCalculation(fs_in.fragPosLightSpace);

    // phase 1: Directional lighting
    vec3 dirResult = calcDirLight(dirLight, norm, viewDir, shadow);
    // point lighting
    vec3 pointResult = {0, 0, 0};
    for (int i = 0; i < numPointLights; i++) {
        pointResult += calcPointLight(pointLights[i], norm, viewDir);
    }
    FragColor = vec4(dirResult + pointResult, 1.0);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadow)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fs_in.texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, fs_in.texCoords));
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fs_in.fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fs_in.fragPos);
    float attenuation = 1.0 /
            (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fs_in.texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, fs_in.texCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

float ShadowCalculation(vec4 fragPosLightSpace) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; 
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.01 * (1.0 - dot(fs_in.fragNormal, normalize(-dirLight.direction))), 0.005);
    // float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0) {
        shadow = 0.0;
    }
    return shadow;
}
