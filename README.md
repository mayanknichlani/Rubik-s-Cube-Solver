# Rubik’s Cube Optimal Solver – Pattern Database Heuristics

This project implements a fast and optimal solver for the 3×3 Rubik’s Cube using IDA\* (Iterative Deepening A\*) with pattern database heuristics. The solver is written in C++ and includes a Flask web interface for visualizing input states and move sequences.

## 🔍 Overview

- ⚙️ **Algorithm**: IDA\* with precomputed pattern databases for edge/corner state estimation
- 🧠 **Heuristics**: Admissible, memory-based pattern DBs simulating ML-like cost models
- 💻 **Frontend**: Web UI for input (2D net) → solution (move sequence + benchmark)
- 🚀 **Performance**: Solves 6-move scrambles in < 0.5 seconds (average)

---

## 🧩 Features

- Optimal solving via IDA\* search with heuristic pruning
- Precomputed lookup tables for edge and corner state estimation
- Symmetry reduction and move pruning to reduce search space
- Flask web app for:
  - Uploading scrambled 2D cube configurations
  - Visualizing optimal solutions step-by-step
  - Displaying timing stats and move counts

---

## 🗂️ Repository Structure

