# BendCore CAD-CORE Integration Guide

Guide for integrating BendCore CAD-CORE into the main BendCore project.

## Overview

BendCore CAD-CORE is the production-ready kernel for the BendCore CAD system. It provides:

1. **C++ Core Library** (`libbendcore.a`)
   - 25 interconnected components
   - Full geometry and topology support
   - Non-destructive parametric modeling
   - Transaction-based command system

2. **WebAssembly Module** (`bendcore_wasm.js`)
   - Browser-compatible compiled kernel
   - 4-5 MB total size (JavaScript + WASM)
   - Drop-in replacement for native library

3. **React Frontend** (optional)
   - 3D visualization
   - Property editing
   - Entity management
   - Can be replaced with custom UI

## Integration Steps

### 1. Add CAD-CORE as Submodule

```bash
cd /path/to/bendcore
git submodule add https://github.com/bendcore/cadcore.git cadcore
cd cadcore
git checkout main  # or specific version tag
```

### 2. Link C++ Library (Desktop App)

**In main CMakeLists.txt**:
```cmake
# Add CAD-CORE subdirectory
add_subdirectory(cadcore)

# Link library to your targets
target_link_libraries(your_desktop_app bendcore)
target_include_directories(your_desktop_app PRIVATE cadcore/include)
```

**Using CMake FetchContent** (alternative):
```cmake
include(FetchContent)
FetchContent_Declare(
    bendcore
    GIT_REPOSITORY https://github.com/bendcore/cadcore.git
    GIT_TAG v0.1.0
)
FetchContent_MakeAvailable(bendcore)

target_link_libraries(your_target bendcore)
```

### 3. Deploy WebAssembly Module (Web App)

**Option A: Pre-built Module**

```bash
# Copy pre-built WASM files
cp cadcore/build-wasm/bendcore_wasm.js web/public/lib/
cp cadcore/build-wasm/bendcore_wasm.wasm web/public/lib/
```

**Option B: Build During CI/CD**

Add to your GitHub Actions workflow:
```yaml
- name: Build BendCore WASM
  run: |
    source emsdk/emsdk_env.sh
    cd cadcore
    mkdir -p build-wasm
    cd build-wasm
    emcmake cmake .. -f ../CMakeLists-wasm.txt
    emmake make -j$(nproc)
    cp bendcore_wasm.* ../web/public/lib/
```

### 4. Use React Components (Optional)

If using the provided React components:

```bash
# Copy components to your project
cp -r cadcore/web/src/components ./web/src/
cp -r cadcore/web/src/services ./web/src/
cp -r cadcore/web/src/types ./web/src/

# Or import from package
npm install @bendcore/cadcore-web
```

**Usage in your React app**:
```tsx
import { Viewer3D, PropertyPanel, EntityTree } from '@bendcore/cadcore-web';

export function MyCADApp() {
  return (
    <div className="cad-workspace">
      <aside>
        <EntityTree />
      </aside>
      <main>
        <Viewer3D />
      </main>
      <aside>
        <PropertyPanel />
      </aside>
    </div>
  );
}
```

### 5. Custom UI Integration

If building custom UI, use the WASM API directly:

```typescript
// Load WASM module
const BendCoreModule = await (window as any).BendCoreModule();

// Create entity
const entity = BendCoreModule.createEntity('sketch', 'Sketch 1');

// Set parameters
BendCoreModule.setParameter(entity.id, 'radius', 25.5);

// Get geometry
const geometry = BendCoreModule.getGeometry(entity.id);

// Save/Load
const json = BendCoreModule.saveDocument().data;
BendCoreModule.loadDocument(json);
```

## API Reference

### Entity Management

```cpp
// C++ API (src/wasm_bindings.cpp)
EntityID createEntity(const std::string& type, const std::string& name);
bool deleteEntity(const EntityID& id);
const Entity* getEntity(const EntityID& id);
std::vector<Entity> getAllEntities();
```

### Parameters

```cpp
bool setParameter(const EntityID& id, const std::string& name, double value);
double getParameter(const EntityID& id, const std::string& name);
```

### Geometry

```cpp
std::shared_ptr<Geometry> getGeometry(const EntityID& id);
std::vector<Triangle> triangulateGeometry(const EntityID& id);
```

### Commands

```cpp
bool executeCommand(const std::string& command, const json& params);
bool undo();
bool redo();
```

### Serialization

```cpp
std::string saveDocument();
bool loadDocument(const std::string& json);
```

