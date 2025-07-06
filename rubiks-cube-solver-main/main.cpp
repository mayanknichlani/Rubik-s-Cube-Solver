#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>
#include "Model/RubiksCube.h"
#include "Solver/IDAstarSolver.h"
#include "PatternDatabases/CornerDBMaker.h"

using namespace std;

// Function to convert color enum to character
char getColorChar(RubiksCube::COLOR color) {
    switch (color) {
        case RubiksCube::COLOR::WHITE: return 'W';
        case RubiksCube::COLOR::GREEN: return 'G';
        case RubiksCube::COLOR::RED: return 'R';
        case RubiksCube::COLOR::BLUE: return 'B';
        case RubiksCube::COLOR::ORANGE: return 'O';
        case RubiksCube::COLOR::YELLOW: return 'Y';
        default: return '?';
    }
}

// Function to print cube state in planar net format
void printCubeState(const RubiksCubeBitboard& cube, const string& title = "Cube State") {
    cout << "\n=== " << title << " ===" << endl;
    
    // Top face (U)
    cout << "    ";
    for (int j = 0; j < 12; j++) {
        if (j >= 2 && j <= 4) {
            char color = getColorChar(cube.getColor(RubiksCube::FACE::UP, 0, j-2));
            cout << color << " ";
        } else {
            cout << "  ";
        }
    }
    cout << endl;
    
    cout << "    ";
    for (int j = 0; j < 12; j++) {
        if (j >= 2 && j <= 4) {
            char color = getColorChar(cube.getColor(RubiksCube::FACE::UP, 1, j-2));
            cout << color << " ";
        } else {
            cout << "  ";
        }
    }
    cout << endl;
    
    cout << "    ";
    for (int j = 0; j < 12; j++) {
        if (j >= 2 && j <= 4) {
            char color = getColorChar(cube.getColor(RubiksCube::FACE::UP, 2, j-2));
            cout << color << " ";
        } else {
            cout << "  ";
        }
    }
    cout << endl;
    
    // Middle row (L, F, R, B)
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 12; j++) {
            if (j >= 0 && j <= 2) {
                char color = getColorChar(cube.getColor(RubiksCube::FACE::LEFT, i, j));
                cout << color << " ";
            } else if (j >= 3 && j <= 5) {
                char color = getColorChar(cube.getColor(RubiksCube::FACE::FRONT, i, j-3));
                cout << color << " ";
            } else if (j >= 6 && j <= 8) {
                char color = getColorChar(cube.getColor(RubiksCube::FACE::RIGHT, i, j-6));
                cout << color << " ";
            } else if (j >= 9 && j <= 11) {
                char color = getColorChar(cube.getColor(RubiksCube::FACE::BACK, i, j-9));
                cout << color << " ";
            }
        }
        cout << endl;
    }
    
    // Bottom face (D)
    cout << "    ";
    for (int j = 0; j < 12; j++) {
        if (j >= 2 && j <= 4) {
            char color = getColorChar(cube.getColor(RubiksCube::FACE::DOWN, 0, j-2));
            cout << color << " ";
        } else {
            cout << "  ";
        }
    }
    cout << endl;
    
    cout << "    ";
    for (int j = 0; j < 12; j++) {
        if (j >= 2 && j <= 4) {
            char color = getColorChar(cube.getColor(RubiksCube::FACE::DOWN, 1, j-2));
            cout << color << " ";
        } else {
            cout << "  ";
        }
    }
    cout << endl;
    
    cout << "    ";
    for (int j = 0; j < 12; j++) {
        if (j >= 2 && j <= 4) {
            char color = getColorChar(cube.getColor(RubiksCube::FACE::DOWN, 2, j-2));
            cout << color << " ";
        } else {
            cout << "  ";
        }
    }
    cout << endl;
    cout << "=== End of " << title << " ===" << endl;
}

