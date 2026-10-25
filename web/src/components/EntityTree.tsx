import React, { useState } from 'react';
import { useCADStore } from '../services/cadStore';
import '../styles/EntityTree.css';

interface TreeItemProps {
  entityId: string;
  name: string;
  type: string;
  isSelected: boolean;
  onSelect: (id: string) => void;
  onDelete: (id: string) => void;
}

const TreeItem: React.FC<TreeItemProps> = ({
  entityId,
  name,
  type,
  isSelected,
  onSelect,
  onDelete,
}) => {
  const [showContextMenu, setShowContextMenu] = useState(false);

  return (
    <div
      className={`tree-item ${isSelected ? 'selected' : ''}`}
      onContextMenu={(e) => {
        e.preventDefault();
        setShowContextMenu(!showContextMenu);
      }}
    >
      <div
        className="tree-item-content"
        onClick={() => onSelect(entityId)}
        title={`${type} - ${entityId}`}
      >
        <span className="tree-item-icon">■</span>
        <span className="tree-item-name">{name}</span>
        <span className="tree-item-type">{type}</span>
      </div>

      {showContextMenu && (
        <div className="context-menu">
          <button onClick={() => onSelect(entityId)}>Select</button>
          <button onClick={() => onDelete(entityId)} className="delete">
            Delete
          </button>
          <button onClick={() => setShowContextMenu(false)}>Close</button>
        </div>
      )}
    </div>
  );
};

export const EntityTree: React.FC = () => {
  const { entities, selectedEntityId, selectEntity, deleteEntity } = useCADStore();
  const [expandedIds, setExpandedIds] = useState<Set<string>>(new Set());

  const handleToggleExpand = (id: string) => {
    setExpandedIds((prev) => {
      const newSet = new Set(prev);
      if (newSet.has(id)) {
        newSet.delete(id);
      } else {
        newSet.add(id);
      }
      return newSet;
    });
  };

  const handleSelect = (id: string) => {
    selectEntity(id);
  };

  const handleDelete = async (id: string) => {
    await deleteEntity(id);
    if (selectedEntityId === id) {
      selectEntity(null);
    }
  };

  return (
    <div className="entity-tree">
      <div className="tree-header">
        <h3>Entities ({entities.length})</h3>
        <div className="tree-actions">
          <button title="Expand all" onClick={() => setExpandedIds(new Set(entities.map((e) => e.id)))}>
            ▼
          </button>
          <button title="Collapse all" onClick={() => setExpandedIds(new Set())}>
            ▶
          </button>
        </div>
      </div>

      <div className="tree-content">
        {entities.length === 0 ? (
          <p className="empty-tree">No entities</p>
        ) : (
          entities.map((entity) => (
            <div key={entity.id} className="tree-item-wrapper">
              <TreeItem
                entityId={entity.id}
                name={entity.name}
                type={entity.type}
                isSelected={selectedEntityId === entity.id}
                onSelect={handleSelect}
                onDelete={handleDelete}
              />
            </div>
          ))
        )}
      </div>
    </div>
  );
};
