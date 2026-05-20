#include "player.hpp"
#include "game_object.hpp"
#include <iostream>
#include <cmath>

#define M_PI 3.14159265358979323846
#define GRAVITY 12.0

float lerp (float a, float b, float t)
{
    return a + t * (b - a);
}

float lerp_angle(float from, float to, float weight) 
{
    // Diferença entre os dois ângulos
    float difference = to - from;
    
    // Normaliza a diferença para o intervalo [-PI, PI]
    while (difference < -M_PI) difference += 2.0f * M_PI;
    while (difference >  M_PI) difference -= 2.0f * M_PI;
    
    // Faz o lerp padrão usando a diferença corrigida
    return from + difference * weight;
}


Player::Player(std::string n, int o_id, int t_id, float speed): 
    AnimatedObject(n, o_id, t_id),
    is_w_pressed(false),
    is_a_pressed(false),
    is_s_pressed(false),
    is_d_pressed(false),
    vel_x(0.0f),
    vel_y(0.0f),
    vel_z(0.0f),
    speed(speed),
    index_angle(0.0f),
    angle_looking(0.0f),
    state(IDLE),
    on_air(false),
    jump_height(7.5),
    time_spin(0.2),
    time_spin_index(0.0),
    spin_speed(100.0),

    hit_box(glm::vec3(-0.5, 0.0, -0.5), glm::vec3(0.5, 1.2, 0.5)),


    teste_colisao(glm::vec3(-0.5, 0.0, -0.5), glm::vec3(0.5, 1.2, 0.5))
{
    std::cout << "PLAYER CRIADO" << std::endl;

    teste_colisao.Update(glm::vec3(2.0, 0.0, 2.0));

}


void Player::Update(float delta_time)
{
    vel_x = (float) (is_d_pressed - is_a_pressed) * speed;
    vel_z = (float) (is_s_pressed - is_w_pressed) * speed;


    
    //Calculando a rotação
    if (!spin)
    {
        if(vel_x != 0.0 || vel_z != 0.0)
            angle_looking = std::atan2(vel_x, vel_z);
        index_angle = lerp_angle(index_angle, angle_looking, 16.0 * delta_time);
        rotation.y = index_angle;
    }
    

    state_machine(delta_time);



    //Gravidade
    if (position.y > 0) // Não está no chão
    {
        vel_y -= GRAVITY * speed * delta_time;
        on_air = true;
    }
    if (vel_y <= -speed * 4)
    {
        vel_y = -speed * 4;
    }

    //Testando a colisao
    /*if (hit_box.Intersects(teste_colisao))
    {
        std::cout << "COLIDI CARALHO" << std::endl;
    }*/


    CollisionPhysics(delta_time);
    

    hit_box.DrawDebug();

    teste_colisao.DrawDebug();


    AnimatedObject::Update(delta_time);
}

void Player::set_w_pressed(bool b)
{
    is_w_pressed = b;
}
void Player::set_s_pressed(bool b)
{
    is_s_pressed = b;
}
void Player::set_a_pressed(bool b)
{
    is_a_pressed = b;
}
void Player::set_d_pressed(bool b)
{
    is_d_pressed = b;
}
void Player::set_space_pressed(bool b)
{
    is_space_pressed = b;
}

bool Player::get_on_air()
{
    return on_air;
}

void Player::state_machine(float delta_time)
{
    switch (state)
    {
    case IDLE:
        if (Player::current_animation != IDLE)
        {
            Player::SetAnimation(IDLE);
        }

        //Saindo do estado
        if (spin)
        {
            state = ATTACKING;
        }
        else if (on_air)
        {
            state = AIR;
        }
        else if (vel_x != 0.0 || vel_z != 0.0)
        {
            state = WALKING;
        }
        break;
    

    case WALKING:
        if (Player::current_animation != WALKING)
        {
            Player::SetAnimation(WALKING);
        }

        //Saindo do estado
        if (spin)
        {
            state = ATTACKING;
        }
        else if (on_air)
        {
            state = AIR;
        }
        else if (vel_x == 0.0 && vel_z == 0.0)
        {
            state = IDLE;
        }
        break;
    
    case AIR:
        if (Player::current_animation != AIR)
        {
            
            Player::SetAnimation(AIR);
        }

        //Corrigindo a rotacao da animacao
        rotation.y = index_angle + M_PI / 4;

        //Saindo do estado
        if (spin)
        {
            state = ATTACKING;
        }
        else if (!on_air)
        {
            if (vel_x == 0.0 && vel_z == 0.0)
            {
                state = IDLE;
            }
            else
            {
                state = WALKING;
            }
        }

        break;

    case ATTACKING:
    if (Player::current_animation != ATTACKING)
    {
        Player::SetAnimation(ATTACKING);
        time_spin_index = time_spin;
    }

    //Corrigindo a rotacao da animacao
    rotation.y += spin_speed * delta_time;

    //Rodando o contador
    if (time_spin_index > 0)
    {
        time_spin_index -= delta_time;
    }
    //Acabou o tempo, vamos sair do estado
    else
    {
        time_spin_index = 0;
        spin = false;

        if (!on_air)
        {
            if (vel_x == 0.0 && vel_z == 0.0)
            {
                state = IDLE;
            }
            else
            {
                state = WALKING;
            }
        }
        else
        {
            state = AIR;
        }
    }

    //Saindo do estado


    default:
        break;
    }
}

void Player::jump(float height)
{
    vel_y = height;
    on_air = true;
}

float Player::get_jump_height()
{
    return jump_height;
}

bool Player::get_spin()
{
    return spin;
}

void Player::set_spin(bool b)
{
    spin = b;
}

//Funcao que cuida da colisao fisica AABB
//para resolver o problema das diagonais
//Existe esse avanço na box
void Player::CollisionPhysics(float delta_time)
{

    //Colisao AABB modificada
    float vel_x_temp = vel_x * delta_time;
    float vel_y_temp = vel_y * delta_time;
    float vel_z_temp = vel_z * delta_time;

    
    hit_box.Update(position);

    //Eixo X
    vel_x_temp = hit_box.GetClipX(teste_colisao, vel_x_temp);
    hit_box.box_min.x += vel_x_temp;
    hit_box.box_max.x += vel_x_temp;

    //Eixo Z
    vel_z_temp = hit_box.GetClipZ(teste_colisao, vel_z_temp);
    hit_box.box_min.z += vel_z_temp;
    hit_box.box_max.z += vel_z_temp;

    //Colisao na vertical
    float old_vel_y = vel_y_temp;
    vel_y_temp = hit_box.GetClipY(teste_colisao, vel_y_temp);
    
    // Verificando se colidimos em Y (chão ou teto)
    if (vel_y_temp != old_vel_y)
    {
        vel_y = 0.0f; // Zera a gravidade real para parar de cair
        on_air = false;
    }

    
    position.x += vel_x_temp;
    position.y += vel_y_temp;
    position.z += vel_z_temp;

    hit_box.Update(position);

    //"Colisao" com o plano
    if (position.y <= 0){ // está no chão (ou abaixo)
        position.y = 0;
        vel_y = 0;
        on_air = false;
    }
}