const express = require('express');
const fs = require('fs');
const cors = require('cors');
const bodyParser = require('body-parser');
const { execFile } = require('child_process');
const path = require('path');

const app = express();
const PORT = 3001;
const CUBE_STATE_FILE = path.join(__dirname, 'cube_state.json');
const MAIN_CPP_EXEC = path.join(__dirname, '../rubiks-cube-solver-main/main.exe');

app.use(cors());
app.use(bodyParser.json({ limit: '1mb' }));

// Save cube state
app.post('/cube-state', (req, res) => {
  const state = req.body;
  fs.writeFile(CUBE_STATE_FILE, JSON.stringify(state), err => {
    if (err) {
      return res.status(500).json({ error: 'Failed to save cube state' });
    }
    res.json({ message: 'Cube state saved' });
  });
});

// Solve cube
app.post('/solve', (req, res) => {
  // Optionally, accept state in body, else use saved file
  let state = req.body && Object.keys(req.body).length ? req.body : null;
  if (state) {
    fs.writeFileSync(CUBE_STATE_FILE, JSON.stringify(state));
  }
  // Call main.exe, passing the state file path as argument
  execFile(MAIN_CPP_EXEC, [CUBE_STATE_FILE], (error, stdout, stderr) => {
    if (error) {
      return res.status(500).json({ error: stderr || error.message });
    }
    res.json({ solution: stdout.trim() });
  });
});

app.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
}); 