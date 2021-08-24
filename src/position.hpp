#include <string>
#include <bitset>
#include <iostream>
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
    int enPassant = 0;
    int castling = 0; // 0bKQkq
    int currTurn = 0;

    /**
     * Constructor - creates a position from a given fen string
     * 
     * @param fen the fen string
     */
    Position(std::string fen);

    bool canCastle(int color, char side);

    /**
     * Default constructor - uses the starting position fen to make a position object
     */
    Position();

    /**
     * prints the current position
     */
    void print();
};