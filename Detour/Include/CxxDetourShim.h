#pragma once

#ifdef __cplusplus
class __attribute__((swift_attr("import_as_ref, retain: dtNavMeshRetain, release: dtNavMeshRelease"))) dtNavMesh;
class __attribute__((swift_attr("import_as_ref, retain: dtNavMeshQueryRetain, release: dtNavMeshQueryRelease"))) dtNavMeshQuery;

extern "C" dtNavMesh* dtNavMeshRetain(dtNavMesh* p);
extern "C" void dtNavMeshRelease(dtNavMesh* p);

extern "C" dtNavMeshQuery* dtNavMeshQueryRetain(dtNavMeshQuery* p);
extern "C" void dtNavMeshQueryRelease(dtNavMeshQuery* p);
#endif

#include "DetourAlloc.h"
#include "DetourAssert.h"
#include "DetourCommon.h"
#include "DetourMath.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "DetourNode.h"
#include "DetourStatus.h"