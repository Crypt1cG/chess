// TODO: castling, legal moves, en passant
#include "game.hpp"

Game::Game() : position() {};
Game::Game(std::string fen) : position(fen) {};

std::vector<Move> Game::getAllMoves(int color, bool pseudoLegal)
{
    std::vector<Move> results;
    std::vector<Move> queenMoves = getAllQueenMoves(color);
    std::vector<Move> rookMoves = getAllRookMoves(color);
    std::vector<Move> bishopMoves = getAllBishopMoves(color);
    std::vector<Move> knightMoves = getAllKnightMoves(color);
    std::vector<Move> kingMoves = getAllKingMoves(color);
    std::vector<Move> pawnMoves = getAllPawnMoves(color);
    results.reserve(queenMoves.size() + rookMoves.size() + bishopMoves.size() + knightMoves.size() + kingMoves.size() + pawnMoves.size());
    results.insert(results.end(), pawnMoves.begin(), pawnMoves.end());
    results.insert(results.end(), queenMoves.begin(), queenMoves.end());
    results.insert(results.end(), rookMoves.begin(), rookMoves.end());
    results.insert(results.end(), bishopMoves.begin(), bishopMoves.end());
    results.insert(results.end(), knightMoves.begin(), knightMoves.end());
    results.insert(results.end(), kingMoves.begin(), kingMoves.end());

    if (pseudoLegal) return results;

    Position original = position;
    std::vector<Move> finalMoves;
    finalMoves.reserve(results.size());
    for (Move& m : results)
    {
        movePiece(m);
        // movePieceNoBranching(m);
        if (!isCheck(color))
            finalMoves.emplace_back(m.from, m.to, m.piece);
        position = original;
    }
    return finalMoves;
}

std::vector<Move> Game::getAllCaptureMoves(int color, bool pseudoLegal)
{
    std::vector<Move> results;
    U64 opPieces = *(position.bitboards[12 + !color]); // [12] is white pieces, [13] is black

    /* pawn stuff */
    std::vector<Move> pawnMoves;
    pawnTargetsToMoves(pawnRAttacks(color), (-(color * 2) + 1) * 7, pawnMoves);
    pawnTargetsToMoves(pawnLAttacks(color), (-(color * 2) + 1) * 9, pawnMoves);

    /* knight stuff */
    std::vector<Move> knightMoves;
    U64 knights = *(position.bitboards[Position::knightIndex + color *  6]);
    int numKnights = __builtin_popcountll(knights);
    int pos = -1;
    for (int i = 0; i < numKnights; i++)
    {
        int ind = __builtin_ffsll(knights); // returns 1 + index of LSB
        knights >>= ind;
        pos += ind;
        U64 targets = Game::knightAttacks[pos];
        targets &= opPieces; // only captures
        std::vector<Move> moves = knightAttacksToMoves(targets, pos, Position::knightIndex);
        knightMoves.insert(knightMoves.end(), moves.begin(), moves.end());
    }
    
    /* king stuff */
    int kingPos = __builtin_ffsll(*(position.bitboards[Position::kingIndex + 6 * color])) - 1;
    U64 targets = Game::kingAttacks[kingPos];
    targets &= opPieces;
    std::vector<Move> kingMoves = knightAttacksToMoves(targets, kingPos, Position::kingIndex);

    /* diag stuff */
    U64 diagAttackers = *(position.bitboards[Position::queenIndex + 6 * color]) | *(position.bitboards[Position::bishopIndex + 6 * color]);
    std::vector<Move> diagAttacks;
    int numAttackers = __builtin_popcountll(diagAttackers);
    pos = -1;
    for (int i = 0; i < numAttackers; i++)
    {
        int ind = __builtin_ffsll(diagAttackers); // returns 1 + index of LSB
        diagAttackers >>= ind;
        pos += ind;
        int type = (1ull << pos) & *(position.bitboards[Position::queenIndex + 6 * color]) ? Position::queenIndex : Position::bishopIndex;
        
        U64 negPos = rayAttacks[pos][0];
        U64 posPos = rayAttacks[pos][2];
        U64 posNeg = rayAttacks[pos][4];
        U64 negNeg = rayAttacks[pos][6];

        // get intersection with enemy pieces
        negPos &= position.occupency;
        posPos &= position.occupency;
        posNeg &= position.occupency;
        negNeg &= position.occupency;

        if (negPos != 0)
        {
            int intPos = __builtin_ffsll(negPos) - 1; // index of first piece intersected by ray
            negPos ^= Game::rayAttacks[intPos][0]; // chop off the rest of the ray
            if (negPos & opPieces) // it intersects the enemy pieces, so it is a capture
                diagAttacks.push_back(Move(pos, intPos, type));
        }
        if (posPos != 0)
        {
            int intPos = __builtin_ffsll(posPos) - 1; // index of first piece intersected by ray
            posPos ^= Game::rayAttacks[intPos][2]; // chop off the rest of the ray
            if (posPos & opPieces) // it intersects the enemy pieces, so it is a capture
                diagAttacks.push_back(Move(pos, intPos, type));
        }
        if (posNeg != 0)
        {
            int intPos = 63 - __builtin_clzll(posNeg); // posNeg is negative overall, first intersection will be MSB
            posNeg ^= Game::rayAttacks[intPos][4]; // chop off the rest of the ray
            if (posNeg & opPieces) // it intersects the enemy pieces, so it is a capture
                diagAttacks.push_back(Move(pos, intPos, type));
        }
        if (negNeg != 0)
        {
            int intPos = 63 - __builtin_clzll(negNeg); // posNeg is negative overall, first intersection will be MSB
            negNeg ^= Game::rayAttacks[intPos][6]; // chop off the rest of the ray
            if (negNeg & opPieces) // it intersects the enemy pieces, so it is a capture
                diagAttacks.push_back(Move(pos, intPos, type));
        }
    }

    /* straight line ray stuff (cardinal directions) */
    U64 cardAttackers = *(position.bitboards[Position::queenIndex + 6 * color]) | *(position.bitboards[Position::rookIndex + 6 * color]);
    std::vector<Move> cardAttacks;
    numAttackers = __builtin_popcountll(cardAttackers);
    pos = -1;
    for (int i = 0; i < numAttackers; i++)
    {
        int ind = __builtin_ffsll(cardAttackers); // returns 1 + index of LSB
        cardAttackers >>= ind;
        pos += ind;
        int type = (1ull << pos) & *(position.bitboards[Position::queenIndex + 6 * color]) ? Position::queenIndex : Position::rookIndex;

        U64 posY = rayAttacks[pos][1];
        U64 posX = rayAttacks[pos][3];
        U64 negY = rayAttacks[pos][5];
        U64 negX = rayAttacks[pos][7];

        // get intersection with enemy pieces
        posY &= position.occupency;
        posX &= position.occupency;
        negY &= position.occupency;
        negX &= position.occupency;

        if (posY != 0)
        {
            int intPos = __builtin_ffsll(posY) - 1; // index of first piece intersected by ray
            posY ^= Game::rayAttacks[intPos][1]; // chop off the rest of the ray
            if (posY & opPieces) // it intersects the enemy pieces, so it is a capture
                cardAttacks.push_back(Move(pos, intPos, type));
        }
        if (posX != 0)
        {
            int intPos = __builtin_ffsll(posX) - 1; // index of first piece intersected by ray
            posX ^= Game::rayAttacks[intPos][3]; // chop off the rest of the ray
            if (posX & opPieces) // it intersects the enemy pieces, so it is a capture
                cardAttacks.push_back(Move(pos, intPos, type));
        }
        if (negY != 0)
        {
            int intPos = 63 - __builtin_clzll(negY); // index of first piece intersected by ray
            negY ^= Game::rayAttacks[intPos][5]; // chop off the rest of the ray
            if (negY & opPieces) // it intersects the enemy pieces, so it is a capture
                cardAttacks.push_back(Move(pos, intPos, type));
        }
        if (negX != 0)
        {
            int intPos = 63 - __builtin_clzll(negX); // index of first piece intersected by ray
            negX ^= Game::rayAttacks[intPos][7]; // chop off the rest of the ray
            if (negX & opPieces) // it intersects the enemy pieces, so it is a capture
                cardAttacks.push_back(Move(pos, intPos, type));
        }
    }

    results.reserve(pawnMoves.size() + knightMoves.size() + kingMoves.size() + diagAttacks.size() + cardAttacks.size());
    results.insert(results.end(), pawnMoves.begin(), pawnMoves.end());
    results.insert(results.end(), knightMoves.begin(), knightMoves.end());
    results.insert(results.end(), kingMoves.begin(), kingMoves.end());
    results.insert(results.end(), diagAttacks.begin(), diagAttacks.end());
    results.insert(results.end(), cardAttacks.begin(), cardAttacks.end());
    return results;
}

