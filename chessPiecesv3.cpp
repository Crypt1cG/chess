#include <array>
#include <vector>
#include <iostream> // for testing
#include <tuple>
#include <chrono>

class Piece //TODO: add constructor for x, y, color
{
public:
    int x, y;
    // should prob be chars
    std::string color;
    std::string id; // r for rook, p for pawn, etc.

    Piece(int x, int y, std::string c)
    {
        this->x = x;
        this->y = y;
        color = c;
    }
    virtual std::vector<std::tuple<int, int>> GetPossiblePositions(std::array<std::array<Piece*, 8>,8>& board, bool allMoves=false)
    {
        /*
         * allMoves is used because when checking for checks, it doesn't matter
         * if a piece putting a king in check can legally capture the king (pins)
         * https://chess.stackexchange.com/questions/2348/can-a-piece-pinned-to-my-king-put-the-opponents-king-in-check#:~:text=Yes.,their%20own%20king%20in%20check.
         */
        std::vector<std::tuple<int, int>> results;
        return results;
    }

    static bool isCheck(std::array<std::array<Piece*, 8>,8>& board, std::string color)
    {
        std::vector<std::tuple<int, int>> allMoves;
        int kingX;
        int kingY;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (board[i][j] != nullptr)
                {
                    if (board[i][j]->color != color)
                    {
                        // std::cout << board[i][j]->id;
                        std::vector<std::tuple<int, int>> moves = board[i][j]->GetPossiblePositions(board, true);
                        allMoves.insert(allMoves.end(), moves.begin(), moves.end());
                    }
                    else if (board[i][j]->id == "K") // this team's king
                    {
                        kingX = j;
                        kingY = i;
                    }
                }
            }
        }
        for (std::tuple<int, int> t : allMoves)
        {
            if (std::get<0>(t) == kingX && std::get<1>(t) == kingY)
                return true;
        }
        return false;
    }

    virtual Piece* Clone()
    {
        return new Piece(this->x, this->y, this->color);
    }

    static std::string boardToFEN(std::array<std::array<Piece*, 8>,8>& board, std::string currTurn);
    // {
    //     std::string result;
    //     for (int i = 0; i < 8; i++)
    //     {
    //         int numBlankSquares = 0;
    //         for (int j = 0; j < 8; j++)
    //         {
    //             if (board[i][j] != nullptr)
    //             {
    //                 if (numBlankSquares != 0)
    //                     result += std::to_string(numBlankSquares);
    //                 numBlankSquares = 0;
    //                 std::string piece = board[i][j]->id;
    //                 if (piece == "k")
    //                     piece = "n"; // knights are n in FEN notation
    //                 else if (piece == "K")
    //                     piece = "k";
    //                 else if (piece == "Q")
    //                     piece = "q";
    //                 if (board[i][j]->color == "w") // white pieces are uppercase
    //                     piece[0] = std::toupper(piece[0]);
    //                 result += piece;
    //             }
    //             else
    //             {
    //                 numBlankSquares++;
    //                 if (numBlankSquares == 8)
    //                     result += std::to_string(numBlankSquares);
    //             }
    //         }
    //         if (i != 7) // no slash at end
    //             result += "/";
    //     }
    //     result += currTurn;

    //     result += " ";
    //     if (board[7][4] != nullptr && board[7][4]->id == "K" && !((King*)(board[7][4]))->hasMoved)
    //     {
    //         if (board[7][7] != nullptr && board[7][7]->id == "r" && !((Rook*)(board[7][7]))->hasMoved)
    //             result += "K"; // white can castle kingside
    //         if (board[7][0] != nullptr && board[7][0]->id == "r" && !((Rook*)(board[7][0]))->hasMoved)
    //             result += "Q"; // white can castle queenside
    //     }
    //     if (board[0][4] != nullptr && board[0][4]->id == "K" && !((King*)(board[0][4]))->hasMoved)
    //     {
    //         if (board[0][7] != nullptr && board[0][7]->id == "r" && !((Rook*)(board[0][7]))->hasMoved)
    //             result += "k"; // black can castle kingside
    //         if (board[0][0] != nullptr && board[0][0]->id == "r" && !((Rook*)(board[0][0]))->hasMoved)
    //             result += "q"; // black can castle queenside
    //     }
    //     return result;
    // }
    
};

