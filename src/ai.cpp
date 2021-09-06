#include "ai.hpp"
#include <chrono>

Ai::Ai() : game() {};
Ai::Ai(std::string fen) : game(fen) {};

U64 Ai::countPositions(int color, int depth)
{
    if (depth == 0) return 1;

    Position original = game.position;

    std::vector<Move> allMoves = game.getAllMoves(color, true);
    U64 numPositions = 0;
    if (game.isCheck(color))
    {
        for (Move& m : allMoves)
        {
            game.movePiece(m);
            if (!game.isCheck(color))
            {
                // game.movePieceNoBranching(m);
                U64 n = countPositions(!color, depth - 1);
                numPositions += n;
                // if (depth == 1)
                    // std::cout << Game::indexToAlg(m.from) << Game::indexToAlg(m.to) << ": " << n << std::endl;
                // if (m.from == 11 && m.to == -6 && depth == 2)
                        // game.position.print();
            }
            game.position = original;
        }
    }
    else
    {
        for (Move& m : allMoves)
        {
            // game.movePiece(m);
            // if (m.piece != Position::kingIndex)
            // {
            //     if (!game.RQBCheck(color))
            //     {
            //         U64 n = countPositions(!color, depth - 1);
            //         numPositions += n;
            //         // if (depth == 1)
            //             // std::cout << Game::indexToAlg(m.from) << Game::indexToAlg(m.to) << ": " << n << std::endl;
            //     }
            // }
            // else
            // {
            //     if (!game.isCheck(color))
            //     {
            //         // game.movePieceNoBranching(m);
            //         U64 n = countPositions(!color, depth - 1);
            //         numPositions += n;
            //         // if (depth == 1)
            //             // std::cout << Game::indexToAlg(m.from) << Game::indexToAlg(m.to) << ": " << n << std::endl;
            //         // if (m.from == 11 && m.to == -6 && depth == 2)
            //             // game.position.print();
            //     }
            // }
            // game.position = original;
            if (!game.moveCausesCheck(m, color)) // moveCausesCheck makes the move
            {
                U64 n = countPositions(!color, depth - 1);
                numPositions += n;
            }
            game.position = original;
        }
    }
    // for (Move& m : allMoves)
    // {
    //     game.movePiece(m);
    //     if (!game.isCheck(color))
    //     {
    //         // game.movePieceNoBranching(m);
    //         U64 n = countPositions(!color, depth - 1);
    //         numPositions += n;
    //         // if (depth == 1)
    //             // std::cout << Game::indexToAlg(m.from) << Game::indexToAlg(m.to) << ": " << n << std::endl;
    //         // if (m.from == 11 && m.to == -6 && depth == 2)
    //             // game.position.print();
    //     }
    //     game.position = original;
    // }
    return numPositions;
}

