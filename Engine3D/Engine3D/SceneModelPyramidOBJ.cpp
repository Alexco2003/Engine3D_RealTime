#include "SceneManager.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Math libraries for 3D graphics (vectors and matrices)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "Camera.h"
#include "Shader.h"
#include "Model.h"

// Window dimensions
static const unsigned int SCR_WIDTH = 800;
static const unsigned int SCR_HEIGHT = 600;

// Create our camera object, starting slightly pulled back on the Z axis
static Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// Variables to keep track of the mouse position
static float lastX = SCR_WIDTH / 2.0f;
static float lastY = SCR_HEIGHT / 2.0f;
static bool firstMouse = true; // Prevents a sudden camera jump when the mouse first enters the window

// Variables to keep our movement smooth
static float deltaTime = 0.0f; // Time between the current frame and the last frame
static float lastFrame = 0.0f;

// Pre-declaring our input functions so the compiler knows they exist
static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void processInput(GLFWwindow* window);

int RunSceneModelPyramidOBJ() {
    // 1. Initialize GLFW and tell it we are using OpenGL 3.3 Core Profile
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. Create the window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Engine 3D - Pyramid Render (.obj)", NULL, NULL);
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

    // ENABLE DEPTH TEST: Tells OpenGL to draw things in front over things in the back
    glEnable(GL_DEPTH_TEST);

    // CREATE OUR SHADER PROGRAM
    Shader ourShader("shaders/basic.vert", "shaders/basic.frag");

    // LOAD 3D MODEL WITH ASSIMP
    Model myModel("models/pyramid.obj");

    // 4. THE RENDER LOOP
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

        // Clear both the color AND the depth buffer so 3D looks correct
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Tell OpenGL to use our shader program
        ourShader.use();

        // Create the Projection Matrix (Lens of the camera / FOV)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // Create the View Matrix (Where the camera is looking)
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        // Create the Model Matrix (Where the model is placed in the world)
        glm::mat4 model = glm::mat4(1.0f); // Identity matrix = place it at 0,0,0
        ourShader.setMat4("model", model);

        // DRAW THE MODEL
        myModel.Draw(ourShader);

        // Swap the front and back buffers (shows the frame we just drew) and check for events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 5. Clean up and exit
    glfwTerminate();
    firstMouse = true; // Reset mouse logic for the next scene
    return 0;
}

// Handles keyboard input (movement and exiting)
static void processInput(GLFWwindow* window) {
    // If ESC is pressed, tell the window it should close
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Move the camera using WASDQE keys
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}
// Handles mouse movement (looking around)
static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
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
    float yoffset = lastY - ypos;

    // Update the last known positions
    lastX = xpos; lastY = ypos;

    // Send the math to the camera class to actually rotate our view
    camera.ProcessMouseMovement(xoffset, yoffset);
}
// Handles mouse wheel scrolling (zooming in/out)
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
// Automatically adjusts the OpenGL viewport if the user resizes the window
static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}