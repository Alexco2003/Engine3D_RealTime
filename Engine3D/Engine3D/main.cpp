#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Math libraries for 3D graphics (vectors and matrices)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include "Camera.h"

// Window dimensions
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Create our camera object, starting slightly pulled back on the Z axis
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// Variables to keep track of the mouse position to calculate how much we looked around
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true; // Prevents a sudden camera jump when the mouse first enters the window

// Variables to keep our movement smooth regardless of computer speed
float deltaTime = 0.0f; // Time between the current frame and the last frame
float lastFrame = 0.0f;

// Pre-declaring our input functions so the compiler knows they exist
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

int main() {
    // 1. Initialize GLFW and tell it we are using OpenGL 3.3 Core Profile
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. Create the window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Engine 3D", NULL, NULL);
    if (window == NULL) {
        std::cout << "Error creating GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Make this window the main context for OpenGL

    // Tell GLFW to call these functions whenever the user resizes the window, moves the mouse, or scrolls
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Hide the mouse cursor and lock it inside the window
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 3. Load GLAD (this loads all the OpenGL functions so we can actually use them)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Error loading GLAD" << std::endl;
        return -1;
    }

    // 4. THE RENDER LOOP (keeps running until we close the app)
    while (!glfwWindowShouldClose(window)) {
        // Calculate deltaTime so movement speed is consistent on all computers
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check if the user pressed any keys (like WASD)
        processInput(window);

        // Debug info: print camera position and angles to the console in real-time
        // The '\r' at the end overwrites the same line instead of spamming the console
        std::cout << "Pos: [X: " << camera.Position.x
            << ", Y: " << camera.Position.y
            << ", Z: " << camera.Position.z
            << "] | Yaw: " << camera.Yaw
            << " | Pitch: " << camera.Pitch << "      \r";

        // Clear the screen with a dark greenish-blue color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap the front and back buffers (shows the frame we just drew) and check for events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 5. Clean up and exit
    glfwTerminate();
    return 0;
}

// Handles keyboard input (movement and exiting)
void processInput(GLFWwindow* window) {
    // If ESC is pressed, tell the window it should close
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Move the camera using WASD keys
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// Handles mouse movement (looking around)
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    // If it's the first time the mouse enters the screen, set initial coordinates to avoid a huge jump
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // Calculate how much the mouse moved since the last frame
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed because Y-coordinates go from bottom to top

    // Update the last known positions
    lastX = xpos;
    lastY = ypos;

    // Send the math to the camera class to actually rotate our view
    camera.ProcessMouseMovement(xoffset, yoffset);
}

// Handles mouse wheel scrolling (zooming in/out)
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// Automatically adjusts the OpenGL viewport if the user resizes the window
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}