int Ai::alphaBeta(int depth, int alpha, int beta, int color)
{
    //? is this a problem - let's say, hypothetically, that it is checkmate, but the depth is 0.
    //? this will still return the static evaluation even though it should return +-infinity
    //? is this true????
    if (depth == 0) return evaluate(game.position);

    std::vector<Move> allMoves = game.getAllMoves(color, true);
    Position original = game.position;

    if (color == Position::whiteID) // maximising player
    {
        if (allMoves.size() == 0) // checkmate or stalemate
        {
            if (game.isCheck(color)) // checkmate
                return INT32_MIN + (DEPTH - depth); // really bad to be checkmated
            return 0; // stalemate is equal
        }

        // for (Move& move : allMoves)
        // {
        //     game.movePiece(move);
        //     // game.movePieceNoBranching(move);
        //     int val = alphaBeta(depth - 1, alpha, beta, !color);
        //     game.position = original;
        //     if (val >= beta) return beta; // sort of like if this score is too good to be true, 
        //                                   // black will never allow this position to be reached,
        //                                   // so we can ignore other moves from this position
        //     if (val > alpha) alpha = val;
        // }
        if (game.isCheck(color))
        {
            for (Move& m : allMoves)
            {
                game.movePiece(m);
                if (!game.isCheck(color))
                {
                    int val = alphaBeta(depth - 1, alpha, beta, !color);
                    if (val >= beta) return beta; // sort of like if this score is too good to be true, 
                                                // black will never allow this position to be reached,
                                                // so we can ignore other moves from this position
                    if (val > alpha) alpha = val;
                }
                game.position = original;
            }
        }
        else
        {
            for (Move& m : allMoves)
            {
                if (!game.moveCausesCheck(m, color)) // moveCausesCheck makes the move
                {
                    int val = alphaBeta(depth - 1, alpha, beta, !color);
                    if (val >= beta) return beta; // sort of like if this score is too good to be true, 
                                                // black will never allow this position to be reached,
                                                // so we can ignore other moves from this position
                    if (val > alpha) alpha = val;
                }
                game.position = original;
            }
        }
        return alpha;
    }
    else
    {
        if (allMoves.size() == 0)
        {
            if (game.isCheck(color))
                return INT32_MAX - (DEPTH - depth); // really bad (good for white) to be checkmated
            return 0;
        }

        // for (Move& move : allMoves)
        // {
        //     game.movePiece(move);
        //     // game.movePieceNoBranching(move);
        //     int val = alphaBeta(depth - 1, alpha, beta, !color);
        //     game.position = original;
        //     if (val <= alpha) return alpha;
        //     if (val < beta)
        //         beta = val;
        // }
        if (game.isCheck(color))
        {
            for (Move& m : allMoves)
            {
                game.movePiece(m);
                if (!game.isCheck(color))
                {
                    int val = alphaBeta(depth - 1, alpha, beta, !color);
                    if (val <= alpha) return alpha;
                    if (val < beta) beta = val;
                }
                game.position = original;
            }
        }
        else
        {
            for (Move& m : allMoves)
            {
                if (!game.moveCausesCheck(m, color)) // moveCausesCheck makes the move
                {
                    int val = alphaBeta(depth - 1, alpha, beta, !color);
                    if (val <= alpha) return alpha;
                    if (val < beta) beta = val;
                }
                game.position = original;
            }
        }
        return beta;
    }
}

void Ai::makeBestMoveAB(int color)
{
    //! current problem - when it sees a good move and multiple ways to get there,
    //! it never picks the one that actually gets it there, plays a diff move knowing
    //! that it is winning
    int depth = DEPTH;
    if (__builtin_popcountll(game.position.occupency) < 12) depth++;

    std::vector<Move> allMoves = game.getAllMoves(color, true);
    Position original = game.position;
    // i think this is good - cause we want to remember them
    int alpha = INT32_MIN;
    int beta = INT32_MAX;
    Move bestMove(0, 0, 0);

    if (color == Position::whiteID)
    {
        int bestScore = INT32_MIN;
        if (game.isCheck(color))
        {
            for (Move& move : allMoves)
            {
                game.movePiece(move);
                if (!game.isCheck(color))
                {
                    // ok so - you can't prune any of the first level moves, all calls to AB should be with +inf and -inf
                    int score = alphaBeta(depth - 1, alpha, beta, !color);
                    std::cout << game.indexToAlg(move.from) << game.indexToAlg(move.to) << ": " << score << std::endl;
                    //? i dont think we need the other alpha beta code............
                    if (score > bestScore)
                    {
                        bestScore = score;
                        bestMove = move;
                    }
                }
                game.position = original;
            }
        }
        else
        {
            for (Move& move : allMoves)
            {
                if (!game.moveCausesCheck(move, color)) // moveCausesCheck makes the move
                {
                    int score = alphaBeta(depth - 1, alpha, beta, !color);
                    std::cout << game.indexToAlg(move.from) << game.indexToAlg(move.to) << ": " << score << std::endl;
                    //? i dont think we need the other alpha beta code............
                    if (score > bestScore)
                    {
                        bestScore = score;
                        bestMove = move;
                    }
                }
                game.position = original;
            }
        }
    }
    else
    {
        int bestScore = INT32_MAX;
        // for (Move& move : allMoves)
        // {
        //     game.movePiece(move);
        //     // game.movePieceNoBranching(move);
        //     int score = alphaBeta(depth - 1, alpha, beta, !color);
        //     // std::cout << Game::indexToAlg(move.from) << Game::indexToAlg(move.to) << ": " << score << std::endl;
        //     game.position = original;
        //     //? i dont think we need the other alpha beta code.......
        //     if (score < bestScore)
        //     {
        //         bestScore = score;
        //         bestMove = move;
        //     }
        // }
        if (game.isCheck(color))
        {
            for (Move& move : allMoves)
            {
                game.movePiece(move);
                if (!game.isCheck(color))
                {
                    // ok so - you can't prune any of the first level moves, all calls to AB should be with +inf and -inf
                    int score = alphaBeta(depth - 1, alpha, beta, !color);
                    //? i dont think we need the other alpha beta code............
                    if (score < bestScore)
                    {
                        bestScore = score;
                        bestMove = move;
                    }
                }
                game.position = original;
            }
        }
        else
        {
            for (Move& move : allMoves)
            {
                if (!game.moveCausesCheck(move, color)) // moveCausesCheck makes the move
                {
                    int score = alphaBeta(depth - 1, alpha, beta, !color);
                    //? i dont think we need the other alpha beta code............
                    if (score < bestScore)
                    {
                        bestScore = score;
                        bestMove = move;
                    }
                }
                game.position = original;
            }
        }
    }

    std::cout << bestMove.from << " " << bestMove.to << std::endl;
    // std::cout << Game::indexToAlg(bestMove.from) << Game::indexToAlg(bestMove.to) << std::endl;
    // game.movePiece(bestMove);
}

