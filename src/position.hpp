#include <string>
#include <bitset>
#include <iostream>
#include <array>
#pragma once

typedef unsigned long long U64;

/**
 * this class is supposed to be like a real chess board, just giving
 * information about where pieces are
 * 
 */
class Position
{
public:
    // use __builtin_popcountll() to count num of 1s in a bitboard
    static const int whiteID = 0;
    static const int blackID = 1;
    // useful for accessing bitboards array
    static const int pawnIndex = 0;
    static const int knightIndex = 1;
    static const int bishopIndex = 2;
    static const int rookIndex = 3;
    static const int queenIndex = 4;
    static const int kingIndex = 5;

    // for all of the following, the least significant bit is index 0
    U64 whitePieces = 0; // contains all white pieces
    U64 blackPieces = 0; // contains all black pieces
    U64 occupency = 0; // contains all pieces
    U64 whitePawns = 0;
    U64 blackPawns = 0;
    U64 whiteKnights = 0;
    U64 blackKnights = 0;
    U64 whiteBishops = 0;
    U64 blackBishops = 0;
    U64 whiteRooks = 0;
    U64 blackRooks = 0;
    U64 whiteQueens = 0;
    U64 blackQueens = 0;
    U64 whiteKing = 0;
    U64 blackKing = 0;
    U64 enPassant = 0; // if en passant is possible, this will have the square behind the en passant-able pawn set
    int castling = 0; // 0bKQkq
    int currTurn = 0;
    std::array<U64*, 15> bitboards;

    /**
     * Constructor - creates a position from a given fen string
     * 
     * @param fen the fen string
     */
    Position(std::string fen);

    bool canCastle(int color, char side);
    int at(int pos);

    /**
     * Default constructor - uses the starting position fen to make a position object
     */
    Position();

    /**
     * prints the current position
     */
    void print();
};