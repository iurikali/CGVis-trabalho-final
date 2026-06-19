#pragma once
#include <string>
#include <vector>
#include <map>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glad/glad.h>
#include <tiny_gltf.h>
#include <unordered_map>

// Número de texturas existentes ! atualizar no shader_fragment também
#define NUM_TEXTURAS 16

#define RED_BRICK 0
#define ROCKY_TERRAIN 1
#define COBBLESTONE 2
#define GRASS_BLOCK 3
#define CHARACTER_TEXTURE 4
#define WUMPA 5
#define CRATE 6
#define CRATE_INTERROGACAO 7
#define SPIKE 8
#define SAND 15

#define LEFT_HAND 9
#define RIGHT_HAND 12

#define SPHERE 0
#define BUNNY  1
#define PLANE  2
#define CHARACTER 3
#define CUBE 4


#define EPSILON 0.005f

#define X_LIMIT 3.0

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

struct GLTFPrimitive {
    size_t first_index;
    size_t num_indices;
    GLuint texture_id;
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
    std::vector<GLTFPrimitive> primitives;
};
extern std::map<std::string, SceneObject> g_VirtualScene;
extern std::map<std::string, AnimatedSceneObject> g_AnimatedScene;

//Classe feita com base no site 
//https://medium.com/@andrebluntindie/3d-aabb-collision-detection-and-resolution-for-voxel-games-5fcbfdb8cdb4
class AABB
{
    public:
    glm::vec3 box_min_original;
    glm::vec3 box_max_original;

    glm::vec3    box_min; // Axis-Aligned Bounding Box do objeto
    glm::vec3    box_max;

    bool disabled;

    AABB(glm::vec3 min, glm::vec3 max);

    void Update(glm::vec3 position, glm::vec3 scale);

    bool IntersectsX(AABB against);

    bool IntersectsY(AABB against);

    bool IntersectsZ(AABB against);

    bool Intersects(AABB against);

    float GetClipX(AABB against, float deltaX);

    float GetClipY(AABB against, float deltaY);

    float GetClipZ(AABB against, float deltaZ);

    void DrawDebug();
};

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

    GameObject(std::string name, int obj_id, int tex_id, bool is_physics, 
    bool is_trigger, bool is_spin, bool is_jump, bool is_destructible, glm::vec3 pos);

    glm::mat4 GetModelMatrix();

    //Componente de colisão (opcional)
    AABB *hitbox = nullptr;
    bool is_destroyed;
    void UpdateHitbox();

    virtual void Update(float delta_time){}
    virtual void Draw() = 0;

    virtual void on_trigger_player(){}
    virtual void on_trigger_spin(float dir){}
    virtual void on_trigger_jump(){}

    bool visible = true;

    //Apagando o componente de hitbox ao apagar o objeto
    virtual ~GameObject() 
    {
        if (hitbox != nullptr) {
            delete hitbox;
        }
    }
};

//Listas de listas para os objetos com colisao fisica e para os objetos triggers
//usando map pq ele não ocupa espaço se tiver um setor vazio
extern std::unordered_map<int, std::vector<GameObject*>> g_collision_physics;
extern std::unordered_map<int, std::vector<GameObject*>> g_collision_triggers;
extern std::unordered_map<int, std::vector<GameObject*>> g_collision_spin;
extern std::unordered_map<int, std::vector<GameObject*>> g_collision_jump;

extern std::vector<GameObject*> g_non_destructible_objects;
extern std::vector<GameObject*> g_destructible_objects;

#define SECTOR_LEN 4.0


//Objetos estaticos (.obj)
class StaticObject : public GameObject
{
public:
    StaticObject(std::string name, int obj_id, int tex_id, bool is_physics, 
    bool is_trigger, bool is_spin, bool is_jump, bool is_destructible, glm::vec3 pos);
    
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

    AnimatedObject(std::string name, int obj_id, int tex_id, bool is_physics, 
    bool is_trigger, bool is_spin, bool is_jump, bool is_destructible, glm::vec3 pos);

    void SetAnimation(int anim_index);
    void Update(float delta_time) override;
    void Draw() override;

private:
    void ProcessSkeletonNode(int nodeIndex, glm::mat4 parentTransform, AnimatedSceneObject& obj_data);
    glm::mat4 GetNodeTransform(int nodeIndex, AnimatedSceneObject& obj_data);
    int GetJointIndex(int nodeIndex, AnimatedSceneObject& obj_data);
};