std::vector<Move> Game::getSEECaptures(int square, int color)
{
    /* these moves are ordered by piece value ascending (pawn captures first, king last) */
    U64 opPieces = *(position.bitboards[12 + !color]); // [12] is white pieces, [13] is black
    U64 targetSquare = 1ull << square;
    std::vector<Move> results;

    //! i dont think this does en passant moves. Oh well
    /* pawn moves */
    U64 rPawnCaptures = pawnRAttacks(color) & targetSquare;
    if (rPawnCaptures != 0)
        pawnTargetsToMoves(rPawnCaptures, (-(color * 2) + 1) * 7, results);
    U64 lPawnCaptures = pawnLAttacks(color) & targetSquare;
    if (lPawnCaptures != 0)
        pawnTargetsToMoves(lPawnCaptures, (-(color * 2) + 1) * 7, results);

    /* knight moves */
    std::vector<Move> knightMoves;
    U64 knights = *(position.bitboards[Position::knightIndex + color *  6]);
    int numKnights = __builtin_popcountll(knights);
    int pos = -1;
    for (int i = 0; i < numKnights; i++)
    {
        int ind = __builtin_ffsll(knights); // returns 1 + index of LSB
        knights >>= ind;
        pos += ind;
        U64 targets = Game::knightAttacks[pos];
        targets &= opPieces; // only captures
        targets &= targetSquare;
        if (targets != 0)
        {
            std::vector<Move> moves = knightAttacksToMoves(targets, pos, Position::knightIndex);
            knightMoves.insert(knightMoves.end(), moves.begin(), moves.end());
        }
    }
    results.insert(results.end(), knightMoves.begin(), knightMoves.end());

    /* bishop moves */
    U64 bishops = *(position.bitboards[Position::bishopIndex + 6 * color]);
    std::vector<Move> bishopAttacks;
    int numAttackers = __builtin_popcountll(bishops);
    pos = -1;
    for (int i = 0; i < numAttackers; i++)
    {
        int ind = __builtin_ffsll(bishops); // returns 1 + index of LSB
        bishops >>= ind;
        pos += ind;
        
        U64 negPos = rayAttacks[pos][0];
        U64 posPos = rayAttacks[pos][2];
        U64 posNeg = rayAttacks[pos][4];
        U64 negNeg = rayAttacks[pos][6];

        // get intersection with pieces
        negPos &= position.occupency;
        posPos &= position.occupency;
        posNeg &= position.occupency;
        negNeg &= position.occupency;

        negPos &= targetSquare;
        posPos &= targetSquare;
        posNeg &= targetSquare;
        negNeg &= targetSquare;

        if (negPos != 0)
        {
            int intPos = __builtin_ffsll(negPos) - 1; // index of first piece intersected by ray
            negPos ^= Game::rayAttacks[intPos][0]; // chop off the rest of the ray
            if (negPos & opPieces) // it intersects the enemy pieces, so it is a capture
                bishopAttacks.push_back(Move(pos, intPos, Position::bishopIndex));
        }
        if (posPos != 0)
        {
            int intPos = __builtin_ffsll(posPos) - 1; // index of first piece intersected by ray
            posPos ^= Game::rayAttacks[intPos][2]; // chop off the rest of the ray
            if (posPos & opPieces) // it intersects the enemy pieces, so it is a capture
                bishopAttacks.push_back(Move(pos, intPos, Position::bishopIndex));
        }
        if (posNeg != 0)
        {
            int intPos = 63 - __builtin_clzll(posNeg); // posNeg is negative overall, first intersection will be MSB
            posNeg ^= Game::rayAttacks[intPos][4]; // chop off the rest of the ray
            if (posNeg & opPieces) // it intersects the enemy pieces, so it is a capture
                bishopAttacks.push_back(Move(pos, intPos, Position::bishopIndex));
        }
        if (negNeg != 0)
        {
            int intPos = 63 - __builtin_clzll(negNeg); // posNeg is negative overall, first intersection will be MSB
            negNeg ^= Game::rayAttacks[intPos][6]; // chop off the rest of the ray
            if (negNeg & opPieces) // it intersects the enemy pieces, so it is a capture
                bishopAttacks.push_back(Move(pos, intPos, Position::bishopIndex));
        }
    }
    results.insert(results.end(), bishopAttacks.begin(), bishopAttacks.end());

    /* rook moves */
    U64 rooks = *(position.bitboards[Position::rookIndex + 6 * color]);
    std::vector<Move> rookAttacks;
    numAttackers = __builtin_popcountll(rooks);
    pos = -1;
    for (int i = 0; i < numAttackers; i++)
    {
        int ind = __builtin_ffsll(rooks); // returns 1 + index of LSB
        rooks >>= ind;
        pos += ind;

        U64 posY = rayAttacks[pos][1];
        U64 posX = rayAttacks[pos][3];
        U64 negY = rayAttacks[pos][5];
        U64 negX = rayAttacks[pos][7];

        // get intersection with enemy pieces
        posY &= position.occupency;
        posX &= position.occupency;
        negY &= position.occupency;
        negX &= position.occupency;

        posY &= targetSquare;
        posX &= targetSquare;
        negY &= targetSquare;
        negX &= targetSquare;

        if (posY != 0)
        {
            int intPos = __builtin_ffsll(posY) - 1; // index of first piece intersected by ray
            posY ^= Game::rayAttacks[intPos][1]; // chop off the rest of the ray
            if (posY & opPieces) // it intersects the enemy pieces, so it is a capture
                rookAttacks.push_back(Move(pos, intPos, Position::rookIndex));
        }
        if (posX != 0)
        {
            int intPos = __builtin_ffsll(posX) - 1; // index of first piece intersected by ray
            posX ^= Game::rayAttacks[intPos][3]; // chop off the rest of the ray
            if (posX & opPieces) // it intersects the enemy pieces, so it is a capture
                rookAttacks.push_back(Move(pos, intPos, Position::rookIndex));
        }
        if (negY != 0)
        {
            int intPos = 63 - __builtin_clzll(negY); // index of first piece intersected by ray
            negY ^= Game::rayAttacks[intPos][5]; // chop off the rest of the ray
            if (negY & opPieces) // it intersects the enemy pieces, so it is a capture
                rookAttacks.push_back(Move(pos, intPos, Position::rookIndex));
        }
        if (negX != 0)
        {
            int intPos = 63 - __builtin_clzll(negX); // index of first piece intersected by ray
            negX ^= Game::rayAttacks[intPos][7]; // chop off the rest of the ray
            if (negX & opPieces) // it intersects the enemy pieces, so it is a capture
                rookAttacks.push_back(Move(pos, intPos, Position::rookIndex));
        }
    }
    results.insert(results.end(), rookAttacks.begin(), rookAttacks.end());

    /* queen diag moves */
    U64 queens = *(position.bitboards[Position::queenIndex + 6 * color]);
    std::vector<Move> queenAttacks;
    int numAttackers = __builtin_popcountll(queens);
    pos = -1;
    for (int i = 0; i < numAttackers; i++)
    {
        int ind = __builtin_ffsll(queens); // returns 1 + index of LSB
        queens >>= ind;
        pos += ind;
        
        U64 negPos = rayAttacks[pos][0];
        U64 posPos = rayAttacks[pos][2];
        U64 posNeg = rayAttacks[pos][4];
        U64 negNeg = rayAttacks[pos][6];

        // get intersection with enemy pieces
        negPos &= position.occupency;
        posPos &= position.occupency;
        posNeg &= position.occupency;
        negNeg &= position.occupency;

        negPos &= targetSquare;
        posPos &= targetSquare;
        posNeg &= targetSquare;
        negNeg &= targetSquare;

        if (negPos != 0)
        {
            int intPos = __builtin_ffsll(negPos) - 1; // index of first piece intersected by ray
            negPos ^= Game::rayAttacks[intPos][0]; // chop off the rest of the ray
            if (negPos & opPieces) // it intersects the enemy pieces, so it is a capture
                queenAttacks.push_back(Move(pos, intPos, Position::queenIndex));
        }
        if (posPos != 0)
        {
            int intPos = __builtin_ffsll(posPos) - 1; // index of first piece intersected by ray
            posPos ^= Game::rayAttacks[intPos][2]; // chop off the rest of the ray
            if (posPos & opPieces) // it intersects the enemy pieces, so it is a capture
                queenAttacks.push_back(Move(pos, intPos, Position::queenIndex));
        }
        if (posNeg != 0)
        {
            int intPos = 63 - __builtin_clzll(posNeg); // posNeg is negative overall, first intersection will be MSB
            posNeg ^= Game::rayAttacks[intPos][4]; // chop off the rest of the ray
            if (posNeg & opPieces) // it intersects the enemy pieces, so it is a capture
                queenAttacks.push_back(Move(pos, intPos, Position::queenIndex));
        }
        if (negNeg != 0)
        {
            int intPos = 63 - __builtin_clzll(negNeg); // posNeg is negative overall, first intersection will be MSB
            negNeg ^= Game::rayAttacks[intPos][6]; // chop off the rest of the ray
            if (negNeg & opPieces) // it intersects the enemy pieces, so it is a capture
                queenAttacks.push_back(Move(pos, intPos, Position::queenIndex));
        }
    }

    /* queen straight moves */
    U64 queens = *(position.bitboards[Position::queenIndex + 6 * color]);
    numAttackers = __builtin_popcountll(queens);
    pos = -1;
    for (int i = 0; i < numAttackers; i++)
    {
        int ind = __builtin_ffsll(queens); // returns 1 + index of LSB
        queens >>= ind;
        pos += ind;

        U64 posY = rayAttacks[pos][1];
        U64 posX = rayAttacks[pos][3];
        U64 negY = rayAttacks[pos][5];
        U64 negX = rayAttacks[pos][7];

        // get intersection with enemy pieces
        posY &= position.occupency;
        posX &= position.occupency;
        negY &= position.occupency;
        negX &= position.occupency;

        posY &= targetSquare;
        posX &= targetSquare;
        negY &= targetSquare;
        negX &= targetSquare;

        if (posY != 0)
        {
            int intPos = __builtin_ffsll(posY) - 1; // index of first piece intersected by ray
            posY ^= Game::rayAttacks[intPos][1]; // chop off the rest of the ray
            if (posY & opPieces) // it intersects the enemy pieces, so it is a capture
                queenAttacks.push_back(Move(pos, intPos, Position::queenIndex));
        }
        if (posX != 0)
        {
            int intPos = __builtin_ffsll(posX) - 1; // index of first piece intersected by ray
            posX ^= Game::rayAttacks[intPos][3]; // chop off the rest of the ray
            if (posX & opPieces) // it intersects the enemy pieces, so it is a capture
                queenAttacks.push_back(Move(pos, intPos, Position::queenIndex));
        }
        if (negY != 0)
        {
            int intPos = 63 - __builtin_clzll(negY); // index of first piece intersected by ray
            negY ^= Game::rayAttacks[intPos][5]; // chop off the rest of the ray
            if (negY & opPieces) // it intersects the enemy pieces, so it is a capture
                queenAttacks.push_back(Move(pos, intPos, Position::queenIndex));
        }
        if (negX != 0)
        {
            int intPos = 63 - __builtin_clzll(negX); // index of first piece intersected by ray
            negX ^= Game::rayAttacks[intPos][7]; // chop off the rest of the ray
            if (negX & opPieces) // it intersects the enemy pieces, so it is a capture
                queenAttacks.push_back(Move(pos, intPos, Position::queenIndex));
        }
    }
    results.insert(results.end(), queenAttacks.begin(), queenAttacks.end());

    /* king stuff */
    int kingPos = __builtin_ffsll(*(position.bitboards[Position::kingIndex + 6 * color])) - 1;
    U64 targets = Game::kingAttacks[kingPos];
    targets &= opPieces;
    targets &= targetSquare;
    std::vector<Move> kingMoves;
    if (targets != 0)
        std::vector<Move> kingMoves = knightAttacksToMoves(targets, kingPos, Position::kingIndex);
    results.insert(results.end(), kingMoves.begin(), kingMoves.end());

    std::vector<Move> finalMoves;
    finalMoves.reserve(results.size());
    Position original = position;
    if (isCheck(color))
    {
        for (Move& m : results)
        {
            movePiece(m);
            if (!isCheck(color))
                finalMoves.push_back(m);
            position = original;
        }
    }
    else
    {
        for (Move& m : results)
        {
            if (!moveCausesCheck(m, color))
                finalMoves.push_back(m);
            position = original;
        }
    }
}

