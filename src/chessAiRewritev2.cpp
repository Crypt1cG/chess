#include "gamev2better.cpp"
#include <array>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <climits>

const int pawnVal = 100;
const int knightVal = 300;
const int bishopVal = 300;
const int rookVal = 500;
const int queenVal = 900;

std::array<int, 64> knightMap = {{0, 1, 2, 2, 2, 2, 1, 0,
                                  1, 2, 3, 3, 3, 3, 2, 1,
                                  2, 3, 4, 4, 4, 4, 3, 2,
                                  2, 3, 4, 4, 4, 4, 3, 2,
                                  2, 3, 4, 4, 4, 4, 3, 2,
                                  2, 3, 4, 4, 4, 4, 3, 2,
                                  1, 2, 3, 3, 3, 3, 2, 1,
                                  0, 1, 2, 2, 2, 2, 1, 0}};

std::array<int, 64> queenMap = {{0, 1, 1, 2, 2, 1, 1, 0,
                                 1, 2, 2, 2, 2, 2, 2, 1,
                                 1, 2, 3, 3, 3, 3, 2, 1,
                                 2, 2, 3, 3, 3, 3, 2, 2,
                                 2, 2, 3, 3, 3, 3, 2, 2,
                                 1, 2, 3, 3, 3, 3, 2, 1,
                                 1, 2, 2, 2, 2, 2, 2, 1,
                                 0, 1, 1, 2, 2, 1, 1, 0}};

std::array<int, 64> whiteRookMap = {{1, 1, 1, 1, 1, 1, 1, 1,
                                     1, 2, 2, 2, 2, 2, 2, 1,
                                     0, 1, 1, 1, 1, 1, 1, 0,
                                     0, 1, 1, 1, 1, 1, 1, 0,
                                     0, 1, 1, 1, 1, 1, 1, 0,
                                     0, 1, 1, 1, 1, 1, 1, 0,
                                     0, 1, 1, 1, 1, 1, 1, 0,
                                     1, 1, 1, 2, 2, 1, 1, 1}};

std::array<int, 64> blackRookMap = {{1, 1, 1, 2, 2, 1, 1, 1,
                                     0, 1, 1, 1, 1, 1, 1, 0,
                                     0, 1, 1, 1, 1, 1, 1, 0,
                                     0, 1, 1, 1, 1, 1, 1, 0,
                                     0, 1, 1, 1, 1, 1, 1, 0,
                                     0, 1, 1, 1, 1, 1, 1, 0,
                                     1, 2, 2, 2, 2, 2, 2, 1,
                                     1, 1, 1, 1, 1, 1, 1, 1}};

std::array<int, 64> whiteBishopMap = {{0, 1, 1, 1, 1, 1, 1, 0,
                                       1, 2, 2, 2, 2, 2, 2, 1,
                                       1, 2, 2, 3, 3, 2, 2, 1,
                                       1, 2, 2, 3, 3, 2, 2, 1,
                                       1, 2, 3, 2, 2, 3, 2, 1,
                                       1, 2, 2, 2, 2, 2, 2, 1,
                                       1, 2, 2, 2, 2, 2, 2, 1,
                                       2, 1, 1, 1, 1, 1, 1, 2}};

std::array<int, 64> blackBishopMap = {{2, 1, 1, 1, 1, 1, 1, 2,
                                       1, 2, 2, 2, 2, 2, 2, 1,
                                       1, 2, 2, 2, 2, 2, 2, 1,
                                       1, 2, 3, 2, 2, 3, 2, 1,
                                       1, 2, 2, 3, 3, 2, 2, 1,
                                       1, 2, 2, 3, 3, 2, 2, 1,
                                       1, 2, 2, 2, 2, 2, 2, 1,
                                       0, 1, 1, 1, 1, 1, 1, 0}};

std::array<int, 64> blackPawnMap = {{1, 1, 1, 1, 1, 1, 1, 1,
                                     1, 1, 1, 0, 0, 1, 1, 1,
                                     1, 1, 0, 1, 1, 0, 1, 1,
                                     1, 1, 1, 2, 2, 1, 1, 1,
                                     1, 1, 1, 2, 2, 1, 1, 1,
                                     1, 1, 2, 2, 2, 2, 1, 1,
                                     3, 3, 3, 3, 3, 3, 3, 3,
                                     1, 1, 1, 1, 1, 1, 1, 1}};

