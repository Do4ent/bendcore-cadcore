import { create } from 'zustand';
import { Entity, BendCoreModule, DocumentStats } from '../types/bendcore';

interface CADState {
  // Module state
  module: BendCoreModule | null;
  isInitialized: boolean;
  isLoading: boolean;
  error: string | null;

  // Document state
  entities: Entity[];
  selectedEntityId: string | null;
  stats: DocumentStats | null;
  hasPendingChanges: boolean;

  // View state
  viewMode: '3d' | '2d';
  cameraPosition: [number, number, number];
  showGrid: boolean;
  showAxes: boolean;

  // Actions
  initializeModule: (module: BendCoreModule) => void;
  createEntity: (type: string, name: string) => Promise<Entity | null>;
  deleteEntity: (entityId: string) => Promise<boolean>;
  selectEntity: (entityId: string | null) => void;
  setParameter: (entityId: string, paramName: string, value: number) => Promise<boolean>;
  getParameter: (entityId: string, paramName: string) => number;
  undo: () => Promise<boolean>;
  redo: () => Promise<boolean>;
  refreshEntities: () => Promise<void>;
  saveDocument: () => Promise<string | null>;
  loadDocument: (json: string) => Promise<boolean>;
  updateViewMode: (mode: '3d' | '2d') => void;
  setCameraPosition: (pos: [number, number, number]) => void;
  toggleGrid: () => void;
  toggleAxes: () => void;
}

export const useCADStore = create<CADState>((set, get) => ({
  // Initial state
  module: null,
  isInitialized: false,
  isLoading: false,
  error: null,
  entities: [],
  selectedEntityId: null,
  stats: null,
  hasPendingChanges: false,
  viewMode: '3d',
  cameraPosition: [100, 100, 100],
  showGrid: true,
  showAxes: true,

  // Initialize module
  initializeModule: (module: BendCoreModule) => {
    try {
      module.initDocument();
      set({ module, isInitialized: true, error: null });
    } catch (err) {
      set({ error: 'Failed to initialize CAD module' });
    }
  },

  // Create entity
  createEntity: async (type: string, name: string) => {
    const { module } = get();
    if (!module) return null;

    try {
      const entity = module.createEntity(type, name);
      await get().refreshEntities();
      return entity;
    } catch (err) {
      set({ error: 'Failed to create entity' });
      return null;
    }
  },

  // Delete entity
  deleteEntity: async (entityId: string) => {
    const { module, selectedEntityId } = get();
    if (!module) return false;

    try {
      const success = module.deleteEntity(entityId);
      if (success) {
        if (selectedEntityId === entityId) {
          set({ selectedEntityId: null });
        }
        await get().refreshEntities();
      }
      return success;
    } catch (err) {
      set({ error: 'Failed to delete entity' });
      return false;
    }
  },

  // Select entity
  selectEntity: (entityId: string | null) => {
    set({ selectedEntityId: entityId });
  },

  // Set parameter
  setParameter: async (entityId: string, paramName: string, value: number) => {
    const { module } = get();
    if (!module) return false;

    try {
      const success = module.setParameter(entityId, paramName, value);
      if (success) {
        set({ hasPendingChanges: true });
        await get().refreshEntities();
      }
      return success;
    } catch (err) {
      set({ error: 'Failed to set parameter' });
      return false;
    }
  },

  // Get parameter
  getParameter: (entityId: string, paramName: string) => {
    const { module } = get();
    if (!module) return 0;

    try {
      return module.getParameter(entityId, paramName);
    } catch (err) {
      return 0;
    }
  },

  // Undo
  undo: async () => {
    const { module } = get();
    if (!module) return false;

    try {
      const success = module.undo();
      if (success) {
        await get().refreshEntities();
      }
      return success;
    } catch (err) {
      set({ error: 'Undo failed' });
      return false;
    }
  },

  // Redo
  redo: async () => {
    const { module } = get();
    if (!module) return false;

    try {
      const success = module.redo();
      if (success) {
        await get().refreshEntities();
      }
      return success;
    } catch (err) {
      set({ error: 'Redo failed' });
      return false;
    }
  },

  // Refresh entities
  refreshEntities: async () => {
    const { module } = get();
    if (!module) return;

    try {
      const entities = module.getAllEntities();
      const stats = module.getDocumentStats();
      set({ entities, stats });
    } catch (err) {
      set({ error: 'Failed to refresh entities' });
    }
  },

  // Save document
  saveDocument: async () => {
    const { module } = get();
    if (!module) return null;

    try {
      const result = module.saveDocument();
      if (result.success && result.data) {
        set({ hasPendingChanges: false });
        return result.data;
      }
      return null;
    } catch (err) {
      set({ error: 'Failed to save document' });
      return null;
    }
  },

  // Load document
  loadDocument: async (json: string) => {
    const { module } = get();
    if (!module) return false;

    try {
      const result = module.loadDocument(json);
      if (result.success) {
        await get().refreshEntities();
        set({ hasPendingChanges: false });
        return true;
      }
      return false;
    } catch (err) {
      set({ error: 'Failed to load document' });
      return false;
    }
  },

  // Update view mode
  updateViewMode: (mode: '3d' | '2d') => {
    set({ viewMode: mode });
  },

  // Set camera position
  setCameraPosition: (pos: [number, number, number]) => {
    set({ cameraPosition: pos });
  },

  // Toggle grid
  toggleGrid: () => {
    set((state) => ({ showGrid: !state.showGrid }));
  },

  // Toggle axes
  toggleAxes: () => {
    set((state) => ({ showAxes: !state.showAxes }));
  },
}));
