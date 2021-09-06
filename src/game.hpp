#include "position.hpp"
#include <vector>
#include "move.hpp"

#pragma once

/**
 * used to play a game of chess by either an ai or ui
 */
class Game
{
public:
    static const U64 knightAttacks[64];
    static const U64 kingAttacks[64];
    static const U64 rayAttacks[64][8]; // 0, 2, 4, 6 are diagonals, 1, 3, 5, 7 are straight lines. 0 is -x +y (+7), 1 is +y (+8), 2 is +x +y (+9), 3 is +x (+1), 4 is +x -y (-7), 5 is -y (-8), 6 is -x -y (-9), 7 is -x (-1)

    Position position;
    Game();
    Game(std::string fen);

    std::vector<Move> getAllMoves(int color, bool pseudoLegal=false);
    std::vector<Move> getAllCaptureMoves(int color, bool pseudoLegal=false);

    std::vector<Move> getAllKingMoves(int color);

    std::vector<Move> getAllBishopMoves(int color);

    std::vector<Move> getAllRookMoves(int color);

    std::vector<Move> getAllQueenMoves(int color);

    U64 posRayAttacks(int pos, int dir, U64 friendlyPieces);
    U64 negRayAttacks(int pos, int dir, U64 freindlyPieces);

    std::vector<Move> getAllKnightMoves(int color);
    // should really re name this bc it's used for king as well
    std::vector<Move> knightAttacksToMoves(U64 attacks, int pos, int piece);

    /* pawn stuff */

    /**
     * returns all (pseudolegal) pawn moves for a given color
     */
    std::vector<Move> getAllPawnMoves(int color);
    U64 pawnPushTargets(U64 pawns, int color);
    U64 pawnDblPushTargets(int color);
    /**
     * all attacked squares by color's pawns
     * direction (r/l) is from the pawn's pov 
     */
    U64 pawnRAttacks(int color);
    U64 pawnLAttacks(int color);
    
    /**
     * turns a U64 of targets into actual moves (uses offset to find original position)
     * 
     * @param targets an unsigned long long bitset of targets
     * @param offset the shift required to go from the targets to the original position
     */
    void pawnTargetsToMoves(U64 targets, int offset, std::vector<Move>& moves);


    bool isCheck(int color);
    bool RQBCheck(int color);
    bool moveCausesCheck(Move& m, int color);
    void movePiece(Move move);
    void movePieceNoBranching(Move move);
    /**
     * unsets a given position in all bitboards
     * 
     * @param pos the position to be cleared
     */
    void clearSquare(int pos);

    static std::string indexToAlg(int index);
};