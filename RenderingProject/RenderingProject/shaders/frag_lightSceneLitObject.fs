#version 330 core

in vec3 fragPos;
in vec3 normalVecView;
in vec3 lightPosView;

out vec4 fragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    vec3 normal = normalize(normalVecView);
    vec3 lightDir = normalize(lightPosView - fragPos);
    float diffuseMultiplier = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseMultiplier * lightColor;
    
    float specularStrength = 0.5;
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularMultiplier = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * specularMultiplier * lightColor;
    
    vec3 finalColor = (ambient + diffuse + specular) * objectColor;
    fragColor = vec4(finalColor, 1.0);
}