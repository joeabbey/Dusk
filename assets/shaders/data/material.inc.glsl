layout(std140) uniform MaterialData
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float shininess;
    float dissolve;

    bool has_ambient_map;
    bool has_diffuse_map;
    bool has_specular_map;
    bool has_bump_map;
}
material_data;

uniform sampler2D ambient_map;
uniform sampler2D diffuse_map;
uniform sampler2D specular_map;
uniform sampler2D bump_map;
