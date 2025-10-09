import CxxRecast

public struct RecastBuilderConfig {
  public var cellSize: Float = 0.1
  public var cellHeight: Float = 0.1
  public var walkableSlopeAngle: Float = 45
  public var walkableHeight: Int32 = 2
  public var walkableClimb: Int32 = 1
  public var walkableRadius: Int32 = 0
  public var maxEdgeLen: Int32 = 12
  public var maxSimplificationError: Float = 1.3
  public var minRegionArea: Int32 = 0
  public var mergeRegionArea: Int32 = 0
  public var maxVertsPerPoly: Int32 = 6
  public var detailSampleDist: Float = 0
  public var detailSampleMaxError: Float = 0

  public init() {}
}

public enum RecastBuilderError: Error {
  case buildFailed(Int32)
}

public struct RecastBuilder {
  public init() {}

  // Returns Detour navmesh tile bytes
  public func buildSoloNavMesh(verts: [Float], tris: [Int32], config: RecastBuilderConfig = .init())
    throws -> [UInt8]
  {
    var cfg = RecastConfig(
      cellSize: config.cellSize,
      cellHeight: config.cellHeight,
      walkableSlopeAngle: config.walkableSlopeAngle,
      walkableHeight: config.walkableHeight,
      walkableClimb: config.walkableClimb,
      walkableRadius: config.walkableRadius,
      maxEdgeLen: config.maxEdgeLen,
      maxSimplificationError: config.maxSimplificationError,
      minRegionArea: config.minRegionArea,
      mergeRegionArea: config.mergeRegionArea,
      maxVertsPerPoly: config.maxVertsPerPoly,
      detailSampleDist: config.detailSampleDist,
      detailSampleMaxError: config.detailSampleMaxError
    )

    var outData: UnsafeMutablePointer<UInt8>? = nil
    var outSize: Int32 = 0
    let rc = verts.withUnsafeBufferPointer { vbuf in
      tris.withUnsafeBufferPointer { tbuf in
        RecastBuildSoloNavMesh(
          vbuf.baseAddress, Int32(verts.count / 3), tbuf.baseAddress, Int32(tris.count / 3), &cfg,
          &outData, &outSize)
      }
    }
    guard rc == 0, let ptr = outData, outSize > 0 else {
      throw RecastBuilderError.buildFailed(rc)
    }
    let bytes = Array(UnsafeBufferPointer(start: ptr, count: Int(outSize)))
    RecastFree(ptr)
    return bytes
  }
}
