# BendCore CAD Web Interface

Modern React frontend for BendCore CAD-CORE kernel. Features real-time 3D visualization, parametric property editing, and entity hierarchy management.

## Features

- **3D Viewer**: WebGL rendering with Three.js and React Three Fiber
- **Property Panel**: Real-time parameter editing and validation
- **Entity Tree**: Hierarchical entity browsing and selection
- **State Management**: Zustand for reactive state updates
- **TypeScript**: Full type safety with strict compilation
- **Performance**: Optimized WebAssembly integration

## Architecture

```
┌─────────────────────────────────────────────┐
│          React Application (Vite)            │
│                                             │
│  ┌────────┐  ┌─────────┐  ┌────────────┐  │
│  │Viewer3D│  │Property │  │EntityTree  │  │
│  │        │  │Panel    │  │            │  │
│  └────────┘  └─────────┘  └────────────┘  │
│       │           │              │         │
│       └───────────┼──────────────┘         │
│                   │                        │
│                   ▼                        │
│            ┌─────────────┐                 │
│            │ Zustand     │                 │
│            │ Store       │                 │
│            └─────────────┘                 │
│                   │                        │
│                   ▼                        │
│            ┌─────────────┐                 │
│            │ WASM Module │                 │
│            │ Bindings    │                 │
│            └─────────────┘                 │
│                   │                        │
└───────────────────┼────────────────────────┘
                    │
            ┌───────▼──────────┐
            │ BendCore C++     │
            │ Kernel (WASM)    │
            └──────────────────┘
```

## Project Structure

```
web/
├── index.html                 # HTML entry point
├── package.json              # NPM dependencies
├── tsconfig.json             # TypeScript configuration
├── vite.config.ts            # Vite build configuration
├── .eslintrc.cjs             # ESLint configuration
├── public/                   # Static assets
│   └── lib/                  # WASM modules
│       ├── bendcore_wasm.js
│       └── bendcore_wasm.wasm
└── src/
    ├── main.tsx              # React entry point
    ├── App.tsx               # Main application component
    ├── components/           # React components
    │   ├── Viewer3D.tsx       # 3D visualization
    │   ├── PropertyPanel.tsx  # Parameter editor
    │   └── EntityTree.tsx     # Entity hierarchy
    ├── services/             # State management
    │   └── cadStore.ts       # Zustand store
    ├── types/                # TypeScript definitions
    │   └── bendcore.ts       # WASM API types
    └── styles/               # CSS stylesheets
        ├── App.css
        ├── PropertyPanel.css
        └── EntityTree.css
```

## Development

### Prerequisites

- Node.js 18+
- npm or yarn

### Installation

```bash
cd web
npm install
```

### Development Server

```bash
npm run dev
# Open http://localhost:5173
```

### Production Build

```bash
npm run build
# Output: dist/ directory
```

### Type Checking

```bash
npm run type-check
```

### Linting

```bash
npm run lint
npm run lint:fix
```

### Testing

```bash
npm run test
npm run coverage
```

## Components

### Viewer3D

3D visualization component using Three.js and React Three Fiber.

```tsx
<Viewer3D entityId="entity-uuid" />
```

**Features**:
- WebGL rendering
- Orbit controls
- Real-time geometry updates
- Grid and axes visualization
- Multiple rendering modes

### PropertyPanel

Parameter editor for selected entities.

```tsx
<PropertyPanel />
```

**Features**:
- Real-time parameter editing
- Numeric input validation
- Quick parameter addition
- Entity metadata display
- Action buttons

### EntityTree

Hierarchical entity browser and selector.

```tsx
<EntityTree />
```

**Features**:
- Entity listing with filtering
- Selection management
- Drag-and-drop reordering
- Context menu actions
- Expand/collapse controls

## State Management

### Zustand Store (`cadStore.ts`)

Global state management with Zustand.

```typescript
// Initialize module
const { initializeModule } = useCADStore();
initializeModule(wasmModule);

// Create entity
const { createEntity } = useCADStore();
await createEntity('sketch', 'Sketch 1');

// Set parameter
const { setParameter } = useCADStore();
await setParameter(entityId, 'radius', 25.5);

// Undo/Redo
const { undo, redo } = useCADStore();
await undo();
await redo();
```

## WebAssembly Integration

### Loading WASM Module

The WASM module is automatically loaded from `/lib/bendcore_wasm.js`:

```typescript
// Automatically loads bendcore_wasm.wasm and initializes
const module = await (window as any).BendCoreModule();
```

### API Functions

See `src/types/bendcore.ts` for full API documentation.

```typescript
// Entity management
module.createEntity(type, name): Entity
module.deleteEntity(entityId): boolean
module.getEntity(entityId): Entity | null
module.getAllEntities(): Entity[]

// Parameters
module.setParameter(entityId, paramName, value): boolean
module.getParameter(entityId, paramName): number

// Geometry
module.getGeometry(entityId): Geometry
module.triangulateGeometry(entityId): TriangleData

// Serialization
module.saveDocument(): { success: boolean; data: string }
module.loadDocument(json: string): { success: boolean }

// History
module.undo(): boolean
module.redo(): boolean
```

## Performance

- **Bundle Size**: ~500KB (gzipped, including WASM)
- **Load Time**: <2s on 4G connection
- **Frame Rate**: 60 FPS for real-time updates
- **Memory**: <100MB for typical projects

## Browser Support

- Chrome/Edge 90+
- Firefox 88+
- Safari 15+
- Requires WebAssembly and WebGL support

## Styling

Uses CSS Modules for component-level styling with a modern design system.

**Colors**:
- Primary: #4f46e5 (Indigo)
- Secondary: #f0f0f0 (Light gray)
- Danger: #ef4444 (Red)
- Backgrounds: #ffffff, #f5f5f5

**Typography**:
- System fonts: -apple-system, BlinkMacSystemFont, Segoe UI, Roboto
- Base size: 14px
- Responsive scaling

## Building for Deployment

```bash
# Production build with optimization
npm run build

# Preview production build locally
npm run preview

# Deploy to hosting
# Copy dist/ contents to web server
```

## Troubleshooting

### WASM Module Not Loading

1. Check that `public/lib/bendcore_wasm.js` and `.wasm` are present
2. Verify CORS headers allow loading from your domain
3. Check browser console for detailed errors

### 3D Viewer Not Rendering

1. Verify WebGL support: https://get.webgl.org/
2. Check that Three.js is properly imported
3. Ensure camera is positioned correctly

### State Management Issues

1. Check Zustand store initialization
2. Verify WASM module is loaded before operations
3. Use React DevTools for state inspection

## Contributing

1. Follow TypeScript strict mode guidelines
2. Run linter before committing: `npm run lint:fix`
3. Add tests for new features
4. Update documentation

## Build Pipeline Integration

This project integrates with GitHub Actions CI/CD:

```yaml
# See ../.github/workflows/ci.yml
- React build
- Type checking
- Linting
- Testing
- WASM integration
```

## License

Proprietary - BendCore CAD System

---

**Status**: Production Ready  
**Version**: 0.1.0  
**Last Updated**: 2026-10-25