std::vector<Move> Game::getAllKingMoves(int color)
{
    U64 king;
    U64 pieces;
    std::vector<Move> results;
    if (color == Position::whiteID)
    {
        king = position.whiteKing;
        pieces = position.whitePieces;
    }
    else
    {
        king = position.blackKing;        
        pieces = position.blackPieces;
    }

    int kingPos = __builtin_ffsll(king) - 1; // builtin returns 1 + index of LSB

    // castling stuff
    if (!isCheck(color)) // can't castle out of check
    {
        if (position.canCastle(color, 'q'))
        {
            U64 mask = 0b1110ull << (kingPos - 4);
            if (!(mask & position.occupency)) // 3 empty spots between king and rook
            {
                Position old = position;
                movePiece(Move(kingPos, kingPos - 1, Position::kingIndex));
                // movePieceNoBranching(Move(kingPos, kingPos - 1, Position::kingIndex));
                if (!isCheck(color))
                    // results.emplace_back(kingPos, kingPos - 2, 5); // using 5 instead of Position::kingIndex bc that was giving compiler error????
                    results.push_back(Move(kingPos, kingPos - 2, Position::kingIndex));
                position = old;
            }
        }
        if (position.canCastle(color, 'k'))
        {
            U64 mask = 0b11ull << (kingPos + 1);
            if (!(mask & position.occupency)) // 2 empty spots between king and rook
            {
                Position old = position;
                movePiece(Move(kingPos, kingPos + 1, Position::kingIndex));
                // movePieceNoBranching(Move(kingPos, kingPos + 1, Position::kingIndex));
                if (!isCheck(color))
                    // results.emplace_back(kingPos, kingPos + 2, Position::kingIndex);
                    results.push_back(Move(kingPos, kingPos + 2, Position::kingIndex));
                position = old;
            }
        }
    }

    U64 targets = Game::kingAttacks[kingPos];
    targets &= ~pieces;
    std::vector<Move> moves = knightAttacksToMoves(targets, kingPos, Position::kingIndex); // i know the function says "Knight moves" but it works for this as well
    // results.reserve(results.size() + moves.size());
    results.insert(results.end(), moves.begin(), moves.end());
    return results;
}

std::vector<Move> Game::getAllKnightMoves(int color)
{
    // loop through all knights
    U64 knights;
    U64 pieces;
    std::vector<Move> results;
    if (color == Position::whiteID)
    {
        pieces = position.whitePieces;
        knights = position.whiteKnights;
    }
    else
    {
        pieces = position.blackPieces;
        knights = position.blackKnights;
    }

    int knightCount = __builtin_popcountll(knights);
    int pos = -1;
    for (int i = 0; i < knightCount; i++) // loops thru all the knights
    {
        int ind = __builtin_ffsll(knights); // returns 1 + index of LSB
        knights >>= ind;
        pos += ind;
        U64 targets = Game::knightAttacks[pos];
        targets &= ~pieces; // knight can move anywhere but where same color pieces are
        std::vector<Move> thisKnightMoves = knightAttacksToMoves(targets, pos, Position::knightIndex);
        results.insert(results.end(), thisKnightMoves.begin(), thisKnightMoves.end());
    }
    return results;
}

