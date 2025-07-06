from flask import Flask, render_template, request, jsonify
from flask_cors import CORS  # Add this import
import subprocess
import tempfile
import os

app = Flask(__name__)
CORS(app)  # Enable CORS for all routes

# Color to bit mapping (must match frontend and C++ logic)
COLOR_TO_BIT = {
    'W': 0,  # white
    'G': 1,  # green
    'R': 2,  # red
    'B': 3,  # blue
    'O': 4,  # orange
    'Y': 5   # yellow
}
FACE_ORDER = ['U', 'L', 'F', 'R', 'B', 'D']

def color_array_to_bitboard(face_colors):
    # face_colors: list of 8 color codes (no center)
    bits = 0
    for i in range(8):
        color = face_colors[i]
        bit = COLOR_TO_BIT.get(color, 0)
        sticker_val = 1 << bit
        bits |= (sticker_val << (i * 8))
    return bits

def face_arrays_to_planar(data):
    # Reconstruct planar net from 6 face arrays (8 stickers each, no center)
    # Follows the standard net layout
    net = [[None for _ in range(12)] for _ in range(9)]
    # U face (rows 0-2, cols 2-4)
    u = data['U']
    idx = 0
    for i in range(3):
        for j in range(3):
            if i == 1 and j == 1:
                net[i][j+2] = 'U'
            else:
                net[i][j+2] = u[idx]
                idx += 1
    # L face (rows 3-5, cols 0-2)
    l = data['L']
    idx = 0
    for i in range(3,6):
        for j in range(3):
            if i == 4 and j == 1:
                net[i][j] = 'L'
            else:
                net[i][j] = l[idx]
                idx += 1
    # F face (rows 3-5, cols 3-5)
    f = data['F']
    idx = 0
    for i in range(3,6):
        for j in range(3,6):
            if i == 4 and j == 4:
                net[i][j] = 'F'
            else:
                net[i][j] = f[idx]
                idx += 1
    # R face (rows 3-5, cols 6-8)
    r = data['R']
    idx = 0
    for i in range(3,6):
        for j in range(6,9):
            if i == 4 and j == 7:
                net[i][j] = 'R'
            else:
                net[i][j] = r[idx]
                idx += 1
    # B face (rows 3-5, cols 9-11)
    b = data['B']
    idx = 0
    for i in range(3,6):
        for j in range(9,12):
            if i == 4 and j == 10:
                net[i][j] = 'B'
            else:
                net[i][j] = b[idx]
                idx += 1
    # D face (rows 6-8, cols 2-4)
    d = data['D']
    idx = 0
    for i in range(6,9):
        for j in range(2,5):
            if i == 7 and j == 3:
                net[i][j] = 'D'
            else:
                net[i][j] = d[idx]
                idx += 1
    return net

def print_planar_net(net):
    for row in net:
        print(' '.join(x if x is not None else ' ' for x in row))

def solve_cube_with_cpp(bitboards):
    """
    Solve Rubik's cube using C++ solver
    bitboards: list of 6 uint64 values representing the cube state
    """
    # Create temporary file with bitboards in the correct format
    with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as temp_file:
        temp_filename = temp_file.name
        # Write all 6 bitboards on a single line, space-separated
        temp_file.write(' '.join(str(board) for board in bitboards))
        # Add a newline at the end (optional but good practice)
        temp_file.write('\n')
    
    print(f"Created temporary file: {temp_filename}")
    
    # Read and verify the file content
    with open(temp_filename, 'r') as f:
        content = f.read()
        print(f"File content: {content.strip()}")
    
    try:
        # Run the C++ solver with absolute path
        solver_path = os.path.join(os.getcwd(), 'rubiks_solver.exe')
        print(f"Using solver path: {solver_path}")
        
        if not os.path.exists(solver_path):
            print(f"Error: Solver executable not found at {solver_path}")
            return None
            
        result = subprocess.run([
            solver_path, temp_filename
        ], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, timeout=300)
        
        # Clean up temporary file
        os.remove(temp_filename)
        
        # Print results
        if result.returncode == 0:
            print("C++ Solver Output:")
            print(result.stdout)
        else:
            print("C++ Solver Error:")
            print(f"Return code: {result.returncode}")
            print(f"STDOUT: {result.stdout}")
            print(f"STDERR: {result.stderr}")
            
        return result
        
    except subprocess.TimeoutExpired:
        print("C++ solver timed out after 5 minutes")
        os.remove(temp_filename)
        return None
    except Exception as e:
        print(f"Error running C++ solver: {e}")
        if os.path.exists(temp_filename):
            os.remove(temp_filename)
        return None

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/test', methods=['GET'])
def test():
    """Simple test endpoint to verify server is working"""
    return jsonify({'status': 'Server is running', 'message': 'Hello from Flask!'})

