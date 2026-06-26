#include "collisions.hpp"
#include "matrices.h"
#include <glm/gtc/type_ptr.hpp>
#include "game_object.hpp"


extern GLuint g_AABB_VAO;

AABB::AABB(glm::vec3 min, glm::vec3 max) : box_min(min), box_max(max), box_min_original(min), box_max_original(max),disabled(false) {}

void AABB::Update(glm::vec3 position, glm::vec3 scale)
{
    box_min = position + box_min_original * scale;
    box_max = position + box_max_original * scale;
}

bool AABB::IntersectsX(AABB against)
{
    return box_min.x < against.box_max.x && box_max.x > against.box_min.x;
}
 bool AABB::IntersectsY(AABB against)
{
    return box_min.y < against.box_max.y && box_max.y > against.box_min.y;
}
 bool AABB::IntersectsZ(AABB against)
{
    return box_min.z < against.box_max.z && box_max.z > against.box_min.z;
}
bool AABB::Intersects(AABB against)
{
    if (!disabled && !against.disabled)
        return IntersectsX(against) && IntersectsY(against) && IntersectsZ(against);
    else
        return false;
}


float AABB::GetClipX(AABB against, float deltaX)
{
    //are we overlapping the other axes?
    //(if we aren't, then an intersection could never actually take place)
    if(IntersectsY(against) && IntersectsZ(against))
    {
        //if we are moving right and our right bounds are smaller than
        //or equal to the other left bounds
        if(deltaX > 0 && box_max.x <= against.box_min.x + EPSILON)
        {
            //what is the distance to the other AABB?
            float clip = against.box_min.x - box_max.x;
            //if our move delta is larger than the distance to
            //the other AABB, set the move delta that distance
            if (deltaX > clip)
                deltaX = clip;
        }
        //the principle explained in the code above is the same for
        //everything else
        if (deltaX < 0 && box_min.x >= against.box_max.x - EPSILON)
        {
            float clip = against.box_max.x - box_min.x;
            if (deltaX < clip)
                deltaX = clip;
        }
        return deltaX;
    }
    return deltaX;
}
float AABB::GetClipY(AABB against, float deltaY)
{
    if (IntersectsX(against) && IntersectsZ(against))
    {
        if (deltaY > 0 && box_max.y <= against.box_min.y + EPSILON)
        {
            float clip = against.box_min.y - box_max.y;
            if (deltaY > clip)
                deltaY = clip;
        }
        if (deltaY < 0 && box_min.y >= against.box_max.y - EPSILON)
        {
            float clip = against.box_max.y - box_min.y;
            if (deltaY < clip)
                deltaY = clip;
        }
        return deltaY;
    }
    return deltaY;
}
float AABB::GetClipZ(AABB against, float deltaZ)
{
    
    if (IntersectsX(against) && IntersectsY(against))
    {
        if (deltaZ > 0 && box_max.z <= against.box_min.z + EPSILON)
        {
            float clip = against.box_min.z - box_max.z;
            if (deltaZ > clip)
                deltaZ = clip;
        }
        if (deltaZ < 0 && box_min.z >= against.box_max.z - EPSILON)
        {
            float clip = against.box_max.z - box_min.z;
            if (deltaZ < clip)
                deltaZ = clip;
        }
        return deltaZ;
    }
    return deltaZ;
}

//Funcao do gemini para visualizar as AABB
void AABB::DrawDebug() 
{
    if (!disabled)
    {
        // 1. Descobre o centro e o tamanho matemático da caixa
        glm::vec3 center = (box_max + box_min) / 2.0f;
        glm::vec3 size   = box_max - box_min;

        // 2. Cria a matriz Model que estica e move o cubo pro lugar exato
        glm::mat4 model = Matrix_Translate(center.x, center.y, center.z) * Matrix_Scale(size.x, size.y, size.z);

        // 3. Puxa as variáveis da GPU
        extern GLint g_model_uniform;
        extern GLint g_texture_id_uniform;
        
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        
        // O Truque do Magenta: Textura 99 não existe, então o shader vai pintar as linhas de rosa choque!
        glUniform1i(g_texture_id_uniform, 99);

        // Como é uma linha, ela não tem vetor "Normal". 
        // Essa função força o shader a achar que a Normal aponta pra cima, para a luz não deixar a linha preta.
        glVertexAttrib4f(1, 0.0f, 1.0f, 0.0f, 0.0f);

        // 4. Manda desenhar as linhas!
        glBindVertexArray(g_AABB_VAO);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}