#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    // w == 0.0 -> directional light
    // w == 1.0 -> point light
    vec4 lightVector;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct Flashlight {
    vec3  position;
    vec3  direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;  
in vec3 Normal;  
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform Flashlight fLight;



void main()
{
    // ambient
    vec3 ambient = light.ambient * material.ambient;
  	
    // diffuse 
    vec3 norm = normalize(Normal);

    vec3 lightDir;
    vec3 lVector3 = light.lightVector.xyz;

    if (light.lightVector.w < 0.5) {
        lightDir = normalize(-lVector3);
    } else {
        lightDir = normalize(lVector3 - FragPos);
    }

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular); 

    // attenuation
    if (light.lightVector.w > 0.5) {
        float distance = length(lVector3 - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        ambient  *= attenuation;  
        diffuse  *= attenuation;
        specular *= attenuation;   
    }

    // flashlight
    vec3 fAmbient = fLight.ambient * material.ambient;

    vec3 fLightDir = normalize(fLight.position - FragPos);
    float fDiff = max(dot(norm, fLightDir), 0.0);
    vec3 fDiffuse = fLight.diffuse * fDiff * material.diffuse;

    vec3 fReflectDir = reflect(-fLightDir, norm);
    float fSpec = pow(max(dot(viewDir, fReflectDir), 0.0), material.shininess);
    vec3 fSpecular = fLight.specular * fSpec * material.specular;

    // check if lighting is inside the spotlight cone (soft edges)
    float theta = dot(fLightDir, normalize(-fLight.direction)); 
    float epsilon = fLight.cutOff - fLight.outerCutOff;
    float intensity = clamp((theta - fLight.outerCutOff) / epsilon, 0.0, 1.0);
    fDiffuse *= intensity;
    fSpecular *= intensity;

    float fDistance = length(fLight.position - FragPos);
    float fAttenuation = 1.0 / (fLight.constant + fLight.linear * fDistance + fLight.quadratic * (fDistance * fDistance));
    
    fAmbient *= fAttenuation;
    fDiffuse *= fAttenuation;
    fSpecular *= fAttenuation;

    vec3 fResult = fAmbient + fDiffuse  + fSpecular;

    vec3 result = ambient + diffuse + specular;
    result += fResult;
    FragColor = vec4(result, 1.0);
} 