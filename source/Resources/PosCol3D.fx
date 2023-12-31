// --------------------------------------------------------
// Global Variables
// --------------------------------------------------------

float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;

SamplerState gSamPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState gSamLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState gSamAnisotropic
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};

// --------------------------------------------------------
// Input/Output Structs
// --------------------------------------------------------

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

// --------------------------------------------------------
// Helper Functions
// --------------------------------------------------------

float4 SampleDiffuseMap(SamplerState state, float2 position)
{
    return gDiffuseMap.Sample(state, position);
}

float4 Shade(SamplerState state, VS_OUTPUT input)
{
    return SampleDiffuseMap(state, input.TexCoord);
}

// --------------------------------------------------------
// Vertex Shader
// --------------------------------------------------------

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(float4(input.Position, 1.0f), gWorldViewProj);
    output.TexCoord = input.TexCoord;
    output.Normal = input.Normal;
    output.Tangent = input.Tangent;
    return output;
}

// --------------------------------------------------------
// Pixel Shader(s)
// --------------------------------------------------------

float4 PS_TexturePoint(VS_OUTPUT input) : SV_TARGET
{
    return Shade(gSamPoint, input);
}

float4 PS_TextureLinear(VS_OUTPUT input) : SV_TARGET
{
    return Shade(gSamLinear, input);
}

float4 PS_TextureAnisotropic(VS_OUTPUT input) : SV_TARGET
{
    return Shade(gSamAnisotropic, input);
}

// --------------------------------------------------------
// Technique
// --------------------------------------------------------

technique11 TexturePointTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_TexturePoint()));
    }
}

technique11 TextureLinearTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_TextureLinear()));
    }
}

technique11 TextureAnisotropicTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_TextureAnisotropic()));
    }
}
