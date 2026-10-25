import React, { useState, useEffect } from 'react';
import { useCADStore } from '../services/cadStore';
import '../styles/PropertyPanel.css';

export const PropertyPanel: React.FC = () => {
  const { selectedEntityId, entities, setParameter, getParameter, module } = useCADStore();
  const [parameters, setParameters] = useState<Record<string, number>>({});

  const selectedEntity = selectedEntityId
    ? entities.find((e) => e.id === selectedEntityId)
    : null;

  useEffect(() => {
    if (selectedEntity && module) {
      try {
        const params: Record<string, number> = {};

        // Common parameters
        const commonParams = ['x', 'y', 'z', 'width', 'height', 'depth', 'radius', 'angle', 'offset'];

        for (const param of commonParams) {
          try {
            const value = getParameter(selectedEntity.id, param);
            params[param] = value;
          } catch (err) {
            // Parameter doesn't exist, skip
          }
        }

        setParameters(params);
      } catch (err) {
        console.error('Failed to load parameters:', err);
      }
    } else {
      setParameters({});
    }
  }, [selectedEntity, module, getParameter]);

  const handleParameterChange = async (paramName: string, value: number) => {
    if (!selectedEntity) return;

    setParameters((prev) => ({
      ...prev,
      [paramName]: value,
    }));

    await setParameter(selectedEntity.id, paramName, value);
  };

  const handleAddParameter = (paramName: string) => {
    if (!selectedEntity) return;
    handleParameterChange(paramName, 0);
  };

  if (!selectedEntity) {
    return (
      <div className="property-panel empty">
        <p>Select an entity to view properties</p>
      </div>
    );
  }

  return (
    <div className="property-panel">
      <div className="entity-header">
        <h2>{selectedEntity.name}</h2>
        <p className="entity-type">{selectedEntity.type}</p>
        <p className="entity-id">{selectedEntity.id.substring(0, 8)}...</p>
      </div>

      <div className="parameters-section">
        <h3>Parameters</h3>

        {Object.entries(parameters).length > 0 ? (
          <div className="parameters-list">
            {Object.entries(parameters).map(([paramName, value]) => (
              <div key={paramName} className="parameter-item">
                <label>{paramName}</label>
                <input
                  type="number"
                  value={value}
                  onChange={(e) =>
                    handleParameterChange(paramName, parseFloat(e.target.value))
                  }
                  step="0.1"
                />
              </div>
            ))}
          </div>
        ) : (
          <p className="no-parameters">No parameters yet</p>
        )}
      </div>

      <div className="quick-params">
        <h3>Quick Add</h3>
        <div className="quick-param-buttons">
          {['x', 'y', 'z', 'radius', 'angle'].map((param) => (
            !parameters.hasOwnProperty(param) && (
              <button
                key={param}
                onClick={() => handleAddParameter(param)}
                className="quick-param-btn"
              >
                + {param}
              </button>
            )
          ))}
        </div>
      </div>

      <div className="entity-actions">
        <button className="btn btn-primary">Edit</button>
        <button className="btn btn-secondary">Duplicate</button>
        <button className="btn btn-danger">Delete</button>
      </div>
    </div>
  );
};
