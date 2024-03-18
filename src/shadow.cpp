#define GLM_FORCE_PURE

#include <GLFW/glfw3.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <iostream>
#include <cmath>
#include <vector>
#include <cstring>

#include <common/controls.hpp>
#include <common/shader.hpp>

#include <engine/scene.hpp>
#include <engine/object.hpp>
#include <engine/canvas.hpp>


#define M_PI        3.14159265358979323846
#define onii        using
#define oniichan    using
#define suki        std::cout
#define hentai      std::endl
#define chan        namespace
#define mendokusai  std::chrono
#define daikirai    glm

#define gohan_ni     using
#define ofuro_ni     using
// #define ni          namespace
#define suru         glm::vec3
#define suru_        glm::mat4
#define soretomo     using
#define watashi      glm::vec4

// tehee
oniichan suki;
oniichan hentai;
onii chan mendokusai;
onii chan daikirai;

gohan_ni suru;//?
ofuro_ni suru_;//?
soretomo watashi;//?

enum class CAMERA_MODE{
    PROJECTION_1,
    PROJECTION_2,
    PROJECTION_3,
    LIGHT,
    FREE_VIEW,
};

class MainScene : engine::Scene {
public:
    GLFWwindow* window;

    engine::Object *barrel_1, *barrel_2, *barrel_3, *plane;
    engine::Canvas *canvas;

    CAMERA_MODE camera_mode = CAMERA_MODE::PROJECTION_1;

    GLuint shader_depth, shader_shadow, shader_depth_canvas;
    GLuint u_lightSpaceMatrix_depth, u_lightSpaceMatrix_shadow,
           u_near_plane, u_far_plane;
    GLuint depthMapFBO;
    GLuint depthMap;
    unsigned int SHADOW_WIDTH, SHADOW_HEIGHT;

    float near_plane = 1.0f, far_plane = 50.5f;

    double timer = 1;
    int frame_count = 0;

    MainScene (GLFWwindow* window): Scene(window) {
        this->window = window;

        glClearColor(0.5f, 0.5f, 0.5f, 1.f);

        // Enable depth test
        glEnable(GL_DEPTH_TEST);

        // Accept fragment if it is closer to the camera than the former one
        // glDepthFunc(GL_LEQUAL); 

        // Cull triangles which normal is not towards the camera
        // glEnable(GL_CULL_FACE);

        // Blending
        glEnable(GL_BLEND);
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Associate the object instance with the GLFW window
        glfwSetWindowUserPointer(window, this);

        start();
    }

    void start() override {
        // Light
        LightPosition = vec3(0, 10, 0);
        

        // create shaders
        shader_shadow = LoadShaders("res/shader/shadow_mapping.vs", "res/shader/shadow_mapping.fs");
        u_lightSpaceMatrix_shadow = glGetUniformLocation(shader_shadow, "lightSpaceMatrix");
        

        shader_depth = LoadShaders("res/shader/depth_pass.vs", "res/shader/depth_pass.fs");
        u_lightSpaceMatrix_depth = glGetUniformLocation(shader_depth, "lightSpaceMatrix");

        shader_depth_canvas = LoadShaders("res/shader/depth_canvas.vs", "res/shader/depth_canvas.fs");
        u_near_plane = glGetUniformLocation(shader_depth_canvas, "near_plane");
        u_far_plane = glGetUniformLocation(shader_depth_canvas, "far_plane");
        

        

        // ======================== PLANE ========================

        // LOAD PLANE MODEL
        plane = new engine::Object("res/obj/plane.obj", "res/bmp/plane.bmp", shader_shadow, this);
        

        // ======================== BARREL ========================

        // LOAD BARREL MODEL
        barrel_1 = new engine::Object("res/obj/barrel.obj", "res/bmp/barrel.bmp", shader_shadow, this);
        barrel_1->transform = glm::translate(barrel_1->transform, vec3(4, 4, 1));
        

        barrel_2 = new engine::Object("res/obj/barrel.obj", "res/bmp/barrel.bmp", shader_shadow, this);
        barrel_2->transform = glm::translate(barrel_2->transform, vec3(8, 2, -7));
        

        
        barrel_3 = new engine::Object("res/obj/barrel.obj", "res/bmp/barrel.bmp", shader_shadow, this);
        barrel_3->transform = glm::translate(barrel_3->transform, vec3(16, 1, -13));
        


        // ======================== CANVAS FOR VISUALIZATION ========================
        canvas = new engine::Canvas(this, shader_depth_canvas);
        



        // ======================== Configure depth map FBO ========================
        glGenFramebuffers(1, &depthMapFBO);


        // create depth texture
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
            glDrawBuffer(GL_NONE);
            // glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // binding keys
        glfwSetKeyCallback(window, keyCallbackStatic);
    }