std::vector<Move> Game::getAllBishopMoves(int color)
{
    U64 bishops = color == Position::whiteID ? position.whiteBishops : position.blackBishops;
    U64 pieces = color == Position::whiteID ? position.whitePieces : position.blackPieces;
    std::vector<Move> results;

    int bishopCount = __builtin_popcountll(bishops);
    int pos = -1;
    for (int i = 0; i < bishopCount; i++)
    {
        int ind = __builtin_ffsll(bishops); // returns 1 + index of LSB
        bishops >>= ind;
        pos += ind;
        U64 negPos = posRayAttacks(pos, 0, pieces);
        U64 posPos = posRayAttacks(pos, 2, pieces);

        U64 posNeg = negRayAttacks(pos, 4, pieces);
        U64 negNeg = negRayAttacks(pos, 6, pieces);

        results.reserve(__builtin_popcountll(negPos) + __builtin_popcountll(posPos) + __builtin_popcountll(posNeg) + __builtin_popcountll(negNeg));
        std::vector<Move> moves = knightAttacksToMoves(negPos, pos, Position::bishopIndex);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(posPos, pos, Position::bishopIndex);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(posNeg, pos, Position::bishopIndex);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(negNeg, pos, Position::bishopIndex);
        results.insert(results.end(), moves.begin(), moves.end());
    }
    return results;
}

std::vector<Move> Game::getAllRookMoves(int color)
{
    U64 rooks = color == Position::whiteID ? position.whiteRooks : position.blackRooks;
    U64 pieces = color == Position::whiteID ? position.whitePieces : position.blackPieces;
    std::vector<Move> results;

    int rookCount = __builtin_popcountll(rooks);
    int pos = -1;
    for (int i = 0; i < rookCount; i++)
    {
        int ind = __builtin_ffsll(rooks); // returns 1 + index of LSB
        rooks >>= ind;
        pos += ind;
        U64 posY = posRayAttacks(pos, 1, pieces);
        U64 posX = posRayAttacks(pos, 3, pieces);

        U64 negY = negRayAttacks(pos, 5, pieces);
        U64 negX = negRayAttacks(pos, 7, pieces);

        results.reserve(__builtin_popcountll(posY) + __builtin_popcountll(posX) + __builtin_popcountll(negY) + __builtin_popcountll(negX));
        std::vector<Move> moves = knightAttacksToMoves(posY, pos, Position::rookIndex);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(posX, pos, Position::rookIndex);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(negY, pos, Position::rookIndex);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(negX, pos, Position::rookIndex);
        results.insert(results.end(), moves.begin(), moves.end());
    }
    return results;
}

std::vector<Move> Game::getAllQueenMoves(int color)
{
    U64 queens = color == Position::whiteID ? position.whiteQueens : position.blackQueens;
    U64 pieces = color == Position::whiteID ? position.whitePieces : position.blackPieces;
    std::vector<Move> results;

    int rookCount = __builtin_popcountll(queens);
    int pos = -1;
    for (int i = 0; i < rookCount; i++)
    {
        int ind = __builtin_ffsll(queens); // returns 1 + index of LSB
        queens >>= ind;
        pos += ind;
        // rook stuff
        U64 posY = posRayAttacks(pos, 1, pieces);
        U64 posX = posRayAttacks(pos, 3, pieces);
        U64 negY = negRayAttacks(pos, 5, pieces);
        U64 negX = negRayAttacks(pos, 7, pieces);

        // bishop stuff
        U64 negPos = posRayAttacks(pos, 0, pieces);
        U64 posPos = posRayAttacks(pos, 2, pieces);
        U64 posNeg = negRayAttacks(pos, 4, pieces);
        U64 negNeg = negRayAttacks(pos, 6, pieces);

        results.reserve(__builtin_popcountll(posY) + __builtin_popcountll(posX) + __builtin_popcountll(negY) + __builtin_popcountll(negX) + 
                        __builtin_popcountll(negPos) + __builtin_popcountll(posPos) + __builtin_popcountll(posNeg) + __builtin_popcountll(negNeg));
        std::vector<Move> moves = knightAttacksToMoves(posY, pos, Position::queenIndex);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(posX, pos, Position::queenIndex);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(negY, pos, Position::queenIndex);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(negX, pos, Position::queenIndex);
        results.insert(results.end(), moves.begin(), moves.end());

        moves = knightAttacksToMoves(negPos, pos, Position::queenIndex);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(posPos, pos, Position::queenIndex);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(posNeg, pos, Position::queenIndex);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(negNeg, pos, Position::queenIndex);
        results.insert(results.end(), moves.begin(), moves.end());
    }
    return results;
}

U64 Game::posRayAttacks(int pos, int dir, U64 friendlyPieces)
{
    // if (dir > 3) // rayAttacks indices 0-3 are positive, 4-7 are negative
        // throw std::invalid_argument("dir not positive!");
    
    U64 ray = Game::rayAttacks[pos][dir];
    // if (ray == 0) return ray;
    U64 rayInt = ray & position.occupency; // intersection of the ray and occupied squares
    if (rayInt == 0) return ray;
    int intPos = __builtin_ffsll(rayInt) - 1; // finds first position intersected (where ray should stop)
    ray ^= Game::rayAttacks[intPos][dir]; // unsets bits past the intersection
    if (ray & friendlyPieces) // the intersection is with a friendly piece
    {
        int n = __builtin_clzll(ray); // number of leading zeros
        ray ^= 1ull << (63 - n); // should unset MSB
    }
    return ray;
}

U64 Game::negRayAttacks(int pos, int dir, U64 friendlyPieces)
{
    // if (dir < 4) // rayAttacks indices 0-3 are positive, 4-7 are negative
        // throw std::invalid_argument("dir not negative!");
    
    U64 ray = Game::rayAttacks[pos][dir];
    // if (ray == 0) return ray;
    U64 rayInt = ray & position.occupency; // intersection of the ray and occupied squares
    if (rayInt == 0) return ray;
    int intPos = 63 - __builtin_clzll(rayInt); // finds first position intersected (where ray should stop)
    ray ^= Game::rayAttacks[intPos][dir]; // unset bits past the intersection
    if (ray & friendlyPieces) // the intersection is with a friendly piece
    {
        int n = __builtin_ffsll(ray); // LSB index + 1
        ray ^= 1ull << (n - 1); // should unset LSB
    }
    return ray;
}

std::vector<Move> Game::knightAttacksToMoves(U64 attacks, int pos, int piece)
{
    std::vector<Move> results;
    int num = __builtin_popcountll(attacks);
    int dest = -1;
    for (int i = 0; i < num; i++)
    {
        int ind = __builtin_ffsll(attacks); // returns 1 + index of LSB
        attacks >>= ind;
        dest += ind;
        results.emplace_back(pos, dest, piece);
    }
    return results;
}

std::vector<Move> Game::getAllPawnMoves(int color)
{
    std::vector<Move> moves;

    U64 pushMoves;
    U64 dblPushMoves;
    U64 lAttacks;
    U64 rAttacks;
    int dir;
    U64 pawns = color == Position::whiteID ? position.whitePawns : position.blackPawns;
    pushMoves = pawnPushTargets(pawns, color);
    if (color == Position::whiteID)
    {
        dir = 1;
    }
    else
    {
        dir = -1;
    }

    dblPushMoves = pawnDblPushTargets(color);
    lAttacks = pawnLAttacks(color);
    rAttacks = pawnRAttacks(color);

    int totalNumMoves = __builtin_popcountll(pushMoves) + __builtin_popcountll(dblPushMoves) + __builtin_popcountll(lAttacks) + __builtin_popcountll(rAttacks);
    moves.reserve(totalNumMoves);

    pawnTargetsToMoves(lAttacks, dir * 9, moves);
    pawnTargetsToMoves(rAttacks, dir * 7, moves);
    pawnTargetsToMoves(pushMoves, dir * 8, moves);
    pawnTargetsToMoves(dblPushMoves, dir * 16, moves);

    return moves;
}

U64 Game::pawnPushTargets(U64 pawns, int color)
{
    if (color == Position::whiteID)
        return pawns >> 8 & ~position.occupency;
    else
        return pawns << 8 & ~position.occupency;
}

