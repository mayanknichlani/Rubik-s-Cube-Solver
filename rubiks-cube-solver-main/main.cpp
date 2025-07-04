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

// Helper to print detailed orientation instructions for each face
void printFaceOrientation(const string& faceName, int faceIdx) {
    cout << "\n" << string(60, '=') << endl;
    cout << "FACE: " << faceName << endl;
    cout << string(60, '=') << endl;
    
    switch(faceIdx) {
        case 0: // UP
            cout << "ORIENTATION: Hold cube with WHITE center on TOP, GREEN center facing you" << endl;
            cout << "You are looking DOWN at the TOP face" << endl;
            cout << "The GREEN (front) edge should be at the BOTTOM of this face view" << endl;
            break;
        case 1: // LEFT
            cout << "ORIENTATION: Hold cube with WHITE center on TOP, GREEN center facing you" << endl;
            cout << "Turn your head LEFT to look at the LEFT face" << endl;
            cout << "The WHITE (top) edge should be at the TOP of this face view" << endl;
            cout << "The GREEN (front) edge should be at the RIGHT of this face view" << endl;
            break;
        case 2: // FRONT
            cout << "ORIENTATION: Hold cube with WHITE center on TOP, GREEN center facing you" << endl;
            cout << "You are looking directly at the FRONT face" << endl;
            cout << "The WHITE (top) edge should be at the TOP of this face view" << endl;
            break;
        case 3: // RIGHT
            cout << "ORIENTATION: Hold cube with WHITE center on TOP, GREEN center facing you" << endl;
            cout << "Turn your head RIGHT to look at the RIGHT face" << endl;
            cout << "The WHITE (top) edge should be at the TOP of this face view" << endl;
            cout << "The GREEN (front) edge should be at the LEFT of this face view" << endl;
            break;
        case 4: // BACK
            cout << "ORIENTATION: Hold cube with WHITE center on TOP, GREEN center facing you" << endl;
            cout << "Turn the cube around to look at the BACK face (or turn your head around)" << endl;
            cout << "The WHITE (top) edge should be at the TOP of this face view" << endl;
            break;
        case 5: // DOWN
            cout << "ORIENTATION: Hold cube with WHITE center on TOP, GREEN center facing you" << endl;
            cout << "You are looking UP at the BOTTOM face" << endl;
            cout << "The GREEN (front) edge should be at the TOP of this face view" << endl;
            break;
    }
    cout << endl;
}

char choice;

