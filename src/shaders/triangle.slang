cbuffer Uniforms
{
    float4x4 modelViewProjection;
}

struct AssembledVertex
{
    float3 position : POSITION;
    float3 color : COLOR;
};

struct CoarseVertex
{
    float3 color;
};

struct Fragment
{
    float4 color;
};

struct VertexStageOutput
{
    CoarseVertex coarseVertex : CoarseVertex;
    float4 sv_position : SV_Position;
};

[shader("vertex")]
VertexStageOutput vertexMain(AssembledVertex assembledVertex)
{
    VertexStageOutput output;

    float3 position = assembledVertex.position;
    float3 color = assembledVertex.color;

    output.coarseVertex.color = color;
    output.sv_position = mul(modelViewProjection, float4(position, 1.0));

    return output;
}

[shader("fragment")]
float4 fragmentMain(CoarseVertex coarseVertex: CoarseVertex) : SV_Target
{
    float3 color = float3(1.f, 0.f, 0.f);

    return float4(color, 1.0);
}