class Pawn : public Piece
{
public:
    bool canBeEnPassant;

    Pawn(int x, int y, std::string c) : Piece(x, y, c)
    {
        id = "p";
        canBeEnPassant = false;
    }

    std::vector<std::tuple<int, int>> GetPossiblePositions(std::array<std::array<Piece*, 8>,8>& board, bool allMoves=false) override
    {
        //TODO: add chekcs for index out of bounds
        std::vector<std::tuple<int, int>> moves;
        int dir;
        if (this->color == "w") // going towards y=0 (negative direction)
            dir = -1;
        else dir = 1;
        
        if (y + dir >= 0 && y + dir < 8) // all pawn moves move fwd at least one
        {
            if (board[y + dir][x] == nullptr) // use short-circuiting to prevent going off board
            {
                moves.push_back(std::make_tuple(x, y + dir));
                if (((color == "w" && y == 6) || (color == "b" && y == 1)) && y + 2 * dir >= 0 && y + 2 * dir < 8) // ********* PART IS NOT NEEDED????
                {
                    if (board[y + 2 * dir][x] == nullptr)
                        moves.push_back(std::make_tuple(x, y + 2 * dir));
                }
            }
            for (int i = 0; i < 3; i += 2)
            {   
                if (x - 1 + i >= 0 && x - 1 + i < 8 && board[y + dir][x - 1 + i] != nullptr)
                {
                    if (board[y + dir][x - 1 + i]->color != this->color) // can capture (checked for nullptr above)
                        moves.push_back(std::make_tuple(x - 1 + i, y + dir));
                }
            }

            //TODO: add en passant
            if (this->color == "w" && this->y == 3) // white and on its fifth rank
            {
                if (board[y][x + 1] != nullptr && board[y][x + 1]->id == "p" && ((Pawn*)(board[y][x + 1]))->canBeEnPassant)
                    moves.push_back(std::make_tuple(x + 1, y + dir));
                if (board[y][x - 1] != nullptr && board[y][x - 1]->id == "p" && ((Pawn*)(board[y][x - 1]))->canBeEnPassant)
                    moves.push_back(std::make_tuple(x - 1, y + dir));
            }
            else if (this->color == "b" && this->y == 4) // black and on its fifth rank
            {
                if (board[y][x + 1] != nullptr && board[y][x + 1]->id == "p" && ((Pawn*)(board[y][x + 1]))->canBeEnPassant)
                    moves.push_back(std::make_tuple(x + 1, y + dir));
                if (board[y][x - 1] != nullptr && board[y][x - 1]->id == "p" && ((Pawn*)(board[y][x - 1]))->canBeEnPassant)
                    moves.push_back(std::make_tuple(x - 1, y + dir));
            }
        }

        if (allMoves)
            return moves;

        // remove moves that result in check
        std::vector<std::tuple<int, int>> finalMoves;
        for (std::tuple<int, int> t : moves)
        {
            int newX = std::get<0>(t);
            int newY = std::get<1>(t);
            std::array<std::array<Piece*, 8>,8> newBoard = board;
            Pawn* tmp = new Pawn(this->x, this->y, this->color);
            tmp->canBeEnPassant = this->canBeEnPassant;
            
            if (abs(this->y - newY) == 2) // double move
            {
                tmp->canBeEnPassant = true;
                // game.pawnsToUpdate.push_back(game.currSelectedPiece);
            }
            if (abs(this->x - newX) == 1) // it moved horizontally - either regular capture or en passant
            {
                if (newBoard[newY][newX] == nullptr) // its moving into an empty square (not a regular capture)
                {
                    // delete newBoard[this->y][newX];
                    // this is still in the old board
                    newBoard[this->y][newX] = nullptr; // the pawn en passant -ed

                }
            }
            // delete newBoard[this->y][this->x];
            newBoard[this->y][this->x] = nullptr;
            tmp->x = newX;
            tmp->y = newY;
            newBoard[newY][newX] = tmp;

            if (!Piece::isCheck(newBoard, this->color))
            {
                finalMoves.push_back(std::make_tuple(newX, newY));
            }
            delete tmp;
        }
        return finalMoves;
    }

