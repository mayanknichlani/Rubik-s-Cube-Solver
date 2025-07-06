# Troubleshooting Guide

## Common Issues and Solutions

### 1. "Cannot connect to server" Error

**Problem**: The web interface shows "Cannot connect to server" when trying to solve.

**Solutions**:
1. **Check if Flask app is running**:
   ```bash
   python app.py
   ```
   You should see output like:
   ```
   * Running on http://0.0.0.0:5000
   * Debug mode: on
   ```

2. **Check if port 5000 is available**:
   - Make sure no other application is using port 5000
   - Try a different port by modifying `app.py`:
     ```python
     app.run(debug=True, host='0.0.0.0', port=5001)
     ```

3. **Test server connection**:
   - Click the "🔗 Test Server" button in the web interface
   - Or manually visit: `http://localhost:5000/test`

### 2. "Solver timeout or executable not found" Error

**Problem**: The solver times out or can't find the executable.

**Solutions**:
1. **Verify C++ solver exists**:
   ```bash
   ls -la rubiks_solver.exe
   ```
   The file should be in the same directory as `app.py`.

2. **Test solver independently**:
   ```bash
   python test_solver.py
   ```
   This will test if the C++ solver works without the web interface.

3. **Check file permissions** (Linux/Mac):
   ```bash
   chmod +x rubiks_solver.exe
   ```

### 3. "Solver error" with Details

**Problem**: The solver runs but returns an error.

**Solutions**:
1. **Check console output**: Look at the terminal where you ran `python app.py` for detailed error messages.

2. **Verify input format**: The solver expects 6 bitboards (64-bit integers) representing the cube state.

3. **Try a simpler cube state**: Use the "Reset" button to get a solved cube, then try solving.

### 4. Cube Animation Not Working

**Problem**: The solution is found but the cube doesn't animate properly.

**Solutions**:
1. **Use step-by-step controls**: Instead of auto-animation, use the "Step Forward" and "Step Backward" buttons.

2. **Check browser console**: Open Developer Tools (F12) and look for JavaScript errors.

3. **Try a different browser**: Some browsers handle JavaScript differently.

### 5. Performance Issues

**Problem**: The solver takes too long or the interface is slow.

**Solutions**:
1. **Use simpler cube states**: Complex scrambles may take longer to solve.

2. **Increase timeout**: Modify the timeout in `app.py`:
   ```python
   result = subprocess.run([...], timeout=600)  # 10 minutes
   ```

3. **Check system resources**: Make sure you have enough RAM and CPU available.

## Debugging Steps

### Step 1: Test Basic Functionality
1. Run the test script:
   ```bash
   python test_solver.py
   ```

2. Start the Flask app:
   ```bash
   python app.py
   ```

3. Open the web interface:
   ```
   http://localhost:5000
   ```

4. Click "🔗 Test Server" to verify connection.

### Step 2: Test with Simple Cube
1. Click "🔄 Reset" to get a solved cube.
2. Click "🧩 Solve" - this should return quickly with no moves needed.

### Step 3: Test with Scrambled Cube
1. Click "🎲 Scramble" to randomize the cube.
2. Click "🧩 Solve" and wait for the solution.
3. Use "▶️ Animate Solution" to see the moves.

## File Structure
```
rubiks-cube-solver/
├── app.py                 # Flask web server
├── rubiks_solver.exe      # C++ solver executable
├── test_solver.py         # Independent solver test
├── templates/
│   └── index.html         # Web interface
└── static/
    └── app.js             # Additional JavaScript (legacy)
```

## Common Error Messages

- **"Cannot connect to server"**: Flask app not running or wrong port
- **"Solver timeout"**: C++ solver taking too long, try simpler cube
- **"Executable not found"**: `rubiks_solver.exe` missing or wrong path
- **"Failed to fetch"**: Network connectivity issue
- **"Server error 500"**: Check Flask app console for details

## Getting Help

If you're still having issues:

1. **Check the console output** from both the Flask app and your browser's Developer Tools
2. **Run the test script** to isolate if it's a web interface or solver issue
3. **Try with a solved cube first** to verify basic functionality
4. **Check file permissions** and ensure all files are in the correct locations 