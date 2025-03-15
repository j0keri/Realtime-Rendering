#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light {
    vec3 position; // should be in view space
    vec3 direction; // should be in view space
    float innerCutOff;
    float outerCutOff;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};

in vec3 fragPos;
in vec3 normalVecView;
in vec2 texCoords;

out vec4 fragColor;

uniform Material material;
uniform Light light;
uniform float emissionIntensity;

void main()
{
    // Ambient
    vec3 ambient =  light.ambient * vec3(texture(material.diffuse, texCoords));
    
    // Diffuse
    vec3 normal = normalize(normalVecView);
    vec3 lightDir = normalize(light.position - fragPos);
    //vec3 lightDir = normalize(-light.direction);
    float diffuseMultiplier = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diffuseMultiplier * vec3(texture(material.diffuse, texCoords));
    
    // Specular
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularMultiplier = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * specularMultiplier * vec3(texture(material.specular, texCoords));
    
    // Emission
    vec3 emission = vec3(texture(material.emission, texCoords)) * emissionIntensity;
    
    // Attenuation
    float lightDist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + (light.linear * lightDist) + (light.quadratic * lightDist * lightDist));
    
    // Spotlight stuff
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); 
    
    
    //ambient *= attenuation;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    fragColor = vec4(ambient + diffuse + specular + emission, 1.0);
}