    Piece* Clone() override
    {
        Pawn* p = new Pawn(this->x, this->y, this->color);
        p->id = "p";
        p->canBeEnPassant = this->canBeEnPassant;
        return p;
    }
};

class Knight : public Piece
{
public:
    Knight(int x, int y, std::string c) : Piece(x, y, c)
    {
        id = "k";
    }
    
    std::vector<std::tuple<int, int>> GetPossiblePositions(std::array<std::array<Piece*, 8>,8>& board, bool allMoves=false) override
    //TODO: add chekcs for index out of bounds
    {
        std::vector<std::tuple<int, int>> moves;

        // int d; //TODO: better variable name
        // for (int i = 0; i < 4; i++)
        // {
        //     if (i < 2) d = 2;
        //     else d = -2; 
        //     if (i % 2 == 0) // top/bottom directions
        //     {
        //         for (int j = -1; j < 3; j += 2)
        //         {
        //             if (y + d >= 0 && y + d < 8 && x + j >= 0 && x + j < 8)
        //             {
        //                 if (board[y + d][x + j] == nullptr || board[y + d][x + j]->color == this->color) // should short circuit if nullptr
        //                     results.push_back(std::make_tuple(x + j, y + d));
        //             }
        //         }
        //     }
        //     else
        //     {
        //         for (int j = -1; j < 3; j += 2)
        //         {
        //             if (y + j >= 0 && y + j < 8 && x + d >= 0 && x + d < 8)
        //             {
        //                 if (board[y + j][x + d] == nullptr || board[y + j][x + d]->color == this->color) // should short circuit if nullptr
        //                     results.push_back(std::make_tuple(x + d, y + j));
        //             }
        //         }
        //     }
        // }

        std::vector<std::tuple<int, int>> toCheck = {std::make_tuple( 1, -2), std::make_tuple(-1, -2),
                                                     std::make_tuple( 1,  2), std::make_tuple(-1,  2),
                                                     std::make_tuple( 2,  1), std::make_tuple( 2, -1),
                                                     std::make_tuple(-2,  1), std::make_tuple(-2, -1)};
        for (std::tuple<int, int> t : toCheck)
        {
            if (y + std::get<1>(t) >= 0 && y + std::get<1>(t) < 8 && x + std::get<0>(t) >= 0 && x + std::get<0>(t) < 8)
            {
                if (board[y + std::get<1>(t)][x + std::get<0>(t)] == nullptr || board[y + std::get<1>(t)][x + std::get<0>(t)]->color != this->color) // should short-circuit if nullptr
                {
                    moves.push_back(std::make_tuple(x + std::get<0>(t), y + std::get<1>(t))); // use std::get to make a copy
                }
            }
        }

        if (allMoves)
            return moves;

        // remove moves that result in check
        std::vector<std::tuple<int, int>> finalMoves;
        for (std::tuple<int, int> t : moves)
        {
            int newX = std::get<0>(t);
            int newY = std::get<1>(t);
            std::array<std::array<Piece*, 8>,8> newBoard = board;
            Knight* tmp = new Knight(this->x, this->y, this->color); // this is bad. changes the version in board

            // delete newBoard[this->y][this->x];
            newBoard[this->y][this->x] = nullptr;
            tmp->x = newX;
            tmp->y = newY;
            newBoard[newY][newX] = tmp;

            if (!Piece::isCheck(newBoard, this->color))
            {
                finalMoves.push_back(std::make_tuple(newX, newY));
            }
            delete tmp;
        }
        return finalMoves;
    }

    Piece* Clone() override
    {
        Knight* k = new Knight(this->x, this->y, this->color);
        k->id = "k";
        return k;
    }
};

class Rook : public Piece
{
public:
    bool hasMoved; // for castling

    Rook(int x, int y, std::string c) : Piece(x, y, c)
    {
        hasMoved = false;
        id = "r";
    }

