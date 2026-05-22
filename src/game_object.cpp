#include "matrices.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>
#include "game_object.hpp"

// As variáveis uniformes do OpenGL definidas na sua main
extern GLuint g_GpuProgramID;
extern GLint g_model_uniform;
extern GLint g_object_id_uniform;
extern GLint g_texture_id_uniform;
extern GLint g_bones_uniform;
extern GLint g_bbox_min_uniform;
extern GLint g_bbox_max_uniform;


extern GLuint g_AABB_VAO;

GameObject::GameObject(std::string n, int o_id, int t_id, CollisionLayer layer, glm::vec3 pos) : 
    name(n), object_id(o_id), texture_id(t_id), position(pos), is_destroyed(false)
    {
        //Adicionando a lista certa
        if (layer == LAYER_FISICO)
        {
            //Calculando o setor
            int sector = (int) position.z / SECTOR_LEN;
            g_collision_physics[sector].push_back(this);
        }
        else if (layer == LAYER_TRIGGER)
        {
            //Calculando o setor
            int sector = (int) position.z / SECTOR_LEN;
            g_collision_triggers[sector].push_back(this);
        }
    }

//Matriz final do objeto
glm::mat4 GameObject::GetModelMatrix()
{
    glm::mat4 matrix_model = Matrix_Translate(position.x, position.y, position.z) *
    Matrix_Rotate_Z(rotation.z) * Matrix_Rotate_Y(rotation.y) * Matrix_Rotate_X(rotation.x) *
    Matrix_Scale(scale.x, scale.y, scale.z);

    return matrix_model;
}

//Os .obj
StaticObject::StaticObject(std::string n, int o_id, int t_id, CollisionLayer layer, glm::vec3 pos) : 
    GameObject(n, o_id, t_id, layer, pos){}

void StaticObject::Draw()
{
    glm::mat4 model = GetModelMatrix();
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(g_object_id_uniform, object_id);
    glUniform1i(g_texture_id_uniform, texture_id);

    SceneObject& obj_data = g_VirtualScene[name];
    
    
    glUniform4f(g_bbox_min_uniform, obj_data.bbox_min.x, obj_data.bbox_min.y, obj_data.bbox_min.z, 1.0f);
    glUniform4f(g_bbox_max_uniform, obj_data.bbox_max.x, obj_data.bbox_max.y, obj_data.bbox_max.z, 1.0f);

    glBindVertexArray(obj_data.vertex_array_object_id);
    glDrawElements(obj_data.rendering_mode, obj_data.num_indices, GL_UNSIGNED_INT, (void*)(obj_data.first_index * sizeof(GLuint)));
    glBindVertexArray(0);

    if (GameObject::hitbox != nullptr)
    {
        hitbox->DrawDebug();
    }
}

//Os .gltf
AnimatedObject::AnimatedObject(std::string n, int o_id, int t_id, CollisionLayer layer, glm::vec3 pos): 
    GameObject(n, o_id, t_id, layer, pos),
    current_animation(0),
    animation_speed(1.0f),
    current_time(0.0f)
{
    for (int i = 0; i < 100; i++)
    {
        final_bone_matrices[i] = Matrix_Identity();
    }
}

//Funcao que troca de animacao
void AnimatedObject::SetAnimation(int anim_index)
{
    AnimatedSceneObject& obj_data = g_AnimatedScene[name];
    //Vendo se tem essa animacao
    if (anim_index >= 0 && anim_index < obj_data.gltf_data.animations.size())
    {
        current_animation = anim_index;
        current_time = 0.0f;
    }
}

void AnimatedObject::Update(float delta_time)
{
    current_time += delta_time * animation_speed;

    AnimatedSceneObject& obj_data = g_AnimatedScene[name];

    if (!obj_data.gltf_data.scenes.empty()) 
    {
        int sceneIndex = obj_data.gltf_data.defaultScene > -1 ? obj_data.gltf_data.defaultScene : 0;
        for (int rootNode : obj_data.gltf_data.scenes[sceneIndex].nodes) 
        {
            ProcessSkeletonNode(rootNode, Matrix_Identity(), obj_data);
        }
    }
}