std::array<int, 64> whitePawnMap = {{1, 1, 1, 1, 1, 1, 1, 1,
                                     3, 3, 3, 3, 3, 3, 3, 3,
                                     1, 1, 2, 2, 2, 2, 1, 1,
                                     1, 1, 1, 2, 2, 1, 1, 1,
                                     1, 1, 1, 2, 2, 1, 1, 1,
                                     1, 1, 0, 1, 1, 0, 1, 1,
                                     1, 1, 1, 0, 0, 1, 1, 1,
                                     1, 1, 1, 1, 1, 1, 1, 1}};

std::chrono::duration<double, std::milli> evalTime;
std::chrono::duration<double, std::milli> getMoveTime;
std::chrono::duration<double, std::milli> totalTime;
long long numPositions = 0;
int DEPTH = 7;
int ENDGAME_DEPTH = 7;


/**
 * returns a number representing the static evaluation of a board for black
 * 
 * @param board the board to be evaluated
 * @return the score (positive if good for black, negative if good for white)
 */
int evaluate(std::array<int, 69>& board)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    int whiteScore = 0;
    int blackScore = 0;
    for (int i = 0; i < 64; i++)
    {
        int p = board[i];
        if (p != 0)
        {
            int id = Game::getID(p);
            int score = 0;
            if (id == Game::pawnID)
            {
                score = pawnVal;
                // if (Game::getColor(p) == Game::whiteID)
                //     score += whitePawnMap[i];
                // else score += blackPawnMap[i];
            }
            else if (id == Game::knightID) 
            {
                score = knightVal;
                score += knightMap[i];
            }
            else if (id == Game::bishopID) 
            {
                score = bishopVal;
                if (Game::getColor(p) == Game::whiteID)
                    score += whiteBishopMap[i];
                else score += blackBishopMap[i];
            }
            else if (id == Game::rookID) 
            {
                score = rookVal;
                if (Game::getColor(p) == Game::whiteID)
                    score += whiteRookMap[i];
                else score += blackRookMap[i];
            }
            else if (id == Game::queenID) 
            {
                score = queenVal;
                score += queenMap[i];
            }

            if (Game::getColor(p) == Game::whiteID)
                whiteScore += score;
            else blackScore += score;
        } 
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    evalTime += t2 - t1;
    return blackScore - whiteScore;
}

