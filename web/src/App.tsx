import React, { useEffect } from 'react';
import { useCADStore } from './services/cadStore';
import { EntityTree } from './components/EntityTree';
import { PropertyPanel } from './components/PropertyPanel';
import { Viewer3D } from './components/Viewer3D';
import './styles/App.css';

const App: React.FC = () => {
  const {
    isInitialized,
    initializeModule,
    selectedEntityId,
    undo,
    redo,
    createEntity,
    hasPendingChanges,
    saveDocument
  } = useCADStore();

  useEffect(() => {
    // Load WASM module
    const loadModule = async () => {
      try {
        // This would be loaded dynamically from the WASM build
        if (typeof (window as any).BendCoreModule !== 'undefined') {
          const module = await (window as any).BendCoreModule();
          initializeModule(module);
        }
      } catch (err) {
        console.error('Failed to load WASM module:', err);
      }
    };

    if (!isInitialized) {
      loadModule();
    }
  }, [isInitialized, initializeModule]);

  const handleCreateEntity = async () => {
    const name = prompt('Enter entity name:');
    if (name) {
      await createEntity('sketch', name);
    }
  };

  const handleSave = async () => {
    const json = await saveDocument();
    if (json) {
      // Trigger download
      const blob = new Blob([json], { type: 'application/json' });
      const url = URL.createObjectURL(blob);
      const a = document.createElement('a');
      a.href = url;
      a.download = 'bendcore_document.json';
      a.click();
      URL.revokeObjectURL(url);
    }
  };

  return (
    <div className="app">
      <header className="app-header">
        <div className="header-left">
          <h1>BendCore CAD</h1>
          <span className="version">v0.1.0</span>
        </div>

        <div className="header-center">
          <button className="toolbar-btn" title="New" onClick={handleCreateEntity}>
            ➕ New
          </button>
          <button className="toolbar-btn" title="Undo" onClick={undo}>
            ↶ Undo
          </button>
          <button className="toolbar-btn" title="Redo" onClick={redo}>
            ↷ Redo
          </button>
        </div>

        <div className="header-right">
          <button
            className={`toolbar-btn ${hasPendingChanges ? 'unsaved' : ''}`}
            onClick={handleSave}
          >
            💾 {hasPendingChanges ? 'Save*' : 'Save'}
          </button>
        </div>
      </header>

      <div className="app-content">
        <aside className="app-sidebar-left">
          <EntityTree />
        </aside>

        <main className="app-main">
          <div className="viewer-container">
            {selectedEntityId ? (
              <Viewer3D entityId={selectedEntityId} />
            ) : (
              <div className="empty-viewer">
                <p>Select an entity to view 3D geometry</p>
              </div>
            )}
          </div>
        </main>

        <aside className="app-sidebar-right">
          <PropertyPanel />
        </aside>
      </div>

      <footer className="app-footer">
        <span className="status-text">Ready</span>
        <span className="stats-text" id="stats">Entities: 0</span>
      </footer>
    </div>
  );
};

export default App;