U64 Game::pawnDblPushTargets(int color)
{
    if (color == Position::whiteID)
    {
        U64 singlePush = pawnPushTargets(position.whitePawns & 0x00ff000000000000, color);
        return singlePush >> 8 & ~position.occupency;
    }
    else
    {
        U64 singlePush = pawnPushTargets(position.blackPawns & 0x000000000000ff00, color);
        return singlePush << 8 & ~position.occupency;
    }
}

U64 Game::pawnLAttacks(int color)
{
    // for white, a file can't attack to left (white's left)
    // for black, h file can't attack to left (black's left)
    if (color == Position::whiteID)
    {
        U64 mask = 0b1111111011111110111111101111111011111110111111101111111011111110; // all but a file
        U64 pawns = position.whitePawns & mask;
        return (pawns >> 9) & (position.blackPieces | position.enPassant); // add in en passant to include en passant captures
    }
    else
    {
        U64 mask = 0b0111111101111111011111110111111101111111011111110111111101111111; // all but h file
        U64 pawns = position.blackPawns & mask;
        return (pawns << 9) & (position.whitePieces | position.enPassant); // add in en passant captures
    }
}

U64 Game::pawnRAttacks(int color)
{
    // for white, h file can't attack to right (white's right)
    // for black, a file can't attack to right (black's right)
    if (color == Position::whiteID)
    {
        U64 mask = 0b0111111101111111011111110111111101111111011111110111111101111111; // all but h file
        U64 pawns = position.whitePawns & mask;
        return pawns >> 7 & (position.blackPieces | position.enPassant);
    }
    else
    {
        U64 mask = 0b1111111011111110111111101111111011111110111111101111111011111110; // all but a file
        U64 pawns = position.blackPawns & mask;
        return pawns << 7 & (position.whitePieces | position.enPassant);
    }
}

void Game::pawnTargetsToMoves(U64 targets, int offset, std::vector<Move>& moves)
{
    if (targets != 0)
    {
        int pos = 64;
        int numMoves = __builtin_popcountll(targets);
        for (int i = 0; i < numMoves; i++)
        {
            int n = __builtin_clzll(targets); // number of leading zeros
            pos -= n + 1;
            targets <<= (n + 1); // if zero leading 0s MSB is 1, still need to shift to get next one
            if (pos >= 8 && pos < 56) // non promotion moves
                // moves.emplace_back(pos + offset, pos, Position::pawnIndex);
                moves.push_back(Move(pos + offset, pos, Position::pawnIndex));
            else
            {
                int dir = offset > 0 ? -1 : 1; // basically the opposite sign of the offset
                // moves.emplace_back(pos + offset, pos + dir * 8, Position::pawnIndex); // queen
                moves.push_back(Move(pos + offset, pos + dir * 8, Position::pawnIndex));
                // moves.emplace_back(pos + offset, pos + dir * 16, Position::pawnIndex); // rook
                moves.push_back(Move(pos + offset, pos + dir * 16, Position::pawnIndex));
                // moves.emplace_back(pos + offset, pos + dir * 24, Position::pawnIndex); // bishop
                moves.push_back(Move(pos + offset, pos + dir * 24, Position::pawnIndex));
                // moves.emplace_back(pos + offset, pos + dir * 32, Position::pawnIndex); // knight
                moves.push_back(Move(pos + offset, pos + dir * 32, Position::pawnIndex));
            }
        }
    }
}

bool Game::isCheck(int color)
{
    U64 king = color == Position::whiteID ? position.whiteKing : position.blackKing;
    // // * for the following, !color represents the white case and color is the black case (blackID is 1, whiteID is 0, !whiteID = 1)
    // bool isWhite = color == Position::whiteID;
    // U64 king = isWhite * position.whiteKing + !isWhite * position.blackKing;
    U64 friendlyPieces = color == Position::whiteID ? position.whitePieces : position.blackPieces;
    // U64 friendlyPieces = isWhite * position.whitePieces + !isWhite * position.blackPieces;
    int kingPos = __builtin_ffsll(king) - 1; // builtin returns 1 + index of LSB
    
    // check for knights attacking
    U64 opKnights = color == Position::whiteID ? position.blackKnights : position.whiteKnights;
    // U64 opKnights = isWhite * position.blackKnights + !isWhite * position.whiteKnights;
    if (knightAttacks[kingPos] & opKnights) return true;

    // check for pawns attacking
    if (king & pawnLAttacks(!color)) return true;
    if (king & pawnRAttacks(!color)) return true;

    if (RQBCheck(color)) return true;
    // rooks/queens/bishops - for all of these, it only intersects occupency at the end of the ray if it is an oppent's piece
    // U64 opRooks = color == Position::whiteID ? position.blackRooks : position.whiteRooks;
    // // U64 opRooks = isWhite * position.blackRooks + !isWhite * position.whiteRooks;
    // U64 opQueens = color == Position::whiteID ? position.blackQueens : position.whiteQueens;
    // // U64 opQueens = isWhite * position.blackQueens + !isWhite * position.whiteQueens;
    // U64 opBishops = color == Position::whiteID ? position.blackBishops : position.whiteBishops;
    // // U64 opBishops = isWhite * position.blackBishops + !isWhite * position.whiteBishops;
    // for (int i = 1; i < 8; i += 2) // rooks/queens
    // {
    //     U64 ray = i < 4 ? posRayAttacks(kingPos, i, friendlyPieces) : negRayAttacks(kingPos, i, friendlyPieces);
    //     if (ray & opRooks || ray & opQueens) return true;
    // }
    // for (int i = 0; i < 8; i += 2) // bishops/queens
    // {
    //     U64 ray = i < 4 ? posRayAttacks(kingPos, i, friendlyPieces) : negRayAttacks(kingPos, i, friendlyPieces);
    //     if (ray & opBishops || ray & opQueens) return true;
    // }

    U64 opKing = color == Position::whiteID ? position.blackKing : position.whiteKing;
    // U64 opKing = isWhite * position.blackKing + !isWhite * position.whiteKing;
    if (kingAttacks[kingPos] & opKing) return true;
    
    return false;
}

bool Game::RQBCheck(int color)
{
    U64 king = color == Position::whiteID ? position.whiteKing : position.blackKing;
    // // * for the following, !color represents the white case and color is the black case (blackID is 1, whiteID is 0, !whiteID = 1)
    // bool isWhite = color == Position::whiteID;
    // U64 king = isWhite * position.whiteKing + !isWhite * position.blackKing;
    U64 friendlyPieces = color == Position::whiteID ? position.whitePieces : position.blackPieces;
    // U64 friendlyPieces = isWhite * position.whitePieces + !isWhite * position.blackPieces;
    int kingPos = __builtin_ffsll(king) - 1; // builtin returns 1 + index of LSB

    // U64 opRooks = color == Position::whiteID ? position.blackRooks : position.whiteRooks;
    U64 opRooks = *(position.bitboards[Position::rookIndex + 6 * !color]);
    // U64 opRooks = isWhite * position.blackRooks + !isWhite * position.whiteRooks;
    // U64 opQueens = color == Position::whiteID ? position.blackQueens : position.whiteQueens;
    U64 opQueens = *(position.bitboards[Position::queenIndex + 6 * !color]);
    // U64 opQueens = isWhite * position.blackQueens + !isWhite * position.whiteQueens;
    // U64 opBishops = color == Position::whiteID ? position.blackBishops : position.whiteBishops;
    U64 opBishops = *(position.bitboards[Position::bishopIndex + 6 * !color]);
    // U64 opBishops = isWhite * position.blackBishops + !isWhite * position.whiteBishops;
    // for (int i = 1; i < 8; i += 2) // rooks/queens
    // {
    //     U64 ray = i < 4 ? posRayAttacks(kingPos, i, friendlyPieces) : negRayAttacks(kingPos, i, friendlyPieces);
    //     if (ray & opRooks || ray & opQueens) return true;
    // }
    // for (int i = 0; i < 8; i += 2) // bishops/queens
    // {
    //     U64 ray = i < 4 ? posRayAttacks(kingPos, i, friendlyPieces) : negRayAttacks(kingPos, i, friendlyPieces);
    //     if (ray & opBishops || ray & opQueens) return true;
    // }

    U64 ray = posRayAttacks(kingPos, 0, friendlyPieces);
    if (ray & opBishops || ray & opQueens) return true; 
    ray = posRayAttacks(kingPos, 1, friendlyPieces);
    if (ray & opRooks || ray & opQueens) return true;
    ray = posRayAttacks(kingPos, 2, friendlyPieces);
    if (ray & opBishops || ray & opQueens) return true;
    ray = posRayAttacks(kingPos, 3, friendlyPieces);
    if (ray & opRooks || ray & opQueens) return true;
    ray = negRayAttacks(kingPos, 4, friendlyPieces);
    if (ray & opBishops || ray & opQueens) return true;
    ray = negRayAttacks(kingPos, 5, friendlyPieces);
    if (ray & opRooks || ray & opQueens) return true;
    ray = negRayAttacks(kingPos, 6, friendlyPieces);
    if (ray & opBishops || ray & opQueens) return true;
    ray = negRayAttacks(kingPos, 7, friendlyPieces);
    if (ray & opRooks || ray & opQueens) return true;
    return false;
}

