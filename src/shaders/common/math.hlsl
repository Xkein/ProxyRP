#pragma once

// the cubemap we use is:
//    +y
// -x +z +x -z
//    -y
// so rotate 90 around X axis
float4 SampleCubeMapWithAdjustedUVW(TextureCube cube, SamplerState cube_sampler, float3 UVW, float lod = 0)
{
    float3 uvw = float3(UVW.x, UVW.z, -UVW.y);
    return cube.SampleLevel(cube_sampler, uvw, lod);
}

// the cubemap we use is:
//    +y
// -x +z +x -z
//    -y
// so rotate 90 around X axis
float4 SampleCubeMapArrayWithAdjustedUVW(TextureCubeArray cube, SamplerState cube_sampler, float4 location, float lod = 0)
{
    float3 uvw = float3(location.x, location.z, -location.y);
    return cube.SampleLevel(cube_sampler, float4(uvw, location.w), lod);
}
