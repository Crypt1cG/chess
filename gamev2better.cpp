#include <array>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <forward_list>

// #define getColor(x) ((x & 0b0000001000) >> 3)
// #define getID(x) (x & 0b0000000111)

/* new version adds 2 numbers to board (xy of each king) used to improve speed
of ischeck function making the ai faster
*/
std::array<std::array<int, 4>, 64> diagonalMoving;

class Game
{
public:
    std::array<int, 69> board; // format: 10 bit number- xxxyyyc_id | first 64 are pieces, 65 is castlingPerms, 66 is enPassantFiles, 67 is currturn
    // int* castlingPerms; // 4 bits - first is white queen side, 2nd white king side, 3rd black queen side, 4th black king side
    // int* enPassantTargets; // 8 bits - one for each file
    int currSelectedPiece; // used for drawing and shit
    int* currTurn; // 0 for white, 1 for black
    int wInCheck;
    int bInCheck;
    std::vector<std::array<int, 2>> currSelectedPossibleCoords;

    static const int pawnID = 1;
    static const int knightID = 2;
    static const int bishopID = 3;
    static const int rookID = 4;
    static const int queenID = 5;
    static const int kingID = 6;
    static const int whiteID = 0;
    static const int blackID = 1;
    static const std::array<std::vector<int>, 64> knightMoves; // {-15, -17, -6, 10, -10, 6, 15, 17}
    static const std::array<std::vector<int>, 64> kingMoves;
    static const std::array<std::array<std::vector<int>, 4>, 64> diagMoves;
    Game()
    {
        // can clone board by just using =
        board = FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -");
        initDiag();
        // board = {{makePiece(0, 0, blackID, rookID), makePiece(1, 0, blackID, knightID), makePiece(2, 0, blackID, bishopID), makePiece(3, 0, blackID, queenID), makePiece(4, 0, blackID, kingID), makePiece(5, 0, blackID, bishopID), makePiece(6, 0, blackID, knightID), makePiece(7, 0, blackID, rookID),
        //           makePiece(0, 1, blackID, pawnID), makePiece(1, 1, blackID, pawnID),   makePiece(2, 1, blackID, pawnID),   makePiece(3, 1, blackID, pawnID),  makePiece(4, 1, blackID, pawnID), makePiece(5, 1, blackID, pawnID),   makePiece(6, 1, blackID, pawnID),   makePiece(7, 1, blackID, pawnID),
        //           0,                                0,                                  0,                                  0,                                 0,                                0,                                  0,                                  0,
        //           0,                                0,                                  0,                                  0,                                 0,                                0,                                  0,                                  0,
        //           0,                                0,                                  0,                                  0,                                 0,                                0,                                  0,                                  0,
        //           0,                                0,                                  0,                                  0,                                 0,                                0,                                  0,                                  0,
        //           makePiece(0, 6, whiteID, pawnID), makePiece(1, 6, whiteID, pawnID),   makePiece(2, 6, whiteID, pawnID),   makePiece(3, 6, whiteID, pawnID),  makePiece(4, 6, whiteID, pawnID), makePiece(5, 6, whiteID, pawnID),   makePiece(6, 6, whiteID, pawnID),   makePiece(7, 6, whiteID, pawnID),
        //           makePiece(0, 7, whiteID, rookID), makePiece(1, 7, whiteID, knightID), makePiece(2, 7, whiteID, bishopID), makePiece(3, 7, whiteID, queenID), makePiece(4, 7, whiteID, kingID), makePiece(5, 7, whiteID, bishopID), makePiece(6, 7, whiteID, knightID), makePiece(7, 7, whiteID, rookID),
        //           0b1111, // castling perms
        //           0, // en passant targets
        //           0, // curr turn
        //           0b100111, // white king xxxyyy
        //           0b100000}}; // black king xxxyyy

        // castlingPerms = &board[64];
        // enPassantTargets = &board[65];
        currTurn = &board[66];
        currSelectedPiece = -1;
        wInCheck = 0;
        bInCheck = 0;
    }

    /**
     * gets the x coord of a piece
     * 
     * @param p an int representing a piece
     * @return the x coord of the piece (number 0-7)
     */
    static int getX(int& p) {return p >> 7;} // return (p & 0b1110000000) >> 7;

    /**
     * gets the y coord of a piece
     * 
     * @param p an int representing a piece
     * @return the y coord of the piece (number 0-7)
     */
    static int getY(int& p) {return (p & 0b0001110000) >> 4;}

    /**
     * gets the color of a piece
     * 
     * @param p an int representing a piece
     * @return the color of the piece (0 = white, 1 = black)
     */
    static int getColor(int& p) {return (p & 0b0000001000) >> 3;}

    /**
     * gets the id of a piece
     * 
     * @param p an int representing a piece
     * @return the id of the piece (0 = blank, 1 = pawn, 2 = knight, 3 = bishop, 4 = rook, 5 = queen, 6 = king)
     */
    static int getID(int& p) {return p & 0b0000000111;}

    /**
     * sets the x coord of a piece
     * 
     * @param p an int representing the piece to be changed
     * @param x the new x value to be given
     */
    static void setX(int& p, int x) {p = (p & 0b0001111111) + (x << 7);}

    /**
     * sets the y coord of a piece
     * 
     * @param p an int representing the piece to be changed
     * @param y the new y value to be given
     */
    static void setY(int& p, int y) {p = (p & 0b1110001111) + (y << 4);}

    /**
     * turns parameters into an int representing a piece
     * 
     * @param x the x coord of the piece (0-7)
     * @param y the y coord of the piece (0-7)
     * @param color the color of the piece (0 or 1)
     * @param id the id of the piece (1-6)
     * @return the piece
     */
    static int makePiece(int x, int y, int color, int id) {return (x << 7) + (y << 4) + (color << 3) + id;}

    /**
     * used to tell if/how a side can castle
     * 
     * @param side a string ("q" or "k") for the desired side to be checked
     * @param color 1 for white, 0 for black
     * @return a bool indicating if given color can castle on given side
     */
    static bool canCastle(std::string side, int color, int castlingPerms)
    {
        if (color == whiteID)
        {
            if (side == "q")
                return castlingPerms >> 3; // used to be (castlingPerms & 0b1000) >> 3;
            else
                return (castlingPerms & 0b0100) >> 2;
        }
        else
        {
            if (side == "q")
                return (castlingPerms & 0b0010) >> 1;
            else
                return castlingPerms & 0b0001;
        }
    }

