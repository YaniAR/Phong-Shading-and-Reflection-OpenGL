#include <iostream>
#include <vector>
#include "Camera.h"

//GLEW
#define GLEW_STATIC
#include <gl/glew.h>

// SFML
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

// SOIL2
#include <SOIL2.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

// Constants
constexpr unsigned int WIDTH{ 800 }, HEIGHT{ 600 };
float lastX{ WIDTH / 2.0 };
float lastY{ HEIGHT / 2.0 };

constexpr float M_PI{ glm::pi<GLfloat>() };
constexpr int STACKS{ 50 };
constexpr int SLICES{ 50 };
constexpr float radius{ 0.5 };

int numberOfPoints{ 0 };
int numberOfIndexes{ 0 };

float vertice[(STACKS + 1) * SLICES * 3];
unsigned int indices[STACKS * SLICES * 10];

void getSphereCoords()
{
    float theta{};
    for (int i{ 0 }; i <= SLICES; ++i)
    {
        float phi{ i * (M_PI / SLICES) };
        for (int j{ 0 }; j < STACKS; ++j)
        {
            theta = (j * (M_PI * 2 / STACKS));
            float x = cosf(theta) * sinf(phi);
            float y = cosf(phi);
            float z = sinf(theta) * sinf(phi);

            vertice[numberOfPoints++] = x * radius;
            vertice[numberOfPoints++] = y * radius;
            vertice[numberOfPoints++] = z * radius;
        }
    }

    for (int i{ 0 }; i < numberOfPoints / 3 - STACKS; i++)
    {
        if ((i + 1) % STACKS == 0)
        {
            indices[numberOfIndexes++] = i;
            indices[numberOfIndexes++] = i - STACKS + 1;
            indices[numberOfIndexes++] = i + STACKS;

            indices[numberOfIndexes++] = i - STACKS + 1;
            indices[numberOfIndexes++] = i + STACKS;
            if (i + 1 == numberOfPoints / 3)
                indices[numberOfIndexes++] = numberOfPoints - STACKS;
            else
                indices[numberOfIndexes++] = i + 1;
        }
        else
        {
            indices[numberOfIndexes++] = i;
            indices[numberOfIndexes++] = i + 1;
            indices[numberOfIndexes++] = i + STACKS;

            // Bottom tri of quad
            indices[numberOfIndexes++] = i + 1;
            indices[numberOfIndexes++] = i + STACKS;
            indices[numberOfIndexes++] = i + STACKS + 1;
        }
    }
}

