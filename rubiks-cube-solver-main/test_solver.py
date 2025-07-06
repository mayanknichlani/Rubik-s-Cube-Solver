#!/usr/bin/env python3
"""
Test script to verify the C++ solver works independently
"""
import subprocess
import tempfile
import os

def test_solver():
    """Test the C++ solver with a simple cube state"""
    
    # Create a simple solved cube state (all faces have their canonical colors)
    # This should be a solved cube
    solved_bitboards = [
        0x0000000000000000,  # U face (white)
        0x0101010101010101,  # L face (green) 
        0x0202020202020202,  # F face (red)
        0x0303030303030303,  # R face (blue)
        0x0404040404040404,  # B face (orange)
        0x0505050505050505   # D face (yellow)
    ]
    
    print("Testing C++ solver with solved cube state...")
    print(f"Bitboards: {solved_bitboards}")
    
    # Create temporary file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as temp_file:
        temp_filename = temp_file.name
        temp_file.write(' '.join(str(board) for board in solved_bitboards))
        temp_file.write('\n')
    
    print(f"Created temporary file: {temp_filename}")
    
    # Read and verify the file content
    with open(temp_filename, 'r') as f:
        content = f.read()
        print(f"File content: {content.strip()}")
    
    try:
        # Run the C++ solver
        solver_path = os.path.join(os.getcwd(), 'rubiks_solver.exe')
        print(f"Using solver path: {solver_path}")
        
        if not os.path.exists(solver_path):
            print(f"Error: Solver executable not found at {solver_path}")
            return False
            
        result = subprocess.run([
            solver_path, temp_filename
        ], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, timeout=60)
        
        # Clean up temporary file
        os.remove(temp_filename)
        
        print(f"Return code: {result.returncode}")
        print(f"STDOUT: {result.stdout}")
        print(f"STDERR: {result.stderr}")
        
        if result.returncode == 0:
            print("✅ Solver executed successfully!")
            
            # Parse solution
            lines = result.stdout.strip().split('\n')
            solution_line = ''
            for line in lines:
                if line.startswith('Solution moves:'):
                    solution_line = line[len('Solution moves:'):].strip()
                    break
            
            moves = solution_line.split() if solution_line else []
            print(f"Solution moves: {moves}")
            
            if len(moves) == 0:
                print("✅ Correctly identified solved cube (no moves needed)")
            else:
                print(f"⚠️  Found {len(moves)} moves for solved cube")
                
            return True
        else:
            print("❌ Solver failed")
            return False
            
    except subprocess.TimeoutExpired:
        print("❌ Solver timed out")
        os.remove(temp_filename)
        return False
    except Exception as e:
        print(f"❌ Error running solver: {e}")
        if os.path.exists(temp_filename):
            os.remove(temp_filename)
        return False

if __name__ == "__main__":
    success = test_solver()
    if success:
        print("\n🎉 Solver test passed!")
    else:
        print("\n💥 Solver test failed!") 