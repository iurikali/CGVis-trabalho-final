#include "player.hpp"
#include "game_object.hpp"
#include <iostream>
#include <cmath>
#include "particle_spin.hpp"





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


Player::Player(std::string n, int o_id, int t_id, glm::vec3 pos, float speed): 
    AnimatedObject(n, o_id, t_id, false, false, false, false, true, pos),
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
    sector(0),
    next_sector(-1),

    hit_box(glm::vec3(-0.2, 0.0, -0.2), glm::vec3(0.2, 1.2, 0.2)),
    spin_hitbox(glm::vec3(-0.8, 0.0, -0.8), glm::vec3(0.8, 1.2, 0.8)),
    jump_hitbox(glm::vec3(-0.2, -0.1, -0.2), glm::vec3(0.2, 0.5, 0.2))
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

    CollisionPhysics(delta_time);
    
    CheckCollisionTrigger(sector - 1);
    CheckCollisionTrigger(sector);
    CheckCollisionTrigger(sector + 1);



    hit_box.DrawDebug();

    if (state == ATTACKING)
    {
        spin_hitbox.Update(position);
        spin_hitbox.DrawDebug();

        CheckCollisionSpin(sector - 1);
        CheckCollisionSpin(sector);
        CheckCollisionSpin(sector + 1);

    }

    if (state == AIR)
    {
        jump_hitbox.Update(position);
        jump_hitbox.DrawDebug();

        CheckCollisionJump(sector - 1);
        CheckCollisionJump(sector);
        CheckCollisionJump(sector + 1);
    }


    AnimatedObject::Update(delta_time);

    std::cout << "X:" << position.x << std::endl;
    std::cout << "Y:" << position.y << std::endl;
    std::cout << "Z:" << position.z << std::endl;
    // std::cout << "Sector:" << sector << std::endl;
    // std::cout << "Sector + 1:" << sector + 1 << std::endl;
    // std::cout << "Sector - 1:" << sector - 1 << std::endl;

    player_center_x = position.x ;
    player_center_z = position.z ;
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

        //Colisao da hitbox pulo

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
        break;

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
    float vel_x_temp = vel_x * delta_time;
    float vel_y_temp = vel_y * delta_time;
    float vel_z_temp = vel_z * delta_time;
    

    hit_box.Update(position);

    //Calculando os setores
    sector = (int)std::floor(position.z / SECTOR_LEN);

    int sectors_to_check[3] = {sector - 1, sector, sector + 1};

    //Eixo Z
    for (int s : sectors_to_check)
    {
        auto it = g_collision_physics.find(s);
        if (it != g_collision_physics.end()) 
        {
            for (GameObject* obj : it->second) 
            {
                if (obj->hitbox != nullptr)
                    vel_z_temp = hit_box.GetClipZ(*(obj->hitbox), vel_z_temp);
            }
        }
    }
    hit_box.box_min.z += vel_z_temp;
    hit_box.box_max.z += vel_z_temp;


    //Eixo X
    for (int s : sectors_to_check)
    {
        auto it = g_collision_physics.find(s);
        if (it != g_collision_physics.end()) 
        {
            for (GameObject* obj : it->second) 
            {
                if (obj->hitbox != nullptr)
                    vel_x_temp = hit_box.GetClipX(*(obj->hitbox), vel_x_temp);
            }
        }
    }
    hit_box.box_min.x += vel_x_temp;
    hit_box.box_max.x += vel_x_temp;

    //Eixo Y
    float old_vel_y = vel_y_temp;
    for (int s : sectors_to_check)
    {
        auto it = g_collision_physics.find(s);
        if (it != g_collision_physics.end()) 
        {
            for (GameObject* obj : it->second) 
            {
                if (obj->hitbox != nullptr)
                {
                    vel_y_temp = hit_box.GetClipY(*(obj->hitbox), vel_y_temp);

                }
            }
        }
    }
    
    //Falando que paramos de cair (mudar essa funcao, vai dar problema se der cabeçada num bloco)
    if (vel_y_temp != old_vel_y) 
    {
        vel_y = 0.0f; 
        on_air = false;
        
    }


    position.x += vel_x_temp;
    position.y += vel_y_temp;
    position.z += vel_z_temp;

    hit_box.Update(position);


    // Colisao com o plano
    if (position.y <= 0.0f)
    { 
        position.y = 0.0f;
        vel_y = 0.0f;
        on_air = false;
    }
    
}


void Player::CheckCollisionTrigger(int sector_index)
{
    //Fazendo a verificação da nossa hitbox com os triggers
    //Garantindo que o setor existe
    auto it = g_collision_triggers.find(sector_index);

    if (it != g_collision_triggers.end())
    {
        //Vamos verificar a colisao fisica com geral do nosso quadrante
        for (GameObject* obj : it->second)
        {
            if (obj->hitbox != nullptr)
            {
                if (hit_box.Intersects(*(obj->hitbox)))
                {
                    obj->on_trigger_player();
                }
            }
        }
    }
}

void Player::CheckCollisionSpin(int sector_index)
{
    //Fazendo a colisao do spin
    auto it = g_collision_spin.find(sector_index);

    if (it != g_collision_spin.end())
    {
        //Vamos verificar a colisao fisica com geral do nosso quadrante
        for (GameObject* obj : it->second)
        {
            if (obj->hitbox != nullptr)
            {
                if (spin_hitbox.Intersects(*(obj->hitbox)))
                {
                    obj->on_trigger_spin(angle_looking);
                }
            }
        }
    }
}

void Player::CheckCollisionJump(int sector_index)
{
    //Fazendo a colisao do pulo
    auto it = g_collision_jump.find(sector_index);

    if (it != g_collision_jump.end())
    {
        //Vamos verificar a colisao fisica com geral do nosso quadrante
        for (GameObject* obj : it->second)
        {
            if (obj->hitbox != nullptr)
            {
                if (jump_hitbox.Intersects(*(obj->hitbox)) && jump_hitbox.box_max.y > obj->hitbox->box_max.y)
                {
                    obj->on_trigger_jump();
                    jump(jump_height);
                }
            }
        }
    }
}

void Player::CreateParticleSpin(glm::vec3 color, int amount, float y, float radius, float speed)
{
    for (int i = 0; i < amount; i++)
    {
        float dir = ((2 * M_PI) / amount) * i;
        glm::vec3 pos = glm::vec3(player_center_x + sinf(dir) * radius, 
        y,
        player_center_z + cosf(dir) * radius);

        new ParticleSpin(color, pos, radius, dir, speed, this);
    }
}

int Player::get_current_state()
{
    return state;
}