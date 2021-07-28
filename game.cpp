#include <array>
#include <vector>
#include <iostream>


class Game
{
public:
    std::array<int, 67> board; // format: 10 bit number- xxxyyyc_id | first 64 are pieces, 65 is castlingPerms, 66 is enPassantFiles, 67 is currturn
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
    Game()
    {
        // can clone board by just using =
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
        //           0}}; // curr turn
        board = {{makePiece(0, 0, blackID, rookID), makePiece(1, 0, blackID, knightID), makePiece(2, 0, blackID, bishopID), makePiece(3, 0, blackID, queenID), makePiece(4, 0, blackID, kingID), makePiece(5, 0, blackID, bishopID), makePiece(6, 0, blackID, knightID), makePiece(7, 0, blackID, rookID),
                  makePiece(0, 1, blackID, pawnID), makePiece(1, 1, blackID, pawnID),   makePiece(2, 1, blackID, pawnID),   makePiece(3, 1, blackID, pawnID),  makePiece(4, 1, blackID, pawnID), makePiece(5, 1, blackID, pawnID),   makePiece(6, 1, blackID, pawnID),   makePiece(7, 1, blackID, pawnID),
                  0,                                0,                                  0,                                  0,                                 0,                                0,                                  0,                                  0,
                  0,                                0,                                  0,                                  0,                                 0,                                0,                                  0,                                  0,
                  0,                                0,                                  0,                                  0,                                 0,                                0,                                  0,                                  0,
                  0,                                0,                                  0,                                  makePiece(3, 5, whiteID, pawnID),                                 0,                                0,                                  0,                                  0,
                  makePiece(0, 6, whiteID, pawnID), makePiece(1, 6, whiteID, pawnID),   makePiece(2, 6, whiteID, pawnID),   0,                                 makePiece(4, 6, whiteID, pawnID), makePiece(5, 6, whiteID, pawnID),   makePiece(6, 6, whiteID, pawnID),   makePiece(7, 6, whiteID, pawnID),
                  makePiece(0, 7, whiteID, rookID), makePiece(1, 7, whiteID, knightID), makePiece(2, 7, whiteID, bishopID), makePiece(3, 7, whiteID, queenID), makePiece(4, 7, whiteID, kingID), makePiece(5, 7, whiteID, bishopID), makePiece(6, 7, whiteID, knightID), makePiece(7, 7, whiteID, rookID),
                  0b1111, // castling perms
                  0, // en passant targets
                  1}}; // curr turn
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
    static int getX(int& p) {return (p & 0b1110000000) >> 7;}

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
    bool canCastle(std::string side, int& color, int castlingPerms)
    {
        if (color == whiteID)
        {
            if (side == "q")
                return (castlingPerms & 0b1000) >> 3;
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
    std::vector<std::array<int, 2>> getMoves(std::array<int, 67>& b, int x, int y, bool allMoves=false)
    {
        int piece = b[y * 8 + x];
        int id = getID(piece);
        int color = getColor(piece);
        std::vector<std::array<int, 2>> results;

        switch (id)
        {
        case pawnID:
        {
            int dir;
            if (color == whiteID) dir = -1;
            else dir = 1;
            if (y + dir >= 0 && y + dir < 8)
            {
                if (b[(y + dir) * 8 + x] == 0)
                {
                    results.push_back({{x, y + dir}});
                    if ((color == whiteID && y == 6) || (color == blackID && y == 1))
                    {
                        if (b[(y + 2 * dir) * 8 + x] == 0)
                            results.push_back({{x, y + 2 * dir}});
                    }
                }
                if (x - 1 >= 0 && b[(y + dir) * 8 + x - 1] != 0)
                {
                    if (getColor(b[(y + dir) * 8 + x - 1]) != color)
                        results.push_back({{x - 1, y + dir}});
                }
                if (x + 1 < 8 && b[(y + dir) * 8 + x + 1] != 0)
                {
                    if (getColor(b[(y + dir) * 8 + x + 1]) != color)
                        results.push_back({{x + 1, y + dir}});
                }
                // en passant
                if ((color == whiteID && y == 3) || (color == blackID && y == 4))
                {
                    if (canBeEnPassant(x + 1, y, b[65]))
                        results.push_back({{x + 1, y + dir}});
                    if (canBeEnPassant(x - 1, y, b[65]))
                        results.push_back({{x - 1, y + dir}});
                }
            }
            break;
        }
        case knightID:
        {
            std::array<std::array<int, 2>, 8> toCheck = {{ {{ 1, -2}}, {{-1, -2}},
                                                           {{ 1,  2}}, {{-1,  2}},
                                                           {{ 2,  1}}, {{ 2, -1}},
                                                           {{-2,  1}}, {{-2, -1}} }};
            for (std::array<int, 2> sq : toCheck)
            {   
                if ((y + sq[1] >= 0 && y + sq[1] < 8 && x + sq[0] >= 0 && x + sq[0] < 8) && (b[(y + sq[1]) * 8 + x + sq[0]] == 0 || getColor(b[(y + sq[1]) * 8 + x + sq[0]]) != color))
                    results.push_back({{sq[0] + x, sq[1] + y}});
            }
            break;
        }
        case bishopID:
        {
            int yInc, xInc, currX, currY;
            for (int i = 0; i < 4; i++)
            {
                if (i < 2) yInc = -1;
                else yInc = 1;
                if (i % 2 == 0) // -,- and +,+
                    xInc = yInc;
                else            // -,+ and +,-
                    xInc = -yInc;
                
                currX = x + xInc;
                currY = y + yInc;

                while (currY >= 0 && currY < 8 && currX >= 0 && currX < 8)
                {
                    if (b[currY * 8 + currX] == 0 || getColor(b[currY * 8 + currX]) != color)
                        results.push_back({{currX, currY}});
                    if (b[currY * 8 + currX] != 0)
                        break;
                    currY += yInc;
                    currX += xInc;
                }
            }
            break;
        }
        case rookID:
        {
            int inc, currY, currX;
            for (int i = 0; i < 4; i++)
            {
                if (i < 2) inc = -1;
                else inc = 1;

                if (i % 2 == 0)
                {
                    currY = y + inc;

                    while (currY >= 0 && currY < 8)
                    {
                        if (b[currY * 8 + x] == 0 || getColor(b[currY * 8 + x]) != color)
                            results.push_back({{x, currY}});
                        if (b[currY * 8 + x] != 0)
                            break;
                        currY += inc;
                    }
                }
                else
                {
                    currX = x + inc;

                    while (currX >= 0 && currX < 8)
                    {
                        if (b[y * 8 + currX] == 0 || getColor(b[y * 8 + currX]) != color)
                            results.push_back({{currX, y}});
                        if (b[y * 8 + currX] != 0)
                            break;
                        currX += inc;
                    }
                }
            }
            break;
        }
        case queenID:
        {
            // bishop code:
            int yInc, xInc, currX, currY;
            for (int i = 0; i < 4; i++)
            {
                if (i < 2) yInc = -1;
                else yInc = 1;
                if (i % 2 == 0) // -,- and +,+
                    xInc = yInc;
                else            // -,+ and +,-
                    xInc = -yInc;
                
                currX = x + xInc;
                currY = y + yInc;

                while (currY >= 0 && currY < 8 && currX >= 0 && currX < 8)
                {
                    if (b[currY * 8 + currX] == 0 || getColor(b[currY * 8 + currX]) != color)
                        results.push_back({{currX, currY}});
                    if (b[currY * 8 + currX] != 0)
                        break;
                    currY += yInc;
                    currX += xInc;
                }
            }
            // rook code:
            int inc;
            for (int i = 0; i < 4; i++)
            {
                if (i < 2) inc = -1;
                else inc = 1;

                if (i % 2 == 0)
                {
                    currY = y + inc;

                    while (currY >= 0 && currY < 8)
                    {
                        if (b[currY * 8 + x] == 0 || getColor(b[currY * 8 + x]) != color)
                            results.push_back({{x, currY}});
                        if (b[currY * 8 + x] != 0)
                            break;
                        currY += inc;
                    }
                }
                else
                {
                    currX = x + inc;

                    while (currX >= 0 && currX < 8)
                    {
                        if (b[y * 8 + currX] == 0 || getColor(b[y * 8 + currX]) != color)
                            results.push_back({{currX, y}});
                        if (b[y * 8 + currX] != 0)
                            break;
                        currX += inc;
                    }
                }
            }
            break;
        }
        case kingID:
        {
            std::array<std::array<int, 2>, 8> toCheck = {{ {{ 1,  1}}, {{ 0, -1}},
                                                           {{ 1,  0}}, {{-1,  1}},
                                                           {{ 1, -1}}, {{-1,  0}},
                                                           {{ 0,  1}}, {{-1, -1}} }};
            for (std::array<int, 2> sq : toCheck)
            {   
                if ((y + sq[1] >= 0 && y + sq[1] < 8 && x + sq[0] >= 0 && x + sq[0] < 8) && (b[(y + sq[1]) * 8 + x + sq[0]] == 0 || getColor(b[(y + sq[1]) * 8 + x + sq[0]]) != color))
                    results.push_back({{sq[0] + x, sq[1] + y}});
            }
            if (!((color == whiteID && wInCheck) || (color == blackID && bInCheck)) && x == 4) // RECURSION PROBLEM
            {
                std::array<int, 67> tmpBoard;
                if (b[y * 8 + x + 1] == 0 && b[y * 8 + x + 2] == 0)
                {
                    tmpBoard = b;
                    movePiece(x, y, x + 1, y, tmpBoard); // check the in-between spot to ensure we're not castling thru check
                    if (canCastle("k", color, b[64]) && !isCheck(tmpBoard, color))
                        results.push_back({{x + 2, y}});
                }
                if (b[y * 8 + x - 1] == 0 && b[y * 8 + x - 2] == 0 && b[y * 8 + x - 3] == 0)
                {
                    tmpBoard = b;
                    movePiece(x, y, x - 1, y, tmpBoard); // check the in-between spot to ensure we're not castling thru check
                    if (canCastle("q", color, b[64]) && !isCheck(tmpBoard, color))
                       results.push_back({{x - 2, y}});
                }
            }
            break;
        }
        }

        if (allMoves)
            return results;

        // remove moves resulting in check
        std::vector<std::array<int, 2>> finalMoves;
        // std::vector<std::array<int, 2>> oppMoves = getAllMoves(b, !color, true, true);
        for (std::array<int, 2> m : results)
        {
            std::array<int, 67> newBoard = b;
            movePiece(x, y, m[0], m[1], newBoard);
            if (!isCheck(newBoard, color))
                finalMoves.push_back({{m[0], m[1]}});
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
    bool canBeEnPassant(int x, int y, int targets)
    {
        if (x >= 0 && x < 8 && y >= 0 && y < 8)
        {
            if (getID(board[y * 8 + x]) == pawnID)
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
    bool isCheck(std::array<int, 67>& b, int color)
    {
        std::vector<std::array<int, 2>> allMoves;
        int kingX, kingY;
        for (int i = 0; i < 64; i++)
        {
            if (b[i] != 0)
            {
                if (getColor(b[i]) != color)
                {
                    std::vector<std::array<int, 2>> moves = getMoves(b, i % 8, i / 8, true);
                    allMoves.insert(allMoves.end(), moves.begin(), moves.end());
                }
                else if (getID(b[i]) == kingID)
                {
                    kingX = i % 8; // is getX(board[i]) faster?
                    kingY = i / 8; // is getY(board[i]) faster?
                }
            }
        }

        for (std::array<int, 2> m : allMoves)
        {
            if (m[0] == kingX && m[1] == kingY)
            {
                if (color == whiteID) wInCheck = true;
                else bInCheck = true;
                return true;
            }
        }
        if (color == whiteID) wInCheck = false;
        else bInCheck = false;
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
    void movePiece(int currX, int currY, int destX, int destY, std::array<int, 67>& b)
    {
        int p = b[currY * 8 + currX];
        int color = getColor(p);
        int id = getID(p);
        int newEnPassant = 0;
        switch (id)
        {
        case pawnID:
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
                b[64] = b[64] & 0b0011; // remove white castling rights
            else
                b[64] = b[64] & 0b1100; // remove black castling rights
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

    /**
     * gets all possible moves for a side
     * 
     * @param b the board
     * @param c the color of the side
     * @param allMoves if true doesn't check if moves will result in check
     * @param getKing if true returns opposite side's king x and y as last element in vector
     * @return a list of the coordinates the side attacks
     */
    std::vector<std::array<int, 2>> getAllMoves(std::array<int, 67>& b, int c, bool allMoves=true, bool getKing=false)
    {
        std::vector<std::array<int, 2>> moves;
        int kingX, kingY;
        for (int i = 0; i < 64; i++)
        {
            if (b[i] != 0)
            {
                if (getColor(b[i]) == c)
                {
                    std::vector<std::array<int, 2>> m = getMoves(b, i % 8, i / 8, true);
                    moves.insert(moves.end(), m.begin(), m.end());
                }
                else if (getKing && getID(b[i]) == kingID)
                {
                    kingX = i % 8; // is getX(board[i]) faster?
                    kingY = i / 8; // is getY(board[i]) faster?
                }
            }
        }
        if (allMoves)
        {
            if (getKing)
                moves.push_back({{kingX, kingY}});
            return moves;
        }
    }
    
    /**
     * same as other isCheck, but you give it the moves
     * 
     * @param b the board
     * @param color the color to be checked
     * @param moves all of the opponents possible moves (assumes that king x and y for this team are last element)
     * @return true if in check, false otherwise
     */
    bool isCheck(std::array<int, 67>& b, int color, std::vector<std::array<int, 2>> moves)
    {
        int kingX, kingY;
        std::array<int, 2> kingCoords = moves[moves.size() - 1];
        kingX = kingCoords[0];
        kingY = kingCoords[1];
        for (std::array<int, 2> m : moves)
        {
            if (m[0] == kingX && m[1] == kingY)
            {
                // if (color == whiteID) wInCheck = true;
                // else bInCheck = true;
                return true;
            }
        }
        // if (color == whiteID) wInCheck = false;
        // else bInCheck = false;
        return false; 
    }
    
};

// int main()
// {
//     Game g;
//     std::cout << Game::getX(g.board[0]) << Game::getY(g.board[0]) << Game::getColor(g.board[0]) << Game::getID(g.board[0]) << std::endl;

// }