layout(std140) uniform DuskMaterialData
{
    vec4 Ambient;
    vec4 Diffuse;
    vec4 Specular;

    float Shininess;
    float Dissolve;

    bool HasAmbientMap;
    bool HasDiffuseMap;
    bool HasSpecularMap;
    bool HasBumpMap;

} _MaterialData;

uniform sampler2D _AmbientMap;
uniform sampler2D _DiffuseMap;
uniform sampler2D _SpecularMap;
uniform sampler2D _BumpMap;
