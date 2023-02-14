

float4 vert(uint vertex_index : SV_VertexID) : SV_Position
{
    const float3 fullscreen_triangle_position[3] =
    {
        float3(3.0, 1.0, 0.5),
        float3(-1.0, 1.0, 0.5),
        float3(-1.0, -3.0, 0.5),
    };
    
    return float4(fullscreen_triangle_position[vertex_index], 1.0);
}
