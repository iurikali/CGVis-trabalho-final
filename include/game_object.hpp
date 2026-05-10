#pragma once
#include <string>
#include <vector>
#include <map>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glad/glad.h>
#include <tiny_gltf.h>

struct SceneObject
{
    std::string  name;        // Nome do objeto
    size_t       first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    size_t       num_indices; // Número de índices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
    GLuint       vertex_array_object_id; // ID do VAO onde estão armazenados os atributos do modelo
    glm::vec3    bbox_min; // Axis-Aligned Bounding Box do objeto
    glm::vec3    bbox_max;
};

//Estrutura do gltf para animações, Gemini que montou
// Definimos o limite máximo de ossos que um vértice pode estar ligado (padrão da indústria é 4)
#define MAX_BONE_INFLUENCE 4
const int MAX_BONES = 100;

// Nova estrutura de vértice que a GPU vai receber
struct AnimatedVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoords;
    glm::ivec4 boneIDs; 
    glm::vec4 weights;  
};

// Estrutura atualizada para guardar os dados do objeto animado
struct AnimatedSceneObject {
    std::string name;
    size_t first_index;
    size_t num_indices;
    GLenum rendering_mode;
    GLuint vertex_array_object_id;
    glm::vec3 bbox_min;
    glm::vec3 bbox_max;
    
    std::vector<glm::mat4> inverseBindMatrices; 
    GLuint diffuse_texture_id = 0;

    tinygltf::Model gltf_data;
    int current_animation_index = 0;
};
extern std::map<std::string, SceneObject> g_VirtualScene;
extern std::map<std::string, AnimatedSceneObject> g_AnimatedScene;

//GameObject vai ser classe pai dos objetos estáticos e dos animados
class GameObject
{
public:
    std::string name;
    int object_id;
    int texture_id;

    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};

    GameObject(std::string name, int obj_id, int tex_id);
    virtual ~GameObject() = default;

    glm::mat4 GetModelMatrix();


    virtual void Update(float delta_time){}
    virtual void Draw() = 0;
};

//Objetos estaticos (.obj)
class StaticObject : public GameObject
{
public:
    StaticObject(std::string name, int obj_id, int tex_id);
    
    void Draw() override;
};

//Objetos animados (.gltf)
class AnimatedObject : public GameObject
{
public:
    
    int current_animation;
    float animation_speed;
    float current_time;

    glm::mat4 final_bone_matrices[100];

    AnimatedObject(std::string name, int obj_id, int tex_id);

    void SetAnimation(int anim_index);
    void Update(float delta_time) override;
    void Draw() override;

private:
    void ProcessSkeletonNode(int nodeIndex, glm::mat4 parentTransform, AnimatedSceneObject& obj_data);
    glm::mat4 GetNodeTransform(int nodeIndex, AnimatedSceneObject& obj_data);
    int GetJointIndex(int nodeIndex, AnimatedSceneObject& obj_data);
};