// Updated inputFace function with better instructions
vector<vector<RubiksCube::COLOR>> inputFace(const string& faceName, int faceIdx) {
    vector<vector<RubiksCube::COLOR>> face(3, vector<RubiksCube::COLOR>(3));
    
    printFaceOrientation(faceName, faceIdx);
    
    if (choice == '1') {
        // String input method
        string faceString;
        cout << "Enter all 9 colors as one string (reading left to right, top to bottom):" << endl;
        cout << "Grid positions:" << endl;
        cout << " 0 1 2 " << endl;
        cout << " 3 4 5 " << endl;
        cout << " 6 7 8 " << endl;
        cout << "Example: 'WWWGGGRRR' = white top row, green middle row, red bottom row" << endl;
        
        while (true) {
            cout << "\nEnter 9 colors: ";
            cin >> faceString;
            
            if (faceString.length() == 9) {
                bool validInput = true;
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
                                validInput = false;
                                break;
                        }
                    }
                    if (!validInput) break;
                }
                if (validInput) break; // Success
            } else {
                cout << "String must be exactly 9 characters long!" << endl;
            }
        }
    } else {
        // Individual input method
        cout << "Enter colors for each position (reading left to right, top to bottom):" << endl;
        cout << "Grid positions:" << endl;
        cout << " 0 1 2 " << endl;
        cout << " 3 4 5 " << endl;
        cout << " 6 7 8 " << endl;
        
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                cout << "Position " << (row * 3 + col) << " (row " << row << ", col " << col << "): ";
                face[row][col] = getColorInput();
            }
        }
    }
    
    // Display the entered face for verification
    cout << "\nYou entered this face pattern:" << endl;
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            switch (face[row][col]) {
                case RubiksCube::COLOR::WHITE: cout << "W "; break;
                case RubiksCube::COLOR::GREEN: cout << "G "; break;
                case RubiksCube::COLOR::RED: cout << "R "; break;
                case RubiksCube::COLOR::BLUE: cout << "B "; break;
                case RubiksCube::COLOR::ORANGE: cout << "O "; break;
                case RubiksCube::COLOR::YELLOW: cout << "Y "; break;
            }
        }
        cout << endl;
    }
    
    // Ask for confirmation
    char confirm;
    cout << "Is this correct? (y/n): ";
    cin >> confirm;
    if (toupper(confirm) != 'Y') {
        cout << "Let's re-enter this face..." << endl;
        return inputFace(faceName, faceIdx); // Recursive call to re-enter
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
    
    cout << "CUBE SETUP INSTRUCTIONS:" << endl;
    cout << "========================" << endl;
    cout << "Before starting, please arrange your cube in the STANDARD ORIENTATION:" << endl;
    cout << "1. Hold your cube with the WHITE center on TOP" << endl;
    cout << "2. Hold your cube with the GREEN center on the FRONT (facing you)" << endl;
    cout << endl;
    cout << "This means your cube should have:" << endl;
    cout << "   - TOP face (UP): White center" << endl;
    cout << "   - FRONT face: Green center" << endl;
    cout << "   - LEFT face: Orange center" << endl;
    cout << "   - RIGHT face: Red center" << endl;
    cout << "   - BACK face: Blue center" << endl;
    cout << "   - BOTTOM face (DOWN): Yellow center" << endl;
    cout << endl;
    
    cout << "COLOR CODES:" << endl;
    cout << "============" << endl;
    cout << "W = WHITE   G = GREEN    R = RED" << endl;
    cout << "B = BLUE    O = ORANGE   Y = YELLOW" << endl;
    cout << endl;
    
    cout << "INPUT METHODS:" << endl;
    cout << "==============" << endl;
    cout << "You can choose to input each face by:" << endl;
    cout << "1. Typing all 9 colors as one string (e.g., 'WWWGGGRRR')" << endl;
    cout << "2. Entering each color individually" << endl;
    cout << endl;
    cout << "For both methods, read the colors from left to right, top to bottom:" << endl;
    cout << " 0 1 2 " << endl;
    cout << " 3 4 5 " << endl;
    cout << " 6 7 8 " << endl;
    cout << endl;
    
    cout << "IMPORTANT: Make sure your cube is in the standard orientation before starting!" << endl;
    cout << "Press Enter when ready..." << endl;
    cin.ignore();
    cin.get();
    
    // Choose input method
    cout << "Choose input method:" << endl;
    cout << "1. Enter all 9 colors as one string per face" << endl;
    cout << "2. Enter colors one by one" << endl;
    cout << "Enter choice (1 or 2): ";
    cin >> choice;
    
    // Create faces array in the correct order for setCubeState
    // setCubeState expects: UP, LEFT, FRONT, RIGHT, BACK, DOWN
    vector<vector<vector<RubiksCube::COLOR>>> faces(6);
    
    // Input faces in the order expected by setCubeState
    faces[0] = inputFace("UP (White center)", 0);        // UP
    faces[1] = inputFace("LEFT (Orange center)", 1);     // LEFT  
    faces[2] = inputFace("FRONT (Green center)", 2);     // FRONT
    faces[3] = inputFace("RIGHT (Red center)", 3);       // RIGHT
    faces[4] = inputFace("BACK (Blue center)", 4);       // BACK
    faces[5] = inputFace("DOWN (Yellow center)", 5);     // DOWN
    
    cout << "\n====================================================================================" << endl;
    cout << "                              INPUT COMPLETE!" << endl;
    cout << "====================================================================================" << endl;
    
    // Set the cube to the
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