    std::vector<std::tuple<int, int>> GetPossiblePositions(std::array<std::array<Piece*, 8>,8>& board, bool allMoves=false) override
    //TODO: add chekcs for index out of bounds
    {
        std::vector<std::tuple<int, int>> moves;

        int inc;

        for (int i = 0; i < 4; i++)
        {
            if (i < 2) inc = -1;
            else inc = 1;
            if (i % 2 == 0) // change y
            {
                int currY = y + inc; // add inc so it doesn't start on top of itself

                while (currY * inc <= (int)(3.5 + 3.5 * inc)) // while its on the board (currY<=7 -currY<=0)
                {
                    if (board[currY][x] == nullptr || board[currY][x]->color != this->color) // should short-circuit if nullptr
                        moves.push_back(std::make_tuple(x, currY));
                    if (board[currY][x] != nullptr) // there is a piece, we can't go further
                            break;
                    currY += inc;
                }
            }
            else // change x
            {
                int currX = x + inc; // add inc so it doesn't start on top of itself

                while (currX * inc <= (int)(3.5 + 3.5 * inc)) // while its on the board (currX<=7 -currX<=0)
                {
                    if (board[y][currX] == nullptr || board[y][currX]->color != this->color) // should short-circuit if nullptr
                        moves.push_back(std::make_tuple(currX, y));
                    if (board[y][currX] != nullptr) // there is a piece, we can't go further
                            break;
                    currX += inc;
                }
            }
        }

        if (allMoves)
            return moves;
            
        // remove moves that result in check
        std::vector<std::tuple<int, int>> finalMoves;
        for (std::tuple<int, int> t : moves)
        {
            int newX = std::get<0>(t);
            int newY = std::get<1>(t);
            std::array<std::array<Piece*, 8>,8> newBoard = board;
            Rook* tmp = new Rook(this->x, this->y, this->color);
            tmp->hasMoved = this->hasMoved;
            
            tmp->hasMoved = true;

            // delete newBoard[this->y][this->x]; will delete it in the old board
            newBoard[this->y][this->x] = nullptr;
            tmp->x = newX;
            tmp->y = newY;
            newBoard[newY][newX] = tmp;

            if (!Piece::isCheck(newBoard, this->color))
            {
                finalMoves.push_back(std::make_tuple(newX, newY));
            }
            delete tmp;
        }
        return finalMoves;
    }

    Piece* Clone() override
    {
        Rook* r = new Rook(this->x, this->y, this->color);
        r->id = "r";
        r->hasMoved = this->hasMoved;
        return r;
    }
};

class Bishop : public Piece
{
public:
    Bishop(int x, int y, std::string c) : Piece(x, y, c)
    {
        id = "b";
    }

    std::vector<std::tuple<int, int>> GetPossiblePositions(std::array<std::array<Piece*, 8>,8>& board, bool allMoves=false) override
    //TODO: add chekcs for index out of bounds
    {
        std::vector<std::tuple<int, int>> moves;

        int yInc;

        for (int i = 0; i < 4; i++)
        {
            if (i < 2) yInc = -1;
            else yInc = 1;
            if (i % 2 == 0) // -,- and +,+
            {
                int xInc = yInc;
                int currY = y + yInc; // add inc so it doesn't start on top of itself
                int currX = x + xInc;

                // while (currY * yInc <= (int)(3.5 + 3.5 * yInc) && currX * xInc <= (int)(3.5 + 3.5 * xInc)) // while its on the board (currY<=7 -currY<=0)
                while (currY >= 0 && currY < 8 && currX >= 0 && currX < 8)
                {
                    if (board[currY][currX] == nullptr || board[currY][currX]->color != this->color)
                        moves.push_back(std::make_tuple(currX, currY));
                    if (board[currY][currX] != nullptr) // there is a piece, we can't go further
                            break;
                    currY += yInc;
                    currX += xInc;
                }
            }
            else // -,+ and +,-
            {
                int xInc = -yInc;
                int currX = x + xInc; // add inc so it doesn't start on top of itself
                int currY = y + yInc;

                // while (currY * yInc <= (int)(3.5 + 3.5 * yInc) && currX * xInc <= (int)(3.5 + 3.5 * xInc)) // while its on the board (currX<=7 -currX<=0)
                while (currY >= 0 && currY < 8 && currX >= 0 && currX < 8)
                {
                    if (board[currY][currX] == nullptr || board[currY][currX]->color != this->color) // should short-circuit if nullptr
                        moves.push_back(std::make_tuple(currX, currY));
                    if (board[currY][currX] != nullptr) // there is a piece, we can't go further
                            break;
                    currY += yInc;
                    currX += xInc;
                }
            }
        }

        if (allMoves)
            return moves;
            
        // remove moves that result in check
        std::vector<std::tuple<int, int>> finalMoves;
        for (std::tuple<int, int> t : moves)
        {
            int newX = std::get<0>(t);
            int newY = std::get<1>(t);
            std::array<std::array<Piece*, 8>,8> newBoard = board;
            Bishop* tmp = new Bishop(this->x, this->y, this->color);

            // delete newBoard[this->y][this->x];
            newBoard[this->y][this->x] = nullptr;
            tmp->x = newX;
            tmp->y = newY;
            newBoard[newY][newX] = tmp;
            if (!Piece::isCheck(newBoard, this->color))
            {
                finalMoves.push_back(std::make_tuple(newX, newY));
            }
            delete tmp;
        }
        return finalMoves;
    }