static GLuint LoadCubemap(std::vector<const GLchar* > faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    int imageWidth, imageHeight;
    unsigned char* image;

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    for (GLuint i = 0; i < faces.size(); i++)
    {
        image = SOIL_load_image(faces[i], &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        SOIL_free_image_data(image);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

bool firstMouse{ true };
void MouseCallBack(Camera& camera, float xPos, float yPos)
{
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset{ xPos - lastX };
    float yOffset{ lastY - yPos };


    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

int main()
{
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    sf::Clock clock{};

    sf::ContextSettings settings;

    settings.majorVersion = 4;
    settings.minorVersion = 6;
    settings.attributeFlags = 1;

    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 0;

    sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "OpenGL", sf::Style::Titlebar | sf::Style::Close, settings);
    window.setActive();

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();
    
    // Setup OpenGL options
    glEnable(GL_DEPTH_TEST);

    // enable alpha support
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader lightingShader("lighting.vs", "lighting.frag");
    Shader skyboxShader("skybox.vs", "skybox.frag");

    getSphereCoords();

    GLfloat skyboxVertices[] = {
        // Positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    std::vector<const GLchar*> faces{};
    faces.push_back("Yokohama3/posx.jpg");
    faces.push_back("Yokohama3/negx.jpg");
    faces.push_back("Yokohama3/posy.jpg");
    faces.push_back("Yokohama3/negy.jpg");
    faces.push_back("Yokohama3/posz.jpg");
    faces.push_back("Yokohama3/negz.jpg");

    // Sphere

    GLuint VBO{}, sphereVAO{}, EBO{};
    glGenVertexArrays(1, &sphereVAO);
    glBindVertexArray(sphereVAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numberOfPoints * sizeof(GLfloat), vertice, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numberOfIndexes * sizeof(GLuint), indices, GL_STATIC_DRAW);

    // Position attribute
    GLint posAttrib{ glGetAttribLocation(lightingShader.Program, "position") };
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(posAttrib);

    // Normal attribute
    GLint normalAttrib{ glGetAttribLocation(lightingShader.Program, "normal") };
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(0));
    glEnableVertexAttribArray(normalAttrib);
    glBindVertexArray(0);

    // Skybox
    GLuint skyboxVAO{}, skyboxVBO{};

    glGenVertexArrays(1, &skyboxVAO);
    glBindVertexArray(skyboxVAO);

    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

    // Loades Cube Map textures
    GLuint cubemapTexture{ LoadCubemap(faces) };

    glm::mat4 projection{ glm::perspective(camera.GetZoom(), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f) };
            
    bool running{ true };
    float currentFrame{};
    float deltaTime{};
    float lastFrame{};

    while (running)
    {
        currentFrame = clock.getElapsedTime().asSeconds();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        sf::Event event{};
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:             running = false;    break;
            case sf::Event::Resized:            glViewport(0, 0, event.size.width, event.size.height);  break;
            case sf::Event::KeyPressed:
                switch (event.key.code)
                {
                case sf::Keyboard::Escape:      running = false;
                case sf::Keyboard::Up:
                case sf::Keyboard::W:           camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime); break;
                case sf::Keyboard::Down:
                case sf::Keyboard::S:           camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime); break;
                case sf::Keyboard::Right:
                case sf::Keyboard::D:           camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime); break;
                case sf::Keyboard::Left:
                case sf::Keyboard::A:           camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime); break;
                default:    break;
                }
            case sf::Event::MouseMoved:         MouseCallBack(camera, sf::Mouse::getPosition().x, sf::Mouse::getPosition().y);
                break;
            default:                            break;
            }
        }

        glm::mat4 model{};
        glm::mat4 view{};
        glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.Use();

        // get camera view
        view = camera.GetViewMatrix();

        // Get the uniform locations
        GLint modelLoc{ glGetUniformLocation(lightingShader.Program, "model") };
        GLint viewLoc{ glGetUniformLocation(lightingShader.Program, "view") };
        GLint projLoc{ glGetUniformLocation(lightingShader.Program, "projection") };
        GLint cameraPosLoc{ glGetUniformLocation(lightingShader.Program, "cameraPos") };

        // Pass the matrices to the shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(cameraPosLoc, 1, glm::value_ptr(camera.GetPosition()));

        // Draw the sphere
        glBindVertexArray(sphereVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawElements(GL_TRIANGLES, numberOfIndexes, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        /* Phong
        GLint lightPosLoc = glGetUniformLocation(lightingShader.Program, "light.position");

        glm::vec3 lightColor;
        lightColor.r = sin(clock.getElapsedTime().asSeconds() * 2.0f);
        lightColor.g = sin(clock.getElapsedTime().asSeconds() * 0.7f);
        lightColor.b = sin(clock.getElapsedTime().asSeconds() * 1.3f);

        glm::vec3 diffuseColor = lightColor * glm::vec3(0.7f); // Decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // Low influence
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light.ambient"), ambientColor.r, ambientColor.g, ambientColor.b);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light.diffuse"), diffuseColor.r, diffuseColor.g, diffuseColor.b);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);

        // Set material properties
        glUniform3f(glGetUniformLocation(lightingShader.Program, "material.ambient"), 1.0f, 0.5f, 0.31f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 1.0f, 0.5f, 0.31f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "material.specular"), 0.5f, 0.5f, 0.5f); // Specular doesn't have full effect on this object's material
        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

        GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        // Create camera transformations
        view = camera.GetViewMatrix();

        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
        GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Draw the container (using container's vertex attributes)
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, numberOfPoints * sizeof(GLfloat), vertice, GL_STATIC_DRAW);
        model = glm::mat4();
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawElements(GL_TRIANGLES, numberOfIndexes, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0
        */

        glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.Use();
        
        glBindVertexArray(skyboxVAO);
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));

        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

        // Draw the cubemap
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        window.display();
    }

    glDeleteBuffers(1, &skyboxVBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteVertexArrays(1, &sphereVAO);

    return 0;
}