std::vector<std::array<int, 2>> getAllMoves(std::array<int, 69>& board, int color, bool pseudoLegal = false)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    std::vector<std::array<int, 2>> allMoves;
    for (int i = 0; i < 64; i++)
    {
        int p = board[i];
        if (p != 0 && Game::getColor(p) == color)
        {
            // std::vector<int> moves = Game::getMoves(board, Game::getX(p), Game::getY(p), pseudoLegal);
            // for (int m : moves)
            //     allMoves.push_back({i, m});
            std::vector<std::array<int, 2>> moves = Game::getMovesNew(board, Game::getX(p), Game::getY(p), pseudoLegal);
            allMoves.reserve(moves.size());
            allMoves.insert(allMoves.end(), moves.begin(), moves.end());
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    getMoveTime += t2 - t1;
    return allMoves;
}

int miniMax(std::array<int, 69>& board, int depth, bool maximisingPlayer)
{
    if (depth == 0) 
    {
        numPositions++; 
        return evaluate(board);
    }

    if (maximisingPlayer) // black
    {
        std::vector<std::array<int, 2>> allMoves = getAllMoves(board, Game::blackID);
        if (allMoves.size() == 0)
        {
            if (Game::isCheck(board, Game::blackID))
                return INT_MIN + (DEPTH - depth); // checkmate - make sooner ones worth more
            return 0; // stalemate
        }

        int value = INT_MIN;
        for (std::array<int, 2>& move : allMoves)
        {
            std::array<int, 69> newBoard = board;
            Game::movePiece(move[0], move[1], newBoard);
            value = std::max(value, miniMax(newBoard, depth - 1, false));
        }
        return value;
    }
    else
    {
        std::vector<std::array<int, 2>> allMoves = getAllMoves(board, Game::whiteID);
        if (allMoves.size() == 0)
        {
            if (Game::isCheck(board, Game::whiteID))
                return INT_MAX - (DEPTH - depth); // checkmate - make sooner ones worth more
            return 0; // stalemate
        }

        int value = INT_MAX;
        for (std::array<int, 2>& move : allMoves)
        {
            std::array<int, 69> newBoard = board;
            Game::movePiece(move[0], move[1], newBoard);
            value = std::min(value, miniMax(newBoard, depth - 1, true));
        }
        return value;
    }
}

int alphaBeta(std::array<int, 69>& board, int depth, int alpha, int beta, bool maximisingPlayer)
{
    if (maximisingPlayer) // black
    {
        if (depth == 0) 
        {
            numPositions++; 
            return evaluate(board);
        }

        std::vector<std::array<int, 2>> allMoves = getAllMoves(board, Game::blackID, true);
        if (allMoves.size() == 0)
        {
            if (Game::isCheck(board, Game::blackID))
                return INT_MIN + (DEPTH - depth); // checkmate add DEPTH - depth to make further moves worse
            return 0; // stalemate
        }

        for (std::array<int, 2>& move : allMoves)
        {
            std::array<int, 69> newBoard = board;
            Game::movePiece(move[0], move[1], newBoard);
            if (!Game::isCheck(newBoard, Game::blackID))
            {
                int score = alphaBeta(newBoard, depth - 1, alpha, beta, false);
                if (score >= beta)
                    return beta;
                if (score > alpha)
                    alpha = score;
            }
        }
        return alpha;
    }
    else
    {
        if (depth == 0) 
        {
            numPositions++; 
            return evaluate(board);
        }

        std::vector<std::array<int, 2>> allMoves = getAllMoves(board, Game::whiteID, true);
        if (allMoves.size() == 0)
        {
            if (Game::isCheck(board, Game::whiteID))
            {
                return INT_MAX  - (DEPTH - depth); // checkmate sub DEPTH - depth to make further moves worse
                std::cout << "checkmate found" << std::endl;
            }
            return 0; // stalemate
        }

        for (std::array<int, 2>& move : allMoves)
        {
            std::array<int, 69> newBoard = board;
            Game::movePiece(move[0], move[1], newBoard);
            if (!Game::isCheck(newBoard, Game::whiteID))
            {
                int score = alphaBeta(newBoard, depth - 1, alpha, beta, true);
                if (score <= alpha)
                    return alpha;
                if (score < beta)
                    beta = score;
            }
        }
        return beta;
    }
}

/**
 * uses minimax to score all possible moves and make the best move
 * 
 * @param board the board
 * @param color the color (only works with black rn)
 */
void makeBestMove(std::array<int, 69>& board, int color)
{
    numPositions = 0;
    auto t1 = std::chrono::high_resolution_clock::now();
    std::vector<std::array<int, 2>> allMoves = getAllMoves(board, color);
    std::vector<int> scores;

    if (allMoves.size() == 0) // checkmate or stalemate, can't make any moves
        return;

    int value;
    if (color == Game::blackID)
        value = INT_MIN;
    else value = INT_MAX;
    for (std::array<int, 2>& move : allMoves)
    {
        std::array<int, 69> newBoard = board;
        Game::movePiece(move[0], move[1], newBoard);
        int score = miniMax(newBoard, DEPTH - 1, !color);
        if (color == Game::blackID)
            value = std::max(value, score);
        else value = std::min(value, score);
        scores.push_back(score);
    }

    std::array<int, 2> bestMove = allMoves[std::find(scores.begin(), scores.end(), value) - scores.begin()];
    Game::movePiece(bestMove[0], bestMove[1], board);

    auto t2 = std::chrono::high_resolution_clock::now();
    totalTime = t2 - t1;
    std::cout << "Evaluated " << numPositions << " positions in " << totalTime.count() << " ms" << std::endl;
}

void makeBestMoveAB(std::array<int, 69>& board, int color)
{
    int numPieces = 0;
    for (int i = 0; i < 64; i++)
        if (board[i] != 0) numPieces++;
    if (numPieces < 5) DEPTH = ENDGAME_DEPTH + 1; // when there's not that many pieces, can increase depth
    else if (numPieces < 9) DEPTH = ENDGAME_DEPTH;

    numPositions = 0;
    auto t1 = std::chrono::high_resolution_clock::now();
    std::vector<std::array<int, 2>> allMoves = getAllMoves(board, color);
    std::vector<int> scores;
    scores.reserve(allMoves.size());

    if (allMoves.size() == 0) // checkmate or stalemate, can't make any moves
        return;

    int value;
    if (color == Game::blackID)
        value = INT_MIN;
    else value = INT_MAX;
    for (std::array<int, 2> move : allMoves)
    {
        std::array<int, 69> newBoard = board;
        Game::movePiece(move[0], move[1], newBoard);
        int score = alphaBeta(newBoard, DEPTH - 1, INT_MIN, INT_MAX, !color);
        if (color == Game::blackID)
            value = std::max(value, score);
        else value = std::min(value, score);
        // // int score = 5;
        scores.push_back(score);
    }
    std::array<int, 2> bestMove = allMoves[std::find(scores.begin(), scores.end(), value) - scores.begin()];
    std::cout << bestMove[0] << " " << bestMove[1] << std::endl;
    Game::movePiece(bestMove[0], bestMove[1], board);

    auto t2 = std::chrono::high_resolution_clock::now();
    totalTime = t2 - t1;
    // std::cout << "Evaluated " << numPositions << " positions in " << totalTime.count() << " ms" << std::endl;
}

long long countPositions(std::array<int, 69>& board, int color, int depth)
{
    if (depth == 0) return 1;
    else
    {
        long long count = 0;
        std::vector<std::array<int, 2>> allMoves = getAllMoves(board, color);
        for (std::array<int, 2> move : allMoves)
        {
            std::array<int, 69> newBoard = board;
            Game::movePiece(move[0], move[1], newBoard);
            // if (!Game::isCheck(newBoard, color))
            // {
                int c = countPositions(newBoard, !color, depth - 1); 
                // if (depth == 5)
                    // std::cout << Game::indexToAlg(move[0]) << "->" << Game::indexToAlg(move[1]) << ": " << c << std::endl;
                // if (depth == 3 && Game::indexToAlg(move[0]) == "a1")
                // {
                //     Game::printBoard(newBoard);
                //     std::cout << newBoard[64] << std::endl;
                // }
                count += c;
            // }
        }
        return count;
    }
}

// for testing only
int main(int argc, char *argv[])
{
    std::string FEN = argv[1]; // 0 is call to exe, 1 is first arg
    // for (int i = 0; i < argc; i++)
    //     std::cout << argv[i] << std::endl;

    // std::array<int, 69> board = Game::FENtoBoard("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");

    std::array<int, 69> board = Game::FENtoBoard(FEN);
    // std::array<int, 69> board = Game::FENtoBoard("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
    Game::initDiag();
    Game::initRook();

    /* FOR REGULAR USE */
    // auto t1 = std::chrono::high_resolution_clock::now();
    // makeBestMoveAB(board, board[66]); // need to make this use the id from the fen string
    // auto t2 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double, std::milli> time = t2 - t1;
    // std::cout << time.count() << " ms" << std::endl;

    /* FOR PERFT TESTS */
    int d = std::stoi(argv[2]);
    long long num = countPositions(board, board[66], d);
    std::cout << num << std::endl;

    // auto t1 = std::chrono::high_resolution_clock::now();
    // for (int i = 0; i < 80000000; i++)
    // {
    //     Game::isCheck(board, Game::whiteID);
    // }
    // auto t2 = std::chrono::high_resolution_clock::now()"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -";
    // std::chrono::duration<double, std::milli> time = t2 - t1;
    // std::cout << time.count() << " ms" << std::endl;
    // makeBestMoveAB(board, Game::whiteID);
    // std::cout << "eval time: " << evalTime.count() << " (" << (evalTime.count() / totalTime.count()) * 100 << "%)" << std::endl;
    // std::cout << "move getting time: " << getMoveTime.count() << " (" << (getMoveTime.count() / totalTime.count()) * 100 << "%)" << std::endl;
}