bool Game::moveCausesCheck(Move& m, int color)
{
    /*if (m.piece == Position::pawnIndex) // have to consider extra for en passant
    {
        // Position original = position;
        movePiece(m);
        bool check = RQBCheck(color);
        // position = original;
        return check;
    }
    else*/ if (m.piece == Position::kingIndex) // just do the normal
    {
        // Position original = position;
        movePiece(m);
        bool check = isCheck(color);
        // position = original;
        return check;
    }
    else
    {
        movePiece(m);
        U64 king = *(position.bitboards[Position::kingIndex + 6 * color]);
        int kingPos = __builtin_ffsll(king) - 1; // builtin returns 1 + index of LSB
        U64 diagAttackers = *(position.bitboards[Position::queenIndex + 6 * !color]) | *(position.bitboards[Position::bishopIndex + 6 * !color]);
        U64 cardAttackers = *(position.bitboards[Position::queenIndex + 6 * !color]) | *(position.bitboards[Position::rookIndex + 6 * !color]);
        U64 friendlyPieces = color == Position::whiteID ? position.whitePieces : position.blackPieces;

        int kingX = kingPos % 8;
        int kingY = kingPos / 8;
        int startX = m.from % 8;
        int startY = m.from / 8;
        if (kingX == startX)
        {
            if (m.from > kingPos)
            {
                U64 ray = posRayAttacks(kingPos, 1, friendlyPieces); // positive y
                if (ray & cardAttackers) return true;
            }
            else
            {
                U64 ray = negRayAttacks(kingPos, 5, friendlyPieces); // negative y
                if (ray & cardAttackers) return true;
            }
        }
        else if (kingY == startY)
        {
            if (m.from > kingPos)
            {
                U64 ray = posRayAttacks(kingPos, 3, friendlyPieces); // positive x
                if (ray & cardAttackers) return true;
            }
            else
            {
                U64 ray = negRayAttacks(kingPos, 7, friendlyPieces); // negative x
                if (ray & cardAttackers) return true;
            }
        }
        else if (kingX - startX == kingY - startY)
        {
            if (m.from > kingPos)
            {
                U64 ray = posRayAttacks(kingPos, 2, friendlyPieces); // +x +y
                if (ray & diagAttackers) return true;
            }
            else
            {
                U64 ray = negRayAttacks(kingPos, 6, friendlyPieces); // -x -y
                if (ray & diagAttackers) return true;
            }
        }
        else if (kingX - startX == -(kingY - startY))
        {
            if (m.from > kingPos)
            {
                U64 ray = posRayAttacks(kingPos, 0, friendlyPieces); // -x +y
                if (ray & diagAttackers) return true;
            }
            else
            {
                U64 ray = negRayAttacks(kingPos, 4, friendlyPieces); // +x -y
                if (ray & diagAttackers) return true;
            }
        }
        return false;
    }
}

// void Game::movePiece(Move move) // this function currently taking about 40% of the time
// {
//     U64 piece = 1ull << move.from;
//     int color;
//     U64* otherPieces;
//     U64* pawns;
//     U64* knights;
//     U64* bishops;
//     U64* rooks;
//     U64* queens;
//     U64* king;
//     if (piece & position.whitePieces)
//     {
//         color = Position::whiteID;
//         otherPieces = &position.whitePieces;
//         pawns = &position.whitePawns;
//         knights = &position.whiteKnights;
//         bishops = &position.whiteBishops;
//         rooks = &position.whiteRooks;
//         queens = &position.whiteQueens;
//         king = &position.whiteKing;
//     }
//     else
//     {
//         color = Position::blackID;
//         otherPieces = &position.blackPieces;
//         pawns = &position.blackPawns;
//         knights = &position.blackKnights;
//         bishops = &position.blackBishops;
//         rooks = &position.blackRooks;
//         queens = &position.blackQueens;
//         king = &position.blackKing;
//     }

//     int dest = move.to;
//     int destY;
//     if (move.to < 0)
//     {
//         dest = (move.to + 1) % 8 + 8 - 1;
//         destY = (move.to + 1) / 8 - 1;
//     }
//     else if (move.to > 63)
//     {
//         dest = 56 + (move.to % 8);
//         destY = move.to / 8;
//     }
//     bool isCapture = (1ull << dest) & position.occupency; // if dest is occupied must be capture
//     clearSquare(dest);
//     // NOTE: not the best way to do this - basically trying to remove castling rights if rook is captured (im just not checking it it's a rook (should still work tho))
//     if (dest == 0) position.castling &= 0b1110;
//     else if (dest == 7) position.castling &= 0b1101;
//     else if (dest == 56) position.castling &= 0b1011;
//     else if (dest == 63) position.castling &= 0b0111;
//     U64 enPassant = 0;

