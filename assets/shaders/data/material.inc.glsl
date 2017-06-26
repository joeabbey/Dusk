layout(std140) uniform DuskMaterialData
{
    vec4 Ambient;
    vec4 Diffuse;
    vec4 Specular;

    float Shininess;
    float Dissolve;

    uint MapFlags;

} _MaterialData;

const uint AmbientMapFlag  = 1u; // 00001
const uint DiffuseMapFlag  = 2u; // 00010
const uint SpecularMapFlag = 4u; // 00100
const uint BumpMapFlag     = 8u; // 01000

uniform sampler2D _AmbientMap;
uniform sampler2D _DiffuseMap;
uniform sampler2D _SpecularMap;
uniform sampler2D _BumpMap;
