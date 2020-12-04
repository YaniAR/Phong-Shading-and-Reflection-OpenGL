#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

// GL Includes
#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const GLfloat YAW{ -90.0f };
const GLfloat PITCH { 0.0f };
const GLfloat SPEED{ 24.0f };
const GLfloat SENSITIVTY{ 0.25f };
const GLfloat ZOOM { 45.0f };

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
private:
    // Camera Attributes
    glm::vec3 m_position{};
    glm::vec3 m_front{};
    glm::vec3 m_up{};
    glm::vec3 m_right{};
    glm::vec3 m_worldUp{};

    // Eular Angles
    GLfloat m_yaw{};
    GLfloat m_pitch{};

    // Camera options
    GLfloat m_movementSpeed{};
    GLfloat m_mouseSensitivity{};
    GLfloat m_zoom{};

    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front{};
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        m_right = glm::normalize(glm::cross(m_front, m_worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        m_up = glm::normalize(glm::cross(m_right, m_front));
    }
public:
    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH)
        : m_front{ glm::vec3(0.0f, 0.0f, -1.0f) }, m_movementSpeed{ SPEED }, m_mouseSensitivity{ SENSITIVTY }, m_zoom{ ZOOM }
    {
        this->m_position = position;
        this->m_worldUp = up;
        this->m_yaw = yaw;
        this->m_pitch = pitch;
        this->updateCameraVectors();
    }

    // Constructor with scalar values
    Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch)
        : m_front{ glm::vec3(0.0f, 0.0f, -1.0f) }, m_movementSpeed{ SPEED }, m_mouseSensitivity{ SENSITIVTY }, m_zoom{ ZOOM }
    {
        this->m_position = glm::vec3(posX, posY, posZ);
        this->m_worldUp = glm::vec3(upX, upY, upZ);
        this->m_yaw = yaw;
        this->m_pitch = pitch;
        this->updateCameraVectors();
    }

    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
    {
        GLfloat velocity = m_movementSpeed * deltaTime;

        if (direction == FORWARD)
        {
            m_position += m_front * velocity;
        }

        if (direction == BACKWARD)
        {
            m_position -= m_front * velocity;
        }

        if (direction == LEFT)
        {
            m_position -= m_right * velocity;
        }

        if (direction == RIGHT)
        {
            m_position += m_right * velocity;
        }
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true)
    {
        xOffset *= m_mouseSensitivity;
        yOffset *= m_mouseSensitivity;

        m_yaw += xOffset;
        m_pitch += yOffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (m_pitch > 89.0f)
                m_pitch = 89.0f;

            else if (m_pitch < -89.0f)
                m_pitch = -89.0f;
        }
        updateCameraVectors();
    }

    GLfloat GetZoom() { return m_zoom; }
    glm::vec3 GetPosition() { return m_position; }
    glm::vec3 GetFront() { return m_front; }
};

#endif