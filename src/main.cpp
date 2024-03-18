#define GLM_FORCE_PURE
#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "shadow.cpp"

int main(void) {
    GLFWwindow* window;

	// =======================================================
	//     _____   ____________   ________    _______       __
	//    /  _/ | / /  _/_  __/  / ____/ /   / ____/ |     / /
	//    / //  |/ // /  / /    / / __/ /   / /_   | | /| / / 
	//  _/ // /|  // /  / /    / /_/ / /___/ __/   | |/ |/ /  
	// /___/_/ |_/___/ /_/     \____/_____/_/      |__/|__/
	// ========================================================

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Options untuk GLFW*/
    glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// menggunakan OpenGL 3.0 (masih dapat menggunakan legacy)

    /* Create a windowed mode window and its OpenGL context */
    std::string window_title = "Pertemuan 5";
    window = glfwCreateWindow(640, 480, window_title.c_str(), NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval( 0 );

    // =======================================================
    //     _____   ____________   ________    _______       __
    //    /  _/ | / /  _/_  __/  / ____/ /   / ____/ |     / /
    //    / //  |/ // /  / /    / / __/ /   / __/  | | /| / / 
    //  _/ // /|  // /  / /    / /_/ / /___/ /___  | |/ |/ /  
    // /___/_/ |_/___/ /_/     \____/_____/_____/  |__/|__/   
	// ========================================================
    GLenum err = glewInit();
    glewExperimental = true;
    if (GLEW_OK != err) {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "Error: " << glewGetErrorString(err);
    }

    std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;

    // ========================================================

    // Uncomment for control
    // Ensure we can capture the escape key being pressed below
	// glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // // Hide the mouse and enable unlimited movement
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // // Set the mouse at the center of the screen
    // glfwPollEvents();
    // glfwSetCursorPos(window, 640/2, 480/2);

	// Instantiate MainScene
    MainScene* scene = new MainScene(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

        /* update frame of the scene */
        scene->update();

        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR)
        {
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}