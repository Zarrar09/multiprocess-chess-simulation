#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <sys/wait.h>

using namespace std;

char convertCol(int colIndex)
{
    switch (colIndex)
    {
        case (0):
            return 'A';
            break;

        case (1):
            return 'B';
            break;

        case (2):
            return 'C';
            break;

        case (3):
            return 'D';
            break;

        case (4):
            return 'E';
            break;

        case (5):
            return 'F';
            break;

        case (6):
            return 'G';
            break;

        case (7):
            return 'H';
            break;
    
        default:
            cout << "Invalid column" << endl;
            return 'Z';
            break;
    }
}

char convertRow(int rowIndex, int extraRow = 0)
{
    return '0' + (8 - rowIndex + extraRow);
}

int main(int argc, char* argv[])
{
    int read_FD  = atoi(argv[1]); // controller_white[0] - white reads board from here
    int write_FD = atoi(argv[2]); // white_controller[1] - white writes move to here

    srand(time(0));

    cout << "White player (PID " << getpid() << ") started" << endl;

    char chessBoard[8][8];

    char allMoves[200][6];
    char pieceCaptures[100][6];
    char kingCaptures[10][6];

    int currentCount = 0;
    int pieceCaptureCount = 0;
    int kingCaptureCount = 0;

    while(true)
    {
        currentCount = 0;
        pieceCaptureCount = 0;
        kingCaptureCount = 0;

        int bytesRead = read(read_FD, &chessBoard, 64);
        if(bytesRead == 0)
        {
            cout << "White player: Game Over" << endl;
            break;
        }

        for(int i = 0; i < 8; i++)
        {
            for(int j = 0; j < 8; j++)
            {
                // check if current square has a white piece
                if(chessBoard[i][j] >= 'A' && chessBoard[i][j] <= 'Z')
                {
                    int currentRow = i;
                    int currentCol = j;

                    // PAWN
                    if(chessBoard[i][j] == 'P')
                        {
                            // one square forward
                            if(currentRow - 1 >= 0 && chessBoard[currentRow - 1][j] == '.')
                            {
                                char move[6];
                                move[0] = convertCol(j);
                                move[1] = '0' + (8 - currentRow);
                                move[2] = convertCol(j);
                                move[3] = '0' + (8 - currentRow + 1);
                                move[4] = '\n';
                                move[5] = '\0';
                                strcpy(allMoves[currentCount], move);
                                currentCount++;

                                // two squares forward from starting row
                                if(currentRow == 6 && chessBoard[currentRow - 2][j] == '.')
                                {
                                    char move2[6];
                                    move2[0] = convertCol(j);
                                    move2[1] = '0' + (8 - currentRow);
                                    move2[2] = convertCol(j);
                                    move2[3] = '0' + (8 - currentRow + 2);
                                    move2[4] = '\n';
                                    move2[5] = '\0';
                                    strcpy(allMoves[currentCount], move2);
                                    currentCount++;
                                }
                            }

                            // left diagonal capture
                            if(currentRow - 1 >= 0 && currentCol - 1 >= 0 && chessBoard[currentRow - 1][currentCol - 1] >= 'a' && chessBoard[currentRow - 1][currentCol - 1] <= 'z')
                            {
                                char move[6];
                                move[0] = convertCol(currentCol);
                                move[1] = '0' + (8 - currentRow);
                                move[2] = convertCol(currentCol - 1);
                                move[3] = '0' + (8 - currentRow + 1);
                                move[4] = '\n';
                                move[5] = '\0';
                                if(chessBoard[currentRow - 1][currentCol - 1] == 'k')
                                {
                                    strcpy(kingCaptures[kingCaptureCount], move);
                                    kingCaptureCount++;
                                }
                                else
                                {
                                    strcpy(pieceCaptures[pieceCaptureCount], move);
                                    pieceCaptureCount++;
                                }
                            }

                            // right diagonal capture
                            if(currentRow - 1 >= 0 && currentCol + 1 < 8 && chessBoard[currentRow - 1][currentCol + 1] >= 'a' && chessBoard[currentRow - 1][currentCol + 1] <= 'z')
                            {
                                char move[6];
                                move[0] = convertCol(currentCol);
                                move[1] = '0' + (8 - currentRow);
                                move[2] = convertCol(currentCol + 1);
                                move[3] = '0' + (8 - currentRow + 1);
                                move[4] = '\n';
                                move[5] = '\0';
                                if(chessBoard[currentRow - 1][currentCol + 1] == 'k')
                                {
                                    strcpy(kingCaptures[kingCaptureCount], move);
                                    kingCaptureCount++;
                                }
                                else
                                {
                                    strcpy(pieceCaptures[pieceCaptureCount], move);
                                    pieceCaptureCount++;
                                }
                            }
                        }
                
                    // ROOK 
                    if(chessBoard[i][j] == 'R')
                    {
                        int directions[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};

                        for(int d = 0; d < 4; d++)
                        {
                            int nextRow = currentRow + directions[d][0];
                            int nextCol = currentCol + directions[d][1];

                            while(nextRow >= 0 && nextRow < 8 && nextCol >= 0 && nextCol < 8)
                            {
                                // blocked by own piece
                                if(chessBoard[nextRow][nextCol] >= 'A' && chessBoard[nextRow][nextCol] <= 'Z')
                                {
                                    break;
                                }

                                char move[6];
                                move[0] = convertCol(currentCol);
                                move[1] = '0' + (8 - currentRow);
                                move[2] = convertCol(nextCol);
                                move[3] = '0' + (8 - nextRow);
                                move[4] = '\n';
                                move[5] = '\0';

                                if(chessBoard[nextRow][nextCol] == 'k')
                                {
                                    strcpy(kingCaptures[kingCaptureCount], move);
                                    kingCaptureCount++;
                                }
                                else if(chessBoard[nextRow][nextCol] >= 'a' && chessBoard[nextRow][nextCol] <= 'z')
                                {
                                    strcpy(pieceCaptures[pieceCaptureCount], move);
                                    pieceCaptureCount++;
                                }
                                else
                                {
                                    strcpy(allMoves[currentCount], move);
                                    currentCount++;
                                }

                                // stop if path addition if enemy piece
                                if(chessBoard[nextRow][nextCol] >= 'a' && chessBoard[nextRow][nextCol] <= 'z')
                                {
                                    break;
                                }

                                nextRow += directions[d][0];
                                nextCol += directions[d][1];
                            }
                        }
                    }
                
                    // BISHOP 
                    if(chessBoard[i][j] == 'B')
                    {
                        int diagonalDirection[4][2] = {{-1,-1},{-1,1},{1,-1},{1,1}};

                        for(int d = 0; d < 4; d++)
                        {
                            int nextRow = currentRow + diagonalDirection[d][0];
                            int nextCol = currentCol + diagonalDirection[d][1];

                            while(nextRow >= 0 && nextRow < 8 && nextCol >= 0 && nextCol < 8)
                            {
                                // blocked by own piece
                                if(chessBoard[nextRow][nextCol] >= 'A' && chessBoard[nextRow][nextCol] <= 'Z')
                                {
                                    break;
                                }

                                char move[6];
                                move[0] = convertCol(currentCol);
                                move[1] = '0' + (8 - currentRow);
                                move[2] = convertCol(nextCol);
                                move[3] = '0' + (8 - nextRow);
                                move[4] = '\n';
                                move[5] = '\0';

                                if(chessBoard[nextRow][nextCol] == 'k')
                                {
                                    strcpy(kingCaptures[kingCaptureCount], move);
                                    kingCaptureCount++;
                                }
                                else if(chessBoard[nextRow][nextCol] >= 'a' && chessBoard[nextRow][nextCol] <= 'z')
                                {
                                    strcpy(pieceCaptures[pieceCaptureCount], move);
                                    pieceCaptureCount++;
                                }
                                else
                                {
                                    strcpy(allMoves[currentCount], move);
                                    currentCount++;
                                }

                                if(chessBoard[nextRow][nextCol] >= 'a' && chessBoard[nextRow][nextCol] <= 'z')
                                {
                                    break;
                                }

                                nextRow += diagonalDirection[d][0];
                                nextCol += diagonalDirection[d][1];
                            }
                        }
                    }
                
                    // KNIGHT 
                    if(chessBoard[i][j] == 'N')
                    {
                        int lJumps[8][2] = {{-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1}};

                        for(int k = 0; k < 8; k++)
                        {
                            int finalRow = currentRow + lJumps[k][0];
                            int finalCol = currentCol + lJumps[k][1];

                            // check to see if jump lands outside the board
                            if(finalRow < 0 || finalRow >= 8 || finalCol < 0 || finalCol >= 8)
                            {
                                continue;
                            }

                            // check to see if there is already a white piece on final destination
                            if(chessBoard[finalRow][finalCol] >= 'A' && chessBoard[finalRow][finalCol] <= 'Z')
                            {
                                continue;
                            }

                            char move[6];
                            move[0] = convertCol(currentCol);
                            move[1] = '0' + (8 - currentRow);
                            move[2] = convertCol(finalCol);
                            move[3] = '0' + (8 - finalRow);
                            move[4] = '\n';
                            move[5] = '\0';

                            if(chessBoard[finalRow][finalCol] == 'k')
                            {
                                strcpy(kingCaptures[kingCaptureCount], move);
                                kingCaptureCount++;
                            }
                            else if(chessBoard[finalRow][finalCol] >= 'a' && chessBoard[finalRow][finalCol] <= 'z')
                            {
                                strcpy(pieceCaptures[pieceCaptureCount], move);
                                pieceCaptureCount++;
                            }
                            else
                            {
                                strcpy(allMoves[currentCount], move);
                                currentCount++;
                            }
                        }
                    }
                
                    // QUEEN 
                    if(chessBoard[i][j] == 'Q')
                    {
                        int allDirections[8][2] = {{-1,0},{1,0},{0,-1},{0,1},{-1,-1},{-1,1},{1,-1},{1,1}};

                        for(int d = 0; d < 8; d++)
                        {
                            int nextRow = currentRow + allDirections[d][0];
                            int nextCol = currentCol + allDirections[d][1];

                            while(nextRow >= 0 && nextRow < 8 && nextCol >= 0 && nextCol < 8)
                            {
                                // white piece check
                                if(chessBoard[nextRow][nextCol] >= 'A' && chessBoard[nextRow][nextCol] <= 'Z')
                                {
                                    break;
                                }

                                char move[6];
                                move[0] = convertCol(currentCol);
                                move[1] = '0' + (8 - currentRow);
                                move[2] = convertCol(nextCol);
                                move[3] = '0' + (8 - nextRow);
                                move[4] = '\n';
                                move[5] = '\0';

                                if(chessBoard[nextRow][nextCol] == 'k')
                                {
                                    strcpy(kingCaptures[kingCaptureCount], move);
                                    kingCaptureCount++;
                                }
                                else if(chessBoard[nextRow][nextCol] >= 'a' && chessBoard[nextRow][nextCol] <= 'z')
                                {
                                    strcpy(pieceCaptures[pieceCaptureCount], move);
                                    pieceCaptureCount++;
                                }
                                else
                                {
                                    strcpy(allMoves[currentCount], move);
                                    currentCount++;
                                }

                                if(chessBoard[nextRow][nextCol] >= 'a' && chessBoard[nextRow][nextCol] <= 'z')
                                {
                                    break;
                                }

                                nextRow += allDirections[d][0];
                                nextCol += allDirections[d][1];
                            }
                        }
                    }

                    // KING 
                    if(chessBoard[i][j] == 'K')
                    {
                        int allDirections[8][2] = {{-1,0},{1,0},{0,-1},{0,1},{-1,-1},{-1,1},{1,-1},{1,1}};

                        for(int d = 0; d < 8; d++)
                        {
                            int nextRow = currentRow + allDirections[d][0];
                            int nextCol = currentCol + allDirections[d][1];

                            if(nextRow < 0 || nextRow >= 8 || nextCol < 0 || nextCol >= 8)
                            {
                                continue;
                            }

                            if(chessBoard[nextRow][nextCol] >= 'A' && chessBoard[nextRow][nextCol] <= 'Z')
                            {
                                continue;
                            }

                            char move[6];
                            move[0] = convertCol(currentCol);
                            move[1] = '0' + (8 - currentRow);
                            move[2] = convertCol(nextCol);
                            move[3] = '0' + (8 - nextRow);
                            move[4] = '\n';
                            move[5] = '\0';

                            if(chessBoard[nextRow][nextCol] == 'k')
                            {
                                strcpy(kingCaptures[kingCaptureCount], move);
                                kingCaptureCount++;
                            }
                            else if(chessBoard[nextRow][nextCol] >= 'a' && chessBoard[nextRow][nextCol] <= 'z')
                            {
                                strcpy(pieceCaptures[pieceCaptureCount], move);
                                pieceCaptureCount++;
                            }
                            else
                            {
                                strcpy(allMoves[currentCount], move);
                                currentCount++;
                            }
                        }
                    }
                }
            }
        }

        /*
            LOGIC: Instead of moving the pieces randomly I decided to create like a set of priority moves
            so I created 3 arrays:
                1. The first array is all the moves that can take the king
                2. the second is just all moves that can take the piece of the opponent
                3. lastly is the array that has all the moves that are possible and we randomly pick from it
                4. if no moves possible then we just display 
        */
        char chosenMove[6];

        if(kingCaptureCount > 0)
        {
            int index = rand() % kingCaptureCount; 
            strcpy(chosenMove, kingCaptures[index]);
        }
        else if(pieceCaptureCount > 0)
        {
            int index = rand() % pieceCaptureCount;
            strcpy(chosenMove, pieceCaptures[index]);
        }
        else if(currentCount > 0)
        {
            int index = rand() % currentCount;
            strcpy(chosenMove, allMoves[index]);
        }
        else
        {
            cout << "White player: no moves available" << endl;
            break;
        }

        write(write_FD, chosenMove, 5);

    }

    close(read_FD);
    close(write_FD);
    return 0;
}