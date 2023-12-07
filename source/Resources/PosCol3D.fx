// --------------------------------------------------------
// Input/Output Structs
// --------------------------------------------------------

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Color : COLOR;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
};
