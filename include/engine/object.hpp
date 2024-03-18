#pragma once

#define GLM_FORCE_PURE
// #include <glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstring>
#include <common/loader.hpp>
#include <common/vbo_indexer.hpp>
#include <common/bmp_loader.hpp>
#include <engine/scene.hpp>

namespace engine {
    class Object {
    public:
        GLuint program_id;
        GLuint VAO;
        GLuint IBO;

        GLuint u_mvp, u_model, u_view;
        GLuint position_buffer;
        GLuint uv_buffer;
        GLuint normal_buffer;
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;

        GLuint texture_buffer;
        GLuint u_texture, u_shadow;

        GLuint u_light_source;

        std::vector<unsigned int> indices;

        glm::mat4 transform =glm::mat4(1.);
        Scene *scene;
        
        Object () {}

        Object (const char* obj_file, const char* bmp_file, GLuint program_id, Scene* scene) {
            this->scene = scene;
            // std::vector<glm::vec3> temp_positions;
            // std::vector<glm::vec2> temp_uvs;
            // std::vector<glm::vec3> temp_normals;

            loadOBJ(obj_file, positions, uvs, normals);
            // loadOBJ(obj_file, temp_positions, temp_uvs, temp_normals);
            // indexVBO(temp_positions, temp_uvs, temp_normals, indices, positions, uvs, normals);
            
            this->program_id = program_id;

            texture_buffer = loadBMP(bmp_file);
            

            // Binding VAO
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            // Populating position to GPU
            glGenBuffers(1, &position_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
            glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0], GL_STATIC_DRAW);
            
            // Attribute pointer for position
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(
                0,                  // attribute
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
            );

            // Populating uv to GPU
            glGenBuffers(1, &uv_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
            glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
            glVertexAttribPointer(
                1,                                // attribute
                2,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            // Populating normals to GPU
            glGenBuffers(1, &normal_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
            
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(
                2,                                // attribute
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            glGenBuffers(1, &IBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

        }

        ~Object() {
            glDeleteBuffers(1, &position_buffer);
            glDeleteBuffers(1, &uv_buffer);
            glDeleteBuffers(1, &normal_buffer);
            glDeleteBuffers(1, &IBO);
            glDeleteProgram(program_id);
            glDeleteTextures(1, &texture_buffer);
            glDeleteVertexArrays(1, &VAO);
        }

        void render_with_projection(GLuint shader) {
            ProjectionMatrix = getProjectionMatrix();
            ViewMatrix  = getViewMatrix();
            render(shader);
        }

        void render_with_projection() {
            render_with_projection(this->program_id);
        }

        void render() {
            render(program_id);
        }

        void render(GLuint shader) {
            glUseProgram(shader);

            glm::mat4 MVP = this->scene->ProjectionMatrix * this->scene->ViewMatrix * transform;

            // Send our transformation to the currently bound shader, 
            // in the "MVP" uniform
            
            u_mvp = glGetUniformLocation(shader, "MVP");
            if (u_mvp != -1 ) {
                glUniformMatrix4fv(u_mvp, 1, GL_FALSE, &MVP[0][0]);
            }
            u_view = glGetUniformLocation(shader, "V");
            if (u_view != -1) {
                glUniformMatrix4fv(u_view, 1, GL_FALSE, &this->scene->ViewMatrix[0][0]);
            }
            u_model = glGetUniformLocation(shader, "M");
            if (u_model != -1) {
                glUniformMatrix4fv(u_model, 1, GL_FALSE, &transform[0][0]);
            }

            u_light_source = glGetUniformLocation(shader, "LightPosition_worldspace");
            if (u_light_source != -1) {
                glUniform3fv(u_light_source, 1, &this->scene->LightPosition[0]);
            }

            u_texture = glGetUniformLocation(shader, "textureSampler");
            if (u_texture != -1) {
                // Bind our texture in Texture Unit 0
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_buffer);
                // Set our "textureSampler" sampler to use Texture Unit 0
                glUniform1i(u_texture, 0);
            }
            u_shadow = glGetUniformLocation(shader, "shadowMap");
            if (u_texture != -1) {
                glUniform1i(u_shadow, 1);
            }
            
            // Draw the object
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, positions.size() );
        }

        void setTextureBuffer(GLuint texture_buffer) {
            
            this->texture_buffer = texture_buffer;
        }
    };
}
