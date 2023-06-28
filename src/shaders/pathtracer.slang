RWStructuredBuffer<float3> buffer;
uniform RaytracingAccelerationStructure sceneBVH;
StructuredBuffer<float3> vertices;
StructuredBuffer<float3> vertNormal;
StructuredBuffer<float3> vertColor;
StructuredBuffer<uint> indices;

uint getVertId(const uint primitiveId)
{
    return indices[primitiveId * 3];
}

[shader("compute")]
[numthreads(16, 16, 1)]
void main(uint3 groupId: SV_GroupID, uint3 groupThreadId: SV_GroupThreadID, uint3 dispatchThreadId: SV_DispatchThreadID,
          uint3 groupIndex: SV_GroupIndex)
{

    const uint2 resolution = uint2(800u, 600u);
    const uint2 pixel = dispatchThreadId.xy;
    const uint pixel1D = pixel.x + resolution.x * pixel.y;

    if (any(pixel >= resolution))
        return;

    const float3 cameraOrigin = float3(-0.001f, 1.0f, 6.0f);
    const float2 p = (pixel + float2(0.5f, 0.5f)) / resolution.y;
    const float2 ndc = float2(2, -2) * p + float2(-1, 1);

    const float fov = 1.0f / 5.0f;
    const float3 rayDir = float3(fov * ndc.x, fov * ndc.y, -1.f);

    let rayFlags = RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES | RAY_FLAG_FORCE_OPAQUE;
    RayQuery<RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES | RAY_FLAG_FORCE_OPAQUE> q;
    RayDesc ray;
    ray.Origin = cameraOrigin;
    ray.TMin = 0.01f;
    ray.Direction = rayDir;
    ray.TMax = 1e4f;

    q.TraceRayInline(sceneBVH, rayFlags, 0xff, ray);
    q.Proceed();
    if (q.CommittedStatus() == COMMITTED_TRIANGLE_HIT)
    {
        float t = q.CommittedRayT();
        const uint primitiveId = q.CommittedPrimitiveIndex();
        const float2 barycentrics = q.CommittedTriangleBarycentrics();
        const float3 triangleWeight = float3(1 - barycentrics.x - barycentrics.y, barycentrics.x, barycentrics.y);
        float3 pos = vertices[indices[primitiveId * 3]] * triangleWeight.x +
                     vertices[indices[primitiveId * 3 + 1]] * triangleWeight.y +
                     vertices[indices[primitiveId * 3 + 2]] * triangleWeight.z;
        float3 normal = vertNormal[indices[primitiveId * 3]] * triangleWeight.x +
                        vertNormal[indices[primitiveId * 3 + 1]] * triangleWeight.y +
                        vertNormal[indices[primitiveId * 3 + 2]] * triangleWeight.z;
        float3 color = vertColor[indices[primitiveId * 3]] * triangleWeight.x +
                       vertColor[indices[primitiveId * 3 + 1]] * triangleWeight.y +
                       vertColor[indices[primitiveId * 3 + 2]] * triangleWeight.z;
        // pos = normalize(pos);
        normal = normalize(normal);
        // buffer[pixel1D] = normal * 0.5f + float3(0.5f);
        buffer[pixel1D] = color;
        // buffer[pixel1D] = pos * 0.25f + float3(0.5f);

        // buffer[pixel1D] = float3(0.5f) + 0.5f * vertNormal[getVertId(primitiveId)];

        // buffer[pixel1D] = float3(primitiveId / 10.f, primitiveId / 100.f, primitiveId / 1000.f);
        // buffer[pixel1D] = triangleWeight;
    }
    else
    {
        buffer[pixel1D] = float3(0.f);
    }

    // float3 color = float3(float(pixel.x) / resolution.x, float(pixel.y) / resolution.y, 0.1f);
    // buffer[pixel1D] = color;
}
