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
    results.insert(results.end(), queenMoves.begin(), queenMoves.end());
    results.insert(results.end(), rookMoves.begin(), rookMoves.end());
    results.insert(results.end(), bishopMoves.begin(), bishopMoves.end());
    results.insert(results.end(), knightMoves.begin(), knightMoves.end());
    results.insert(results.end(), kingMoves.begin(), kingMoves.end());
    results.insert(results.end(), pawnMoves.begin(), pawnMoves.end());
    return results;
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
    if (position.canCastle(color, 'q'))
    {
        U64 mask = 0b1110ull << (kingPos - 4);
        if (!(mask & position.occupency)) // 3 empty spots between king and rook
        {
            // if (!isCheck(inbetweenpos))
            results.emplace_back(kingPos, kingPos - 2);
        }
    }
    if (position.canCastle(color, 'k'))
    {
        U64 mask = 0b11ull << (kingPos + 1);
        if (!(mask & position.occupency)) // 2 empty spots between king and rook
        {
            // if (!isCheck(inbetweenpos))
            results.emplace_back(kingPos, kingPos + 2);
        }
    }

    U64 targets = Game::kingAttacks[kingPos];
    targets &= ~pieces;
    std::vector<Move> moves = knightAttacksToMoves(targets, kingPos); // i know the function says "Knight moves" but it works for this as well
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
        std::vector<Move> thisKnightMoves = knightAttacksToMoves(targets, pos);
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
        std::vector<Move> moves = knightAttacksToMoves(negPos, pos);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(posPos, pos);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(posNeg, pos);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(negNeg, pos);
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
        std::vector<Move> moves = knightAttacksToMoves(posY, pos);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(posX, pos);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(negY, pos);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(negX, pos);
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
        std::vector<Move> moves = knightAttacksToMoves(posY, pos);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(posX, pos);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(negY, pos);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(negX, pos);
        results.insert(results.end(), moves.begin(), moves.end());

        moves = knightAttacksToMoves(negPos, pos);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(posPos, pos);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(posNeg, pos);
        results.insert(results.end(), moves.begin(), moves.end());
        moves = knightAttacksToMoves(negNeg, pos);
        results.insert(results.end(), moves.begin(), moves.end());
    }
    return results;
}

U64 Game::posRayAttacks(int pos, int dir, U64 friendlyPieces)
{
    if (dir > 3) // rayAttacks indices 0-3 are positive, 4-7 are negative
        throw std::invalid_argument("dir not positive!");
    
    U64 ray = Game::rayAttacks[pos][dir];
    if (ray == 0) return ray;
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
    if (dir < 4) // rayAttacks indices 0-3 are positive, 4-7 are negative
        throw std::invalid_argument("dir not negative!");
    
    U64 ray = Game::rayAttacks[pos][dir];
    if (ray == 0) return ray;
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

std::vector<Move> Game::knightAttacksToMoves(U64 attacks, int pos)
{
    std::vector<Move> results;
    int num = __builtin_popcountll(attacks);
    int dest = -1;
    for (int i = 0; i < num; i++)
    {
        int ind = __builtin_ffsll(attacks); // returns 1 + index of LSB
        attacks >>= ind;
        dest += ind;
        results.emplace_back(pos, dest);
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

    pawnTargetsToMoves(pushMoves, dir * 8, moves);
    pawnTargetsToMoves(dblPushMoves, dir * 16, moves);
    pawnTargetsToMoves(lAttacks, dir * 9, moves);
    pawnTargetsToMoves(rAttacks, dir * 7, moves);

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
        return pawns >> 9 & position.blackPieces;
    }
    else
    {
        U64 mask = 0b0111111101111111011111110111111101111111011111110111111101111111; // all but h file
        U64 pawns = position.blackPawns & mask;
        return pawns << 9 & position.whitePieces;
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
        return pawns >> 7 & position.blackPieces;
    }
    else
    {
        U64 mask = 0b1111111011111110111111101111111011111110111111101111111011111110; // all but a file
        U64 pawns = position.blackPawns & mask;
        return pawns << 7 & position.whitePieces;
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
                moves.emplace_back(pos + offset, pos);
            else
            {
                int dir = offset > 0 ? -1 : 1; // basically the opposite sign of the offset
                moves.emplace_back(pos + offset, pos + dir * 8);
                moves.emplace_back(pos + offset, pos + dir * 16);
                moves.emplace_back(pos + offset, pos + dir * 24);
                moves.emplace_back(pos + offset, pos + dir * 32);
            }
        }
    }
}

