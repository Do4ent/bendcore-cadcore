# BendCore CAD Architecture

## System Overview

BendCore is a professional 3D CAD system built with modern C++20, featuring:

- **Core Engine**: High-performance geometry and topology (BRep)
- **WebAssembly Integration**: Browser-based access via Emscripten
- **React Frontend**: Interactive 3D viewport and UI components
- **Multi-platform Support**: Linux, Windows, macOS

## Core Components

### Foundation (Phase M1-M3)

- **EntityId System**: UUID v4 unique identifiers for all entities
- **CadDocument**: Central document model managing all entities
- **EntityTree**: Hierarchical parent-child relationships with cycle detection
- **CommandManager**: Execute/Undo architecture with full transaction support
- **DocumentEventBus**: Event-driven architecture for reactive updates
- **DependencyGraph**: Topological sorting for parameter dependency resolution

### Geometry (Phase M4)

- **GeometryEngine**: 2D/3D vector, matrix, and transformation operations
- **BRep (Boundary Representation)**: Solid modeling using vertices, edges, faces, and shells
- **Sketch System**: 2D constraint-based sketching

### Features (Phase M5-M8)

- **Part Design**: Pad, Pocket, Hole, Fillet, Chamfer, Pattern, Mirror operations
- **Assembly**: Component assembly with constraints (Coincident, Distance, Angle)
- **Collision Detection**: Interference detection between parts

### Advanced Features (Phase M9-M20)

- **Direct Editing**: Parametric editing with geometry reconstruction
- **Sketcher Constraints**: Automatic constraint solving
- **Sheet Metal**: Flatten, Bend, Unfold operations
- **Manufacturing**: NC code generation, BOM generation
- **Drawings**: 2D technical drawings with annotations

## Build System

- **CMake**: Primary build system (C++)
- **Emscripten**: WebAssembly compilation
- **Node.js/npm**: React frontend and web tools

## Testing

- **Unit Tests**: 50 comprehensive tests covering all major systems
- **Integration Tests**: Multi-component interaction validation
- **CI/CD**: GitHub Actions for automated builds and testing

## Technology Stack

- **C++20**: Core engine (std::ranges, concepts, modules)
- **CMake**: Build configuration
- **React 18+**: Frontend UI framework
- **Three.js**: 3D rendering and viewport
- **WebAssembly**: Browser engine integration
- **TypeScript**: Type-safe frontend code

## Development Phases (M1-M20)

| Phase | Component | Status |
|-------|-----------|--------|
| M1-M3 | Foundation & Core | COMPLETE |
| M4 | Geometry Engine | COMPLETE |
| M5-M8 | Part Design & Assembly | COMPLETE |
| M9-M12 | Advanced Features | COMPLETE |
| M13-M16 | Manufacturing & Export | COMPLETE |
| M17-M20 | Final Integration | COMPLETE |

## Running the Application

```bash
# Build release
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --parallel $(nproc)

# Run tests
ctest --output-on-failure

# Start application
./bendcore_app
```

## Directory Structure

```
bendcore-cadcore/
├── src/              # C++ source code
├── include/          # C++ headers
├── tests/            # Unit and integration tests
├── web/              # React frontend
├── docs/             # Documentation
├── examples/         # Example projects
├── scripts/          # Build and utility scripts
└── CMakeLists.txt    # Build configuration
```