    /**
     * gets all possible (legal) moves for a piece
     * 
     * @param x the x coord of the piece
     * @param y the y coord of the piece
     * @return a vector of int arrays [0] is x, [1] is y
     */
    static std::vector<int> getMoves(std::array<int, 69>& b, int x, int y, bool allMoves=false)
    {
        int pos = y * 8 + x;
        int piece = b[pos];
        int id = getID(piece);
        int color = getColor(piece);
        std::vector<int> results;
        switch (id)
        {
        case pawnID:
        {
            results.reserve(4); // max 4 pawn moves
            int dir; // could probably make +-8
            if (color == whiteID) dir = -1;
            else dir = 1;
            if (y + dir >= 1 && y + dir < 7) // use 1 and 7 bc last rank promotes
            {
                if (b[pos + dir * 8] == 0)
                {
                    results.push_back(pos + dir * 8);
                    if ((color == whiteID && y == 6) || (color == blackID && y == 1))
                    {
                        if (b[pos + 16 * dir] == 0)
                            results.push_back(pos + 16 * dir);
                    }
                }
                if (x - 1 >= 0 && b[pos + dir * 8 - 1] != 0)
                {
                    if (getColor(b[pos + dir * 8 - 1]) != color)
                        results.push_back(pos + dir * 8 - 1);
                }
                if (x + 1 < 8 && b[pos + dir * 8 + 1] != 0)
                {
                    if (getColor(b[pos + dir * 8 + 1]) != color)
                        results.push_back(pos + dir * 8 + 1);
                }
                // en passant
                if ((color == whiteID && y == 3) || (color == blackID && y == 4))
                {
                    if (canBeEnPassant(b, x + 1, y, b[65]))
                        results.push_back(pos + dir * 8 + 1);
                    if (canBeEnPassant(b, x - 1, y, b[65]))
                        results.push_back(pos + dir * 8 - 1);
                }
            }
            else if (y + dir == 0 || y + dir == 7) // last rank, promotion time
            {
                if (b[pos + dir * 8] == 0)
                {
                    results.push_back(pos + 16 * dir); // queen promotion, should be y=-1 or y=8
                    results.push_back(pos + 24 * dir); // knight promotion
                    results.push_back(pos + 32 * dir); // bishop
                    results.push_back(pos + 40 * dir); // rook


                }
                if (x + 1 < 8 && b[(y + dir) * 8 + x + 1] != 0 && getColor(b[(y + dir) * 8 + x + 1]) != color)
                {
                    results.push_back(pos + 1 + 16 * dir); // queen promotion, should be y=-1 or y=8
                    results.push_back(pos + 1 + 24 * dir); // knight promotion
                    results.push_back(pos + 1 + 32 * dir); // bishop
                    results.push_back(pos + 1 + 40 * dir); // rook
                }
                if (x - 1 >= 0 && b[(y + dir) * 8 + x - 1] != 0 && getColor(b[(y + dir) * 8 + x - 1]) != color)
                {
                    results.push_back(pos - 1 + 16 * dir); // queen promotion, should be y=-1 or y=8
                    results.push_back(pos - 1 + 24 * dir); // knight promotion
                    results.push_back(pos - 1 + 32 * dir); // bishop
                    results.push_back(pos - 1 + 40 * dir); // rook
                }
            }
            break;
        }
        case knightID:
        {
            results.reserve(8); // max 8 knight moves
            std::vector<int> moves = knightMoves[pos];
            for (int offset : moves)
            {
                if ((b[pos + offset] == 0 || getColor(b[pos + offset]) != color))
                    results.push_back(pos + offset);
                    // results.insert(results.end(), {{(pos + offset) % 8, (pos + offset) / 8}});
            }

            // this way is faster (2-3x), i think because it has only one insertion to results vs many push_backs
            // however it seems a little slower when there are no moves (lots of erasing)
            // int i = 0;
            // while (i < moves.size())
            // {
            //     int offset = moves[i];
            //     if (!(b[pos + offset] == 0 || getColor(b[pos + offset]) != color))
            //     {
            //         moves.erase(moves.begin() + i);
            //         i--;
            //     }
            //     i++;
            // }
            // if (moves.size() != 0)
            //     results.insert(results.end(), moves.begin(), moves.end());
                // results = moves;
            break;
        }
        case bishopID:
        {
            // int yInc, xInc, currX, currY;
            // for (int i = 0; i < 4; i++)
            // {
            //     if (i < 2) yInc = -1;
            //     else yInc = 1;
            //     if (i % 2 == 0) // -,- and +,+
            //         xInc = yInc;
            //     else            // -,+ and +,-
            //         xInc = -yInc;
                
            //     currX = x + xInc;
            //     currY = y + yInc;

            //     while (currY >= 0 && currY < 8 && currX >= 0 && currX < 8)
            //     {
            //         if (b[currY * 8 + currX] == 0 || getColor(b[currY * 8 + currX]) != color)
            //             results.push_back({{currX, currY}});
            //         if (b[currY * 8 + currX] != 0)
            //             break;
            //         currY += yInc;
            //         currX += xInc;
            //     }
            // }

            // new bishop stuff
            std::array<int, 4> numMovesDiag = diagonalMoving[pos]; // negneg, posneg, pospos, negpos
            results.reserve(14); // max 14 bishop moves
            // std::vector<int> tmpResults;

            // neg neg (inc is -9)
            int loc = pos;
            for(int i = 0; i < numMovesDiag[0]; i++)
            {
                loc -= 9;
                if (b[loc] == 0 || getColor(b[loc]) != color)
                    results.push_back(loc);
                if (b[loc] != 0)
                    break;
            }

            // pos neg (inc is -7)
            loc = pos;
            for(int i = 0; i < numMovesDiag[1]; i++)
            {
                loc -= 7;
                if (b[loc] == 0 || getColor(b[loc]) != color)
                    results.push_back(loc);
                if (b[loc] != 0)
                    break;
            }

            // pos pos (inc is 9)
            loc = pos;
            for(int i = 0; i < numMovesDiag[2]; i++)
            {
                loc += 9;
                if (b[loc] == 0 || getColor(b[loc]) != color)
                    results.push_back(loc);
                if (b[loc] != 0)
                    break;
            }

            // neg pos (inc is 7)
            loc = pos;
            for(int i = 0; i < numMovesDiag[3]; i++)
            {
                loc += 7;
                if (b[loc] == 0 || getColor(b[loc]) != color)
                    results.push_back(loc);
                if (b[loc] != 0)
                    break;
            }
            // results.insert(results.end(), tmpResults.begin(), tmpResults.end());
            break;
        }
        case rookID:
        {
            // int inc, currY, currX;
            // for (int i = 0; i < 4; i++)
            // {
            //     if (i < 2) inc = -1;
            //     else inc = 1;

            //     if (i % 2 == 0)
            //     {
            //         currY = y + inc;

            //         while (currY >= 0 && currY < 8)
            //         {
            //             if (b[currY * 8 + x] == 0 || getColor(b[currY * 8 + x]) != color)
            //                 results.push_back({{x, currY}});
            //             if (b[currY * 8 + x] != 0)
            //                 break;
            //             currY += inc;
            //         }
            //     }
            //     else
            //     {
            //         currX = x + inc;

            //         while (currX >= 0 && currX < 8)
            //         {
            //             if (b[y * 8 + currX] == 0 || getColor(b[y * 8 + currX]) != color)
            //                 results.push_back({{currX, y}});
            //             if (b[y * 8 + currX] != 0)
            //                 break;
            //             currX += inc;
            //         }
            //     }
            // }

            // std::array<int, 4> incs = {-1, -8, 1, 8};
            results.reserve(15); // max 15 rook moves
            int actualY = y * 8;
            for (int i = x - 1; i >= 0; i--)
            {
                if (b[actualY + i] == 0 || getColor(b[actualY + i]) != color)
                    results.emplace_back(actualY + i);
                if (b[actualY + i] != 0)
                    break;
            }
            for (int i = x + 1; i < 8; i++)
            {
                if (b[actualY + i] == 0 || getColor(b[actualY + i]) != color)
                    results.emplace_back(actualY + i);
                if (b[actualY + i] != 0)
                    break;
            }
            for (int i = y - 1; i >= 0; i--)
            {
                if (b[i * 8 + x] == 0 || getColor(b[i * 8 + x]) != color)
                    results.emplace_back(i * 8 + x);
                if (b[i * 8 + x] != 0)
                    break;
            }
            for (int i = y + 1; i < 8; i++)
            {
                if (b[i * 8 + x] == 0 || getColor(b[i * 8 + x]) != color)
                    results.emplace_back(i * 8 + x);
                if (b[i * 8 + x] != 0)
                    break;
            }
            break;
        }
        case queenID:
        {
            results.reserve(29); // max 29 queen moves (15 rook + 14 bishop)
            // bishop code:
            // new bishop stuff
            std::array<int, 4> numMovesDiag = diagonalMoving[pos]; // negneg, posneg, pospos, negpos

            // neg neg (inc is -9)
            int loc = pos;
            for(int i = 0; i < numMovesDiag[0]; i++)
            {
                loc -= 9;
                if (b[loc] == 0 || getColor(b[loc]) != color)
                    results.push_back(loc);
                if (b[loc] != 0)
                    break;
            }

            // pos neg (inc is -7)
            loc = pos;
            for(int i = 0; i < numMovesDiag[1]; i++)
            {
                loc -= 7;
                if (b[loc] == 0 || getColor(b[loc]) != color)
                    results.push_back(loc);
                if (b[loc] != 0)
                    break;
            }

            // pos pos (inc is 9)
            loc = pos;
            for(int i = 0; i < numMovesDiag[2]; i++)
            {
                loc += 9;
                if (b[loc] == 0 || getColor(b[loc]) != color)
                    results.push_back(loc);
                if (b[loc] != 0)
                    break;
            }

            // neg pos (inc is 7)
            loc = pos;
            for(int i = 0; i < numMovesDiag[3]; i++)
            {
                loc += 7;
                if (b[loc] == 0 || getColor(b[loc]) != color)
                    results.push_back(loc);
                if (b[loc] != 0)
                    break;
            }
            // rook code:
            int actualY = y * 8;
            for (int i = x - 1; i >= 0; i--)
            {
                if (b[actualY + i] == 0 || getColor(b[actualY + i]) != color)
                    results.push_back(actualY + i);
                if (b[actualY + i] != 0)
                    break;
            }
            for (int i = x + 1; i < 8; i++)
            {
                if (b[actualY + i] == 0 || getColor(b[actualY + i]) != color)
                    results.push_back(actualY + i);
                if (b[actualY + i] != 0)
                    break;
            }
            for (int i = y - 1; i >= 0; i--)
            {
                if (b[i * 8 + x] == 0 || getColor(b[i * 8 + x]) != color)
                    results.push_back(i * 8 + x);
                if (b[i * 8 + x] != 0)
                    break;
            }
            for (int i = y + 1; i < 8; i++)
            {
                if (b[i * 8 + x] == 0 || getColor(b[i * 8 + x]) != color)
                    results.push_back(i * 8 + x);
                if (b[i * 8 + x] != 0)
                    break;
            }
            break;
        }
        case kingID:
        {
            results.reserve(10); // max 10 king moves
            std::vector<int> moves = kingMoves[pos];
            for (int offset : kingMoves[pos])
            {   
                if (b[pos + offset] == 0 || getColor(b[pos + offset]) != color)
                    results.push_back(pos + offset);
            }
            
            // this is faster than the for loop (~2x)
            // int i = 0;
            // while (i < moves.size())
            // {
            //     int offset = moves[i];
            //     if (!(b[pos + offset] == 0 || getColor(b[pos + offset]) != color))
            //     {
            //         moves.erase(moves.begin() + i);
            //         i--;
            //     }
            //     i++;
            // }
            // if (moves.size() != 0)
            //     results.insert(results.end(), moves.begin(), moves.end());

            if (!isCheck(b, color) && x == 4)
            {
                std::array<int, 69> tmpBoard;
                if (getID(b[pos + 3]) == rookID && getColor(b[pos + 3]) == color && b[pos + 1] == 0 && b[pos + 2] == 0)
                {
                    tmpBoard = b;
                    movePiece(x, y, x + 1, y, tmpBoard); // check the in-between spot to ensure we're not castling thru check
                    if (canCastle("k", color, b[64]) && !isCheck(tmpBoard, color))
                        results.push_back(pos + 2);
                }
                if (getID(b[pos - 4]) == rookID && getColor(b[pos - 4]) == color && b[pos - 1] == 0 && b[pos - 2] == 0 && b[pos - 3] == 0)
                {
                    tmpBoard = b;
                    movePiece(x, y, x - 1, y, tmpBoard); // check the in-between spot to ensure we're not castling thru check
                    if (canCastle("q", color, b[64]) && !isCheck(tmpBoard, color))
                       results.push_back(pos - 2);
                }
            }
            break;
        }
        }

        if (allMoves)
            return results;

        // remove moves resulting in check
        std::vector<int> finalMoves;
        finalMoves.reserve(results.size());
        // std::vector<std::array<int, 2>> oppMoves = getAllMoves(b, !color, true, true);
        for (int m : results)
        {
            std::array<int, 69> newBoard = b;
            movePiece(pos, m, newBoard);
            if (!isCheck(newBoard, color))
                finalMoves.push_back(m);
        }
        return finalMoves;
    }

