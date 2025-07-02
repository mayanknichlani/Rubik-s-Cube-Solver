//
// Created by Lakshya Mittal on 26-12-2021.
//

#ifndef RUBIKS_CUBE_SOLVER_RUBIKSCUBEBITBOARD_H
#define RUBIKS_CUBE_SOLVER_RUBIKSCUBEBITBOARD_H

#include "RubiksCube.h"
#include <string>

class RubiksCubeBitboard : public RubiksCube {

private:
    uint64_t solved_side_config[6]{};

    int arr[3][3] = {{0, 1, 2},
                     {7, 8, 3},
                     {6, 5, 4}};

    uint64_t one_8 = (1 << 8) - 1, one_24 = (1 << 24) - 1;

    void rotateFace(int ind);
    void rotateSide(int s1, int s1_1, int s1_2, int s1_3, int s2, int s2_1, int s2_2, int s2_3);
    int get5bitCorner(std::string corner);

public:
    uint64_t bitboard[6]{};

    RubiksCubeBitboard();

    RubiksCube::COLOR getColor(RubiksCube::FACE face, unsigned row, unsigned col) const override;
    bool isSolved() const override;

    RubiksCube &u() override;
    RubiksCube &uPrime() override;
    RubiksCube &u2() override;

    RubiksCube &l() override;
    RubiksCube &lPrime() override;
    RubiksCube &l2() override;

    RubiksCube &f() override;
    RubiksCube &fPrime() override;
    RubiksCube &f2() override;

    RubiksCube &r() override;
    RubiksCube &rPrime() override;
    RubiksCube &r2() override;

    RubiksCube &b() override;
    RubiksCube &bPrime() override;
    RubiksCube &b2() override;

    RubiksCube &d() override;
    RubiksCube &dPrime() override;
    RubiksCube &d2() override;

    void print() const;
    std::vector<MOVE> randomShuffleCube(unsigned int times);

    // Method to set up a specific cube state
    // faces: 6x3x3 array where faces[face][row][col] represents the color
    // face order: UP, LEFT, FRONT, RIGHT, BACK, DOWN
    void setCubeState(const std::vector<std::vector<std::vector<RubiksCube::COLOR>>>& faces);

    bool operator==(const RubiksCubeBitboard &r1) const;
    RubiksCubeBitboard &operator=(const RubiksCubeBitboard &r1);

    uint64_t getCorners();
};

struct HashBitboard {
    size_t operator()(const RubiksCubeBitboard &r1) const;
};

#endif //RUBIKS_CUBE_SOLVER_RUBIKSCUBEBITBOARD_H 