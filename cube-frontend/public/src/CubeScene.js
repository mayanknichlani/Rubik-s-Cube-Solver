import * as THREE from 'https://cdn.jsdelivr.net/npm/three@0.160.1/build/three.module.js';
import { OrbitControls } from 'https://cdn.jsdelivr.net/npm/three@0.160.1/examples/jsm/controls/OrbitControls.js';

export class CubeScene {
  constructor(canvas) {
    this.canvas = canvas;
    this.scene = new THREE.Scene();
    this.camera = new THREE.PerspectiveCamera(60, window.innerWidth / window.innerHeight, 0.1, 1000);
    this.camera.position.set(5, 5, 5);
    this.scene.add(this.camera);

    this.renderer = new THREE.WebGLRenderer({ canvas, antialias: true });
    this.renderer.setSize(window.innerWidth, window.innerHeight);
    this.renderer.setPixelRatio(window.devicePixelRatio);
    this.renderer.setClearColor(0xf0f0f0);

    this.controls = new OrbitControls(this.camera, this.renderer.domElement);
    this.controls.enableDamping = true;
    this.controls.dampingFactor = 0.05;

    // Lights
    const ambientLight = new THREE.AmbientLight(0xffffff, 0.4);
    const directionalLight = new THREE.DirectionalLight(0xffffff, 1);
    directionalLight.position.set(5, 10, 7);
    this.scene.add(ambientLight, directionalLight);

    // Container for cubelets
    this.cubeGroup = new THREE.Group();
    this.scene.add(this.cubeGroup);

    this.selectedColor = 0x888888;
    this.cubelets = [];
    this.cubeState = this.getCubeState();

    this.createCube();
    this.setupColorPalette();
    this.setupCubeletClick();

    window.addEventListener('resize', () => this.onResize(), false);
    this.onResize();

    this.animate();
  }

  onResize() {
    this.camera.aspect = window.innerWidth / window.innerHeight;
    this.camera.updateProjectionMatrix();
    this.renderer.setSize(window.innerWidth, window.innerHeight);
  }

  createCube() {
    const size = 0.95;
    const gap = 0.1;
    const grey = 0x888888;

    for (let x = -1; x <= 1; x++) {
      for (let y = -1; y <= 1; y++) {
        for (let z = -1; z <= 1; z++) {
          if (x === 0 && y === 0 && z === 0) continue;

          const geometry = new THREE.BoxGeometry(size, size, size);
          const materials = Array(6).fill().map(() => new THREE.MeshBasicMaterial({ color: grey }));
          const cubelet = new THREE.Mesh(geometry, materials);

          cubelet.position.set(x * (size + gap), y * (size + gap), z * (size + gap));

          const edges = new THREE.EdgesGeometry(geometry);
          const edgeLines = new THREE.LineSegments(edges, new THREE.LineBasicMaterial({ color: 0x000000 }));
          cubelet.add(edgeLines);

          this.cubeGroup.add(cubelet);
          this.cubelets.push(cubelet);
        }
      }
    }
  }

  setupColorPalette() {
    const colorMap = {
      'color-white': 0xffffff,
      'color-green': 0x009b48,
      'color-red': 0xff0000,
      'color-blue': 0x0000ff,
      'color-orange': 0xff6f00,
      'color-yellow': 0xffff00
    };

    for (const id in colorMap) {
      const btn = document.getElementById(id);
      if (btn) {
        btn.addEventListener('click', () => {
          this.selectedColor = colorMap[id];
        });
      }
    }
  }

  setupCubeletClick() {
    this.raycaster = new THREE.Raycaster();
    this.mouse = new THREE.Vector2();

    this.canvas.addEventListener('click', (event) => {
      const rect = this.canvas.getBoundingClientRect();
      this.mouse.x = ((event.clientX - rect.left) / rect.width) * 2 - 1;
      this.mouse.y = -((event.clientY - rect.top) / rect.height) * 2 + 1;
      this.raycaster.setFromCamera(this.mouse, this.camera);
      const intersects = this.raycaster.intersectObjects(this.cubelets);
      if (intersects.length > 0) {
        const intersect = intersects[0];
        const faceIndex = Math.floor(intersect.faceIndex / 2);
        const material = intersect.object.material[faceIndex];
        if (material) {
          material.color.setHex(this.selectedColor);
          this.saveCubeState();
        }
      }
    });
  }

  saveCubeState() {
    this.cubeState = this.getCubeState();
    // Optionally, you can log or persist this.cubeState here
    // console.log(this.cubeState);
  }

  getCubeState() {
    // Returns a 3D array [x][y][z][face] of color hex values for each face of each cubelet
    // Only for visible cubelets (not the center)
    const state = [];
    let idx = 0;
    for (let x = -1; x <= 1; x++) {
      state[x + 1] = [];
      for (let y = -1; y <= 1; y++) {
        state[x + 1][y + 1] = [];
        for (let z = -1; z <= 1; z++) {
          if (x === 0 && y === 0 && z === 0) {
            state[x + 1][y + 1][z + 1] = null;
            continue;
          }
          const cubelet = this.cubelets[idx++];
          const faces = [];
          for (let f = 0; f < 6; f++) {
            faces.push(cubelet.material[f].color.getHex());
          }
          state[x + 1][y + 1][z + 1] = faces;
        }
      }
    }
    return state;
  }

  animate() {
    requestAnimationFrame(() => this.animate());
    this.controls.update();
    this.renderer.render(this.scene, this.camera);
  }
}