    /**
     * checks if a square can be captured by en passant
     * 
     * @param x the x coord of the square to be checked
     * @param y the y coord of the square to be checked
     * @return true if en passant is possible, false otherwise
     */
    static bool canBeEnPassant(std::array<int, 69>& b, int x, int y, int targets)
    {
        if (x >= 0 && x < 8 && y >= 0 && y < 8)
        {
            if (getID(b[y * 8 + x]) == pawnID)
                return targets & ((0b1 << 8) >> x);
        }
        return false;
    }

    /**
     * checks if the player given by color is in check
     * 
     * @param color the side in question
     * @return true if in check, false otherwise
     */
    static bool isCheck(std::array<int, 69>& b, int color)
    {
        int kingX = b[67 + color] >> 3;
        int kingY = b[67 + color] & 0b000111;
        int kingPos = kingY * 8 + kingX;
        int y, x;
        bool valid;
        // for (std::array<int, 2> sq : knightMoves)
        // {
        //     y = kingY + sq[1];
        //     x = kingX + sq[0];
        //     bool valid = y >= 0 && y < 8 && x >= 0 && x < 8;
        //     if (valid && getColor(b[y * 8 + x]) != color && getID(b[y * 8 + x]) == knightID)
        //         return true;
        // }
        // the following code is about 2x as fast as the for loop????
        y = kingY - 2;
        x = kingX + 1;
        valid = y >= 0 && y < 8 && x >= 0 && x < 8;
        if (valid && getColor(b[y * 8 + x]) != color && getID(b[y * 8 + x]) == knightID)
            return true;
        
        y = kingY - 2;
        x = kingX - 1;
        valid = y >= 0 && y < 8 && x >= 0 && x < 8;
        if (valid && getColor(b[y * 8 + x]) != color && getID(b[y * 8 + x]) == knightID)
            return true;
        
        y = kingY + 2;
        x = kingX + 1;
        valid = y >= 0 && y < 8 && x >= 0 && x < 8;
        if (valid && getColor(b[y * 8 + x]) != color && getID(b[y * 8 + x]) == knightID)
            return true;
        
        y = kingY + 2;
        x = kingX - 1;
        valid = y >= 0 && y < 8 && x >= 0 && x < 8;
        if (valid && getColor(b[y * 8 + x]) != color && getID(b[y * 8 + x]) == knightID)
            return true;

        y = kingY + 1;
        x = kingX + 2;
        valid = y >= 0 && y < 8 && x >= 0 && x < 8;
        if (valid && getColor(b[y * 8 + x]) != color && getID(b[y * 8 + x]) == knightID)
            return true;
        
        y = kingY - 1;
        x = kingX + 2;
        valid = y >= 0 && y < 8 && x >= 0 && x < 8;
        if (valid && getColor(b[y * 8 + x]) != color && getID(b[y * 8 + x]) == knightID)
            return true;
        
        y = kingY + 1;
        x = kingX - 2;
        valid = y >= 0 && y < 8 && x >= 0 && x < 8;
        if (valid && getColor(b[y * 8 + x]) != color && getID(b[y * 8 + x]) == knightID)
            return true;

        y = kingY - 1;
        x = kingX - 2;
        valid = y >= 0 && y < 8 && x >= 0 && x < 8;
        if (valid && getColor(b[y * 8 + x]) != color && getID(b[y * 8 + x]) == knightID)
            return true;

        // new rook code
        int actualY = kingY * 8;
        for (int i = kingX - 1; i >= 0; i--)
        {
            if (b[actualY + i] != 0)
            {
                if (getColor(b[actualY + i]) != color && (getID(b[actualY + i]) == rookID || getID(b[actualY + i]) == queenID))
                    return true;
                break;
            }
        }
        for (int i = kingX + 1; i < 8; i++)
        {
            if (b[actualY + i] != 0)
            {
                if (getColor(b[actualY + i]) != color && (getID(b[actualY + i]) == rookID || getID(b[actualY + i]) == queenID))
                    return true;
                break;
            }
        }
        for (int i = kingY - 1; i >= 0; i--)
        {
            if (b[i * 8 + kingX] != 0)
            {
                if (getColor(b[i * 8 + kingX]) != color && (getID(b[i * 8 + kingX]) == rookID || getID(b[i * 8 + kingX]) == queenID))
                    return true;
                break;
            }
        }
        for (int i = kingY + 1; i < 8; i++)
        {
            if (b[i * 8 + kingX] != 0)
            {
                if (getColor(b[i * 8 + kingX]) != color && (getID(b[i * 8 + kingX]) == rookID || getID(b[i * 8 + kingX]) == queenID))
                    return true;
                break;
            }
        }

        //check diagonals (bishops/queens)
        // int yInc, xInc, currX, currY;
        // for (int i = 0; i < 4; i++)
        // {
        //     if (i < 2) yInc = -1;
        //     else yInc = 1;
        //     if (i % 2 == 0) // -,- and +,+
        //         xInc = yInc;
        //     else            // -,+ and +,-
        //         xInc = -yInc;
            
        //     currX = kingX + xInc;
        //     currY = kingY + yInc;

        //     while (currY >= 0 && currY < 8 && currX >= 0 && currX < 8)
        //     {
        //         if (b[currY * 8 + currX] != 0)
        //         {
                    
        //             // std::vector<std::array<int, 2>> moves = getMoves(b, currX, currY, true);
        //             // allMoves.insert(allMoves.end(), moves.begin(), moves.end());
        //             // since we break if there is a piece, we know that if we are here, there is nothing between the king and this piece, meaning it is check
        //             if (getColor(b[currY * 8 + currX]) != color && (getID(b[currY * 8 + currX]) == bishopID || getID(b[currY * 8 + currX]) == queenID))
        //                 return true;
        //             break;
        //         }
        //         // if (b[currY * 8 + currX] != 0)
        //         //     break;
        //         currY += yInc;
        //         currX += xInc;
        //     }
        // }

        // new bishop stuff
        std::array<int, 4> numMovesDiag = diagonalMoving[kingPos]; // negneg, posneg, pospos, negpos

        // neg neg (inc is -9)
        int loc = kingPos;
        for(int i = 0; i < numMovesDiag[0]; i++)
        {
            loc -= 9;
            if (b[loc] != 0)
            {
                if (getColor(b[loc]) != color && (getID(b[loc]) == bishopID || getID(b[loc]) == queenID))
                    return true;
                break;
            }
        }

        // pos neg (inc is -7)
        loc = kingPos;
        for(int i = 0; i < numMovesDiag[1]; i++)
        {
            loc -= 7;
            if (b[loc] != 0)
            {
                if (getColor(b[loc]) != color && (getID(b[loc]) == bishopID || getID(b[loc]) == queenID))
                    return true;
                break;
            }
        }

        // pos pos (inc is 9)
        loc = kingPos;
        for(int i = 0; i < numMovesDiag[2]; i++)
        {
            loc += 9;
            if (b[loc] != 0)
            {
                if (getColor(b[loc]) != color && (getID(b[loc]) == bishopID || getID(b[loc]) == queenID))
                    return true;
                break;
            }
        }

        // neg pos (inc is 7)
        loc = kingPos;
        for(int i = 0; i < numMovesDiag[3]; i++)
        {
            loc += 7;
            if (b[loc] != 0)
            {
                if (getColor(b[loc]) != color && (getID(b[loc]) == bishopID || getID(b[loc]) == queenID))
                    return true;
                break;
            }
        }

        
        // std::array<std::array<int, 2>, 2> pawnSquares;
        // if (color == whiteID)
        //     pawnSquares = {{ {{-1, -1}}, {{1, -1}} }}; // black pawns attack from negative y
        // else
        //     pawnSquares = {{ {{-1, 1}}, {{1, 1}} }}; // white pawns attack from positive y
        

        // for (std::array<int, 2> sq : pawnSquares)
        // {
        //     if ((kingX + sq[0] >= 0 && kingX + sq[0] < 8 && kingY + sq[1] >= 0 && kingY + sq[1] < 8) && getColor(b[(kingY + sq[1]) * 8 + kingX + sq[0]]) != color && getID(b[(kingY + sq[1]) * 8 + kingX + sq[0]]) == pawnID)
        //     {
        //         // std::vector<std::array<int, 2>> moves = getMoves(b, kingX + sq[0], kingY + sq[1], true);
        //         // allMoves.insert(allMoves.end(), moves.begin(), moves.end());
        //         return true;
        //     }
        // }
        
        // this code is 2x faster than code above
        if (color == whiteID)
        {
            x = kingX + 1;
            y = kingY - 1;
            valid = y >= 0 && y < 8 && x >= 0 && x < 8;
            if (valid && getColor(b[y * 8 + x]) != color && getID(b[y * 8 + x]) == pawnID)
                return true;
            
            x = kingX - 1;
            y = kingY - 1;
            valid = y >= 0 && y < 8 && x >= 0 && x < 8;
            if (valid && getColor(b[y * 8 + x]) != color && getID(b[y * 8 + x]) == pawnID)
                return true;
        }
        else
        {
            x = kingX + 1;
            y = kingY + 1;
            valid = y >= 0 && y < 8 && x >= 0 && x < 8;
            if (valid && getColor(b[y * 8 + x]) != color && getID(b[y * 8 + x]) == pawnID)
                return true;
            
            x = kingX - 1;
            y = kingY + 1;
            valid = y >= 0 && y < 8 && x >= 0 && x < 8;
            if (valid && getColor(b[y * 8 + x]) != color && getID(b[y * 8 + x]) == pawnID)
                return true;
        }

        // need to check for kings in surrounding squares
        for (int offset : kingMoves[kingPos])
        {
            if (getID(b[kingPos + offset]) == kingID)
                return true;
        }
        
        return false;
    }