bool Game::isCheck(int color)
{
    U64 king = color == Position::whiteID ? position.whiteKing : position.blackKing;
    U64 friendlyPieces = color == Position::whiteID ? position.whitePieces : position.blackPieces;
    int kingPos = __builtin_ffsll(king) - 1; // builtin returns 1 + index of LSB
    
    // check for knights attacking
    U64 opKnights = color == Position::whiteID ? position.blackKnights : position.whiteKnights;
    if (knightAttacks[kingPos] & opKnights) return true;

    // check for pawns attacking
    if (king & pawnLAttacks(!color)) return true;
    if (king & pawnRAttacks(!color)) return true;

    // rooks/queens/bishops - for all of these, it only intersects occupency at the end of the ray if it is an oppent's piece
    U64 opRooks = color == Position::whiteID ? position.blackRooks : position.whiteRooks;
    U64 opQueens = color == Position::whiteID ? position.blackQueens : position.whiteQueens;
    U64 opBishops = color == Position::whiteID ? position.blackBishops : position.whiteBishops;
    for (int i = 1; i < 8; i += 2) // rooks/queens
    {
        U64 ray = i < 4 ? posRayAttacks(kingPos, i, friendlyPieces) : negRayAttacks(kingPos, i, friendlyPieces);
        if (ray & opRooks || ray & opQueens) return true;
    }
    for (int i = 0; i < 8; i += 2) // bishops/queens
    {
        U64 ray = i < 4 ? posRayAttacks(kingPos, i, friendlyPieces) : negRayAttacks(kingPos, i, friendlyPieces);
        if (ray & opBishops || ray & opQueens) return true;
    }
    return false;
}

void Game::movePiece(Move move)
{
    U64 piece = 1ull << move.from;
    int color;
    U64 otherPieces;
    U64 pawns;
    U64 knights;
    U64 bishops;
    U64 rooks;
    U64 queens;
    U64 king;
    if (piece & position.whitePieces)
    {
        color = Position::whiteID;
        otherPieces = position.whitePieces;
        pawns = position.whitePawns;
        knights = position.whiteKnights;
        bishops = position.whiteBishops;
        rooks = position.whiteRooks;
        queens = position.whiteQueens;
        king = position.whiteKing;
    }
    else
    {
        color = Position::blackID;
        otherPieces = position.blackPieces;
        pawns = position.blackPawns;
        knights = position.blackKnights;
        bishops = position.blackBishops;
        rooks = position.blackRooks;
        queens = position.blackQueens;
        king = position.blackKing;
    }

    clearSquare(move.to);
    if (piece & pawns) // it is a pawn
    {
        if (move.to >= 0 && move.to < 64)
            pawns |= 1ull << move.to;
        else
        {
            
        }
    }
    else if (piece & knights) // it is a knight
    {
        knights |= 1ull << move.to;
    }
    else if (piece & bishops)
    {
        bishops |= 1ull << move.to;
    }
    else if (piece & rooks)
    {
        rooks |= 1ull << move.to;
    }
    else if (piece & queens)
    {
        queens |= 1ull << move.to;
    }
    else if (piece & king)
    {
        queens |= 1ull << move.to;
    }
    clearSquare(move.from);
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

int main()
{
    Game g = Game("8/1P2k3/8/8/8/8/8/3K4 w - -");
    // Game g = Game();
    g.position.print();
    // std::vector<Move> moves = g.getAllMoves(Position::whiteID);
    // std::cout << moves.size() << std::endl;
    std::vector<Move> moves = g.getAllPawnMoves(Position::whiteID);
    for (Move& m : moves)
    {
        std::cout << m.from << "->" << m.to << std::endl;
    }
    // std::cout << std::endl;
    // moves = g.getAllKnightMoves(Position::whiteID);
    // for (Move& m : moves)
    // {
    //     std::cout << m.from << "->" << m.to << std::endl;
    // }
    // std::cout << std::endl;
    // moves = g.getAllKingMoves(Position::whiteID);
    // for (Move& m : moves)
    // {
    //     std::cout << m.from << "->" << m.to << std::endl;
    // }
    // std::cout << std::endl << "bishops: " << std::endl;
    // moves = g.getAllBishopMoves(Position::whiteID);
    // for (Move& m : moves)
    // {
    //     std::cout << m.from << "->" << m.to << std::endl;
    // }
    // std::cout << std::endl << "rooks: " << std::endl;
    // moves = g.getAllRookMoves(Position::whiteID);
    // for (Move& m : moves)
    // {
    //     std::cout << m.from << "->" << m.to << std::endl;
    // }
    // std::cout << std::endl << "queens: " << std::endl;
    // moves = g.getAllQueenMoves(Position::whiteID);
    // for (Move& m : moves)
    // {
    //     std::cout << m.from << "->" << m.to << std::endl;
    // }
}