// Reflection
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{             
    vec3 I = normalize(FragPos - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}

// Phong
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
     vec3 specular;
};


out vec4 color;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform Material material;
uniform Light light;

void main()
{

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(FragPos - light.position);
    float coeff = max(-dot(lightDir, norm), 0.0f);

    vec3 ambience = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * (coeff * material.diffuse);

    vec3 viewDir = normalize(FragPos - viewPos);
    float shininess = pow(max(-dot(viewDir, reflect(lightDir, norm)), 0.0f), material.shininess);
    vec3 specular = light.specular * (material.specular * shininess);

    vec3 result = ambience + diffuse + specular;
    
    color = vec4(result, 1.0);
}
