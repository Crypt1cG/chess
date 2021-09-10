#include "game.hpp"
#pragma once

class Ai
{
public:
    static const int pawnVal   = 100;
    static const int knightVal = 300;
    static const int bishopVal = 300;
    static const int rookVal   = 500;
    static const int queenVal  = 900;
    static const int DEPTH = 4;

    // for evaluation
    //TODO: check these... should match what's found in res/maps
    static const U64 knightBonusMap1 = 0b0011110001111110111111111111111111111111111111110111111000111100;
    static const U64 knightBonusMap2 = 0b0000000000111100011111100111111001111110011111100011110000000000;
    static const U64 knightBonusMap3 = 0b0000000000000000001111000011110000111100001111000000000000000000;

    static const U64 queenBonusMap1 = 0b0111111011111111111111111111111111111111111111111111111101111110;
    static const U64 queenBonusMap2 = 0b0001100001111110011111101111111111111111011111100111111000011000;
    static const U64 queenBonusMap3 = 0b0000000000000000001111000011110000111100001111000000000000000000;

    // color dependent ones
    static const U64 whiteKingMap1 = 0b1111111111111111111111111110011110000001100000010000000000000000;
    static const U64 whiteKingMap2 = 0b1100001111000011000000000000000000000000000000000000000000000000;

    static const U64 blackKingMap1 = 0b0000000000000000100000011000000111100111111111111111111111111111;
    static const U64 blackKingMap2 = 0b0000000000000000000000000000000000000000000000001100001111000011;

    static const U64 whiteRookMap1 = 0b1111111101111110011111100111111001111110011111101111111111111111;
    static const U64 whiteRookMap2 = 0b0001100000000000000000000000000000000000000000000111111000000000;

    static const U64 blackRookMap1 = 0b1111111111111111011111100111111001111110011111100111111011111111;
    static const U64 blackRookMap2 = 0b0000000001111110000000000000000000000000000000000000000000011000;

    static const U64 whiteBishopMap1 = 0b1111111111111111111111111111111111111111111111111111111101111110;
    static const U64 whiteBishopMap2 = 0b1000000101111110011111100111111001111110011111100111111000000000;
    static const U64 whiteBishopMap3 = 0b0000000001000010011001100011110000011000000110000000000000000000;

    static const U64 blackBishopMap1 = 0b0111111011111111111111111111111111111111111111111111111111111111;
    static const U64 blackBishopMap2 = 0b0000000001111110011111100111111001111110011111100111111010000001;
    static const U64 blackBishopMap3 = 0b0000000000000000000110000001100000111100011001100100001000000000;

    static const U64 whitePawnMap1 = 0b0000000011100111110110111111111111111111111111111111111100000000;
    static const U64 whitePawnMap2 = 0b0000000001100110000000000001100000011000001111001111111100000000;
    static const U64 whitePawnMap3 = 0b0000000000000000000000000000000000000000000000001111111100000000;

    static const U64 blackPawnMap1 = 0b0000000011111111111111111111111111111111110110111110011100000000;
    static const U64 blackPawnMap2 = 0b0000000011111111001111000001100000011000000000000110011000000000;
    static const U64 blackPawnMap3 = 0b0000000011111111000000000000000000000000000000000000000000000000;

    Game game;

    Ai();
    Ai(std::string fen);

    U64 countPositions(int color, int depth);
    int evaluate(Position p);
    int alphaBeta(int depth, int alpha, int beta, int color);
    int quiescenceSearch(int alpha, int beta, int color);
    int staticExchangeEval(int square, int color);
    int SEECapture(Move move, int color);
    void makeBestMoveAB(int color);
};