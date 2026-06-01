#include "particles.hpp"
#include "game_object.hpp"
#include <iostream>
#include <cmath>
#include <glm/gtc/type_ptr.hpp>


Particles::Particles(glm::vec3 color, glm::vec3 pos_inicial, float theta, float fi, 
    float speed, float speed_disappear, glm::vec3 scale):
    StaticObject("", 0, 0, false, false, false, false, true, glm::vec3(pos_inicial))
{
    this->color = color;
    this->fi = fi;
    this->speed = speed;
    this->theta = theta;
    this->speed_disappear = speed_disappear;

    this->scale = scale;
}


void Particles::Update(float delta_time)
{
    if (!is_destroyed)
    {
        if (scale.x > 0)
        {
            scale.x -= delta_time * speed_disappear;
            scale.y -= delta_time * speed_disappear;
            scale.z -= delta_time * speed_disappear;

            //Coordenadas esfericas
            position.x += sinf(theta) * cosf(fi) * delta_time * speed;
            position.y += sinf(fi) * delta_time * speed;
            position.z += cosf(theta) * cosf(fi) * delta_time * speed;
        }
        else
        {
            is_destroyed = true;
        }
    }
    
}


void Particles::Draw()
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