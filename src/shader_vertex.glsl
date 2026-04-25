#version 330 core

// Atributos lidos do VBO (note as locations casando perfeitamente com o C++)
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoords_in;
layout (location = 3) in ivec4 boneIds;
layout (location = 4) in vec4 weights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Array com as matrizes dos ossos geradas no C++ a cada frame
const int MAX_BONES = 100;
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec4 position_world;
out vec4 position_model;
out vec4 frag_normal;
out vec2 texcoords;

void main()
{
    mat4 boneTransform = mat4(0.0);
    
    // Calcula a matriz final do vértice multiplicando os ossos pelos seus pesos
    for(int i = 0 ; i < 4 ; i++)
    {
        // Pula ossos vazios
        if(boneIds[i] == -1 || boneIds[i] >= MAX_BONES) 
            continue;
            
        boneTransform += finalBonesMatrices[boneIds[i]] * weights[i];
    }
    
    // Se por acaso o modelo não tiver pesos carregados corretamente (fallback)
    if(boneTransform == mat4(0.0)) {
        boneTransform = mat4(1.0); 
    }

    // Posição local afetada pelos ossos (W deve ser 1.0)
    vec4 localPosition = boneTransform * vec4(position, 1.0f);
    
    gl_Position = projection * view * model * localPosition;
    
    position_model = localPosition;
    position_world = model * localPosition;
    
    // A normal também gira junto com os ossos (W deve ser 0.0)
    vec4 localNormal = boneTransform * vec4(normal, 0.0f);
    frag_normal = inverse(transpose(model)) * localNormal;
    
    texcoords = texcoords_in;
}