    Piece* Clone() override
    {
        Bishop* b = new Bishop(this->x, this->y, this->color);
        b->id = "b";
        return b;
    }
};

class Queen : public Piece
{
public:
    Queen(int x, int y, std::string c) : Piece(x, y, c)
    {
        id = "Q";
    }

    std::vector<std::tuple<int, int>> GetPossiblePositions(std::array<std::array<Piece*, 8>,8>& board, bool allMoves=false) override
    {
        std::vector<std::tuple<int, int>> results;

        Rook tempR(this->x, this->y, this->color);
        std::vector<std::tuple<int, int>> rResults = tempR.GetPossiblePositions(board, allMoves);

        Bishop tempB(this->x, this->y, this->color);
        std::vector<std::tuple<int, int>> bResults = tempB.GetPossiblePositions(board, allMoves);

        results.insert(results.end(), rResults.begin(), rResults.end());
        results.insert(results.end(), bResults.begin(), bResults.end());

        return results;
    }

    Piece* Clone() override
    {
        Queen* q = new Queen(this->x, this->y, this->color);
        q->id = "Q";
        return q;
    }
};

class King : public Piece
{
public:
    bool hasMoved;
    bool inCheck;

    King(int x, int y, std::string c) : Piece(x, y, c) 
    {
        hasMoved = false;
        id = "K";
        inCheck = false;
    }

