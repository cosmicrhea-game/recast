#pragma once

#include <stdint.h>
#include <stddef.h>
#include "DetourNavMesh.h" // for dtNavMeshParams

#ifdef __cplusplus
extern "C" {
#endif

typedef void* DetourNavMeshRef;
typedef void* DetourNavMeshQueryRef;

// Lifecycle
DetourNavMeshRef DetourNavMeshCreate(void);
void DetourNavMeshDestroy(DetourNavMeshRef nav);

// Initialization
// Returns 0 on success, non-zero on failure.
int32_t DetourNavMeshInitWithParams(DetourNavMeshRef nav, const dtNavMeshParams* params);

// Queries
const dtNavMeshParams* DetourNavMeshGetParams(DetourNavMeshRef nav);
int32_t DetourNavMeshGetMaxTiles(DetourNavMeshRef nav);
void DetourNavMeshCalcTileLoc(DetourNavMeshRef nav, const float* pos3, int32_t* tx, int32_t* ty);
int32_t DetourNavMeshIsValidPolyRef(DetourNavMeshRef nav, unsigned int polyRef);

// Tile and poly operations
unsigned int DetourNavMeshGetTileRefAt(DetourNavMeshRef nav, int32_t x, int32_t y, int32_t layer);
unsigned int DetourNavMeshGetPolyRefBase(DetourNavMeshRef nav, unsigned int tileRef);

int32_t DetourNavMeshSetPolyFlags(DetourNavMeshRef nav, unsigned int polyRef, unsigned short flags);
int32_t DetourNavMeshGetPolyFlags(DetourNavMeshRef nav, unsigned int polyRef, unsigned short* outFlags);
int32_t DetourNavMeshSetPolyArea(DetourNavMeshRef nav, unsigned int polyRef, unsigned char area);
int32_t DetourNavMeshGetPolyArea(DetourNavMeshRef nav, unsigned int polyRef, unsigned char* outArea);

// Encode/decode poly ids
unsigned int DetourNavMeshEncodePolyId(DetourNavMeshRef nav, unsigned int salt, unsigned int it, unsigned int ip);
void DetourNavMeshDecodePolyId(DetourNavMeshRef nav, unsigned int ref, unsigned int* salt, unsigned int* it, unsigned int* ip);
unsigned int DetourNavMeshDecodePolyIdSalt(DetourNavMeshRef nav, unsigned int ref);
unsigned int DetourNavMeshDecodePolyIdTile(DetourNavMeshRef nav, unsigned int ref);
unsigned int DetourNavMeshDecodePolyIdPoly(DetourNavMeshRef nav, unsigned int ref);

// Tile add/remove (data ownership by navmesh when flags include DT_TILE_FREE_DATA)
int32_t DetourNavMeshAddTile(DetourNavMeshRef nav, unsigned char* data, int32_t dataSize, int32_t flags, unsigned int lastRef, unsigned int* outRef);
int32_t DetourNavMeshRemoveTile(DetourNavMeshRef nav, unsigned int tileRef, unsigned char** outData, int32_t* outDataSize);

// NavMeshQuery lifecycle
DetourNavMeshQueryRef DetourNavMeshQueryCreate(void);
void DetourNavMeshQueryDestroy(DetourNavMeshQueryRef query);
int32_t DetourNavMeshQueryInit(DetourNavMeshQueryRef query, DetourNavMeshRef nav, int32_t maxNodes);

// Find nearest poly (returns 0 on success)
int32_t DetourNavMeshQueryFindNearestPoly(
  DetourNavMeshQueryRef query,
  const float* center,
  const float* halfExtents,
  unsigned int* nearestRef,
  float* nearestPt
);

// Pathfinding with default dtQueryFilter
int32_t DetourNavMeshQueryFindPath(
  DetourNavMeshQueryRef query,
  unsigned int startRef,
  unsigned int endRef,
  const float* startPos,
  const float* endPos,
  unsigned int* path,
  int* pathCount,
  int maxPath
);

int32_t DetourNavMeshQueryFindStraightPath(
  DetourNavMeshQueryRef query,
  const float* startPos,
  const float* endPos,
  const unsigned int* path,
  int pathSize,
  float* straightPath,                // (x,y,z) * count
  unsigned char* straightPathFlags,   // optional, may be null
  unsigned int* straightPathRefs,     // optional, may be null
  int* straightPathCount,
  int maxStraightPath
);

#ifdef __cplusplus
}
#endif

