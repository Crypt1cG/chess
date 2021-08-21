#include <string>
#pragma once

typedef unsigned long long U64;

/**
 * this class is supposed to be like a real chess board, just giving
 * information about where pieces are
 * 
 */
class Position
{
    // use __builtin_popcountll() to count num of 1s in a bitboard

    U64 whitePieces; // contains all white pieces
    U64 blackPieces; // contains all black pieces
    U64 occupency; // contains all pieces
    U64 whitePawns;
    U64 blackPawns;
    U64 whiteKnights;
    U64 blackKnights;
    U64 whiteBishops;
    U64 blackBishops;
    U64 whiteRooks;
    U64 blackRooks;
    U64 whiteQueens;
    U64 blackQueens;
    U64 whiteKing;
    U64 blackKing;

    /**
     * Constructor - creates a position from a given fen string
     * 
     * @param fen the fen string
     */
    Position(std::string fen);

    /**
     * Default constructor - uses the starting position fen to make a position object
     */
    Position();

    
};