    /**
     * moves a piece (also takes care of stuff like en passant and castling rights)
     * 
     * @param currX the current x coord of the piece
     * @param currY the current y coord of the piece
     * @param destX the destination x coord of the piece
     * @param destY the destination y coord of the piece
     * @param b the board to move the piece on (helpful for test boards)
     */
    static void movePiece(int currX, int currY, int destX, int destY, std::array<int, 69>& b)
    {
        int p = b[currY * 8 + currX];
        int color = getColor(p);
        int id = getID(p);
        int newEnPassant = 0;
        switch (id)
        {
        case pawnID:
            if (destY >= 1 && destY < 7) // non promotion moves
            {
                if (abs(currY - destY) == 2)
                {
                        newEnPassant += ((0b1 << 8) >> destX);
                }
                if (abs(currX - destX) == 1) // it moved horizontally, either regular or en passant capture
                {
                    if (b[destY * 8 + destX] == 0) // moving to empty square = en passant
                    {
                        b[currY * 8 + destX] = 0; // the pawn en passant -ed
                    }
                }
            }
            else // promotion stuff
            {
                // might be a better way to do this
                if (destY == -1) // queen promotion (white)
                {
                    b[0 * 8 + destX] = makePiece(destX, 0, color, queenID);
                    b[currY * 8 + currX] = 0;
                    return; // don't need to do anything else like en passant bc this can't be a double move
                }
                else if (destY == 8) // queen promotion (black)
                {
                    b[7 * 8 + destX] = makePiece(destX, 7, color, queenID);
                    b[currY * 8 + currX] = 0;
                    return; // don't need to do anything else like en passant bc this can't be a double move
                }
                else if (destY == -2) // knight (white)
                {
                    b[0 * 8 + destX] = makePiece(destX, 0, color, knightID);
                    b[currY * 8 + currX] = 0;
                    return; // don't need to do anything else like en passant bc this can't be a double move
                }
                else if (destY == 9) // knight (black)
                {
                    b[7 * 8 + destX] = makePiece(destX, 7, color, knightID);
                    b[currY * 8 + currX] = 0;
                    return; // don't need to do anything else like en passant bc this can't be a double move
                }
                else if (destY == -3) // bishop (white)
                {
                    b[0 * 8 + destX] = makePiece(destX, 0, color, bishopID);
                    b[currY * 8 + currX] = 0;
                    return; // don't need to do anything else like en passant bc this can't be a double move
                }
                else if (destY == 10) // bishop (black)
                {
                    b[7 * 8 + destX] = makePiece(destX, 7, color, bishopID);
                    b[currY * 8 + currX] = 0;
                    return; // don't need to do anything else like en passant bc this can't be a double move
                }
                else if (destY == -4) // rook (white)
                {
                    b[0 * 8 + destX] = makePiece(destX, 0, color, rookID);
                    b[currY * 8 + currX] = 0;
                    return; // don't need to do anything else like en passant bc this can't be a double move
                }
                else // rook (black)
                {
                    b[7 * 8 + destX] = makePiece(destX, 7, color, rookID);
                    b[currY * 8 + currX] = 0;
                    return; // don't need to do anything else like en passant bc this can't be a double move
                }
            }
            break;
        case kingID:
            if (abs(currX - destX) == 2) // castling
            {
                if (destX == 6) // castled king side
                {
                    // move the rook
                    b[destY * 8 + 5] = b[destY * 8 + 7];
                    b[destY * 8 + 7] = 0;
                    setX(b[destY * 8 + 5], 5);
                }
                else // castled queen side
                {
                    // move the rook
                    b[destY * 8 + 3] = b[destY * 8 + 0];
                    b[destY * 8 + 0] = 0;
                    setX(b[destY * 8 + 3], 3);
                }
            }
            if (color == whiteID)
            {
                b[64] = b[64] & 0b0011; // remove white castling rights
                b[67] = (destX << 3) + destY;
            }
            else
            {
                b[64] = b[64] & 0b1100; // remove black castling rights
                b[68] = (destX << 3) + destY;
            }
            break;
        case rookID:
            if (color == whiteID)
            {
                if (currX == 7) // king side
                    b[64] = b[64] & 0b1011;
                else if (currX == 0) // queen side
                    b[64] = b[64] & 0b0111;
            }
            else
            {
                if (currX == 7) // king side
                    b[64] = b[64] & 0b1110;
                else if (currX == 0) // queen side
                    b[64] = b[64] & 0b1101;
            }
            break;
        }
        b[destY * 8 + destX] = b[currY * 8 + currX];
        b[currY * 8 + currX] = 0;
        setX(b[destY * 8 + destX], destX);
        setY(b[destY * 8 + destX], destY);
        b[65] = newEnPassant;
    }

