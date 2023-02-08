
[[vk::input_attachment_index(0)]] SubpassInput in_color;

float3 ACESToneMapping(float3 color, float adapted_lum)
{
    const float A = 2.51f;
    const float B = 0.03f;
    const float C = 2.43f;
    const float D = 0.59f;
    const float E = 0.14f;

    color *= adapted_lum;
    return (color * (A * color + B)) / (color * (C * color + D) + E);
}

float4 frag() : SV_Target
{
    float3 color = in_color.SubpassLoad().rgb;
    color = ACESToneMapping(color, 4.5f);

    return float4(pow(color, 1.0 / 2.2), 1.0);
}