int Ai::evaluate(Position p)
{
    int whiteScore = 0;
    int blackScore = 0;

    whiteScore += __builtin_popcountll(p.whitePawns) * pawnVal;
    whiteScore += __builtin_popcountll(p.whiteKnights) * knightVal;
    whiteScore += __builtin_popcountll(p.whiteBishops) * bishopVal;
    whiteScore += __builtin_popcountll(p.whiteRooks) * rookVal;
    whiteScore += __builtin_popcountll(p.whiteQueens) * queenVal;

    blackScore += __builtin_popcountll(p.blackPawns) * pawnVal;
    blackScore += __builtin_popcountll(p.blackKnights) * knightVal;
    blackScore += __builtin_popcountll(p.blackBishops) * bishopVal;
    blackScore += __builtin_popcountll(p.blackRooks) * rookVal;
    blackScore += __builtin_popcountll(p.blackQueens) * queenVal;
    
    // use the piece maps to get more accurate score
    /* pawns */
    U64 whitePawns = game.position.whitePawns;
    whitePawns &= whitePawnMap1;
    whiteScore += __builtin_popcountll(whitePawns) / 2; // divide by 2 becuase there's a lot of pawns
    whitePawns &= whitePawnMap2;
    whiteScore += __builtin_popcountll(whitePawns); // count them again if they're good
    whitePawns &= whitePawnMap3;
    whiteScore += __builtin_popcountll(whitePawns) * 2; // these are the pawns that can promote - really good

    U64 blackPawns = game.position.blackPawns;
    blackPawns &= blackPawnMap1;
    blackScore += __builtin_popcountll(blackPawns) / 2; // divide by 2 because there's a lot of pawns
    blackPawns &= blackPawnMap2;
    blackScore += __builtin_popcountll(blackPawns); // count them again if they're good
    blackPawns &= blackPawnMap3;
    blackScore += __builtin_popcountll(blackPawns) * 2; // these are the pawns that can promote - really good

    /* knights */
    U64 whiteKnights = game.position.whiteKnights;
    whiteKnights &= knightBonusMap1;
    whiteScore += __builtin_popcountll(whiteKnights);
    whiteKnights &= knightBonusMap2;
    whiteScore += __builtin_popcountll(whiteKnights); // count again if good
    whiteKnights &= knightBonusMap3;
    whiteScore += __builtin_popcountll(whiteKnights); // count again if really good

    U64 blackKnights = game.position.blackKnights;
    blackKnights &= knightBonusMap1;
    blackScore += __builtin_popcountll(blackKnights);
    blackKnights &= knightBonusMap2;
    blackScore += __builtin_popcountll(blackKnights); // count again if good
    blackKnights &= knightBonusMap3;
    blackScore += __builtin_popcountll(blackKnights); // count again if really good

    /* bishops */
    U64 whiteBishops = game.position.whiteBishops;
    whiteBishops &= whiteBishopMap1;
    whiteScore += __builtin_popcountll(whiteBishops);
    whiteBishops &= whiteBishopMap2;
    whiteScore += __builtin_popcountll(whiteBishops); // count again if good
    whiteBishops &= whiteBishopMap3;
    whiteScore += __builtin_popcountll(whiteBishops); // count again if really good

    U64 blackBishops = game.position.blackBishops;
    blackBishops &= blackBishopMap1;
    blackScore += __builtin_popcountll(blackBishops);
    blackBishops &= blackBishopMap2;
    blackScore += __builtin_popcountll(blackBishops); // count again if good
    blackBishops &= blackBishopMap3;
    blackScore += __builtin_popcountll(blackBishops); // count again if really good

    /* rooks */
    U64 whiteRooks = game.position.whiteRooks;
    whiteRooks &= whiteRookMap1;
    whiteScore += __builtin_popcountll(whiteRooks);
    whiteRooks &= whiteRookMap2;
    whiteScore += __builtin_popcountll(whiteRooks); // count again if good

    U64 blackRooks = game.position.blackRooks;
    blackRooks &= blackRookMap1;
    blackScore += __builtin_popcountll(blackRooks);
    blackRooks &= blackRookMap2;
    blackScore += __builtin_popcountll(blackRooks); // count again if good

    /* queens */
    U64 whiteQueens = game.position.whiteQueens;
    whiteQueens &= queenBonusMap1;
    whiteScore += __builtin_popcountll(whiteQueens);
    whiteQueens &= queenBonusMap2;
    whiteScore += __builtin_popcountll(whiteQueens); // count again if good
    whiteQueens &= queenBonusMap3;
    whiteScore += __builtin_popcountll(whiteQueens); // count again if really good

    U64 blackQueens = game.position.blackQueens;
    blackQueens &= queenBonusMap1;
    blackScore += __builtin_popcountll(blackQueens);
    blackQueens &= queenBonusMap2;
    blackScore += __builtin_popcountll(blackQueens); // count again if good
    blackQueens &= queenBonusMap3;
    blackScore += __builtin_popcountll(blackQueens); // count again if really good

    /* kings */
    U64 whiteKing = game.position.whiteKing;
    whiteKing &= whiteKingMap1;
    whiteScore += __builtin_popcountll(whiteKing);
    whiteKing &= whiteKingMap2;
    whiteScore += __builtin_popcountll(whiteKing);

    U64 blackKing = game.position.blackKing;
    blackKing &= blackKingMap1;
    blackScore += __builtin_popcountll(blackKing);
    blackKing &= blackKingMap2;
    blackScore += __builtin_popcountll(blackKing);

    return whiteScore - blackScore;
}