    static void movePiece(int pos, int dest, std::array<int, 69>& b)
    {
        int p = b[pos];
        int destX, destY;
        if (dest > 0)
        {
            destY = dest / 8;
            destX = dest % 8;
        }
        else // white promotions, negative y which leads to negative dest
        {
            destY = dest / 8 - 1;
            destX = 8 - dest % 8;
        }
        int currX = pos % 8;
        int currY = pos / 8;
        int color = getColor(p);
        int id = getID(p);
        int newEnPassant = 0;
        switch (id)
        {
        case pawnID:
            if (destY >= 1 && destY < 7) // non promotion moves
            {
                if (abs(currY - destY) == 2)
                {
                        newEnPassant += ((0b1 << 8) >> destX);
                }
                if (abs(currX - destX) == 1) // it moved horizontally, either regular or en passant capture
                {
                    if (b[dest] == 0) // moving to empty square = en passant
                    {
                        b[currY * 8 + destX] = 0; // the pawn en passant -ed
                    }
                }
            }
            else // promotion stuff
            {
                // might be a better way to do this
                if (destY == -1) // queen promotion (white)
                {
                    b[0 * 8 + destX] = makePiece(destX, 0, color, queenID);
                    b[pos] = 0;
                    return; // don't need to do anything else like en passant bc this can't be a double move
                }
                else if (destY == 8) // queen promotion (black)
                {
                    b[7 * 8 + destX] = makePiece(destX, 7, color, queenID);
                    b[pos] = 0;
                    return; // don't need to do anything else like en passant bc this can't be a double move
                }
                else if (destY == -2) // knight (white)
                {
                    b[0 * 8 + destX] = makePiece(destX, 0, color, knightID);
                    b[pos] = 0;
                    return; // don't need to do anything else like en passant bc this can't be a double move
                }
                else if (destY == 9) // knight (black)
                {
                    b[7 * 8 + destX] = makePiece(destX, 7, color, knightID);
                    b[pos] = 0;
                    return; // don't need to do anything else like en passant bc this can't be a double move
                }
                else if (destY == -3) // bishop (white)
                {
                    b[0 * 8 + destX] = makePiece(destX, 0, color, bishopID);
                    b[pos] = 0;
                    return; // don't need to do anything else like en passant bc this can't be a double move
                }
                else if (destY == 10) // bishop (black)
                {
                    b[7 * 8 + destX] = makePiece(destX, 7, color, bishopID);
                    b[pos] = 0;
                    return; // don't need to do anything else like en passant bc this can't be a double move
                }
                else if (destY == -4) // rook (white)
                {
                    b[0 * 8 + destX] = makePiece(destX, 0, color, rookID);
                    b[pos] = 0;
                    return; // don't need to do anything else like en passant bc this can't be a double move
                }
                else // rook (black)
                {
                    b[7 * 8 + destX] = makePiece(destX, 7, color, rookID);
                    b[pos] = 0;
                    return; // don't need to do anything else like en passant bc this can't be a double move
                }
            }
            break;
        case kingID:
            if (abs(currX - destX) == 2) // castling
            {
                if (destX == 6) // castled king side
                {
                    // move the rook
                    b[destY * 8 + 5] = b[destY * 8 + 7];
                    b[destY * 8 + 7] = 0;
                    setX(b[destY * 8 + 5], 5);
                }
                else // castled queen side
                {
                    // move the rook
                    b[destY * 8 + 3] = b[destY * 8 + 0];
                    b[destY * 8 + 0] = 0;
                    setX(b[destY * 8 + 3], 3);
                }
            }
            if (color == whiteID)
            {
                b[64] = b[64] & 0b0011; // remove white castling rights
                b[67] = (destX << 3) + destY;
            }
            else
            {
                b[64] = b[64] & 0b1100; // remove black castling rights
                b[68] = (destX << 3) + destY;
            }
            break;
        case rookID:
            if (color == whiteID)
            {
                if (currX == 7) // king side
                    b[64] = b[64] & 0b1011;
                else if (currX == 0) // queen side
                    b[64] = b[64] & 0b0111;
            }
            else
            {
                if (currX == 7) // king side
                    b[64] = b[64] & 0b1110;
                else if (currX == 0) // queen side
                    b[64] = b[64] & 0b1101;
            }
            break;
        }
        b[dest] = b[pos];
        b[pos] = 0;
        setX(b[dest], destX);
        setY(b[dest], destY);
        b[65] = newEnPassant;
    }
   
