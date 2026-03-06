#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
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
    2. 20 total moves then draw

EXECV vs EXECLP:
    1.  EXECLP: You list the arguments one by one and MUST always TERMINATE with last param = NULL
    2.  EXECV : You must send parameters through an array

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

    int white_controller[2];
    int controller_white[2];

    int black_controller[2];
    int controller_black[2];

    if((pipe(white_controller) == -1) || (pipe(controller_white) == -1) || (pipe(black_controller) == -1) || (pipe(controller_black) == -1))
    {
        cout << "Error creating pipe" << endl;
        return -1;
    }

    int pid = fork();

    if(pid == -1)
    {
        cout << "Error forking Controller 1st time" << endl;
        return -1;
    }

    if(pid == 0) // white player
    {
        close(black_controller[0]); 
        close(black_controller[1]); 
        close(controller_black[0]); 
        close(controller_black[1]); 

        close(white_controller[0]); // WHITE WRITES TO CONTROLLER
        close(controller_white[1]); // WHITE READS FROM CONTROLLER

        string read_FD = to_string(controller_white[0]);
        string write_FD = to_string(white_controller[1]);

        if(execlp("./whitePlayer", "whitePlayer", read_FD.c_str(), write_FD.c_str(), NULL) == -1)
        {
            perror("execlp failed");
            exit(1);
        }
    }
    if(pid > 0)
    {
        int pid2 =  fork();

        if(pid2 == -1)
        {
            cout << "Error forking Controller 2nd time" << endl;
            return -1;
        }

        if(pid2 == 0)
        {
            close(white_controller[0]);
            close(white_controller[1]);
            close(controller_white[0]);
            close(controller_white[1]);
    
            close(black_controller[0]);
            close(controller_black[1]);
    
            string read_FD = to_string(controller_black[0]);
            string write_FD = to_string(black_controller[1]);
    
            char* args[] = {(char*)"blackPlayer", (char*)read_FD.c_str(), (char*)write_FD.c_str(), NULL};
            
            if(execv("./blackPlayer", args) == -1)
            {
                perror("execv failed");
                exit(1);
            }
        }
        
        if(pid2 > 0)
        {
            close(controller_white[0]);
            close(controller_black[0]);

            close(white_controller[1]);
            close(black_controller[1]);

            // game loop begins
            write(controller_white[1], &chessBoard, 64);
        }
    }

    return 0;
}