#include <array>
#include <vector>
#include <string>
#include <stack>
#pragma once

class Move
{
public:
    int from;
    int to;

    Move(int f, int t);
};

class Board
{
public:
    static const int empty = 0;
    static const int pawnID = 1;
    static const int knightID = 2;
    static const int bishopID = 3;
    static const int rookID = 4;
    static const int queenID = 5;
    static const int kingID = 6;

    static const int whiteID = 0;
    static const int blackID = 1;

    // the following are lists of positions (not offsets) representing the
    // squares that can be moved to by a piece at a position (don't ask why they're inline)
    inline static std::array<std::vector<int>, 64> knightMoves;
    inline static std::array<std::vector<int>, 64> kingMoves;
    inline static std::array<std::array<std::vector<int>, 4>, 64> rookMoves;
    inline static std::array<std::array<std::vector<int>, 4>, 64> bishopMoves;

    std::array<int, 64> squares;
    int currTurn; // whiteID or blackID
    int castling; // 4 bits KQkq
    int enPassant; // 8 bits, 1 for each file
    int blackKingPos; // yyyxxx
    int whiteKingPos; // yyyxxx
    std::stack<std::pair<int, int>> moves; // yyyxxxci_d

    static int makePiece(int pos, int color, int id);
    // static int getPos(int piece);
    // static int getX(int piece);
    // static int getY(int piece);
    static int getColor(int piece);
    static int getID(int piece);
    // static void setX(int& piece, int x);
    // static void setY(int& piece, int y);
    bool canCastle(char side, int color);
    bool canBeEnPassant(int file);
    std::vector<Move> getMoves(int pos, bool pseudoLegal=false);
    bool isCheck(int color);
    void movePiece(Move move);
    void unMakeMove();
    Board(std::string fen);
    Board();
    void print();
    static void initMoves();
    static std::string indexToAlg(int index);
};