    // next 3 functions aren't used...
    /**
     * same as other isCheck, but you give it the moves
     * 
     * @param b the board
     * @param color the color to be checked
     * @param moves all of the opponents possible moves (assumes that king x and y for this team are last element)
     * @return true if in check, false otherwise
     */
    // bool isCheck(std::array<int, 69>& b, int color, std::vector<std::array<int, 2>> moves)
    // {
    //     int kingX, kingY;
    //     std::array<int, 2> kingCoords = moves[moves.size() - 1];
    //     kingX = kingCoords[0];
    //     kingY = kingCoords[1];
    //     for (std::array<int, 2> m : moves)
    //     {
    //         if (m[0] == kingX && m[1] == kingY)
    //         {
    //             // if (color == whiteID) wInCheck = true;
    //             // else bInCheck = true;
    //             return true;
    //         }
    //     }
    //     // if (color == whiteID) wInCheck = false;
    //     // else bInCheck = false;
    //     return false; 
    // }
    

    static std::array<int, 69> FENtoBoard(std::string fen)
    {
        std::array<int, 69> board;
        for (int i = 0; i < 69; i++)
        {
            board[i] = 0;
        }

        size_t firstSpace = fen.find(" ");

        std::string boardStr = fen.substr(0, firstSpace);
        
        std::string toMove = fen.substr(firstSpace + 1, 1); // "w" or "b"

        size_t secondSpace = fen.find(" ", firstSpace + 1);
        size_t thirdSpace = fen.find(" ", secondSpace + 1);

        std::string castling = fen.substr(secondSpace + 1, thirdSpace - secondSpace); // KQkq means both can castle king and queen side

        std::string enPassant = fen.substr(thirdSpace + 1, fen.length()); // sqaure behind a pawn that just move 2 squares ex. "e6"
        int boardIndex = 0;
        for (char c : boardStr)
        {
            if (c == 'p') // black pawn
                board[boardIndex] = makePiece(boardIndex % 8, boardIndex / 8, blackID, pawnID);
            else if (c == 'n') // black knight
                board[boardIndex] = makePiece(boardIndex % 8, boardIndex / 8, blackID, knightID);
            else if (c == 'b') // black bishop
                board[boardIndex] = makePiece(boardIndex % 8, boardIndex / 8, blackID, bishopID);
            else if (c == 'r') // black rook
                board[boardIndex] = makePiece(boardIndex % 8, boardIndex / 8, blackID, rookID);
            else if (c == 'q') // black queen
                board[boardIndex] = makePiece(boardIndex % 8, boardIndex / 8, blackID, queenID);
            else if (c == 'k') // black king
            {
                board[boardIndex] = makePiece(boardIndex % 8, boardIndex / 8, blackID, kingID);
                board[68] = (boardIndex % 8 << 3) + boardIndex / 8;
            }
            /* white pieces */
            else if (c == 'P') // white pawn
                board[boardIndex] = makePiece(boardIndex % 8, boardIndex / 8, whiteID, pawnID);
            else if (c == 'N') // white knight
                board[boardIndex] = makePiece(boardIndex % 8, boardIndex / 8, whiteID, knightID);
            else if (c == 'B') // white bishop
                board[boardIndex] = makePiece(boardIndex % 8, boardIndex / 8, whiteID, bishopID);
            else if (c == 'R') // white bishop
                board[boardIndex] = makePiece(boardIndex % 8, boardIndex / 8, whiteID, rookID);
            else if (c == 'Q') // white queen
                board[boardIndex] = makePiece(boardIndex % 8, boardIndex / 8, whiteID, queenID);
            else if (c == 'K') // white king
            {
                board[boardIndex] = makePiece(boardIndex % 8, boardIndex / 8, whiteID, kingID);
                board[67] = (boardIndex % 8 << 3) + boardIndex / 8;
            }
            /* numbers */
            else if (c == '1') // 1 blank
                boardIndex += 0;
            else if (c == '2') // 2 blanks
                boardIndex += 1;
            else if (c == '3') // 3 blank
                boardIndex += 2;
            else if (c == '4') // 4 blanks
                boardIndex += 3;
            else if (c == '5') // 5 blank
                boardIndex += 4;
            else if (c == '6') // 6 blanks
                boardIndex += 5;
            else if (c == '7') // 7 blank
                boardIndex += 6;
            else if (c == '8') // 8 blanks
                boardIndex += 7;
            
            if (c != '/')
                boardIndex += 1;
        }
        
        if (toMove == "w") board[66] = whiteID;
        else board[66] = blackID;

        int castlingNum = 0;
        for (char c : castling)
        {
            if (c == '-')
                break;
            else if (c == 'K') // white king side
                castlingNum += 0b0100;
            else if (c == 'Q') // white queen side
                castlingNum += 0b1000;
            else if (c == 'k') // black king side
                castlingNum += 0b0001;
            else if (c == 'q') // black queen side
                castlingNum += 0b0010;
        }
        board[64] = castlingNum;

        if (enPassant != "-")
        {
            char file = enPassant.at(0);
            if (file == 'a')
                board[65] = 0b10000000;
            else if (file == 'b')
                board[65] = 0b01000000;
            else if (file == 'c')
                board[65] = 0b00100000;
            else if (file == 'd')
                board[65] = 0b00010000;
            else if (file == 'e')
                board[65] = 0b00001000;
            else if (file == 'f')
                board[65] = 0b00000100;
            else if (file == 'g')
                board[65] = 0b00000010;
            else if (file == 'h')
                board[65] = 0b00000001;
        }
        return board;
    }

