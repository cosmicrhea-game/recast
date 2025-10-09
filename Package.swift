// swift-tools-version: 6.1

import PackageDescription

let package = Package(
  name: "recast",
  // platforms: [
  //   .macOS(.v13)
  // ],

  products: [
    .library(
      name: "Detour",
      targets: ["Detour"]
    ),
    .library(
      name: "DetourCrowd",
      targets: ["DetourCrowd"]
    ),
    .library(
      name: "Recast",
      targets: ["Recast"]
    ),
  ],

  targets: [
    .target(
      name: "Detour",
      dependencies: [
        "CxxDetour", "CxxDetourCrowd",
      ],
      path: "./DetourSwift",
      swiftSettings: [
        .interoperabilityMode(.Cxx)
      ]
    ),

    .target(
      name: "CxxDetour",
      path: "./Detour",
      publicHeadersPath: "Include"
    ),

    .target(
      name: "CxxDetourCrowd",
      dependencies: ["CxxDetour"],
      path: "./DetourCrowd",
      publicHeadersPath: "Include"
    ),

    .target(
      name: "Recast",
      dependencies: [
        "CxxRecast"
      ],
      path: "./RecastSwift",
      swiftSettings: [
        .interoperabilityMode(.Cxx)
      ]
    ),

    .target(
      name: "DetourCrowd",
      dependencies: [
        "CxxDetour", "CxxDetourCrowd",
      ],
      path: "./DetourCrowdSwift",
      swiftSettings: [
        .interoperabilityMode(.Cxx)
      ]
    ),

    .target(
      name: "CxxRecast",
      dependencies: [
        "CxxDetour"
      ],
      path: "./Recast",
      publicHeadersPath: "Include"
    ),

    .testTarget(
      name: "DetourSwiftTests",
      dependencies: ["Detour", "CxxDetour"],
      path: "./Tests/DetourSwiftTests",
      swiftSettings: [
        .interoperabilityMode(.Cxx)
      ]
    ),

    .testTarget(
      name: "RecastSwiftTests",
      dependencies: ["Detour", "Recast", "DetourCrowd"],
      path: "./Tests/RecastSwiftTests",
      swiftSettings: [
        .interoperabilityMode(.Cxx)
      ]
    ),
  ],

  cxxLanguageStandard: .cxx17
)
