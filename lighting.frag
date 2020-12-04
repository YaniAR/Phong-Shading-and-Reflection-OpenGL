// Reflection
#version 330 core
struct Material
{
     vec3 ambient;
     vec3 diffuse;
     vec3 specular;
     float shininess;
};

struct Light
{
     vec3 position;
     vec3 ambient;
     vec3 diffuse;
     float specular;
};

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 cameraPos;
uniform samplerCube skybox;

uniform Material material;
uniform Light light;

void main()
{             
    vec3 norm = normalize(Normal);
    
    vec3 Incident = normalize(FragPos - cameraPos);
    vec3 Refl = reflect(Incident, norm);

    float refractiveFactor = 0.5 + (2.5 * pow(1 + dot(Incident, norm), 5.0));
    vec3 Refr = refract(Incident, norm, 1.00/1.33);
    
    vec3 Color = mix(Refl, Refr, 0.5);
    vec3 reflectedColor = texture(skybox, Color).rgb;

    vec3 lightDir = normalize(FragPos - light.position);
    float coeff = max(-dot(lightDir, norm), 0.0f);

    vec3 ambience = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * (coeff * material.diffuse);

    vec3 viewDir = normalize(FragPos - cameraPos);
    float shininess = pow(max(-dot(viewDir, reflect(lightDir, norm)), 0.0f), material.shininess);
    vec3 specular = light.specular * (material.specular * shininess);

    vec3 result = ambience + diffuse + (reflectedColor * specular);
  
    result = mix(result, reflectedColor, light.specular);
    FragColor = vec4(result, 1.0);
}
