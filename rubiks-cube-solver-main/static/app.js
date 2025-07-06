// --- Cube and Color Definitions ---
const FACE_ORDER = ['U', 'L', 'F', 'R', 'B', 'D'];
const FACE_COLORS = {
    U: 'W',    // White
    L: 'G',    // Green
    F: 'R',    // Red
    R: 'B',    // Blue
    D: 'Y',    // Yellow
    B: 'O'     // Orange
};
const COLOR_PALETTE = [
    { name: 'white', color: 'W' },
    { name: 'green', color: 'G' },
    { name: 'red', color: 'R' },
    { name: 'blue', color: 'B' },
    { name: 'orange', color: 'O' },
    { name: 'yellow', color: 'Y' }
];

// Color to bit mapping (must match C++ logic)
const COLOR_TO_BIT = {
    'W': 0,  // white
    'G': 1,  // green
    'R': 2,  // red
    'B': 3,  // blue
    'O': 4,  // orange
    'Y': 5   // yellow
};
const FACE_POSITIONS = {
    U: { row: 0, col: 1 },
    L: { row: 1, col: 0 },
    F: { row: 1, col: 1 },
    R: { row: 1, col: 2 },
    D: { row: 2, col: 1 },
    B: { row: 1, col: 3 }
};
// Mapping from (i,j) to 8-element array index
const STICKER_MAP = [
    [0, 1, 2],
    [7, -1, 3],
    [6, 5, 4]
];

// --- State ---
let selectedColor = COLOR_PALETTE[0].color;
// 6 faces, each 8 stickers (no center), default to their canonical color
let cubeState = {};
FACE_ORDER.forEach(face => {
    cubeState[face] = Array(8).fill(FACE_COLORS[face]);
});

// --- Render Functions ---
function renderCubeNet() {
    const container = document.getElementById('cube-container');
    container.innerHTML = '';
    const net = document.createElement('div');
    net.className = 'cube-net';
    // 3 rows x 4 cols grid
    for (let row = 0; row < 3; row++) {
        for (let col = 0; col < 4; col++) {
            // Find which face (if any) is at this grid position
            let face = null;
            for (const f of FACE_ORDER) {
                if (FACE_POSITIONS[f].row === row && FACE_POSITIONS[f].col === col) {
                    face = f;
                    break;
                }
            }
            if (face) {
                const faceDiv = document.createElement('div');
                faceDiv.className = 'face';
                faceDiv.dataset.face = face;
                for (let i = 0; i < 3; i++) {
                    for (let j = 0; j < 3; j++) {
                        const sticker = document.createElement('div');
                        sticker.className = 'sticker';
                        const idx = STICKER_MAP[i][j];
                        if (idx === -1) {
                            // Center sticker, show canonical color, not editable
                            sticker.classList.add('center');
                            sticker.style.background = colorToHex(FACE_COLORS[face]);
                        } else {
                            sticker.style.background = colorToHex(cubeState[face][idx]);
                            sticker.dataset.face = face;
                            sticker.dataset.idx = idx;
                            sticker.addEventListener('click', onStickerClick);
                        }
                        faceDiv.appendChild(sticker);
                    }
                }
                net.appendChild(faceDiv);
            } else {
                // Empty cell
                const empty = document.createElement('div');
                empty.style.width = '120px';
                empty.style.height = '120px';
                net.appendChild(empty);
            }
        }
    }
    container.appendChild(net);
}

function renderPalette() {
    const palette = document.getElementById('palette');
    palette.innerHTML = '';
    COLOR_PALETTE.forEach((col, idx) => {
        const colorDiv = document.createElement('div');
        colorDiv.className = 'palette-color' + (selectedColor === col.color ? ' selected' : '');
        colorDiv.style.background = colorToHex(col.color);
        colorDiv.addEventListener('click', () => {
            selectedColor = col.color;
            renderPalette();
        });
        palette.appendChild(colorDiv);
    });
}

