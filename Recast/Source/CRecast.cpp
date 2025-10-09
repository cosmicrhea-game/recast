#include "CRecast.h"
#include "Recast.h"
#include "DetourNavMeshBuilder.h"
#include "DetourAlloc.h"
#include <string.h>
#include <vector>

// Minimal rcContext that does nothing
struct DummyContext : public rcContext {
  DummyContext() : rcContext() { enableLog(false); enableTimer(false); }
};

int32_t RecastBuildSoloNavMesh(
    const float* verts, int32_t numVerts,
    const int32_t* tris, int32_t numTris,
    const RecastConfig* cfg,
    unsigned char** outData, int32_t* outDataSize)
{
  if (!verts || !tris || !cfg || !outData || !outDataSize) return -1;

  DummyContext ctx;

  float bmin[3], bmax[3];
  rcCalcBounds(verts, numVerts, bmin, bmax);
  // Ensure we have some Y thickness; flat meshes can have bmin.y == bmax.y
  const float minYSpan = cfg->cellHeight * 2.0f;
  if (bmax[1] - bmin[1] < minYSpan) {
    bmax[1] = bmin[1] + minYSpan;
  }

  // Grid size
  int gw = 0, gh = 0;
  rcCalcGridSize(bmin, bmax, cfg->cellSize, &gw, &gh);

  rcHeightfield solid;
  if (!rcCreateHeightfield(&ctx, solid, gw, gh, bmin, bmax, cfg->cellSize, cfg->cellHeight))
    return -2;

  // Mark triangles walkable and rasterize
  std::vector<unsigned char> triAreas(static_cast<size_t>(numTris), 1);
  rcRasterizeTriangles(&ctx, verts, numVerts, tris, triAreas.data(), numTris, solid, 1);

  // Filter walkable surfaces
  rcFilterLowHangingWalkableObstacles(&ctx, cfg->walkableClimb, solid);
  rcFilterLedgeSpans(&ctx, cfg->walkableHeight, cfg->walkableClimb, solid);
  rcFilterWalkableLowHeightSpans(&ctx, cfg->walkableHeight, solid);

  // Compact heightfield
  rcCompactHeightfield chf;
  if (!rcBuildCompactHeightfield(&ctx, cfg->walkableHeight, cfg->walkableClimb, solid, chf))
    return -3;

  // Erode walkable area by agent radius (in voxels)
  rcErodeWalkableArea(&ctx, cfg->walkableRadius, chf);

  // Build distance field and regions
  rcBuildDistanceField(&ctx, chf);
  rcBuildRegionsMonotone(&ctx, chf, 0, cfg->minRegionArea, cfg->mergeRegionArea);

  // Contours
  rcContourSet cset;
  if (!rcBuildContours(&ctx, chf, cfg->maxSimplificationError, cfg->maxEdgeLen, cset))
    return -4;

  // Poly mesh
  rcPolyMesh pmesh;
  if (!rcBuildPolyMesh(&ctx, cset, cfg->maxVertsPerPoly, pmesh))
    return -5;

  // Detail mesh
  rcPolyMeshDetail dmesh;
  if (!rcBuildPolyMeshDetail(&ctx, pmesh, chf, cfg->detailSampleDist, cfg->detailSampleMaxError, dmesh))
    return -6;

  // Create Detour data
  dtNavMeshCreateParams params{};
  // Ensure we have non-null flags for dtCreateNavMeshData
  std::vector<unsigned short> tempFlags;
  if (!pmesh.flags) {
    tempFlags.resize((size_t)pmesh.npolys, 1);
    params.polyFlags = tempFlags.data();
  }
  params.verts = pmesh.verts;
  params.vertCount = pmesh.nverts;
  params.polys = pmesh.polys;
  params.polyAreas = pmesh.areas;
  if (pmesh.flags)
    params.polyFlags = pmesh.flags;
  params.polyCount = pmesh.npolys;
  params.nvp = pmesh.nvp;
  params.detailMeshes = dmesh.meshes;
  params.detailVerts = dmesh.verts;
  params.detailVertsCount = dmesh.nverts;
  params.detailTris = dmesh.tris;
  params.detailTriCount = dmesh.ntris;
  // Convert from voxel units to world units for Detour
  params.walkableHeight = (float)cfg->walkableHeight * cfg->cellHeight;
  params.walkableRadius = (float)cfg->walkableRadius * cfg->cellSize;
  params.walkableClimb = (float)cfg->walkableClimb * cfg->cellHeight;
  rcVcopy(params.bmin, pmesh.bmin);
  rcVcopy(params.bmax, pmesh.bmax);
  params.cs = pmesh.cs;
  params.ch = pmesh.ch;
  params.buildBvTree = true;

  // Ensure every polygon has a traversal flag so default filters can pass
  for (int i = 0; i < pmesh.npolys; ++i) {
    if (pmesh.flags) {
      pmesh.flags[i] = pmesh.flags[i] ? pmesh.flags[i] : 1;
    }
  }

  unsigned char* data = nullptr;
  int dataSize = 0;
  if (!dtCreateNavMeshData(&params, &data, &dataSize)) {
    return -7;
  }

  *outData = data;
  *outDataSize = dataSize;

  return 0;
}

extern "C" void RecastFree(void* p) {
  if (p) dtFree(p);
}


