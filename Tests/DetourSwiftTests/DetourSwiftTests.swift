import CxxDetour
import Testing

@testable import Detour

@Suite struct DetourSwiftTests {
  @Test func initParamsAndMaxTiles() {
    let navMesh = NavigationMesh(
      origin: (0, 0, 0),
      tileWidth: 32,
      tileHeight: 32,
      maxTiles: 4,
      maxPolys: 2048
    )

    #expect(navMesh.maxTiles == 4)

    _ = navMesh
  }
}