    void update() override {
        Scene::update();
        SHADOW_WIDTH = window_width;
        SHADOW_HEIGHT = window_height;

        // calculating FPS
        timer -= delta_time/10e8;
        frame_count++;
        if (timer < 0) {
            std::cout << "FPS: " << frame_count << std::endl;
            timer = 1;
            frame_count = 0;
        }

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // moving light in circular pattern
        LightPosition.x = 0;
        LightPosition.z = 5.f;
        LightPosition.y = 10.f;
        // LightPosition.z = cos(glfwGetTime()) * 400.f;
        // LightPosition.y = sin(glfwGetTime()) * 400.f;

        // ======================== CAMERA STUFF ========================

        // vec3 camera_position;
        auto up = vec3( 0, 1, 0);
        auto aspect_ratio = (float)window_width/window_height;
        switch (camera_mode) {
            case CAMERA_MODE::PROJECTION_1:
                ViewMatrix = glm::lookAt(
                    vec3(0, 4, 5), // posisi kamera
                    vec3(4, 1, 1), // melihat barrel
                    up
                );
                ProjectionMatrix = glm::perspective<float>(glm::radians(45.f), aspect_ratio, 0.1f, 1000.0f);
                break;  
            case CAMERA_MODE::PROJECTION_2:
                ViewMatrix = glm::lookAt(
                    vec3(0, 1, 0), // posisi kamera
                    vec3(4, 1, 1), // melihat barrel
                    up
                );
                ProjectionMatrix = glm::perspective<float>(glm::radians(45.f), aspect_ratio, 0.1f, 1000.0f);
                break;
            case CAMERA_MODE::PROJECTION_3:
                ViewMatrix = glm::lookAt(
                    vec3(3, 2, 6), // posisi kamera
                    vec3(4, 1, 1), // melihat barrel
                    up
                );
                ProjectionMatrix = glm::perspective<float>(glm::radians(45.f), aspect_ratio, 0.1f, 1000.0f);
                break;
            case CAMERA_MODE::LIGHT:
                ViewMatrix = glm::lookAt(
                    LightPosition, // posisi kamera
                    vec3(4, 2, 1), // melihat barrel
                    up
                );
                ProjectionMatrix = glm::perspective<float>(glm::radians(45.f), aspect_ratio, 0.1f, 1000.0f);
                break;
            case CAMERA_MODE::FREE_VIEW:
                // Don't touch
                computeMatricesFromInputs(window);
                ViewMatrix = getViewMatrix();
                ProjectionMatrix = getProjectionMatrix();
                break;
        }

        // ======================== Generate Depth Map from Light's Perspective ========================
        
        glUseProgram(shader_depth);
        
        
        auto lightProjection = glm::perspective<float>(
            glm::radians(45.0f), 
            (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, 
            near_plane, far_plane);
        // auto lightProjection = glm::ortho(
        //     -20.0f, 20.0f, 
        //     -20.0f, 20.0f, 
        //     near_plane, far_plane);
        auto lightView = glm::lookAt(LightPosition, vec3(4, 6, 1), up);
        auto lightSpaceMatrix = lightProjection * lightView;
        glUniformMatrix4fv(u_lightSpaceMatrix_depth, 1, GL_FALSE, &lightSpaceMatrix[0][0]);
        glUniform1f(glGetUniformLocation(shader_depth, "near_plane"), near_plane);
        glUniform1f(glGetUniformLocation(shader_depth, "far_plane"), far_plane);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            plane->render(shader_depth);
            barrel_1->render(shader_depth);
            barrel_2->render(shader_depth);
            barrel_3->render(shader_depth);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // ======================== RENDER SCENE WITH SHADOW ========================
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // glUseProgram(shader_shadow);
        
        // glUniformMatrix4fv(u_lightSpaceMatrix_shadow, 1, GL_FALSE, &lightSpaceMatrix[0][0]);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, depthMap);
        
        // plane->render_with_projection();
        // barrel_1->render_with_projection();
        // barrel_2->render_with_projection();
        // barrel_3->render_with_projection();
        

        // // ======================== RENDER DEPTH MAP AS CANVAS ========================
        glUseProgram(shader_depth_canvas);

        glUniform1f(u_near_plane, near_plane);
        glUniform1f(u_far_plane, far_plane);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        canvas->render(depthMap);

    }

private:
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS) {
            std::cout << key << std::endl;
            switch (key) {
            case GLFW_KEY_1:
                camera_mode = CAMERA_MODE::PROJECTION_1;
                break;
            case GLFW_KEY_2:
                camera_mode = CAMERA_MODE::PROJECTION_2;
                break;
            case GLFW_KEY_3:
                camera_mode = CAMERA_MODE::PROJECTION_3;
                break;
            case GLFW_KEY_9:
                camera_mode = CAMERA_MODE::LIGHT;
                break;
            case GLFW_KEY_0:
                camera_mode = CAMERA_MODE::FREE_VIEW;
                break;
            case GLFW_KEY_MINUS:
                far_plane -= 5;
                break;
            case GLFW_KEY_EQUAL:
                far_plane += 5;
                break;
            }
        }
    }

    static void keyCallbackStatic(GLFWwindow* window, int key, int scancode, int action, int mods) {
        MainScene* instance = static_cast<MainScene*>(glfwGetWindowUserPointer(window));
        instance->key_callback(window, key, scancode, action, mods);
    }
    

};