    static void initDiag()
    {
        std::array<std::array<int, 4>, 64> tmp;
        for (int i = 0; i < 64; i++)
        {
            int row = i / 8;
            int col = i % 8;
            int numDiagNegNeg, numDiagPosPos, numDiagPosNeg, numDiagNegPos;
            if (row > col)
            {
                numDiagNegNeg = col;
                numDiagPosPos = 7 - row;
            }
            else 
            {
                numDiagNegNeg = row;
                numDiagPosPos = 7 - col;
            }

            if (7 - row > col)
            {
                numDiagPosNeg = row;
                numDiagNegPos = col;
            }
            else
            {
                numDiagPosNeg = 7 - col;
                numDiagNegPos = 7 - row;
            }

            tmp[i][0] = numDiagNegNeg;
            tmp[i][1] = numDiagPosNeg;
            tmp[i][2] = numDiagPosPos;
            tmp[i][3] = numDiagNegPos;
        }
        diagonalMoving = tmp;
    }
};

// all knight moves are {-15, -17, -6, 10, -10, 6, 15, 17}
const std::array<std::vector<int>, 64> Game::knightMoves = {{ {10, 17}, {10, 15, 17}, {10, 15, 17, 6}, {10, 15, 17, 6}, {10, 15, 17, 6}, {10, 15, 17, 6}, {15, 17, 6}, {15, 6}, 
                                                              {-6, 10, 17},  {-6, 10, 17, 15}, {-6, 10, 17, 15, 6, -10}, {-6, 10, 17, 15, 6, -10}, {-6, 10, 17, 15, 6, -10}, {-6, 10, 17, 15, 6, -10}, {17, 15, 6, -10}, {15, 6, -10},
                                                              {-15, -6, 10, 17}, {-15, -6, 10, 17, -17, 15}, {-15, -6, 10, 17, -17, 15, -10, 6}, {-15, -6, 10, 17, -17, 15, -10, 6}, {-15, -6, 10, 17, -17, 15, -10, 6}, {-15, -6, 10, 17, -17, 15, -10, 6}, {-15, 17, -17, 15, -10, 6}, {-17, 15, -10, 6},
                                                              {-15, -6, 10, 17}, {-15, -6, 10, 17, -17, 15}, {-15, -6, 10, 17, -17, 15, -10, 6}, {-15, -6, 10, 17, -17, 15, -10, 6}, {-15, -6, 10, 17, -17, 15, -10, 6}, {-15, -6, 10, 17, -17, 15, -10, 6}, {-15, 17, -17, 15, -10, 6}, {-17, 15, -10, 6},
                                                              {-15, -6, 10, 17}, {-15, -6, 10, 17, -17, 15}, {-15, -6, 10, 17, -17, 15, -10, 6}, {-15, -6, 10, 17, -17, 15, -10, 6}, {-15, -6, 10, 17, -17, 15, -10, 6}, {-15, -6, 10, 17, -17, 15, -10, 6}, {-15, 17, -17, 15, -10, 6}, {-17, 15, -10, 6},
                                                              {-15, -6, 10, 17}, {-15, -6, 10, 17, -17, 15}, {-15, -6, 10, 17, -17, 15, -10, 6}, {-15, -6, 10, 17, -17, 15, -10, 6}, {-15, -6, 10, 17, -17, 15, -10, 6}, {-15, -6, 10, 17, -17, 15, -10, 6}, {-15, 17, -17, 15, -10, 6}, {-17, 15, -10, 6},
                                                              {-15, -6, 10}, {-15, -6, 10, -17}, {-15, -6, 10, -17, -10, 6}, {-15, -6, 10, -17, -10, 6}, {-15, -6, 10, -17, -10, 6}, {-15, -6, 10, -17, -10, 6}, {-15, -17, -10, 6}, {-17, -10, 6},
                                                              {-15, -6}, {-15, -6, -17}, {-15, -6, -17, -10}, {-15, -6, -17, -10}, {-15, -6, -17, -10}, {-15, -6, -17, -10}, {-15, -17, -10}, {-17, -10}}};

