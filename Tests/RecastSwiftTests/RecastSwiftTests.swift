import Recast
import Testing

@testable import Detour

@Suite struct RecastSwiftTests {
  // Builds a tiny square mesh and verifies we can pathfind across it
  @Test func buildAndPathfind() {
    // Simple square split into two triangles on XZ plane (Y=0)
    let verts: [Float] = [
      0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1,
    ]
    let tris: [Int32] = [
      0, 1, 2,
      0, 2, 3,
    ]

    let bytes = try? RecastBuilder().buildSoloNavMesh(verts: verts, tris: tris)
    #expect(bytes?.isEmpty == false)
    guard let bytes else { return }

    // Create dtNavMesh and add single tile using our Swift wrapper (no C++ types)
    let nav = NavigationMesh(
      origin: (0, 0, 0),
      tileWidth: 1,
      tileHeight: 1,
      maxTiles: 1,
      maxPolys: 1024
    )
    let tileRef = bytes.withUnsafeBytes { raw -> UInt32? in
      guard let base = raw.baseAddress?.assumingMemoryBound(to: UInt8.self) else { return nil }
      return nav.addTileUnmanaged(
        dataPointer: UnsafeMutablePointer(mutating: base), dataSize: bytes.count, flags: 0x01)
    }
    #expect(tileRef != nil)

    // Query: find nearest start/end, path, and straight path using Swift wrapper
    let query = NavMeshQuery(navMesh: nav)
    let half: (Float, Float, Float) = (0.5, 1.0, 0.5)
    let start: (Float, Float, Float) = (0.1 as Float, 0.0 as Float, 0.1 as Float)
    let end: (Float, Float, Float) = (0.9 as Float, 0.0 as Float, 0.9 as Float)
    let sRef = query.findNearestPoly(center: start, halfExtents: half)?.ref ?? 0
    let eRef = query.findNearestPoly(center: end, halfExtents: half)?.ref ?? 0
    #expect(sRef != 0 && eRef != 0)

    let corridor = query.findPath(startRef: sRef, endRef: eRef, startPos: start, endPos: end)
    #expect(!corridor.isEmpty)

    let straight = query.findStraightPath(startPos: start, endPos: end, corridor: corridor)
    #expect(straight.count > 1)

    // Verify endpoints approximately match start/end
    if let first = straight.first, let last = straight.last {
      let eps: Float = 1e-3
      #expect(
        abs(first.0 - start.0) < eps && abs(first.1 - start.1) < eps && abs(first.2 - start.2) < eps
      )
      #expect(abs(last.0 - end.0) < eps && abs(last.1 - end.1) < eps && abs(last.2 - end.2) < eps)
    }
  }
}
