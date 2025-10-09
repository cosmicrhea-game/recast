#include "CDetour.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include <new>

extern "C" DetourNavMeshRef DetourNavMeshCreate(void) {
    dtNavMesh* mesh = dtAllocNavMesh();
    return reinterpret_cast<DetourNavMeshRef>(mesh);
}

extern "C" void DetourNavMeshDestroy(DetourNavMeshRef nav) {
    if (!nav) return;
    dtFreeNavMesh(reinterpret_cast<dtNavMesh*>(nav));
}

extern "C" int32_t DetourNavMeshInitWithParams(DetourNavMeshRef nav, const dtNavMeshParams* params) {
    if (!nav || !params) return -1;
    dtStatus st = reinterpret_cast<dtNavMesh*>(nav)->init(params);
    return dtStatusFailed(st) ? (int32_t)st : 0;
}

extern "C" const dtNavMeshParams* DetourNavMeshGetParams(DetourNavMeshRef nav) {
    if (!nav) return nullptr;
    return reinterpret_cast<dtNavMesh*>(nav)->getParams();
}

extern "C" int32_t DetourNavMeshGetMaxTiles(DetourNavMeshRef nav) {
    if (!nav) return 0;
    return reinterpret_cast<dtNavMesh*>(nav)->getMaxTiles();
}

extern "C" void DetourNavMeshCalcTileLoc(DetourNavMeshRef nav, const float* pos3, int32_t* tx, int32_t* ty) {
    if (!nav || !pos3 || !tx || !ty) return;
    reinterpret_cast<dtNavMesh*>(nav)->calcTileLoc(pos3, tx, ty);
}

extern "C" int32_t DetourNavMeshIsValidPolyRef(DetourNavMeshRef nav, unsigned int polyRef) {
    if (!nav) return 0;
    return reinterpret_cast<dtNavMesh*>(nav)->isValidPolyRef(polyRef) ? 1 : 0;
}

extern "C" unsigned int DetourNavMeshGetTileRefAt(DetourNavMeshRef nav, int32_t x, int32_t y, int32_t layer) {
    if (!nav) return 0;
    return reinterpret_cast<dtNavMesh*>(nav)->getTileRefAt(x, y, layer);
}

extern "C" unsigned int DetourNavMeshGetPolyRefBase(DetourNavMeshRef nav, unsigned int tileRef) {
    if (!nav) return 0;
    const dtMeshTile* tile = reinterpret_cast<dtNavMesh*>(nav)->getTileByRef(tileRef);
    if (!tile) return 0;
    return reinterpret_cast<dtNavMesh*>(nav)->getPolyRefBase(tile);
}

extern "C" int32_t DetourNavMeshSetPolyFlags(DetourNavMeshRef nav, unsigned int polyRef, unsigned short flags) {
    if (!nav) return -1;
    return (int32_t)reinterpret_cast<dtNavMesh*>(nav)->setPolyFlags(polyRef, flags);
}

extern "C" int32_t DetourNavMeshGetPolyFlags(DetourNavMeshRef nav, unsigned int polyRef, unsigned short* outFlags) {
    if (!nav || !outFlags) return -1;
    return (int32_t)reinterpret_cast<dtNavMesh*>(nav)->getPolyFlags(polyRef, outFlags);
}

extern "C" int32_t DetourNavMeshSetPolyArea(DetourNavMeshRef nav, unsigned int polyRef, unsigned char area) {
    if (!nav) return -1;
    return (int32_t)reinterpret_cast<dtNavMesh*>(nav)->setPolyArea(polyRef, area);
}

extern "C" int32_t DetourNavMeshGetPolyArea(DetourNavMeshRef nav, unsigned int polyRef, unsigned char* outArea) {
    if (!nav || !outArea) return -1;
    return (int32_t)reinterpret_cast<dtNavMesh*>(nav)->getPolyArea(polyRef, outArea);
}

extern "C" unsigned int DetourNavMeshEncodePolyId(DetourNavMeshRef nav, unsigned int salt, unsigned int it, unsigned int ip) {
    if (!nav) return 0;
    return reinterpret_cast<dtNavMesh*>(nav)->encodePolyId(salt, it, ip);
}

extern "C" void DetourNavMeshDecodePolyId(DetourNavMeshRef nav, unsigned int ref, unsigned int* salt, unsigned int* it, unsigned int* ip) {
    if (!nav || !salt || !it || !ip) return;
    reinterpret_cast<dtNavMesh*>(nav)->decodePolyId(ref, *salt, *it, *ip);
}

