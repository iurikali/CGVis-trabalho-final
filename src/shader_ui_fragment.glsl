#version 330 core

in vec2 texcoords;
out vec4 color;

uniform sampler2D ui_texture;

void main()
{
    vec4 texColor = texture(ui_texture, texcoords);
    
    // Se o pixel for 100% transparente no PNG, descartamos para não bugar o Z-Buffer
    if(texColor.a < 0.1) discard; 
    
    color = texColor;
}