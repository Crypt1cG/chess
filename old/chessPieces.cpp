#include <array>
#include <vector>
#include <iostream> // for testing
#include <tuple>
#include "chessPieces.h"

class Piece //TODO: add constructor for x, y, color
{
public:
    int x, y;
    std::string color;
    Piece(int x, int y, std::string c)
    {
        this->x = x;
        this->y = y;
        color = c;
    }
    virtual std::vector<std::tuple<int, int>> GetPossiblePositions(std::array<std::array<std::string, 8>,8>& board)
    {
        std::vector<std::tuple<int, int>> results;
        return results;
    }
};

class Pawn : public Piece
{
public:
    bool hasMoved;

    Pawn(int x, int y, std::string c) : Piece(x, y, c)
    {
        hasMoved = false;
    }

    std::vector<std::tuple<int, int>> GetPossiblePositions(std::array<std::array<std::string, 8>,8>& board) override
    {
        //TODO: add chekcs for index out of bounds
        std::vector<std::tuple<int, int>> results;
        int dir;
        if (this->color == "w") // going towards y=0 (negative direction)
            dir = -1;
        else dir = 1;

        if (y + dir >= 0 && y + dir < 8 && board[y + dir][x] == " ") // use short-circuiting to prevent going off board
            results.push_back(std::make_tuple(x, y + dir));

        for (int i = 0; i < 3; i += 2)
        {   
            if (y + dir >= 0 && y + dir < 8 && x - 1 + i >= 0 && x - 1 + i < 8 && board[y + dir][x - 1 + i] != " ")
            {
                if (board[y + dir][x - 1 + i].substr(0, 1) != this->color) // can capture
                    results.push_back(std::make_tuple(x - 1 + i, y + dir));
            }
        }
        if (!hasMoved && y + 2 * dir >= 0 && y + 2 * dir < 8)
        {
            if (board[y + 2 * dir][x] == " " && board[y + dir][x] == " ")
                results.push_back(std::make_tuple(x, y + 2 * dir));
        }
        //TODO: add en passant
        return results;
    }
};

class Knight : public Piece
{
public:
    Knight(int x, int y, std::string c) : Piece(x, y, c) { }
    
    std::vector<std::tuple<int, int>> GetPossiblePositions(std::array<std::array<std::string, 8>,8>& board) override
    //TODO: add chekcs for index out of bounds
    {
        std::vector<std::tuple<int, int>> results;

        int d; //TODO: better variable name
        for (int i = 0; i < 4; i++)
        {
            if (i < 2) d = 2;
            else d = -2; 
            if (i % 2 == 0) // top/bottom directions
            {
                for (int j = -1; j < 3; j += 2)
                {
                    if (y + d >= 0 && y + d < 8 && x + j >= 0 && x + j < 8)
                    {
                        if (board[y + d][x + j].substr(0, 1) != this->color)
                            results.push_back(std::make_tuple(x + j, y + d));
                    }
                }
            }
            else
            {
                for (int j = -1; j < 3; j += 2)
                {
                    if (y + j >= 0 && y + j < 8 && x + d >= 0 && x + d < 8)
                    {
                        if (board[y + j][x + d].substr(0, 1) != this->color)
                            results.push_back(std::make_tuple(x + d, y + j));
                    }
                }
            }
        }

        return results;
    }
};

class Rook : public Piece
{
public:
    Rook(int x, int y, std::string c) : Piece(x, y, c) { }

    std::vector<std::tuple<int, int>> GetPossiblePositions(std::array<std::array<std::string, 8>,8>& board) override
    //TODO: add chekcs for index out of bounds
    {
        std::vector<std::tuple<int, int>> results;

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
                    if (board[currY][x].substr(0, 1) != this->color)
                        results.push_back(std::make_tuple(x, currY));
                    if (board[currY][x].substr(0, 1) != " ") // there is a piece, we can't go further
                            break;
                    currY += inc;
                }
            }
            else // change x
            {
                int currX = x + inc; // add inc so it doesn't start on top of itself

                while (currX * inc <= (int)(3.5 + 3.5 * inc)) // while its on the board (currX<=7 -currX<=0)
                {
                    if (board[y][currX].substr(0, 1) != this->color)
                        results.push_back(std::make_tuple(currX, y));
                    if (board[y][currX].substr(0, 1) != " ") // there is a piece, we can't go further
                            break;
                    currX += inc;
                }
            }
        }
        return results;
    }
};