// Function to print moves in a readable format
void printMoves(const vector<RubiksCube::MOVE>& moves, const RubiksCubeBitboard& cube, const string& title = "Moves") {
    cout << title << ": ";
    for (size_t i = 0; i < moves.size(); ++i) {
        cout << (i == 0 ? "" : " ") << cube.getMove(moves[i]);
    }
    cout << endl;
}

int main(int argc, char* argv[]) {
    // Check if file input is provided
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <input_file>" << endl;
        cout << "Input file should contain 6 bitboards (space-separated)" << endl;
        return 1;
    }
    
    // Database file path - adjust this path to match your system
    string fileName = "Databases/cornerDepth5V1.txt";
    
    cout << "=== Rubik's Cube Solver - IDA* Algorithm ===" << endl;
    cout << "=============================================" << endl;
    
    // Step 1: Read input from file
    cout << "\n[STEP 1] Reading cube state from file..." << endl;
    ifstream fin(argv[1]);
    if (!fin.is_open()) {
        cout << "ERROR: Cannot open input file: " << argv[1] << endl;
        return 1;
    }
    
    vector<uint64_t> boards(6);
    for (int i = 0; i < 6; i++) {
        string val_str;
        fin >> val_str;
        boards[i] = stoull(val_str); // Use stoull for unsigned long long
    }
    fin.close();
    
    cout << "Received bitboards:" << endl;
    for (int i = 0; i < 6; i++) {
        cout << "Face " << i << ": " << boards[i] << " (0x" << hex << boards[i] << dec << ")" << endl;
    }
    
    // Step 2: Create cube from input
    cout << "\n[STEP 2] Creating cube from input state..." << endl;
    RubiksCubeBitboard cube(boards);
    printCubeState(cube, "Input Cube State");
    
    // Step 3: Solve with IDA*
    cout << "\n[STEP 3] Solving with IDA* algorithm..." << endl;
    cout << "This may take a few moments...\n" << endl;
    
    // Create IDA* solver and solve the cube
    IDAstarSolver<RubiksCubeBitboard, HashBitboard> idaStarSolver(cube, fileName);
    auto solutionMoves = idaStarSolver.solve();
    
    if (solutionMoves.empty()) {
        cout << "ERROR: No solution found!" << endl;
        cout << "This could happen if:" << endl;
        cout << "1. The cube state is impossible to achieve" << endl;
        cout << "2. The bitboards are corrupted or incorrectly formatted" << endl;
        cout << "3. The solver reached its limit without finding a solution" << endl;
        return 1;
    }
    
    cout << "SUCCESS: Solution found!" << endl;
    printMoves(solutionMoves, cube, "Solution Moves");
    cout << "Total solution length: " << solutionMoves.size() << " moves" << endl;
    
    // Step 4: Show step-by-step solution
    cout << "\n[STEP 4] Step-by-step solution demonstration:" << endl;
    cout << "=============================================" << endl;
    
    RubiksCubeBitboard solvingCube = cube;
    printCubeState(solvingCube, "Starting State (Input)");
    
    for (int i = 0; i < solutionMoves.size(); i++) {
        cout << "\n[STEP " << (i + 1) << "/" << solutionMoves.size() << "] ";
        cout << "Applying move: " << cube.getMove(solutionMoves[i]) << endl;
        
        solvingCube.move(solutionMoves[i]);
        printCubeState(solvingCube, "After Move " + to_string(i + 1));
    }
    
    // Step 5: Verify final state
    cout << "\n[STEP 5] Verification:" << endl;
    cout << "=====================" << endl;
    
    if (solvingCube.isSolved()) {
        cout << "✓ SUCCESS: Cube is now solved!" << endl;
    } else {
        cout << "✗ ERROR: Cube is not solved!" << endl;
    }
    
    printCubeState(solvingCube, "Final State");
    
    // Summary
    cout << "\n[SUMMARY]" << endl;
    cout << "=========" << endl;
    cout << "Solution moves: ";
    printMoves(solutionMoves, cube, "");
    cout << "Total moves to solve: " << solutionMoves.size() << " moves" << endl;
    
    return 0;
}