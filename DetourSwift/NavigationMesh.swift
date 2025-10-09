import CDetour
import CxxDetour  // for dtNavMeshParams
import Foundation

public final class NavigationMesh {
  private let underlying: DetourNavMeshRef
  private let DT_TILE_FREE_DATA: Int32 = 0x01

  // Internal access to the raw handle for other Swift wrappers in this package
  var handle: DetourNavMeshRef { underlying }

  public init(params: dtNavMeshParams) {
    guard let navMesh = DetourNavMeshCreate() else {
      fatalError("DetourNavMeshCreate returned nil")
    }
    var p = params
    _ = DetourNavMeshInitWithParams(navMesh, &p)
    self.underlying = navMesh
  }

  deinit {
    DetourNavMeshDestroy(underlying)
  }

  public func tileLocation(for position: (Float, Float, Float)) -> (x: Int32, y: Int32) {
    var tx: Int32 = 0
    var ty: Int32 = 0
    var pos = [position.0, position.1, position.2]
    DetourNavMeshCalcTileLoc(underlying, &pos, &tx, &ty)
    return (tx, ty)
  }

  public var maxTiles: Int32 {
    DetourNavMeshGetMaxTiles(underlying)
  }

  public func isValid(polygonID: UInt32) -> Bool {
    DetourNavMeshIsValidPolyRef(underlying, polygonID) != 0
  }

  public func tileID(atX x: Int32, y: Int32, layer: Int32) -> UInt32 {
    DetourNavMeshGetTileRefAt(underlying, x, y, layer)
  }

  public func basePolygonID(forTileID tileID: UInt32) -> UInt32 {
    DetourNavMeshGetPolyRefBase(underlying, tileID)
  }

  public func setPolygonFlags(_ polygonID: UInt32, flags: UInt16) -> Int32 {
    DetourNavMeshSetPolyFlags(underlying, polygonID, flags)
  }

  public func polygonFlags(for polygonID: UInt32) -> UInt16? {
    var out: UInt16 = 0
    let st = DetourNavMeshGetPolyFlags(underlying, polygonID, &out)
    return st == 0 ? out : nil
  }

  public func setPolygonArea(_ polygonID: UInt32, area: UInt8) -> Int32 {
    DetourNavMeshSetPolyArea(underlying, polygonID, area)
  }

  public func polygonArea(for polygonID: UInt32) -> UInt8? {
    var out: UInt8 = 0
    let st = DetourNavMeshGetPolyArea(underlying, polygonID, &out)
    return st == 0 ? out : nil
  }

  // Convenience initializer that avoids exposing C++ types in public API
  public convenience init(
    origin: (Float, Float, Float),
    tileWidth: Float,
    tileHeight: Float,
    maxTiles: Int32,
    maxPolys: Int32
  ) {
    var p = dtNavMeshParams()
    p.orig = origin
    p.tileWidth = tileWidth
    p.tileHeight = tileHeight
    p.maxTiles = maxTiles
    p.maxPolys = maxPolys
    self.init(params: p)
  }

  // MARK: - Tile add/remove

  // Adds a tile by copying the provided Data into a C-allocated buffer and
  // transferring ownership to the navmesh (DT_TILE_FREE_DATA).
  // Returns the created tile reference on success, or nil on failure.
  public func addTileManaged(_ tileData: Data, lastRef: UInt32 = 0, extraFlags: Int32 = 0)
    -> UInt32?
  {
    // Allocate C buffer and copy bytes
    let size = tileData.count
    guard size > 0 else { return nil }
    let buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: size)
    _ = tileData.withUnsafeBytes { src -> Void in
      if let base = src.baseAddress {
        buffer.initialize(from: base.assumingMemoryBound(to: UInt8.self), count: size)
      }
    }

    var outRef: UInt32 = 0
    let status = DetourNavMeshAddTile(
      underlying,
      buffer,
      Int32(size),
      DT_TILE_FREE_DATA | extraFlags,
      lastRef,
      &outRef
    )

    if status != 0 {
      // On failure, free the buffer we allocated
      buffer.deallocate()
      return nil
    }
    return outRef
  }

  // Adds a tile from an unmanaged buffer. If you do not include DT_TILE_FREE_DATA
  // in flags, you retain ownership of the memory and must free it later.
  public func addTileUnmanaged(
    dataPointer: UnsafeMutablePointer<UInt8>,
    dataSize: Int,
    flags: Int32 = 0,
    lastRef: UInt32 = 0
  ) -> UInt32? {
    guard dataSize > 0 else { return nil }
    var outRef: UInt32 = 0
    let status = DetourNavMeshAddTile(
      underlying,
      dataPointer,
      Int32(dataSize),
      flags,
      lastRef,
      &outRef
    )
    return status == 0 ? outRef : nil
  }

  // Removes a tile. If the tile did not transfer ownership to the navmesh when added,
  // this returns the original tile data as Data and frees the C buffer after copying.
  // If the navmesh owned the memory (DT_TILE_FREE_DATA), returns nil.
  public func removeTile(_ tileRef: UInt32) -> Data? {
    var outDataPtr: UnsafeMutablePointer<UInt8>? = nil
    var outSize: Int32 = 0
    let status = DetourNavMeshRemoveTile(underlying, tileRef, &outDataPtr, &outSize)
    if status != 0 { return nil }
    guard let ptr = outDataPtr, outSize > 0 else {
      return nil
    }
    let data = Data(bytes: ptr, count: Int(outSize))
    // Free the buffer returned by Detour (allocated originally by the caller)
    dtFree(ptr)
    return data
  }
}