void AnimatedObject::Draw() 
{
    glm::mat4 model = GetModelMatrix();
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(g_object_id_uniform, object_id);
    
    // Avisa o shader qual número do array usar (Gaveta/Texture Unit)
    glUniform1i(g_texture_id_uniform, texture_id);
    
    glUniformMatrix4fv(g_bones_uniform, 100, GL_FALSE, glm::value_ptr(final_bone_matrices[0]));

    AnimatedSceneObject& obj_data = g_AnimatedScene[name];
    
    GLint has_texture_uniform = glGetUniformLocation(g_GpuProgramID, "has_texture");
    
    glBindVertexArray(obj_data.vertex_array_object_id);

    // Agora iteramos sobre cada parte (primitiva) do modelo!
    for (const auto& prim : obj_data.primitives) 
    {
        if (prim.texture_id != 0) 
        {
            glActiveTexture(GL_TEXTURE0 + texture_id); 
            glBindTexture(GL_TEXTURE_2D, prim.texture_id);
            glUniform1i(has_texture_uniform, 1); 
        } 
        else 
        {
            glUniform1i(has_texture_uniform, 0); 
        }

        // Desenha APENAS os índices relativos a esta primitiva específica.
        // Multiplicamos o first_index por sizeof(GLuint) porque o OpenGL espera um deslocamento em bytes.
        glDrawElements(
            obj_data.rendering_mode, 
            prim.num_indices, 
            GL_UNSIGNED_INT, 
            (void*)(prim.first_index * sizeof(GLuint))
        );
    }
    
    glBindVertexArray(0);

    if (GameObject::hitbox != nullptr)
    {
        hitbox->DrawDebug();
    }
}

//Gemini que fez essas 3 funcoes
int AnimatedObject::GetJointIndex(int nodeIndex, AnimatedSceneObject& obj) {
    if (obj.gltf_data.skins.empty()) return -1;
    const auto& joints = obj.gltf_data.skins[0].joints;
    for (size_t i = 0; i < joints.size(); ++i) {
        if (joints[i] == nodeIndex) return i;
    }
    return -1;
}

glm::mat4 AnimatedObject::GetNodeTransform(int nodeIndex, AnimatedSceneObject& obj) {
    tinygltf::Node& node = obj.gltf_data.nodes[nodeIndex];
    glm::vec3 T(0.0f); if(node.translation.size() == 3) T = glm::vec3(node.translation[0], node.translation[1], node.translation[2]);
    glm::quat R = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); if(node.rotation.size() == 4) R = glm::quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
    glm::vec3 S(1.0f); if(node.scale.size() == 3) S = glm::vec3(node.scale[0], node.scale[1], node.scale[2]);
    
    if (obj.gltf_data.animations.empty()) {
        if(node.matrix.size() == 16) return glm::make_mat4(node.matrix.data());
        return Matrix_Translate(T.x, T.y, T.z) * glm::mat4_cast(R) * Matrix_Scale(S.x, S.y, S.z);
    }

    if (obj.current_animation_index >= obj.gltf_data.animations.size()) this->current_animation = 0;
    const tinygltf::Animation& anim = obj.gltf_data.animations[this->current_animation]; 
    bool isNodeAnimated = false;

    for (const auto& channel : anim.channels) {
        if (channel.target_node != nodeIndex) continue;
        const tinygltf::AnimationSampler& sampler = anim.samplers[channel.sampler];
        const tinygltf::Accessor& inputAcc = obj.gltf_data.accessors[sampler.input];
        const tinygltf::Accessor& outputAcc = obj.gltf_data.accessors[sampler.output];

        const tinygltf::BufferView& inputView = obj.gltf_data.bufferViews[inputAcc.bufferView];
        const float* times = reinterpret_cast<const float*>(&obj.gltf_data.buffers[inputView.buffer].data[inputView.byteOffset + inputAcc.byteOffset]);

        float maxTime = times[inputAcc.count - 1];
        float animTime = fmod(this->current_time, maxTime);

        int p0 = 0, p1 = 0;
        for (size_t i = 0; i < inputAcc.count - 1; ++i) {
            if (animTime < times[i + 1]) { p0 = i; p1 = i + 1; break; }
        }
        
        float factor = (animTime - times[p0]) / (times[p1] - times[p0]);
        const tinygltf::BufferView& outputView = obj.gltf_data.bufferViews[outputAcc.bufferView];
        const float* values = reinterpret_cast<const float*>(&obj.gltf_data.buffers[outputView.buffer].data[outputView.byteOffset + outputAcc.byteOffset]);

        isNodeAnimated = true;
        if (channel.target_path == "translation") {
            T = glm::mix(glm::vec3(values[p0*3], values[p0*3+1], values[p0*3+2]), glm::vec3(values[p1*3], values[p1*3+1], values[p1*3+2]), factor);
        } else if (channel.target_path == "rotation") {
            glm::quat start(values[p0*4+3], values[p0*4], values[p0*4+1], values[p0*4+2]);
            glm::quat end(values[p1*4+3], values[p1*4], values[p1*4+1], values[p1*4+2]);
            R = glm::normalize(glm::slerp(start, end, factor));
        } else if (channel.target_path == "scale") {
            S = glm::mix(glm::vec3(values[p0*3], values[p0*3+1], values[p0*3+2]), glm::vec3(values[p1*3], values[p1*3+1], values[p1*3+2]), factor);
        }
    }

    if (!isNodeAnimated && node.matrix.size() == 16) return glm::make_mat4(node.matrix.data());
    return Matrix_Translate(T.x, T.y, T.z) * glm::mat4_cast(R) * Matrix_Scale(S.x, S.y, S.z);
}

