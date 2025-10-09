#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float cellSize;
    float cellHeight;
    float walkableSlopeAngle;
    int32_t walkableHeight;    // in voxels
    int32_t walkableClimb;     // in voxels
    int32_t walkableRadius;    // in voxels
    int32_t maxEdgeLen;        // in voxels
    float maxSimplificationError;
    int32_t minRegionArea;     // in voxels^2
    int32_t mergeRegionArea;   // in voxels^2
    int32_t maxVertsPerPoly;   // Detour nvp
    float detailSampleDist;
    float detailSampleMaxError;
} RecastConfig;

// Builds a single-tile navmesh from input geometry and returns Detour tile data
// suitable for dtNavMesh::init(data,...). Returns 0 on success.
int32_t RecastBuildSoloNavMesh(
    const float* verts, int32_t numVerts,
    const int32_t* tris, int32_t numTris,
    const RecastConfig* cfg,
    unsigned char** outData, int32_t* outDataSize);

// Free memory allocated by Recast/Detour (dtAlloc)
void RecastFree(void* p);

#ifdef __cplusplus
}
#endif


