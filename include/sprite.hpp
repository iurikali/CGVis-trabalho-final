#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>

class Sprite
{
public:
    glm::vec2 position; // Posição (X, Y) em pixels na tela
    glm::vec2 scale;    // Tamanho (Largura, Altura) em pixels
    float rotation;     // Rotação em radianos
    GLuint texture_id;  // O ID da textura já carregada no OpenGL

    Sprite(GLuint tex_id, int amount, float speed, glm::vec2 pos, glm::vec2 size);

    // O Draw precisa receber o shader_id da UI para saber qual programa ativar
    void Draw(float delta_time, GLuint shader_ui_id, glm::mat4 ortho_projection);

private:
    // Retorna a matriz de transformação 2D
    glm::mat4 GetModelMatrix();

    void UpdateSprite(float delta_time);

    int image_index;
    int image_amount;
    float image_speed;
    float image_delta;
    bool image_returning;
    int image_first;
};