function colorToHex(c) {
    switch (c) {
        case 'W': return '#fff';
        case 'G': return '#0f0';
        case 'R': return '#f00';
        case 'B': return '#00f';
        case 'O': return '#fa0';
        case 'Y': return '#ff0';
        default: return '#222';
    }
}

function colorArrayToBitboard(faceColors) {
    // faceColors: array of 8 color codes (no center)
    // Use BigInt to handle 64-bit integers properly
    let bits = BigInt(0);
    for (let i = 0; i < 8; i++) {
        const color = faceColors[i];
        const bit = COLOR_TO_BIT[color] || 0;
        const stickerVal = BigInt(1) << BigInt(bit);
        bits |= (stickerVal << BigInt(i * 8));
    }
    return bits.toString(); // Return as string to preserve full precision
}

function onStickerClick(e) {
    const face = e.target.dataset.face;
    const idx = parseInt(e.target.dataset.idx);
    if (isNaN(idx) || idx < 0 || idx > 7) return;
    cubeState[face][idx] = selectedColor;
    renderCubeNet();
}

function showSolutionNotification(moves) {
    let old = document.getElementById('solution-toast');
    if (old) old.remove();
    const toast = document.createElement('div');
    toast.id = 'solution-toast';
    toast.style.position = 'fixed';
    toast.style.bottom = '32px';
    toast.style.left = '50%';
    toast.style.transform = 'translateX(-50%)';
    toast.style.background = '#222';
    toast.style.color = '#fff';
    toast.style.padding = '20px 36px';
    toast.style.borderRadius = '10px';
    toast.style.boxShadow = '0 4px 24px #000a';
    toast.style.fontSize = '1.25em';
    toast.style.zIndex = 9999;
    toast.style.display = 'flex';
    toast.style.alignItems = 'center';
    toast.style.gap = '18px';
    let msg = '';
    if (!moves || moves.length === 0 || (moves.length === 1 && moves[0] === '')) {
        msg = '<b>No solution found or input is invalid.</b>';
    } else {
        msg = '<b>Solution:</b> ' + moves.join(' ');
    }
    toast.innerHTML = msg + '<span id="close-toast" style="margin-left:18px;cursor:pointer;font-size:1.5em;">&times;</span>';
    document.body.appendChild(toast);
    document.getElementById('close-toast').onclick = () => toast.remove();
    setTimeout(() => { if (toast.parentNode) toast.remove(); }, 10000);
}

// --- Manual Modal ---
document.addEventListener('DOMContentLoaded', () => {
    renderCubeNet();
    renderPalette();

    document.getElementById('manual-icon').onclick = () => {
        document.getElementById('manual-modal').classList.remove('hidden');
    };
    document.getElementById('close-manual').onclick = () => {
        document.getElementById('manual-modal').classList.add('hidden');
    };
    document.getElementById('manual-modal').onclick = (e) => {
        if (e.target === document.getElementById('manual-modal')) {
            document.getElementById('manual-modal').classList.add('hidden');
        }
    };

    document.getElementById('solve-btn').onclick = () => {
        // Convert color arrays to bitboards
        const bitboards = FACE_ORDER.map(face => colorArrayToBitboard(cubeState[face]));
        
        fetch('/solve', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ bitboards: bitboards })
        })
        .then(res => res.json())
        .then(data => {
            showSolutionNotification(data.solution);
        })
        .catch(err => {
            showSolutionNotification([]);
        });
    };

    // Add print state button functionality
    document.getElementById('print-state-btn').onclick = () => {
        // Convert color arrays to bitboards
        const bitboards = FACE_ORDER.map(face => colorArrayToBitboard(cubeState[face]));
        
        fetch('/print_state', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ bitboards: bitboards })
        })
        .then(res => res.json())
        .then(data => {
            showSolutionNotification(['State printed to terminal']);
        })
        .catch(err => {
            showSolutionNotification(['Error printing state']);
        });
    };
}); 