class Bishop : public Piece
{
public:
    Bishop(int x, int y, std::string c) : Piece(x, y, c) { }

    std::vector<std::tuple<int, int>> GetPossiblePositions(std::array<std::array<std::string, 8>,8>& board) override
    //TODO: add chekcs for index out of bounds
    {
        std::vector<std::tuple<int, int>> results;

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

                while (currY * yInc <= (int)(3.5 + 3.5 * yInc) && currX * xInc <= (int)(3.5 + 3.5 * xInc)) // while its on the board (currY<=7 -currY<=0)
                {
                    if (board[currY][currX].substr(0, 1) != this->color)
                        results.push_back(std::make_tuple(currX, currY));
                    if (board[currY][currX].substr(0, 1) != " ") // there is a piece, we can't go further
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

                while (currY * yInc <= (int)(3.5 + 3.5 * yInc) && currX * xInc <= (int)(3.5 + 3.5 * xInc)) // while its on the board (currX<=7 -currX<=0)
                {
                    if (board[currY][currX].substr(0, 1) != this->color)
                        results.push_back(std::make_tuple(currX, currY));
                    if (board[currY][currX].substr(0, 1) != " ") // there is a piece, we can't go further
                            break;
                    currY += yInc;
                    currX += xInc;
                }
            }
        }
        return results;
    }
};

class Queen : public Piece
{
public:
    Queen(int x, int y, std::string c) : Piece(x, y, c) { }

    std::vector<std::tuple<int, int>> GetPossiblePositions(std::array<std::array<std::string, 8>,8>& board) override
    {
        std::vector<std::tuple<int, int>> results;

        Rook tempR(this->x, this->y, this->color);
        std::vector<std::tuple<int, int>> rResults = tempR.GetPossiblePositions(board);

        Bishop tempB(this->x, this->y, this->color);
        std::vector<std::tuple<int, int>> bResults = tempB.GetPossiblePositions(board);

        results.insert(results.end(), rResults.begin(), rResults.end());
        results.insert(results.end(), bResults.begin(), bResults.end());

        return results;
    }
};

class King : public Piece
{
public:
    King(int x, int y, std::string c) : Piece(x, y, c) { }

    std::vector<std::tuple<int, int>> GetPossiblePositions(std::array<std::array<std::string, 8>,8>& board) override
    {
        std::vector<std::tuple<int, int>> results;
        std::vector<std::tuple<int, int>> toCheck = {std::make_tuple(-1, -1), std::make_tuple(-1,  0),
                                                     std::make_tuple(-1,  1), std::make_tuple( 1, -1),
                                                     std::make_tuple( 1,  0), std::make_tuple( 1,  1),
                                                     std::make_tuple( 0, -1), std::make_tuple( 0,  1)};
        for (std::tuple<int, int> t : toCheck)
        {
            if (y + std::get<1>(t) >= 0 && y + std::get<1>(t) < 8 && x + std::get<0>(t) >= 0 && x + std::get<0>(t) < 8)
            {
                if (board[y + std::get<1>(t)][x + std::get<0>(t)].substr(0, 1) != this->color)
                {
                    results.push_back(std::make_tuple(x + std::get<0>(t), y + std::get<1>(t))); // use std::get to make a copy
                }
            }
        }
        return results;
    }
};
// int main()
// {
//     Bishop b(5, 2, "w");


//     std::array<std::array<std::string, 8>,8> board;

//     board = {{  {{" ", " ", " ", " ", " ", " ", " ", " "}},
//                 {{" ", " ", " ", " ", " ", " ", " ", " "}},
//                 {{" ", " ", " ", " ", " ", "wb", " ", " "}},
//                 {{" ", " ", " ", " ", " ", " ", " ", " "}},
//                 {{" ", " ", " ", " ", " ", " ", " ", " "}},
//                 {{" ", " ", " ", " ", " ", " ", " ", " "}},
//                 {{" ", " ", " ", " ", " ", " ", " ", " "}},
//                 {{" ", " ", " ", " ", " ", " ", " ", " "}} }};

//     std::vector<std::tuple<int, int>> results = b.GetPossiblePositions(board);

//     for (int i = 0; i < results.size(); i++)
//     {
//         std::cout << std::get<0>(results[i]) << std::get<1>(results[i]) << std::endl;
//     }
// }