@app.route('/solve', methods=['POST'])
def solve():
    try:
        print("=== SOLVE REQUEST RECEIVED ===")
        data = request.get_json()
        print(f"Received data: {data}")
        
        if 'bitboards' in data:
            # Frontend is sending bitboards directly as strings (BigInt)
            bitboard_strings = data['bitboards']
            print('Received bitboards from frontend:')
            for i, face in enumerate(FACE_ORDER):
                print(f'{face}: {bitboard_strings[i]}')
            # Convert string representations to proper 64-bit integers
            bitboards = [int(bitboard_str) for bitboard_str in bitboard_strings]
        else:
            # Legacy support for color arrays
            print('Planar net reconstructed from face arrays:')
            net = face_arrays_to_planar(data)
            print_planar_net(net)
            for face in FACE_ORDER:
                print(f'{face}: {data[face]}')
            # Convert color arrays to bitboards
            bitboards = [color_array_to_bitboard(data[face]) for face in FACE_ORDER]
        
        print(f"Converted bitboards: {bitboards}")
        
        # Solve the cube
        result = solve_cube_with_cpp(bitboards)
        
        if result is None:
            print("Solver returned None - likely timeout or executable not found")
            return jsonify({'error': 'Solver timeout or executable not found'}), 500
        
        # Check if there was an error
        if result.stderr:
            print(f"C++ solver error: {result.stderr}")
            return jsonify({'error': 'Solver error', 'details': result.stderr}), 500
        
        # Parse the solution moves from the output
        lines = result.stdout.strip().split('\n')
        print(f"Raw solver output lines: {lines}")
        solution_line = ''
        for line in lines:
            if line.startswith('Solution moves:'):
                solution_line = line[len('Solution moves:'):].strip()
                break
        moves = solution_line.split() if solution_line else []
        print(f"Parsed moves: {moves}")
        
        response_data = {'solution': moves}
        print(f"Sending response: {response_data}")
        return jsonify(response_data)
    
    except Exception as e:
        print(f"Error in solve route: {str(e)}")
        import traceback
        traceback.print_exc()
        return jsonify({'error': 'Internal server error', 'details': str(e)}), 500

@app.route('/print_state', methods=['POST'])
def print_state():
    try:
        data = request.get_json()
        
        if 'bitboards' in data:
            # Frontend is sending bitboards directly as strings (BigInt)
            bitboard_strings = data['bitboards']
            print('Received bitboards for state printing:')
            for i, face in enumerate(FACE_ORDER):
                print(f'{face}: {bitboard_strings[i]}')
            # Convert string representations to proper 64-bit integers
            bitboards = [int(bitboard_str) for bitboard_str in bitboard_strings]
        else:
            # Legacy support for color arrays
            print('Face arrays for state printing:')
            for face in FACE_ORDER:
                print(f'{face}: {data[face]}')
            # Convert color arrays to bitboards
            bitboards = [color_array_to_bitboard(data[face]) for face in FACE_ORDER]
        
        # Write bitboards to a temp file
        with tempfile.NamedTemporaryFile(delete=False, mode='w') as f:
            f.write(' '.join(map(str, bitboards)))
            temp_filename = f.name  
        
        # Call the C++ solver with the temp file as input
        solver_path = os.path.join(os.getcwd(), 'rubiks_solver.exe')
        result = subprocess.run([
            solver_path, temp_filename
        ], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        os.remove(temp_filename)
        print(result.stdout)  # Print the full C++ output to the terminal
        
        return jsonify({'status': 'State printed to terminal'})
    
    except Exception as e:
        print(f"Error in print_state route: {str(e)}")
        return jsonify({'error': 'Internal server error', 'details': str(e)}), 500

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)