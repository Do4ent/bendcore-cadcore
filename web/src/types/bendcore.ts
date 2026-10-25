/**
 * BendCore WebAssembly Module Bindings
 */

export interface Entity {
  id: string;
  type: string;
  name: string;
  parameters?: Record<string, number>;
}

export interface Geometry {
  vertices: number[];
  faces: number[];
  edges: number[];
}

export interface TriangleData {
  vertices: number[];
  indices: number[];
}

export interface DocumentStats {
  entityCount: number;
  transactionCount: number;
  hasPendingChanges: boolean;
}

export interface CommandResult {
  success: boolean;
  message?: string;
  error?: string;
}

export interface SerializationResult {
  success: boolean;
  data?: string;
  error?: string;
}

export interface BendCoreModule {
  // Document operations
  initDocument(): void;

  // Entity management
  createEntity(type: string, name: string): Entity;
  deleteEntity(entityId: string): boolean;
  getEntity(entityId: string): Entity | null;
  getAllEntities(): Entity[];

  // Parameter operations
  setParameter(entityId: string, paramName: string, value: number): boolean;
  getParameter(entityId: string, paramName: string): number;

  // Command execution
  executeCommand(command: string, params: any): boolean;

  // History operations
  undo(): boolean;
  redo(): boolean;

  // Geometry operations
  getGeometry(entityId: string): Geometry;
  triangulateGeometry(entityId: string): TriangleData;

  // Document serialization
  saveDocument(): SerializationResult;
  loadDocument(json: string): SerializationResult;

  // Document statistics
  getDocumentStats(): DocumentStats;
}

export interface Point3D {
  x: number;
  y: number;
  z: number;
}

export interface Vector3D {
  x: number;
  y: number;
  z: number;
}

export interface Box3D {
  min: Point3D;
  max: Point3D;
}

export interface Plane {
  origin: Point3D;
  normal: Vector3D;
}

export interface Circle {
  center: Point3D;
  radius: number;
  normal: Vector3D;
}

export interface EntityType {
  SKETCH = "sketch";
  EXTRUDE = "extrude";
  POCKET = "pocket";
  PAD = "pad";
  REVOLUTION = "revolution";
  SWEEP = "sweep";
  LOFT = "loft";
  BLEND = "blend";
  REFERENCE = "reference";
}
