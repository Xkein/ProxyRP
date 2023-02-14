
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 vPosition : SV_Position;
    float3 vColor : COLOR;
    float2 TexCoord : TEXCOORD;
};

float4x4 Model;
float4x4 View;
float4x4 Projection;

VS_OUTPUT vert(VS_INPUT input)
{
    VS_OUTPUT output;
    
    //float4x4 mvp = Projection * View * Model;
    float4x4 mvp = mul(mul(Projection, View), Model);
    output.vPosition = mul(mvp, float4(input.Position, 1.0));
    output.vColor = input.Color;
    output.TexCoord = input.TexCoord;
    
    return output;
}


struct PS_OUTPUT
{
    float4 Color : SV_Target;
};

Texture2D Tex           : register(t1);
SamplerState TexSampler : register(s1);
PS_OUTPUT frag(VS_OUTPUT input)
{
    PS_OUTPUT output;
    
    output.Color = Tex.Sample(TexSampler, input.TexCoord);
    //uint width, height;
    //Tex.GetDimensions(width, height);
    //output.Color = Tex.Load(float3(float2(input.TexCoord * float2(width, height)), 0));
    
    return output;
}