void AnimatedObject::ProcessSkeletonNode(int nodeIndex, glm::mat4 parentTransform, AnimatedSceneObject& obj) {
    tinygltf::Node& node = obj.gltf_data.nodes[nodeIndex];
    glm::mat4 localTransform = GetNodeTransform(nodeIndex, obj);
    glm::mat4 globalTransform = parentTransform * localTransform;

    int jointIndex = GetJointIndex(nodeIndex, obj);
    if (jointIndex != -1 && jointIndex < 100 && (size_t)jointIndex < obj.inverseBindMatrices.size()) {
        // Agora salva no esqueleto individual desta instância!
        this->final_bone_matrices[jointIndex] = globalTransform * obj.inverseBindMatrices[jointIndex];
    }

    for (int childIndex : node.children) {
        ProcessSkeletonNode(childIndex, globalTransform, obj);
    }
}

AABB::AABB(glm::vec3 min, glm::vec3 max) : box_min(min), box_max(max), box_min_original(min), box_max_original(max){}

void AABB::Update(glm::vec3 position)
{
    box_min = position + box_min_original;
    box_max = position + box_max_original;
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
    return IntersectsX(against) && IntersectsY(against) && IntersectsZ(against);
}

float AABB::GetClipX(AABB against, float deltaX)
{
    //are we overlapping the other axes?
    //(if we aren't, then an intersection could never actually take place)
    if(IntersectsY(against) && IntersectsZ(against))
    {
        //if we are moving right and our right bounds are smaller than
        //or equal to the other left bounds
        if(deltaX > 0 && box_max.x <= against.box_min.x)
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
        if (deltaX < 0 && box_min.x >= against.box_max.x)
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
        if (deltaY > 0 && box_max.y <= against.box_min.y)
        {
            float clip = against.box_min.y - box_max.y;
            if (deltaY > clip)
                deltaY = clip;
        }
        if (deltaY < 0 && box_min.y >= against.box_max.y)
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
        if (deltaZ > 0 && box_max.z <= against.box_min.z)
        {
            float clip = against.box_min.z - box_max.z;
            if (deltaZ > clip)
                deltaZ = clip;
        }
        if (deltaZ < 0 && box_min.z >= against.box_max.z)
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