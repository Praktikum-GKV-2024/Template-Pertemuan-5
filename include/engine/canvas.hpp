#pragma once

#include <engine/object.hpp>
#include <engine/scene.hpp>


namespace engine {
    class Canvas: public Object {
    public:
        GLuint vertex_buffer;

        Canvas (Scene* scene, GLuint program_id) {
            this->scene = scene;
            this->program_id = program_id;

            u_texture = glGetUniformLocation(program_id, "textureSampler");
            std::cout << glGetError() << std::endl;



            float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };

            // Binding VAO
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            // Populating position to GPU
            glGenBuffers(1, &vertex_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
            glBufferData(GL_ARRAY_BUFFER, 5 * 6 * sizeof(float), &quadVertices, GL_STATIC_DRAW);
            
            // Attribute pointer for position
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(
                0,                  // attribute
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                5* sizeof(float),   // stride
                (void*)0            // array buffer offset
            );

            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
            glVertexAttribPointer(
                1,                                // attribute
                2,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                5* sizeof(float),                 // stride
                (void*)(3* sizeof(float))         // array buffer offset
            );
            std::cout << glGetError() << std::endl;


        }

        void render(GLuint texture) {
            glUseProgram(program_id);

            // Send our transformation to the currently bound shader, 
            // because this is on GUI, we only need the model transform (only in 2d space)
            // u_model = glGetUniformLocation(program_id, "M");

            // glUniformMatrix4fv(u_model, 1, GL_FALSE, &transform[0][0]);

            // Bind our texture in Texture Unit 0
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);

            // Set our "textureSampler" sampler to use depthmap
            glUniform1i(u_texture, 0);

            // Draw the object
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    };

}