    std::vector<std::tuple<int, int>> GetPossiblePositions(std::array<std::array<Piece*, 8>,8>& board, bool allMoves=false) override
    {
        std::vector<std::tuple<int, int>> moves;
        std::vector<std::tuple<int, int>> toCheck = {std::make_tuple(-1, -1), std::make_tuple(-1,  0),
                                                     std::make_tuple(-1,  1), std::make_tuple( 1, -1),
                                                     std::make_tuple( 1,  0), std::make_tuple( 1,  1),
                                                     std::make_tuple( 0, -1), std::make_tuple( 0,  1)};
        for (std::tuple<int, int> t : toCheck)
        {
            if (y + std::get<1>(t) >= 0 && y + std::get<1>(t) < 8 && x + std::get<0>(t) >= 0 && x + std::get<0>(t) < 8)
            {
                if (board[y + std::get<1>(t)][x + std::get<0>(t)] == nullptr || board[y + std::get<1>(t)][x + std::get<0>(t)]->color != this->color) // should short-circuit if nullptr
                {
                    moves.push_back(std::make_tuple(x + std::get<0>(t), y + std::get<1>(t))); // use std::get to make a copy
                }
            }
        }

        // castling
        if (!hasMoved && !inCheck)
        {   
            std::array<std::array<Piece*, 8>,8> tmpBoard;

            
            // check if board is open to castle king side
            if (board[y][x + 1] == nullptr && board[y][x + 2] == nullptr)
            {
                if (board[y][x + 3] != nullptr)
                {
                    if (board[y][x + 3]->color == this->color && board[y][x + 3]->id == "r") // if there is a rook there
                    {
                        // move king to in-between spot to see if castling thru check
                        tmpBoard = board;
                        tmpBoard[y][x + 1] = this;
                        tmpBoard[y][x] = nullptr;

                        if (!((Rook*)(board[y][x + 3]))->hasMoved && !this->CheckForChecks(tmpBoard, this->color)) // there are prob too many parens here
                            moves.push_back(std::make_tuple(x + 2, y));
                    }
                }
            }
            if (board[y][x - 1] == nullptr && board[y][x - 2] == nullptr && board[y][x - 3] == nullptr)
            {
                if (board[y][x - 4] != nullptr)
                {
                    if (board[y][x - 4]->color == this->color && board[y][x - 4]->id == "r")
                    {
                        // move king to in-between spot to see if castling thru check
                        tmpBoard = board;
                        tmpBoard[y][x - 1] = this;
                        tmpBoard[y][x] = nullptr;

                        if (!((Rook*)(board[y][x - 4]))->hasMoved && !this->CheckForChecks(tmpBoard, this->color)) // there are prob too many parens here
                            moves.push_back(std::make_tuple(x - 2, y));
                    }
                }
            }
        }

        if (allMoves)
            return moves;
            
        // remove moves that result in check
        std::vector<std::tuple<int, int>> finalMoves;
        for (std::tuple<int, int> t : moves)
        {
            int newX = std::get<0>(t);
            int newY = std::get<1>(t);
            std::array<std::array<Piece*, 8>,8> newBoard = board;
            King* tmp = new King(this->x, this->y, this->color);
            tmp->hasMoved = this->hasMoved;
            tmp->inCheck = this->inCheck;
            Rook* tmpR = new Rook(-1, -1, this->color); // just put something here to make sure it can be deleted later
            if (abs(this->x - newX) == 2) // castling - the king moved 2 squares
            {
                // move the rook

                if (newX == 6) // castled king side
                {
                    delete tmpR; // delete the default "filler" rook
                    tmpR = new Rook(newBoard[this->y][7]->x, newBoard[this->y][7]->y, newBoard[this->y][7]->color);
                    tmpR->hasMoved = ((Rook*)(newBoard[this->y][7]))->hasMoved;
                    // delete newBoard[this->y][7];
                    newBoard[this->y][7] = nullptr;
                    newBoard[this->y][5] = tmpR;
                    tmpR->x = 5;
                    tmpR->hasMoved = true;
                }
                else if (newX == 2)
                {
                    delete tmpR; // delete the default "filler" rook
                    tmpR = new Rook(newBoard[this->y][0]->x, newBoard[this->y][0]->y, newBoard[this->y][0]->color);
                    tmpR->hasMoved = ((Rook*)(newBoard[this->y][0]))->hasMoved;
                    // delete newBoard[this->y][7];
                    newBoard[this->y][0] = nullptr;
                    newBoard[this->y][3] = tmpR;
                    tmpR->x = 3;
                    tmpR->hasMoved = true;
                }
            }
            tmp->hasMoved = true;

            // delete newBoard[this->y][this->x];
            newBoard[this->y][this->x] = nullptr;
            tmp->x = newX;
            tmp->y = newY;
            newBoard[newY][newX] = tmp;

            if (!Piece::isCheck(newBoard, this->color))
            {
                finalMoves.push_back(std::make_tuple(newX, newY));
            }
            delete tmp;
            delete tmpR;
        }
        return finalMoves;
    }

    bool CheckForChecks(std::array<std::array<Piece*, 8>,8> b, std::string color)
    {
        // THIS SHOULD PROB BE NOT USED, PROB SHOULD USE PIECE::ISCHECK INSTEAD
        // so this is copy-pasted from Game.CheckForChecks(b, color) prob better way than having the same code 2x
        std::vector<std::tuple<int, int>> allMoves;
        int kingX;
        int kingY;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (b[i][j] != nullptr)
                {
                    if (b[i][j]->color != color)
                    {
                        std::vector<std::tuple<int, int>> moves = b[i][j]->GetPossiblePositions(b, true);
                        allMoves.insert(allMoves.end(), moves.begin(), moves.end());
                    }
                    else if (b[i][j]->id == "K") // this team's king
                    {
                        kingX = j;
                        kingY = i;
                    }
                }
            }
        }
        for (std::tuple<int, int> t : allMoves)
        {
            if (std::get<0>(t) == kingX && std::get<1>(t) == kingY)
                return true;
        }
        return false;
    }

    Piece* Clone() override
    {
        King* k = new King(this->x, this->y, this->color);
        k->id = "K";
        k->hasMoved = this->hasMoved;
        k->inCheck = this->inCheck;
        return k;
    }
};