extern "C" unsigned int DetourNavMeshDecodePolyIdSalt(DetourNavMeshRef nav, unsigned int ref) {
    if (!nav) return 0;
    return reinterpret_cast<dtNavMesh*>(nav)->decodePolyIdSalt(ref);
}

extern "C" unsigned int DetourNavMeshDecodePolyIdTile(DetourNavMeshRef nav, unsigned int ref) {
    if (!nav) return 0;
    return reinterpret_cast<dtNavMesh*>(nav)->decodePolyIdTile(ref);
}

extern "C" unsigned int DetourNavMeshDecodePolyIdPoly(DetourNavMeshRef nav, unsigned int ref) {
    if (!nav) return 0;
    return reinterpret_cast<dtNavMesh*>(nav)->decodePolyIdPoly(ref);
}

extern "C" int32_t DetourNavMeshAddTile(DetourNavMeshRef nav, unsigned char* data, int32_t dataSize, int32_t flags, unsigned int lastRef, unsigned int* outRef) {
    if (!nav || !data) return -1;
    return (int32_t)reinterpret_cast<dtNavMesh*>(nav)->addTile(data, dataSize, flags, lastRef, outRef);
}

extern "C" int32_t DetourNavMeshRemoveTile(DetourNavMeshRef nav, unsigned int tileRef, unsigned char** outData, int32_t* outDataSize) {
    if (!nav) return -1;
    return (int32_t)reinterpret_cast<dtNavMesh*>(nav)->removeTile(tileRef, outData, outDataSize);
}

extern "C" DetourNavMeshQueryRef DetourNavMeshQueryCreate(void) {
    dtNavMeshQuery* q = dtAllocNavMeshQuery();
    return reinterpret_cast<DetourNavMeshQueryRef>(q);
}

extern "C" void DetourNavMeshQueryDestroy(DetourNavMeshQueryRef query) {
    if (!query) return;
    dtFreeNavMeshQuery(reinterpret_cast<dtNavMeshQuery*>(query));
}

extern "C" int32_t DetourNavMeshQueryInit(DetourNavMeshQueryRef query, DetourNavMeshRef nav, int32_t maxNodes) {
    if (!query || !nav) return -1;
    dtStatus st = reinterpret_cast<dtNavMeshQuery*>(query)->init(reinterpret_cast<const dtNavMesh*>(nav), (int)maxNodes);
    return dtStatusFailed(st) ? (int32_t)st : 0;
}

extern "C" int32_t DetourNavMeshQueryFindNearestPoly(
    DetourNavMeshQueryRef query,
    const float* center,
    const float* halfExtents,
    unsigned int* nearestRef,
    float* nearestPt
) {
    if (!query || !center || !halfExtents || !nearestRef) return -1;
    dtQueryFilter filter;
    dtStatus st = reinterpret_cast<dtNavMeshQuery*>(query)->findNearestPoly(center, halfExtents, &filter, nearestRef, nearestPt);
    return dtStatusFailed(st) ? (int32_t)st : 0;
}

extern "C" int32_t DetourNavMeshQueryFindPath(
    DetourNavMeshQueryRef query,
    unsigned int startRef,
    unsigned int endRef,
    const float* startPos,
    const float* endPos,
    unsigned int* path,
    int* pathCount,
    int maxPath
) {
    if (!query || !startPos || !endPos || !path || !pathCount) return -1;
    dtQueryFilter filter;
    dtStatus st = reinterpret_cast<dtNavMeshQuery*>(query)->findPath(startRef, endRef, startPos, endPos, &filter, path, pathCount, maxPath);
    return dtStatusFailed(st) ? (int32_t)st : 0;
}

extern "C" int32_t DetourNavMeshQueryFindStraightPath(
    DetourNavMeshQueryRef query,
    const float* startPos,
    const float* endPos,
    const unsigned int* path,
    int pathSize,
    float* straightPath,
    unsigned char* straightPathFlags,
    unsigned int* straightPathRefs,
    int* straightPathCount,
    int maxStraightPath
) {
    if (!query || !startPos || !endPos || !path || !straightPath || !straightPathCount) return -1;
    dtStatus st = reinterpret_cast<dtNavMeshQuery*>(query)->findStraightPath(
        startPos, endPos, path, pathSize,
        straightPath, straightPathFlags, straightPathRefs,
        straightPathCount, maxStraightPath, 0);
    return dtStatusFailed(st) ? (int32_t)st : 0;
}


