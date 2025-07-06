# Rubik's Cube Solver

A C++ implementation of a Rubik's Cube solver using various algorithms including IDA*, BFS, DFS, and IDDFS.

## Features

- Multiple solving algorithms (IDA*, BFS, DFS, IDDFS)
- Pattern database support for corner pieces
- Bitboard representation for efficient state management
- 3D and 1D array representations as alternatives

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

```bash
./rubiks_solver
```

## Project Structure

- `Model/` - Core Rubik's Cube representation classes
- `Solver/` - Different solving algorithms
- `PatternDatabases/` - Pattern database implementation
- `Databases/` - Pre-computed pattern databases