std::string Piece::boardToFEN(std::array<std::array<Piece*, 8>,8>& board, std::string currTurn)
    {
        std::string result;
        std::string enPassant = "-";
        for (int i = 0; i < 8; i++)
        {
            int numBlankSquares = 0;
            for (int j = 0; j < 8; j++)
            {
                if (board[i][j] != nullptr)
                {
                    if (board[i][j]->id == "p" && ((Pawn*)(board[i][j]))->canBeEnPassant)
                    {
                        if (enPassant == "-")
                            enPassant = std::to_string(board[i][j]->x) + "," + std::to_string(board[i][j]->y);
                        else
                            enPassant += std::to_string(board[i][j]->x) + "," + std::to_string(board[i][j]->y);
                    }
                    if (numBlankSquares != 0)
                        result += std::to_string(numBlankSquares);
                    numBlankSquares = 0;
                    std::string piece = board[i][j]->id;
                    if (piece == "k")
                        piece = "n"; // knights are n in FEN notation
                    else if (piece == "K")
                        piece = "k";
                    else if (piece == "Q")
                        piece = "q";
                    if (board[i][j]->color == "w") // white pieces are uppercase
                        piece[0] = std::toupper(piece[0]);
                    result += piece;
                }
                else
                {
                    numBlankSquares++;
                    if (j == 7)
                        result += std::to_string(numBlankSquares);
                }
            }
            if (i != 7) // no slash at end
                result += "/";
        }
        result += " " + currTurn;

        result += " ";
        std::string castling = "-";
        if (board[7][4] != nullptr && board[7][4]->id == "K" && !((King*)(board[7][4]))->hasMoved)
        {
            if (board[7][7] != nullptr && board[7][7]->id == "r" && !((Rook*)(board[7][7]))->hasMoved)
            {
                if (castling == "-") castling = "";
                result += "K"; // white can castle kingside
            }
            if (board[7][0] != nullptr && board[7][0]->id == "r" && !((Rook*)(board[7][0]))->hasMoved)
            {
                if (castling == "-") castling = "";
                result += "Q"; // white can castle queenside
            }
        }
        if (board[0][4] != nullptr && board[0][4]->id == "K" && !((King*)(board[0][4]))->hasMoved)
        {
            if (board[0][7] != nullptr && board[0][7]->id == "r" && !((Rook*)(board[0][7]))->hasMoved)
            {
                if (castling == "-") castling = "";
                result += "k"; // black can castle kingside
            }
            if (board[0][0] != nullptr && board[0][0]->id == "r" && !((Rook*)(board[0][0]))->hasMoved)
            {
                if (castling == "-") castling = "";
                result += "q"; // black can castle queenside
            }
        }
        result += castling;

        result += " " + enPassant;
        return result;
    }
// int main()
// {


//     std::array<std::array<Piece*, 8>,8> board;

//     board = {{  {{new Rook(0, 0, "b"), new Knight(1, 0, "b"), new Bishop(2, 0, "b"), new Queen(3, 0, "b"), new King(4, 0, "b"), new Bishop(5, 0, "b"), new Knight(6, 0, "b"), new Rook(7, 0, "b")}},
//                     {{nullptr, new Pawn(1, 1, "b"), new Pawn(2, 1, "b"), new Pawn(3, 1, "b"), new Pawn(4, 1, "b"), new Pawn(5, 1, "b"), new Pawn(6, 1, "b"), new Pawn(7, 1, "b")}},
//                     {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
//                     {{new Pawn(0, 3, "b"), nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
//                     {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
//                     {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
//                     {{new Pawn(0, 6, "w"), new Pawn(1, 6, "w"), new Pawn(2, 6, "w"), new Pawn(3, 6, "w"), new Pawn(4, 6, "w"), new Pawn(5, 6, "w"), new Pawn(6, 6, "w"), new Pawn(7, 6, "w")}},
//                     {{new Rook(0, 7, "w"), new Knight(1, 7, "w"), new Bishop(2, 7, "w"), new Queen(3, 7, "w"), new King(4, 7, "w"), new Bishop(5, 7, "w"), new Knight(6, 7, "w"), new Rook(7, 7, "w")}} }};

//     ((Pawn*)(board[3][0]))->canBeEnPassant = true;
//     ((King*)(board[7][4]))->hasMoved = true;
//     ((King*)(board[0][4]))->hasMoved = true;
//     std::cout << Piece::boardToFEN(board, "w");
// }