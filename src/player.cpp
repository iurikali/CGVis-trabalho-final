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
    spin_speed(100.0)

{
    std::cout << "PLAYER CRIADO" << std::endl;
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


    position.x += vel_x * delta_time;
    position.y += vel_y * delta_time;
    position.z += vel_z * delta_time;

    //"Colisao" com o plano
    if (position.y <= 0){ // está no chão (ou abaixo)
        position.y = 0;
        vel_y = 0;
        on_air = false;
    }


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
        //oi
    }

    //Corrigindo a rotacao da animacao
    rotation.y += spin_speed * delta_time;

    //Rodando o contador
    if (time_spin_index > 0)
    {
        time_spin_index -= delta_time;
        std::cout << time_spin_index << std::endl;
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