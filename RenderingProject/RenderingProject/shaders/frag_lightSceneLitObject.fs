#version 330 core
#define NR_POINT_LIGHTS 4  

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_emission1;
    float shininess;
    float emissionIntensity;
};

struct DirectionalLight {
    vec3 direction; // should be in view space
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position; // should be in view space
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position; // should be in view space
    vec3 direction; // should be in view space
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
    
    float innerCutOff;
    float outerCutOff;
};

in vec3 fragPos;
in vec3 normalVecView;
in vec2 texCoords;

out vec4 fragColor;

uniform Material material;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


void main()
{
    // Properties
    vec3 result;
    vec3 normal = normalVecView;
    vec3 viewDir = normalize(-fragPos);
    
    // Directional light influence
    result = calcDirectionalLight(directionalLight, normal, viewDir);
    
    // Point light influences
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        result += calcPointLight(pointLights[i], normal, fragPos, viewDir);
    }
    
    // Spotlight influence
    result += calcSpotLight(spotLight, normal, fragPos, viewDir);
    
    // Emission
    result += vec3(texture(material.texture_emission1, texCoords)) * material.emissionIntensity;
    
    // Final result
    fragColor = vec4(result, 1.0);
}


vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    // Ambient
    vec3 ambient =  light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
    
    // Diffuse
    vec3 lightDir = normalize(-light.direction);
    float diffuseMultiplier = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diffuseMultiplier * vec3(texture(material.texture_diffuse1, texCoords));
    
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularMultiplier = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * specularMultiplier * vec3(texture(material.texture_specular1, texCoords));
    
    return (ambient + diffuse + specular);
}


vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // Ambient
    vec3 ambient =  light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
    
    // Diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    float diffuseMultiplier = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diffuseMultiplier * vec3(texture(material.texture_diffuse1, texCoords));
    
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularMultiplier = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * specularMultiplier * vec3(texture(material.texture_specular1, texCoords));
    
    // Attenuation
    float lightDist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + (light.linear * lightDist) + (light.quadratic * lightDist * lightDist));
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}


vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // Ambient
    vec3 ambient =  light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
    
    // Diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    float diffuseMultiplier = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diffuseMultiplier * vec3(texture(material.texture_diffuse1, texCoords));
    
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularMultiplier = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * specularMultiplier * vec3(texture(material.texture_specular1, texCoords));
    
    // Attenuation
    float lightDist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + (light.linear * lightDist) + (light.quadratic * lightDist * lightDist));
    
    // Intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); 
    
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return (ambient + diffuse + specular);
}