// Camera rotation
    void switchToFace(int faceIndex) // For Cube Map camera
    {
        switch (faceIndex)
        {
        case 0:
            m_pitch = 0;
            m_yaw = 90;
            break;
        case 1:
            m_pitch = 0;
            m_yaw = -90;
            break;
        case 2:
            m_pitch = -90;
            m_yaw = 180;
            break;
        case 3:
            m_pitch = 90;
            m_yaw = 180;
            break;
        case 4:
            m_pitch = 0;
            m_yaw = 180;
            break;
        case 5:
            m_pitch = 0;
            m_yaw = 0;
            break;
        default:
            break;
        }
    }
    
static GLuint createEmptyCubeMap(int width, int height)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (GLuint i{ 0 }; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

int main()
{
    // new cubemap
    Camera reflectionCamera(glm::vec3(0.0f, 0.0f, 0.0f));

    GLint imageWidth, imageHeight;
    SOIL_load_image(faces[1], &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB);
    GLuint newEnvironment = createEmptyCubeMap(imageWidth, imageHeight);

    GLuint FBO{}, DBO{};

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glGenRenderbuffers(1, &DBO);
    glBindRenderbuffer(GL_RENDERBUFFER, DBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, imageWidth, imageHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DBO);
    
    glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
    glm::mat4 model;
    glm::mat4 view;

    glViewport(0, 0, imageWidth, imageHeight);

    GLuint tempTexture{};
    for (GLuint i{ 0 }; i < 6; ++i)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, newEnvironment, 0);

        //reflectionCamera.switchToFace(i);

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
        glUniform3fv(cameraPosLoc, 1, glm::value_ptr(reflectionCamera.GetPosition()));

        GLint lightPosLoc = glGetUniformLocation(lightingShader.Program, "light.position");
        glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

        // Draw the sphere
        glBindVertexArray(sphereVAO);
        model = glm::mat4();
        model = glm::translate(model, spherePositions[1]);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawElements(GL_TRIANGLES, numberOfIndexes, GL_UNSIGNED_INT, 0);

        //glDrawElements(GL_TRIANGLES, numberOfIndexes, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.Use();

        glBindVertexArray(skyboxVAO);
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));

        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
    }
}    
