#include "sprite.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Vamos assumir que você vai criar esse VAO globalmente na main
extern GLuint g_QuadVAO; 

Sprite::Sprite(GLuint tex_id, int amount, float speed, glm::vec2 pos, glm::vec2 size)
{
    this->position = pos;
    this->scale = size;
    this->rotation = 0.0f;
    this->texture_id = tex_id;
    image_returning = false;
    image_delta = 0;
    image_index = tex_id;
    image_amount = amount;
    image_speed = speed;

    image_first = tex_id;
}

glm::mat4 Sprite::GetModelMatrix()
{
    glm::mat4 model = glm::mat4(1.0f);
    
    // 1. Movemos o sprite para a posição correta na tela
    model = glm::translate(model, glm::vec3(position, 0.0f));

    // 2. Rotacionamos (opcional, movemos o pivô para o centro se necessário)
    // Opcional: Se quiser que ele gire pelo centro, faça um translate negativo antes e positivo depois.
    model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f)); 

    // 3. Esticamos o quadrado 1x1 para o tamanho real em pixels
    model = glm::scale(model, glm::vec3(scale, 1.0f));

    return model;
}

void Sprite::Draw(float delta_time, GLuint shader_ui_id, glm::mat4 ortho_projection)
{   
    UpdateSprite(delta_time);

    // 1. Usa o shader da UI
    glUseProgram(shader_ui_id);

    // 2. Define as matrizes
    glm::mat4 model = this->GetModelMatrix();
    GLint model_loc = glGetUniformLocation(shader_ui_id, "model");
    GLint proj_loc = glGetUniformLocation(shader_ui_id, "projection");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(ortho_projection));

    // 3. Vincula a textura na gaveta 31
    glActiveTexture(GL_TEXTURE30);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glUniform1i(glGetUniformLocation(shader_ui_id, "ui_texture"), 30);

    // 4. Desenha
    glBindVertexArray(g_QuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 5. --- LIMPEZA DE ESTADO OBRIGATÓRIA ---
    glBindVertexArray(0);          // Desvincula o VAO do quadrado
    glUseProgram(0);               // Desvincula o shader da UI
    
    // Volta para a textura 0 (onde o mundo 3D espera estar)
    glActiveTexture(GL_TEXTURE0); 
}

void Sprite::UpdateSprite(float delta_time)
{
    if (image_speed != 0)
    {
        image_delta += delta_time;

        if (image_delta >= image_speed)
        {
            if (!image_returning)
                texture_id++;
            else 
                texture_id--;

            image_delta = 0;

            if (texture_id >= image_first + image_amount - 1)
            {
                image_returning = true;
            }
            else if (texture_id <= image_first)
            {
                image_returning = false;
            }
        }
    }

}