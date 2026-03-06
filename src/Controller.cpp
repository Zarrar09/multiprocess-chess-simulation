#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

/*
MY NOTES:

CONTROLLER:
    1. Create 8x8 board
    2. Fork() both white and black player
    3. Must use unnamed pipes
    4. Must have Controller -> Player and Player -> Controller
    5. Send Current Board state, Validate, Update board
    6. Log this move into the game_log.txt
    7. Print updated board, PID, and move executed

PLAYER:
    1. Player White -> execlp() and Black player -> execv()
    2. Read board from CONTROLLER
    3. IF 'EOF' read, return GAME OVER
    4. Calculate possible moves for piece chosen by user

RULES:
    1. Pawn normal movement: 2 beginning, 1 after that always
    2. king cannot kill king
    3. illegal move, reject and prompted for a move again

BOARD: 
    1. Whole board initially fill with .
    2. then add White players, then black players, then fork()

TERMINATION:
    1. King Captured = GG
    2. 20 total moves like white = 20 and black = 20

*/


void createChessBoard(char board[8][8])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            board[i][j] = '.';
        }

    }

    // Black pieces -> row 0 = first row where all the pieces are
    board[0][0] = 'r'; 
    board[0][7] = 'r';
    
    board[0][1] = 'n'; 
    board[0][6] = 'n';
    
    board[0][2] = 'b';
    board[0][5] = 'b';

    board[0][3] = 'q'; 
    board[0][4] = 'k';
     

    // Black pawns
    for (int j = 0; j < 8; j++)
    {
        board[1][j] = 'p';
    }

    // White pawns 
    for (int j = 0; j < 8; j++)
    {
        board[6][j] = 'P';
    }

    board[7][0] = 'R'; 
    board[7][7] = 'R';
    
    board[7][1] = 'N'; 
    board[7][6] = 'N';
    
    board[7][2] = 'B';
    board[7][5] = 'B';

    board[7][3] = 'Q'; 
    board[7][4] = 'K';
}

void printBoard(char board[8][8])
{
    cout << endl << endl;
    cout << "    " << "A B C D E F G H" << endl;
    cout << "    " << "----------------" << endl;
    cout << endl;
    for (int i = 0; i < 8; i++)
    {
        cout << (8 - i) << "   ";   
        for (int j = 0; j < 8; j++)
        {
            cout << board[i][j] << " ";
        }
        cout << "   " << (8 - i) << "\n";
    }
    cout << endl;
    cout << "    " << "----------------" << endl;;
    cout << "    " << "A B C D E F G H" << endl;
}

int main()
{
    char chessBoard[8][8];

    createChessBoard(chessBoard);
    printBoard(chessBoard);

    return 0;
}