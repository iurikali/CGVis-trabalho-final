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


GameObject::GameObject(std::string n, int o_id, int t_id) : name(n), object_id(o_id), texture_id(t_id){}

//Matriz final do objeto
glm::mat4 GameObject::GetModelMatrix()
{
    glm::mat4 matrix_model = Matrix_Translate(position.x, position.y, position.z) *
    Matrix_Rotate_Z(rotation.z) * Matrix_Rotate_Y(rotation.y) * Matrix_Rotate_X(rotation.x) *
    Matrix_Scale(scale.x, scale.y, scale.z);

    return matrix_model;
}

//Os .obj
StaticObject::StaticObject(std::string n, int o_id, int t_id) : GameObject(n, o_id, t_id){}

void StaticObject::Draw()
{
    glm::mat4 model = GetModelMatrix();
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(g_object_id_uniform, object_id);
    glUniform1i(g_texture_id_uniform, texture_id);

    SceneObject& obj_data = g_VirtualScene[name];
    
    glBindVertexArray(obj_data.vertex_array_object_id);
    glDrawElements(obj_data.rendering_mode, obj_data.num_indices, GL_UNSIGNED_INT, (void*)(obj_data.first_index * sizeof(GLuint)));
    glBindVertexArray(0);
}

//Os .gltf
AnimatedObject::AnimatedObject(std::string n, int o_id, int t_id): 
    GameObject(n, o_id, t_id),
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
    
    // Avisa o shader qual número do array usar (Gaveta 4)
    glUniform1i(g_texture_id_uniform, texture_id);
    
    glUniformMatrix4fv(g_bones_uniform, 100, GL_FALSE, glm::value_ptr(final_bone_matrices[0]));

    AnimatedSceneObject& obj_data = g_AnimatedScene[name];
    
    GLint has_texture_uniform = glGetUniformLocation(g_GpuProgramID, "has_texture");
    if (obj_data.diffuse_texture_id != 0) 
    {
        // BLINDAGEM: Forçamos a textura oficial do modelo a entrar na gaveta certa!
        glActiveTexture(GL_TEXTURE0 + texture_id); 
        glBindTexture(GL_TEXTURE_2D, obj_data.diffuse_texture_id);
        
        glUniform1i(has_texture_uniform, 1); 
    } else 
    {
        glUniform1i(has_texture_uniform, 0); 
    }

    glBindVertexArray(obj_data.vertex_array_object_id);
    glDrawElements(obj_data.rendering_mode, obj_data.num_indices, GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}

//Gemini
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

    if (obj.current_animation_index >= obj.gltf_data.animations.size()) obj.current_animation_index = 0;
    const tinygltf::Animation& anim = obj.gltf_data.animations[obj.current_animation_index]; 
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