void printU64(U64 num)
{
    for (int i = 0; i < 64; i++)
    {
        if (i % 8 == 0) std::cout << std::endl;
        if (1ull << i & num)
            std::cout << "■ ";
        else std::cout << ". ";
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    
    // std::string fen = argv[1]; // [0] is call to program
    // int depth = std::stoi(argv[2]);

    /* PERFT TESTS */
    // Ai ai = Ai(fen);
    Ai ai = Ai("1r1q1rk1/1bp1bpp1/p3p2p/N1pnB3/8/1P1P1N2/P1PQ1PPP/R3R1K1 w - -");

    // Ai ai = Ai("r4k1r/2pnn1p1/4bp2/pP1pp3/P3P3/2P1BP1P/2P1BN1P/2KR1R2 b - -");
    // Ai ai = Ai("r1bqkb1r/3n1ppp/p3p3/1ppp4/8/1PB1PN1P/P1PP1PP1/RN1QK2R b KQkq -");
    // Ai ai = Ai("r1b4q/1ppkn3/2n1pR2/p2p2B1/4P2N/P1P5/1PP2PP1/R3KB2 b Q -");

    // Ai ai = Ai("4k2r/pp3p1p/1b1B4/P5P1/1P1p3P/2P1r3/8/R2K1R2 b k -");
    // Ai ai = Ai("rnbqkbnr/pppppp1p/6p1/8/3PP3/8/PPP2PPP/RNBQKBNR b KQkq -");
    // Ai ai = Ai();
    // Ai ai = Ai("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"); // position 3
    // Ai ai = Ai("8/2p5/3p4/KP5r/1R2Pp1k/8/6P1/8 b - -"); // e2e4
    // Ai ai = Ai("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"); // position 2
    // Ai ai = Ai("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1P/PPPBBP1P/R3K2R b KQkq -"); // g2h3
    // Ai ai = Ai("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -"); // position 5
    // Ai ai = Ai("rnbq1k1r/pp1Pbppp/2p5/8/2B5/P7/1PP1NnPP/RNBQK2R b KQ -"); // a2a3
    // Ai ai = Ai("rnbq1k1r/pp1Pbppp/2p5/8/2B5/P7/1PP1N1PP/RNBQK2n w Q -"); // a2a3 f2h1
    // ai.game.position.print();

    // auto t1 = std::chrono::high_resolution_clock::now();
    ai.makeBestMoveAB(ai.game.position.currTurn);
    // auto t2 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double, std::milli> time = t2 - t1;
    // std::cout << time.count() << std::endl;

    // auto t1 = std::chrono::high_resolution_clock::now();
    // U64 num = ai.countPositions(ai.game.position.currTurn, depth);
    // auto t2 = std::chrono::high_resolution_clock::now();
    // std::cout << num << std::endl;
    // std::chrono::duration<double, std::milli> time = t2 - t1;
    // std::cout << time.count() << std::endl;
    




    // Game game = Game("r4k1r/2pnn1p1/4bp2/pP1pp3/P3P3/2P1BP1P/2P1BN1P/2KR1R2 b - -");
    // Position original = game.position;
    // int numTrials = 1000000;
    // auto t1 = std::chrono::high_resolution_clock::now();
    // for (int i = 0; i < numTrials; i++)
    // {
    //     game.movePiece(Move(11, 26, Position::knightIndex));
    //     game.position = original;
    // }
    // auto t2 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double, std::milli> time = t2 - t1;
    // std::cout << "Knight: " << time.count() << std::endl;

    // t1 = std::chrono::high_resolution_clock::now();
    // for (int i = 0; i < numTrials; i++)
    // {
    //     // game.movePiece(Move(14, 22, Position::pawnIndex));
    //     game.movePiece(Move(27, 36, Position::pawnIndex));
    //     game.position = original;
    // }
    // t2 = std::chrono::high_resolution_clock::now();
    // time = t2 - t1;
    // std::cout << "pawn: " << time.count() << std::endl;

    // t1 = std::chrono::high_resolution_clock::now();
    // for (int i = 0; i < numTrials; i++)
    // {
    //     // game.movePiece(Move(20, 38, Position::bishopIndex));
    //     game.movePiece(Move(20, 47, Position::bishopIndex));
    //     game.position = original;
    // }
    // t2 = std::chrono::high_resolution_clock::now();
    // time = t2 - t1;
    // std::cout << "bishop: " << time.count() << std::endl;

    // t1 = std::chrono::high_resolution_clock::now();
    // for (int i = 0; i < numTrials; i++)
    // {
    //     game.movePiece(Move(0, 2, Position::rookIndex));
    //     game.position = original;
    // }
    // t2 = std::chrono::high_resolution_clock::now();
    // time = t2 - t1;
    // std::cout << "rook: " << time.count() << std::endl;

    // t1 = std::chrono::high_resolution_clock::now();
    // for (int i = 0; i < numTrials; i++)
    // {
    //     game.movePiece(Move(5, 6, Position::kingIndex));
    //     game.position = original;
    // }
    // t2 = std::chrono::high_resolution_clock::now();
    // time = t2 - t1;
    // std::cout << "king: " << time.count() << std::endl;
}