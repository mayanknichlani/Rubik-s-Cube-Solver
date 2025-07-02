#include <bits/stdc++.h>
//#include "Model/RubiksCube3dArray.cpp"
//#include "Model/RubiksCube1dArray.cpp"
#include "Model/RubiksCubeBitboard.h"
#include "Solver/DFSSolver.h"
#include "Solver/BFSSolver.h"
#include "Solver/IDDFSSolver.h"
#include "Solver/IDAstarSolver.h"
//#include "PatternDatabases/CornerPatternDatabase.h"
#include "PatternDatabases/CornerDBMaker.h"

using namespace std;

// Helper function to create cube state from a simpler input format
// Input: 6 strings representing each face (9 characters each)
// Format: "WWWGGGRRR" where each character represents a color
// Colors: W=White, G=Green, R=Red, B=Blue, O=Orange, Y=Yellow
vector<vector<vector<RubiksCube::COLOR>>> createCubeFromStrings(const vector<string>& faceStrings) {
    vector<vector<vector<RubiksCube::COLOR>>> faces(6, vector<vector<RubiksCube::COLOR>>(3, vector<RubiksCube::COLOR>(3)));
    
    for (int face = 0; face < 6; face++) {
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                int idx = row * 3 + col;
                char colorChar = toupper(faceStrings[face][idx]);
                
                switch (colorChar) {
                    case 'W': faces[face][row][col] = RubiksCube::COLOR::WHITE; break;
                    case 'G': faces[face][row][col] = RubiksCube::COLOR::GREEN; break;
                    case 'R': faces[face][row][col] = RubiksCube::COLOR::RED; break;
                    case 'B': faces[face][row][col] = RubiksCube::COLOR::BLUE; break;
                    case 'O': faces[face][row][col] = RubiksCube::COLOR::ORANGE; break;
                    case 'Y': faces[face][row][col] = RubiksCube::COLOR::YELLOW; break;
                    default: faces[face][row][col] = RubiksCube::COLOR::WHITE; break;
                }
            }
        }
    }
    return faces;
}

// Function to get color input from user
RubiksCube::COLOR getColorInput() {
    char colorChar;
    while (true) {
        cout << "Enter color (W=White, G=Green, R=Red, B=Blue, O=Orange, Y=Yellow): ";
        cin >> colorChar;
        
        // Convert to uppercase
        colorChar = toupper(colorChar);
        
        switch (colorChar) {
            case 'W': return RubiksCube::COLOR::WHITE;
            case 'G': return RubiksCube::COLOR::GREEN;
            case 'R': return RubiksCube::COLOR::RED;
            case 'B': return RubiksCube::COLOR::BLUE;
            case 'O': return RubiksCube::COLOR::ORANGE;
            case 'Y': return RubiksCube::COLOR::YELLOW;
            default:
                cout << "Invalid color! Please enter W, G, R, B, O, or Y." << endl;
        }
    }
}

// Helper to print orientation instructions for each face
void printFaceOrientation(int faceIdx) {
    switch(faceIdx) {
        case 0:
            cout << "[UP]  White center on Front, Blue center on Top." << endl;
            break;
        case 1:
            cout << "[LEFT]  Orange center on Front, White center on Top." << endl;
            break;
        case 2:
            cout << "[FRONT]  Green center on Front, White center on Top." << endl;
            break;
        case 3:
            cout << "[RIGHT]  Red center on Front, White center on Top." << endl;
            break;
        case 4:
            cout << "[BACK]  Blue center on Front, White center on Top." << endl;
            break;
        case 5:
            cout << "[DOWN]  Yellow center on Front, Green center on Top." << endl;
            break;
    }
}

// Update inputFace to accept faceIdx and print orientation
vector<vector<RubiksCube::COLOR>> inputFace(const string& faceName, int faceIdx) {
    vector<vector<RubiksCube::COLOR>> face(3, vector<RubiksCube::COLOR>(3));
    
    cout << "\n=== INPUTTING " << faceName << " FACE ===" << endl;
    printFaceOrientation(faceIdx);
    
    char choice;
    cout << "Choose input method:" << endl;
    cout << "1. Enter all 9 colors as one string (e.g., 'WWWGGGRRR')" << endl;
    cout << "2. Enter colors one by one" << endl;
    cout << "Enter choice (1 or 2): ";
    cin >> choice;
    
    if (choice == '1') {
        // String input method
        string faceString;
        cout << "Enter all 9 colors as one string (reading left to right, top to bottom):" << endl;
        cout << " 0 1 2 " << endl;
        cout << " 3 4 5 " << endl;
        cout << " 6 7 8 " << endl;
        cout << "Example: 'WWWGGGRRR' for a face with white top row, green middle, red bottom" << endl;
        
        while (true) {
            cout << "Enter 9 colors: ";
            cin >> faceString;
            
            if (faceString.length() == 9) {
                // Convert string to face array
                for (int row = 0; row < 3; row++) {
                    for (int col = 0; col < 3; col++) {
                        int idx = row * 3 + col;
                        char colorChar = toupper(faceString[idx]);
                        
                        switch (colorChar) {
                            case 'W': face[row][col] = RubiksCube::COLOR::WHITE; break;
                            case 'G': face[row][col] = RubiksCube::COLOR::GREEN; break;
                            case 'R': face[row][col] = RubiksCube::COLOR::RED; break;
                            case 'B': face[row][col] = RubiksCube::COLOR::BLUE; break;
                            case 'O': face[row][col] = RubiksCube::COLOR::ORANGE; break;
                            case 'Y': face[row][col] = RubiksCube::COLOR::YELLOW; break;
                            default:
                                cout << "Invalid character '" << colorChar << "' at position " << idx << endl;
                                cout << "Please use only W, G, R, B, O, Y characters." << endl;
                                goto retry_string;
                        }
                    }
                }
                break; // Success
            } else {
                cout << "String must be exactly 9 characters long!" << endl;
            }
            retry_string:
            continue;
        }
    } else {
        // Individual input method
        cout << "Enter colors for each position (reading left to right, top to bottom):" << endl;
        cout << " 0 1 2 " << endl;
        cout << " 3 4 5 " << endl;
        cout << " 6 7 8 " << endl;
        
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                cout << "Position " << (row * 3 + col) << " (" << row << "," << col << "): ";
                face[row][col] = getColorInput();
            }
        }
    }
    
    return face;
}