//     if (piece & *pawns) // it is a pawn
//     {
//         if (move.to >= 0 && move.to < 64)
//         {
//             *pawns |= 1ull << move.to;
//             if (abs(move.to - move.from) == 16) // double pawn move
//             {
//                 enPassant = 1ull << (move.to + 8 - 16 * color); // set en passant to be the square behind the pawn
//             }
//             if (!isCapture && abs(move.to - move.from) % 8 != 0) // this means its moving to an empty square, but not straight forward (cause not multiple of 8)
//             {
//                 // get rid of en passant-ed pawn
//                 clearSquare(move.to + 8 - 16 * color);
//             }
//         }
//         else
//         {
//             // int destX, destY;
//             // int destY;
//             // if (move.to < 0)
//             // {
//             //     destY = (move.to + 1) / 8 - 1;
//             //     // these two lines make queen 8, knight 9, rook 10, bishop 11 so that one case works for white and black
//             //     // destY = -destY;
//             //     // destY += 7;
//             //     // destX = (move.to + 1) % 8 + 8 - 1;
//             //     // dest = destX;
//             // }
//             // else
//             // {
//             //     // destX = move.to % 8;
//             //     destY = move.to / 8;
//             //     // dest = 56 + destX;
//             // }
//             // for white promotions only need to consider destX bc all on 8th rank (indices 0-7)
//             // black promotions add 56 to offset to start of 1st rank
//             if (destY == -1 || destY == 8) // white queen promotion
//                 *queens |= 1ull << dest;
//             // else if (destY == 8) // black queen promotion
//             //     *queens |= 1ull << dest;
//             else if (destY == -2 || destY == 9) // white knight promotion
//                 *knights |= 1ull << dest;
//             // else if (destY == 9) // black knight promotion
//                 // *knights |= 1ull << dest;
//             else if (destY == -3 || destY == 10) // white rook promotion
//                 *rooks |= 1ull << dest;
//             // else if (destY == 10) // black rook promotion
//                 // *rooks |= 1ull << dest;
//             else //if (destY == -4 || destY == 11) // white bishop promotion
//                 *bishops |= 1ull << dest;
//             // else if (destY == 11) // black bishop promotion
//                 // *bishops |= 1ull << dest;
//             // switch (destY)
//             // {
//             // case 8: // queen
//             //     *queens |= 1ull << dest;
//             //     break;
//             // case 9: // knight
//             //     *knights |= 1ull << dest;
//             //     break;
//             // case 10: // rook
//             //     *rooks |= 1ull << dest;
//             //     break;
//             // case 11: // bishop
//             //     *bishops |= 1ull << dest;
//             //     break;
//             // }
//         }
//     }
//     else if (piece & *knights) // it is a knight
//     {
//         *knights |= 1ull << move.to;
//     }
//     else if (piece & *bishops)
//     {
//         *bishops |= 1ull << move.to;
//     }
//     else if (piece & *rooks)
//     {
//         *rooks |= 1ull << move.to;
//         int castlingUpdate = 0;
//         if ((position.castling & 0b1000 && move.from == 63) || (position.castling & 0b0010 && move.from == 7)) // king side
//             castlingUpdate = 0b110111;
//         else if ((position.castling & 0b0100 && move.from == 56) || (position.castling & 0b0001 && move.from == 0))
//             castlingUpdate = 0b111011;
//         if (castlingUpdate)
//         {
//             castlingUpdate >>= (color * 2);
//             position.castling &= castlingUpdate;
//         }
//         // if (color == Position::whiteID)
//         // {
//         //     if (position.castling & 0b1000 && move.from == 63) // white king side
//         //         position.castling &= 0b0111;
//         //     else if (position.castling & 0b0100 && move.from == 56) // white queen side
//         //         position.castling &= 0b1011;
//         // }
//         // else
//         // {
//         //     if (position.castling & 0b0010 && move.from == 7) // black king side
//         //         position.castling &= 0b1101;
//         //     else if (position.castling & 0b0001 && move.from == 0) // black queen side
//         //         position.castling &= 0b1110;
//         // }
//     }
//     else if (piece & *queens)
//     {
//         *queens |= 1ull << move.to;
//     }
//     else if (piece & *king)
//     {
//         *king |= 1ull << move.to;
//         position.castling &= 0b0011 << (2 * color); // should get rid of this side's castling
//         if (move.to - move.from == 2) // king moved 2 squares right (kingside castle)
//         {
//             // move the rook
//             *rooks ^= 1ull << (move.from + 3); // we know rook will be 3 further, and we know it is set, xor will unset
//             *otherPieces ^= 1ull << (move.from + 3); // unset rook from otherpieces
//             position.occupency ^= 1ull << (move.from + 3); // unset rook from occupence
//             *rooks |= 1ull << (move.from + 1); // new rook goes here
//             *otherPieces |= 1ull << (move.from + 1);
//             position.occupency |= 1ull << (move.from + 1);
//         }
//         else if (move.to - move.from == -2) // king moved 2 squares left (queenside castle)
//         {
//             // move the rook
//             *rooks ^= 1ull << (move.from - 4); // we know rook will be 4 before, and we know it is set, xor will unset
//             *otherPieces ^= 1ull << (move.from - 4);
//             position.occupency ^= 1ull << (move.from - 4);
//             *rooks |= 1ull << (move.from - 1); // new rook goes here
//             *otherPieces |= 1ull << (move.from - 1);
//             position.occupency |= 1ull << (move.from - 1);
//         }
//     }

//     clearSquare(move.from);
//     *otherPieces |= 1ull << dest;
//     position.occupency |= 1ull << dest;
//     position.enPassant = enPassant;
//     position.currTurn = !position.currTurn;
// }

void Game::movePiece(Move move)
{
    U64 piece = 1ull << move.from;
    int color;
    U64* otherPieces;
    U64* samePieces;
    if (piece & position.whitePieces)
        color = Position::whiteID;
    else color = Position::blackID;

    otherPieces = position.bitboards[12 + color]; // [12] is white pieces, [13] is black
    samePieces = position.bitboards[move.piece + 6 * color]; // [0-5] are white pieces, [6-11] are black

    U64 enPassant = 0;

    // NOTE: not the best way to do this - basically trying to remove castling rights if rook is captured (im just not checking it it's a rook (should still work tho))
    if (move.to == 0) position.castling &= 0b1110;
    else if (move.to == 7) position.castling &= 0b1101;
    else if (move.to == 56) position.castling &= 0b1011;
    else if (move.to == 63) position.castling &= 0b0111;
    // position.castling &= 0b1111 - ((move.to == 0) * 0b0001) - ((move.to == 7) * 0b0010) - ((move.to == 56) * 0b0100) - ((move.to == 63) * 0b1000);

    if (move.piece == Position::pawnIndex)
    {
        if (move.to >= 0 && move.to < 64)
        {
            // need to do this before clearing dest or else wont work (will think the square is always empty)
            if (!((1ull << move.to) & position.occupency) && abs(move.to - move.from) % 8 != 0) // this means its moving to an empty square, but not straight forward (cause not multiple of 8)
            {
                // get rid of en passant-ed pawn
                clearSquare(move.to + 8 - 16 * color);
            }
            clearSquare(move.to);
            *samePieces |= 1ull << move.to;
            if (abs(move.to - move.from) == 16) // double pawn move
            {
                enPassant = 1ull << (move.to + 8 - 16 * color); // set en passant to be the square behind the pawn
            }
        }
        else
        {
            int dest, promotionIndex;
            dest = (move.to + (move.to < 0)) % 8 + 7 * (move.to < 0) + 56 * !(move.to < 0);
            // promotionIndex = (move.to + (move.to < 0)) / 8 - (move.to < 0);
            promotionIndex = (move.to < 0) * ((move.to + 1) / 8 - 1) + (move.to > 0) * (move.to / 8);
            // promotionIndex = -!(move.to < 0) * promotionIndex + 12 - (move.to < 0) * 7;
            promotionIndex = (move.to < 0) * (promotionIndex + 5) + (move.to > 0) * (-promotionIndex + 12);
            // int destY;
            // if (move.to < 0)
            // {
            //     dest = (move.to + 1) % 8 + 8 - 1;
            //     destY = (move.to + 1) / 8 - 1;
            //     promotionIndex = (move.to + 1) / 8 - 1; // old destY
            //     promotionIndex += 5;
            // }
            // else
            // {
            //     dest = 56 + move.to % 8;
            //     destY = move.to / 8;
            //     promotionIndex = move.to / 8; // old destY
            //     promotionIndex = -promotionIndex + 12;
            // }
            // NOTE: not the best way to do this - basically trying to remove castling rights if rook is captured (im just not checking it it's a rook (should still work tho))
            if (dest == 0) position.castling &= 0b1110;
            else if (dest == 7) position.castling &= 0b1101;
            else if (dest == 56) position.castling &= 0b1011;
            else if (dest == 63) position.castling &= 0b0111;
            // position.castling &= 0b1111 - ((move.to == 0) * 0b0001) - ((move.to == 7) * 0b0010) - ((move.to == 56) * 0b0100) - ((move.to == 63) * 0b1000);

            clearSquare(dest);
            // for white promotions only need to consider destX bc all on 8th rank (indices 0-7)
            // black promotions add 56 to offset to start of 1st rank
            // if (destY == -1 || destY == 8) // white queen promotion
            //     *(position.bitboards[Position::queenIndex + 6 * color]) |= 1ull << dest; // index 4
            // else if (destY == -2 || destY == 9) // white rook promotion
            //     *(position.bitboards[Position::rookIndex + 6 * color]) |= 1ull << dest; // index 3
            // else if (destY == -3 || destY == 10) // white bishop promotion
            //     *(position.bitboards[Position::bishopIndex + 6 * color]) |= 1ull << dest; // index 2
            // else //if (destY == -4 || destY == 11) // white knight promotion
            //     *(position.bitboards[Position::knightIndex + 6 * color]) |= 1ull << dest; // index 1

            *(position.bitboards[promotionIndex + 6 * color]) |= 1ull << dest;
            
            clearSquare(move.from);
            *otherPieces |= 1ull << dest;
            position.occupency |= 1ull << dest;
            position.enPassant = 0;
            position.currTurn = !position.currTurn;
            return;
        }
    }
    else if (move.piece == Position::rookIndex)
    {
        clearSquare(move.to);
        *samePieces |= 1ull << move.to;
        int castlingUpdate = 0b111111;
        if ((position.castling & 0b1000 && move.from == 63) || (position.castling & 0b0010 && move.from == 7)) // king side
            castlingUpdate = 0b110111;
        else if ((position.castling & 0b0100 && move.from == 56) || (position.castling & 0b0001 && move.from == 0)) // queen side
            castlingUpdate = 0b111011;
        
        castlingUpdate >>= (color * 2);
        position.castling &= castlingUpdate;
    }
    else if (move.piece == Position::kingIndex)
    {
        clearSquare(move.to);
        *samePieces |= 1ull << move.to;
        position.castling &= 0b0011 << (2 * color);
        if (move.to - move.from == 2) // king moved 2 squares right (king side castle)
        {
            // move the rook
            clearSquare(move.from + 3); // get rid of old rook
            *(position.bitboards[Position::rookIndex + 6 * color]) |= 1ull << (move.from + 1); // new rook goes here
            *otherPieces |= 1ull << (move.from + 1);
            position.occupency |= 1ull << (move.from + 1);
        }
        else if (move.to - move.from == -2) // king moved 2 squares left (queen side castle)
        {
            // move the rook
            clearSquare(move.from - 4); // get rid of old rook
            *(position.bitboards[Position::rookIndex + 6 * color]) |= 1ull << (move.from - 1); // new rook goes here
            *otherPieces |= 1ull << (move.from - 1);
            position.occupency |= 1ull << (move.from - 1);
        }
    }
    else
    {
        clearSquare(move.to);
        *samePieces |= 1ull << move.to;
    }

    clearSquare(move.from);
    *otherPieces |= 1ull << move.to;
    position.occupency |= 1ull << move.to;
    position.enPassant = enPassant;
    position.currTurn = !position.currTurn;
}