const std::array<std::vector<int>, 64> Game::kingMoves = {{ {1, 8, 9}, {1, 8, 9, -1, 7}, {1, 8, 9, -1, 7}, {1, 8, 9, -1, 7}, {1, 8, 9, -1, 7}, {1, 8, 9, -1, 7}, {1, 8, 9, -1, 7}, {8, -1, 7},
                                                            {-8, -7, 1, 8, 9}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, 8, -1, -9, 7},
                                                            {-8, -7, 1, 8, 9}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, 8, -1, -9, 7},
                                                            {-8, -7, 1, 8, 9}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, 8, -1, -9, 7},
                                                            {-8, -7, 1, 8, 9}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, 8, -1, -9, 7},
                                                            {-8, -7, 1, 8, 9}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, 8, -1, -9, 7},
                                                            {-8, -7, 1, 8, 9}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, -7, 1, 8, 9, -1, -9, 7}, {-8, 8, -1, -9, 7},
                                                            {-8, -7, 1}, {-8, -7, 1, -9, -1}, {-8, -7, 1, -9, -1}, {-8, -7, 1, -9, -1}, {-8, -7, 1, -9, -1}, {-8, -7, 1, -9, -1}, {-8, -7, 1, -9, -1}, {-8, -9, -1}}};

// const std::array<std::array<std::vector<int>, 4>, 64> Game::diagMoves = { {{}, {}, {9, 18, 27, 36, 45, 54, 63}, {}},  {{}, {}, {9, 18, 27, 36, 45, 54}, {7}}, {{}, {}, {9, 18, 27, 36, 45}, {7, 14}}, {{}, {}}}

// int main()
// {
//     // Game g;
//     // auto t1 = std::chrono::high_resolution_clock::now();    
//     // for (int i = 0; i < 10000; i++)
//     // {
//     //     g.isCheck(g.board, Game::whiteID);
//     // }
//     // auto t2 = std::chrono::high_resolution_clock::now();
//     // auto t3 = std::chrono::high_resolution_clock::now();    
//     // for (int i = 0; i < 10000; i++)
//     // {
//     //     g.isCheckOld(g.board, Game::whiteID);
//     // }
//     // auto t4 = std::chrono::high_resolution_clock::now();

//     // std::chrono::duration<double, std::milli> ms_double = t2 - t1;
//     // std::cout << ms_double.count() << " ms" << std::endl;
//     // std::chrono::duration<double, std::milli> ms_double2 = t4 - t3;
//     // std::cout << ms_double2.count() << " ms" << std::endl;
//     // Game::FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -");
//     Game::initDiag();
//     std::array<int, 69> board = Game::FENtoBoard("8/1K6/5B2/3R4/2N3P1/4Q3/8/k7 w - -");
//     int pawnX = 6;
//     int pawnY = 4;
//     int bishopX = 5;
//     int bishopY = 2;
//     int rookX = 3;
//     int rookY = 3;
//     int knightX = 2;
//     int knightY = 4;
//     int kingX = 1;
//     int kingY = 1;
//     int queenX = 4;
//     int queenY = 5;
//     bool allmoves = true;
//     int numTrials = 100000;
//     // king perf test
//     auto t1 = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < numTrials; i++)
//     {
//         Game::getMoves(board, kingX, kingY, allmoves);
//     }
//     auto t2 = std::chrono::high_resolution_clock::now();
//     std::chrono::duration<double, std::milli> time = t2 - t1;
//     std::cout << "King: " << time.count() << std::endl;

//     // pawn perf test
//     t1 = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < numTrials; i++)
//     {
//         Game::getMoves(board, pawnX, pawnY, allmoves);
//     }
//     t2 = std::chrono::high_resolution_clock::now();
//     time = t2 - t1;
//     std::cout << "Pawn: " << time.count() << std::endl;

//     // bishop perf test
//     t1 = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < numTrials; i++)
//     {
//         Game::getMoves(board, bishopX, bishopY, allmoves);
//     }
//     t2 = std::chrono::high_resolution_clock::now();
//     time = t2 - t1;
//     std::cout << "Bishop: " << time.count() << std::endl;

//     // rook perf test
//     t1 = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < numTrials; i++)
//     {
//         Game::getMoves(board, rookX, rookY, allmoves);
//     }
//     t2 = std::chrono::high_resolution_clock::now();
//     time = t2 - t1;
//     std::cout << "Rook: " << time.count() << std::endl;

//     // knight perf test
//     t1 = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < numTrials; i++)
//     {
//         Game::getMoves(board, knightX, knightY, allmoves);
//     }
//     t2 = std::chrono::high_resolution_clock::now();
//     time = t2 - t1;
//     std::cout << "Knight: " << time.count() << std::endl;

//     // queen perf test
//     t1 = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < numTrials; i++)
//     {
//         Game::getMoves(board, queenX, queenY, allmoves);
//     }
//     t2 = std::chrono::high_resolution_clock::now();
//     time = t2 - t1;
//     std::cout << "Queen: " << time.count() << std::endl;

//     std::cout << "done" << std::endl;
// }