int main() {

// CornerDBMaker Testing --------------------------------------------------------------------------
    string fileName = "C:\\Users\\Ashok\\OneDrive\\Desktop\\mayanknichlani\\rubiks-cube-solver-main\\rubiks-cube-solver-main\\CMakeLists.txt";

//    Code to create Corner Database
//    CornerDBMaker dbMaker(fileName, 0x99);
//    dbMaker.bfsAndStore();

    RubiksCubeBitboard cube;
    
    cout << "====================================================================================" << endl;
    cout << "                    RUBIK'S CUBE SOLVER - USER INPUT MODE" << endl;
    cout << "====================================================================================" << endl;
    cout << endl;
    
    cout << "INSTRUCTIONS FOR CUBE ORIENTATION:" << endl;
    cout << "==================================" << endl;
    cout << "1. Hold your cube with WHITE center on TOP" << endl;
    cout << "2. Hold your cube with GREEN center on FRONT (facing you)" << endl;
    cout << "3. This means:" << endl;
    cout << "   - UP face: White center" << endl;
    cout << "   - FRONT face: Green center" << endl;
    cout << "   - LEFT face: Orange center" << endl;
    cout << "   - RIGHT face: Red center" << endl;
    cout << "   - BACK face: Blue center" << endl;
    cout << "   - DOWN face: Yellow center" << endl;
    cout << endl;
    
    cout << "COLOR MAPPING:" << endl;
    cout << "==============" << endl;
    cout << "W = WHITE" << endl;
    cout << "G = GREEN" << endl;
    cout << "R = RED" << endl;
    cout << "B = BLUE" << endl;
    cout << "O = ORANGE" << endl;
    cout << "Y = YELLOW" << endl;
    cout << endl;
    
    cout << "INPUT FORMAT:" << endl;
    cout << "=============" << endl;
    cout << "For each face, you can choose to:" << endl;
    cout << "1. Enter all 9 colors as one string (e.g., 'WWWGGGRRR')" << endl;
    cout << "2. Enter colors one by one" << endl;
    cout << "Reading order: Left to right, top to bottom" << endl;
    // cout << "┌─────────┐" << endl;
    cout << " 0 1 2 " << endl;
    cout << " 3 4 5 " << endl;
    cout << " 6 7 8 " << endl;
    // cout << "└─────────┘" << endl;
    cout << endl;
    
    cout << "Press Enter to start inputting your cube state..." << endl;
    cin.ignore();
    cin.get();
    
    vector<vector<vector<RubiksCube::COLOR>>> faces(6);
    
    // Input each face
    faces[0] = inputFace("UP (White center)", 0);
    faces[1] = inputFace("LEFT (Orange center)", 1);
    faces[2] = inputFace("FRONT (Green center)", 2);
    faces[3] = inputFace("RIGHT (Red center)", 3);
    faces[4] = inputFace("BACK (Blue center)", 4);
    faces[5] = inputFace("DOWN (Yellow center)", 5);
    
    cout << "\n====================================================================================" << endl;
    cout << "                              INPUT COMPLETE!" << endl;
    cout << "====================================================================================" << endl;
    
    // Set the cube to the specified state
    cube.setCubeState(faces);
    
    cout << "\nYour Cube State:" << endl;
    cube.print();
    
    // Check if cube is solved
    if (cube.isSolved()) {
        cout << "Cube is already solved!" << endl;
        return 0;
    }

    cout << "\nSolving the cube..." << endl;
    IDAstarSolver<RubiksCubeBitboard, HashBitboard> idaStarSolver(cube, fileName);
    auto moves = idaStarSolver.solve();

    cout << "\nSolution Moves:" << endl;
    for (auto move: moves) cout << cube.getMove(move) << " ";
    cout << "\n";
    
    cout << "\nFinal Cube State:" << endl;
    idaStarSolver.rubiksCube.print();

    for (int i = 0; i < 6; i++) {
        cout << "bitboard[" << i << "]: " << cube.bitboard[i] << endl;
        // If you can, also print solved_side_config[i]
    }

    return 0;
}