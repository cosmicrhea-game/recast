import CDetour
import Foundation

public final class NavMeshQuery {
  private let q: DetourNavMeshQueryRef

  public init(navMesh: NavigationMesh, maxNodes: Int32 = 2048) {
    guard let h = DetourNavMeshQueryCreate() else {
      fatalError("DetourNavMeshQueryCreate returned nil")
    }
    self.q = h
    _ = DetourNavMeshQueryInit(h, navMesh.handle, maxNodes)
  }

  deinit {
    DetourNavMeshQueryDestroy(q)
  }

  public func findNearestPoly(center: (Float, Float, Float), halfExtents: (Float, Float, Float))
    -> (ref: UInt32, nearest: (Float, Float, Float))?
  {
    var c: [Float] = [center.0, center.1, center.2]
    var he: [Float] = [halfExtents.0, halfExtents.1, halfExtents.2]
    var ref: UInt32 = 0
    var nearest = [Float](repeating: 0, count: 3)
    let st = nearest.withUnsafeMutableBufferPointer { nbp -> Int32 in
      DetourNavMeshQueryFindNearestPoly(q, &c, &he, &ref, nbp.baseAddress)
    }
    if st == 0, ref != 0 {
      return (ref, (nearest[0], nearest[1], nearest[2]))
    }
    return nil
  }

  public func findPath(
    startRef: UInt32, endRef: UInt32, startPos: (Float, Float, Float),
    endPos: (Float, Float, Float), maxPath: Int32 = 256
  ) -> [UInt32] {
    var s: [Float] = [startPos.0, startPos.1, startPos.2]
    var e: [Float] = [endPos.0, endPos.1, endPos.2]
    var path = [UInt32](repeating: 0, count: Int(maxPath))
    var count: Int32 = 0
    path.withUnsafeMutableBufferPointer { pbp in
      _ = DetourNavMeshQueryFindPath(q, startRef, endRef, &s, &e, pbp.baseAddress, &count, maxPath)
    }
    return Array(path.prefix(Int(count)))
  }

  public func findStraightPath(
    startPos: (Float, Float, Float), endPos: (Float, Float, Float), corridor: [UInt32],
    maxStraight: Int32 = 64
  ) -> [(Float, Float, Float)] {
    var s: [Float] = [startPos.0, startPos.1, startPos.2]
    var e: [Float] = [endPos.0, endPos.1, endPos.2]
    var pts = [Float](repeating: 0, count: Int(maxStraight) * 3)
    var flags = [UInt8](repeating: 0, count: Int(maxStraight))
    var refs = [UInt32](repeating: 0, count: Int(maxStraight))
    var count: Int32 = 0
    corridor.withUnsafeBufferPointer { cbp in
      pts.withUnsafeMutableBufferPointer { pbuf in
        flags.withUnsafeMutableBufferPointer { fbuf in
          refs.withUnsafeMutableBufferPointer { rbuf in
            _ = DetourNavMeshQueryFindStraightPath(
              q, &s, &e, cbp.baseAddress, Int32(cbp.count), pbuf.baseAddress, fbuf.baseAddress,
              rbuf.baseAddress, &count, maxStraight)
          }
        }
      }
    }
    var result: [(Float, Float, Float)] = []
    for i in 0..<Int(count) {
      result.append((pts[i * 3 + 0], pts[i * 3 + 1], pts[i * 3 + 2]))
    }
    return result
  }
}
