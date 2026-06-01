#include "particle_spin.hpp"
#include "game_object.hpp"
#include <iostream>
#include <cmath>
#include "player.hpp"
#include <glm/gtc/type_ptr.hpp>


ParticleSpin::ParticleSpin(glm::vec3 color, glm::vec3 pos, float radius, float dir, float speed, Player *player):
    StaticObject("the_cube", CUBE, COBBLESTONE, false, false, false, false, true, glm::vec3(pos.x, player->position.y + pos.y, pos.z))
{
    this->dir = dir;
    this->radius = radius;
    this->speed = speed;
    this->player = player;  
    y = pos.y;
    this->color = color;

    scale = glm::vec3(0.1, 0.1, 0.1);
}


void ParticleSpin::Update(float delta_time)
{
    if (!is_destroyed)
    {
        if (player->get_current_state() == ATTACKING && !ended)
        {
            dir += delta_time * 100;

            position.x = player->player_center_x + sinf(dir) * radius;
            position.z = player->player_center_z + cosf(dir) * radius;
            position.y = player->position.y + y;
        }
        else
        {
            ended = true;
            if (scale.x > 0)
            {
                scale.x -= delta_time * .5;
                scale.z -= delta_time * .5;
                scale.y -= delta_time * .5;


                //position.x += sinf(dir) * delta_time * speed;
                //position.z += cosf(dir) * delta_time * speed;
            }
            else
            {
                is_destroyed = true;    
            }
        }
    }
    
}


void ParticleSpin::Draw()
{
    // Se a partícula já sumiu, economizamos processamento não desenhando nada
    if (is_destroyed || scale.x <= 0) return;

    // 1. Pega a matriz que estica, gira e move a partícula pro lugar exato
    glm::mat4 model = this->GetModelMatrix();

    // 2. Puxa as variáveis globais da GPU
    extern GLint g_model_uniform;
    extern GLint g_texture_id_uniform;
    extern GLint g_particle_color_uniform; // A nossa nova variável!
    
    // Manda a matriz de transformação
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    
    // O Truque 98: Avisa o shader que queremos desenhar uma cor sólida
    glUniform1i(g_texture_id_uniform, 98);

    // Manda a cor que você escolheu no construtor (this->color) para o shader!
    glUniform3f(g_particle_color_uniform, this->color.r, this->color.g, this->color.b);

    // 3. Pega os dados geométricos do modelo de cubo padrão
    extern std::map<std::string, SceneObject> g_VirtualScene;
    auto cube_data = g_VirtualScene["the_cube"];

    // 4. Manda desenhar os TRIÂNGULOS (o cubo preenchido)!
    glBindVertexArray(cube_data.vertex_array_object_id);
    
    glDrawElements(
        cube_data.rendering_mode, // Isso aqui internamente é o GL_TRIANGLES
        cube_data.num_indices,
        GL_UNSIGNED_INT,
        (void*)(cube_data.first_index * sizeof(GLuint))
    );
    
    glBindVertexArray(0); // Limpa o estado
}