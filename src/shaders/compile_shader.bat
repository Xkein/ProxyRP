dxc -T vs_6_0 -spirv -Zi -E vert TestShader.hlsl -Fo TestVS.spirv
dxc -T ps_6_0 -spirv -Zi -E frag TestShader.hlsl -Fo TestPS.spirv