void Game::movePieceNoBranching(Move move)
{
    U64 piece = 1ull << move.from;
    int color = (piece & position.whitePieces != 0) * Position::whiteID + (piece & position.blackPieces != 0) * Position::blackID;

    U64* otherPieces = position.bitboards[12 + color]; // [12] is white pieces, [13] is black
    U64* samePieces = position.bitboards[move.piece + 6 * color]; //[0-5] are white pieces, [6-11] are black

    position.castling &= 0b1111 - (move.to == 0) * 0b0001 - (move.to == 7) * 0b0010 - (move.to == 56) * 0b0100 - move.to == 63 * 0b1000;

    position.castling &= 0b1111 - (move.piece == Position::rookIndex) * ((move.from == 63) * 0b1000 + // white king side
                                                                         (move.from == 7) * 0b0010 + // black king side
                                                                         (move.from == 56) * 0b0100 + // white queen side
                                                                         (move.from == 0)  * 0b0001); // black queen side
    U64 enPassant = 0;

    if (move.piece == Position::pawnIndex)
    {
        if (move.to >= 0 && move.to < 64)
        {
            // need to do this before clearing dest or else wont work (will think the square is always empty)
            if (!((1ull << move.to) & position.occupency) && abs(move.to - move.from) % 8 != 0) // this means its moving to an empty square, but not straight forward (cause not multiple of 8)
            {
                // get rid of en passant-ed pawn
                clearSquare(move.to + 8 - 16 * color);
            }
            clearSquare(move.to);
            *samePieces |= 1ull << move.to;
            if (abs(move.to - move.from) == 16) // double pawn move
            {
                enPassant = 1ull << (move.to + 8 - 16 * color); // set en passant to be the square behind the pawn
            }
        }
        else
        {
            int dest, promotionIndex;
            dest = (move.to + (move.to < 0)) % 8 + 7 * (move.to < 0) + 56 * !(move.to < 0);
            promotionIndex = (move.to < 0) * ((move.to + 1) / 8 - 1) + (move.to > 0) * (move.to / 8);
            promotionIndex = (move.to < 0) * (promotionIndex + 5) + (move.to > 0) * (-promotionIndex + 12);
            position.castling &= 0b1111 - ((move.to == 0) * 0b0001) - ((move.to == 7) * 0b0010) - ((move.to == 56) * 0b0100) - ((move.to == 63) * 0b1000);

            clearSquare(dest);

            *(position.bitboards[promotionIndex + 6 * color]) |= 1ull << dest;
            
            clearSquare(move.from);
            *otherPieces |= 1ull << dest;
            position.occupency |= 1ull << dest;
            position.enPassant = 0;
            position.currTurn = !position.currTurn;
            return;
        }
    }
    else if (move.piece == Position::kingIndex)
    {
        clearSquare(move.to);
        *samePieces |= 1ull << move.to;
        position.castling &= 0b0011 << (2 * color);
        if (move.to - move.from == 2) // king moved 2 squares right (king side castle)
        {
            // move the rook
            clearSquare(move.from + 3); // get rid of old rook
            *(position.bitboards[Position::rookIndex + 6 * color]) |= 1ull << (move.from + 1); // new rook goes here
            *otherPieces |= 1ull << (move.from + 1);
            position.occupency |= 1ull << (move.from + 1);
        }
        else if (move.to - move.from == -2) // king moved 2 squares left (queen side castle)
        {
            // move the rook
            clearSquare(move.from - 4); // get rid of old rook
            *(position.bitboards[Position::rookIndex + 6 * color]) |= 1ull << (move.from - 1); // new rook goes here
            *otherPieces |= 1ull << (move.from - 1);
            position.occupency |= 1ull << (move.from - 1);
        }
    }
    else
    {
        clearSquare(move.to);
        *samePieces |= 1ull << move.to;
    }

    clearSquare(move.from);
    *otherPieces |= 1ull << move.to;
    position.occupency |= 1ull << move.to;
    position.enPassant = enPassant;
    position.currTurn = !position.currTurn;
}

void Game::clearSquare(int pos)
{
    U64 allOthers = ~(1ull << pos);
    position.whitePieces  &= allOthers;
    position.blackPieces  &= allOthers;
    position.occupency    &= allOthers;
    position.whitePawns   &= allOthers;
    position.blackPawns   &= allOthers;
    position.whiteKnights &= allOthers;
    position.blackKnights &= allOthers;
    position.whiteBishops &= allOthers;
    position.blackBishops &= allOthers;
    position.whiteRooks   &= allOthers;
    position.blackRooks   &= allOthers;
    position.whiteQueens  &= allOthers;
    position.blackQueens  &= allOthers;
    position.whiteKing    &= allOthers;
    position.blackKing    &= allOthers;
}

std::string Game::indexToAlg(int index)
{
    int x, y;
    if (index < 0)
    {
        y = (index + 1) / 8 - 1;
        x = (index + 1) % 8 + 8 - 1;
    }
    else
    {
        x = index % 8;
        y = index / 8;
    }

    char file = 'a';
    file += x;
    std::string result = file + std::to_string(8 - y);
    return result;
}

// int main()
// {
//     Game g = Game("R3N3/b6P/p4Np1/p3PBK1/2pk3P/pq6/3n4/5b2 w - -");
//     // Game g = Game();
//     g.position.print();
//     std::vector<Move> moves = g.getAllMoves(Position::whiteID);
//     std::cout << moves.size() << std::endl;
//     // std::vector<Move> moves = g.getAllPawnMoves(Position::whiteID);
//     for (Move& m : moves)
//     {
//         std::cout << Game::indexToAlg(m.from) << "->" << Game::indexToAlg(m.to) << std::endl;
//     }
//     // std::cout << std::endl;
//     // moves = g.getAllKnightMoves(Position::whiteID);
//     // for (Move& m : moves)
//     // {
//     //     std::cout << m.from << "->" << m.to << std::endl;
//     // }
//     // std::cout << std::endl;
//     // moves = g.getAllKingMoves(Position::whiteID);
//     // for (Move& m : moves)
//     // {
//     //     std::cout << m.from << "->" << m.to << std::endl;
//     // }
//     // std::cout << std::endl << "bishops: " << std::endl;
//     // moves = g.getAllBishopMoves(Position::whiteID);
//     // for (Move& m : moves)
//     // {
//     //     std::cout << m.from << "->" << m.to << std::endl;
//     // }
//     // std::cout << std::endl << "rooks: " << std::endl;
//     // moves = g.getAllRookMoves(Position::whiteID);
//     // for (Move& m : moves)
//     // {
//     //     std::cout << m.from << "->" << m.to << std::endl;
//     // }
//     // std::cout << std::endl << "queens: " << std::endl;
//     // moves = g.getAllQueenMoves(Position::whiteID);
//     // for (Move& m : moves)
//     // {
//     //     std::cout << m.from << "->" << m.to << std::endl;
//     // }
// }