import React, { useEffect, useRef, useState } from 'react';
import { Canvas, useFrame, useThree } from '@react-three/fiber';
import { PerspectiveCamera, Grid, OrbitControls } from '@react-three/drei';
import * as THREE from 'three';
import { useCADStore } from '../services/cadStore';

interface ViewerProps {
  entityId?: string;
}

const Mesh3D: React.FC<{ entityId?: string }> = ({ entityId }) => {
  const meshRef = useRef<THREE.Mesh>(null);
  const { module } = useCADStore();
  const [geometry, setGeometry] = useState<THREE.BufferGeometry | null>(null);

  useEffect(() => {
    if (!module || !entityId) return;

    try {
      const triangleData = module.triangulateGeometry(entityId);

      const geom = new THREE.BufferGeometry();
      geom.setAttribute(
        'position',
        new THREE.BufferAttribute(new Float32Array(triangleData.vertices), 3)
      );

      if (triangleData.indices && triangleData.indices.length > 0) {
        geom.setIndex(
          new THREE.BufferAttribute(new Uint32Array(triangleData.indices), 1)
        );
      }

      geom.computeVertexNormals();
      setGeometry(geom);
    } catch (err) {
      console.error('Failed to load geometry:', err);
    }
  }, [module, entityId]);

  if (!geometry) {
    return (
      <mesh>
        <boxGeometry args={[1, 1, 1]} />
        <meshPhongMaterial color="#888888" />
      </mesh>
    );
  }

  return (
    <mesh ref={meshRef} geometry={geometry}>
      <meshPhongMaterial color="#4f46e5" wireframe={false} />
    </mesh>
  );
};

const GridComponent: React.FC = () => {
  const { showGrid } = useCADStore();

  if (!showGrid) return null;

  return (
    <Grid
      args={[100, 100]}
      cellSize={1}
      cellColor="#6f6f6f"
      sectionSize={10}
      sectionColor="#9f9f9f"
      fadeDistance={500}
      fadeStrength={1}
      infiniteGrid
    />
  );
};

const AxesHelper: React.FC = () => {
  const { showAxes } = useCADStore();

  if (!showAxes) return null;

  return (
    <primitive
      object={new THREE.AxesHelper(50)}
    />
  );
};

const Viewer3DCanvas: React.FC<ViewerProps> = ({ entityId }) => {
  return (
    <Canvas
      style={{ width: '100%', height: '100%' }}
      camera={{ position: [100, 100, 100], fov: 50 }}
    >
      <PerspectiveCamera makeDefault position={[100, 100, 100]} fov={50} />

      <ambientLight intensity={0.5} />
      <directionalLight position={[100, 100, 100]} intensity={0.8} />
      <pointLight position={[-100, -100, -100]} intensity={0.3} />

      <GridComponent />
      <AxesHelper />

      {entityId && <Mesh3D entityId={entityId} />}

      <OrbitControls />
    </Canvas>
  );
};

export const Viewer3D: React.FC<ViewerProps> = (props) => {
  return (
    <div style={{ width: '100%', height: '100%' }}>
      <React.Suspense fallback={<div>Loading 3D viewer...</div>}>
        <Viewer3DCanvas {...props} />
      </React.Suspense>
    </div>
  );
};
