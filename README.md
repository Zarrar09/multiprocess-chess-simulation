# multiprocess-chess-simulation
The program has a Controller process and two independent Player processes representing White and Black. The Controller maintains the entire game state, including the chessboard, move validation, and game progression. Player don't have direct access to the board; instead, they interact with the Controller exclusively through UNIX pipes.
