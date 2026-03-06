# Multiprocess Chess Simulation

A simplified chess simulation implemented using a **multi-process architecture in C/C++**, designed to demonstrate core **Operating Systems concepts** such as process creation, inter-process communication (IPC), and controlled synchronization between independent processes.

The system models a chess game using a **Controller–Player architecture**, where separate processes communicate through UNIX pipes while maintaining strict process isolation.

---

## System Architecture

The program consists of three independent processes:

1. **Controller Process**
2. **White Player Process**
3. **Black Player Process**

The Controller acts as the central authority that manages the game state.


### Responsibilities

**Controller**
- Maintains the chess board and overall game state
- Sends board updates to players
- Validates and applies moves
- Logs moves and manages game progression

**Players**
- Receive the board state from the controller
- Evaluate possible moves
- Select a move and send it back to the controller

Players do not have direct access to the board, ensuring clear separation of responsibilities.

---

This project demonstrates several operating system concepts:

- Process creation using `fork()`
- Process replacement using `exec()`
- Inter-process communication (IPC) using UNIX pipes
- Bidirectional communication between processes
- File descriptor management
- Process synchronization through blocking I/O

---

## Compilation

Compile each component using `g++`.

Example:

```bash
g++ src/Controller.cpp -o controller
g++ src/whitePlayer.cpp -o whitePlayer
g++ src/blackPlayer.cpp -o blackPlayer