## Build Integration

### CMake Integration

**Standard Build**:
```cmake
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel $(nproc)
```

**With CAD-CORE**:
```cmake
# Automatically builds CAD-CORE as dependency
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DWITH_CADCORE=ON \
    -DCADCORE_WASM=ON
cmake --build . --parallel $(nproc)
```

### Parallel Build Configuration

```bash
# Use all CPU cores
cmake --build . --parallel $(nproc)

# Or specify number of jobs
cmake --build . --parallel 8
```

## Testing Integration

### Running CAD-CORE Tests

```bash
# From main project
cd cadcore/build
ctest --output-on-failure
```

### Integration Tests

```bash
# Test main project with CAD-CORE
ctest --output-on-failure

# Run specific component tests
ctest -R Geometry --output-on-failure
```

## Version Management

### Tagging CAD-CORE Releases

```bash
cd cadcore
git tag -a v0.1.0 -m "Initial production release"
git push origin v0.1.0
```

### Version Compatibility

```
BendCore v4.0.0 requires:
├── CAD-CORE v0.1.0+
├── React 18.2+
└── Node.js 18+
```

## Deployment

### Desktop Application

```bash
# Build with CAD-CORE
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# Link bendcore library
# Output: ./bendcore_app (or .exe on Windows)
```

### Web Application

```bash
# Build WASM module
cd cadcore/build-wasm
emmake make

# Build web app
cd ../../web
npm run build

# Deploy dist/ to web server
```

## Performance Considerations

### Memory Management
- CAD-CORE uses internal memory pools
- WASM module: 50-100 MB for typical projects
- No memory leaks (verified with Valgrind)

### CPU Usage
- Geometry computation cached
- Incremental rebuild optimization
- Parallel processing for large models

### Storage
- JSON-based serialization
- Typical project: 1-10 MB
- Atomic save with recovery

## Security Considerations

### WASM Sandbox
- WASM runs in isolated environment
- No direct file system access
- Network access controlled

### Data Protection
- Serialization without encryption (implement if needed)
- WASM module validation before loading
- Input validation on all API calls

## Migration from Legacy CAD System

### Entity Data Migration

```cpp
// Migrate legacy entity to CAD-CORE
LegacyEntity legacyEnt = loadLegacy("old_entity.xml");
EntityID newId = cadcoreDoc.createEntity(
    legacyEnt.type,
    legacyEnt.name
);

// Transfer properties
for (const auto& prop : legacyEnt.properties) {
    cadcoreDoc.setParameter(newId, prop.name, prop.value);
}
```

### Format Conversion

```bash
# Tool to convert legacy files
./tools/convert_legacy_format \
    --input old_project.bend \
    --output new_project.json
```

## Troubleshooting Integration

### WASM Module Not Loading

```
Error: BendCoreModule is undefined

Solution:
1. Ensure bendcore_wasm.js is served correctly
2. Check CORS headers
3. Verify Content-Type: application/wasm
4. Check browser console for detailed errors
```

### Linker Errors

```
undefined reference to 'bendcore::...'

Solution:
1. Verify target_link_libraries includes bendcore
2. Check include directories are set
3. Ensure bendcore library is built before linking
```

### Performance Issues

```
Solution:
1. Use Release build (-DCMAKE_BUILD_TYPE=Release)
2. Enable optimizations (-O3)
3. Use geometry caching
4. Profile with perf or valgrind
```

## Support and Resources

### Documentation
- Main README: `../README.md`
- Build Guide: `../BUILD.md`
- Web Guide: `../web/README.md`

### Examples
- Desktop integration: See main project
- Web integration: `../web/src/`
- API usage: `../src/wasm_bindings.cpp`

### Development
- Issues: GitHub repository
- Discussions: GitHub discussions
- Contributing: See CONTRIBUTING.md

## Roadmap

### Current (v0.1.0)
- ✅ Core functionality
- ✅ WASM bindings
- ✅ React components
- ✅ CI/CD pipeline

### Planned (v0.2.0)
- Advanced geometry features
- Cloud synchronization
- Plugin system
- Performance optimizations

### Future (v1.0.0)
- Full feature parity with legacy system
- Collaborative editing
- Advanced manufacturing features
- Multi-user support

---

**Status**: Production Ready for Integration  
**Version**: 0.1.0  
**Last Updated**: 2026-10-25

For questions or integration support, please refer to the project documentation.
