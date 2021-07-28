#include "gamev2better.cpp"
#include <array>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>

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

std::chrono::duration<double, std::milli> evalTime;
std::chrono::duration<double, std::milli> getMoveTime;
std::chrono::duration<double, std::milli> totalTime;
long long numPositions = 0;
int DEPTH = 6;


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
            if (id == Game::pawnID) score = pawnVal;
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

std::vector<std::array<int, 2>> getAllMoves(std::array<int, 69>& board, int color)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    std::vector<std::array<int, 2>> allMoves;
    for (int i = 0; i < 64; i++)
    {
        int p = board[i];
        if (p != 0 && Game::getColor(p) == color)
        {
            std::vector<int> moves = Game::getMoves(board, Game::getX(p), Game::getY(p));
            for (int m : moves)
                allMoves.push_back({i, m});
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

        std::vector<std::array<int, 2>> allMoves = getAllMoves(board, Game::blackID);
        if (allMoves.size() == 0)
        {
            if (Game::isCheck(board, Game::blackID))
                return INT_MIN + (DEPTH - depth); // checkmate
            return 0; // stalemate
        }

        for (std::array<int, 2>& move : allMoves)
        {
            std::array<int, 69> newBoard = board;
            Game::movePiece(move[0], move[1], newBoard);
            int score = alphaBeta(newBoard, depth - 1, alpha, beta, false);
            if (score >= beta)
                return beta;
            if (score > alpha)
                alpha = score;
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

        std::vector<std::array<int, 2>> allMoves = getAllMoves(board, Game::whiteID);
        if (allMoves.size() == 0)
        {
            if (Game::isCheck(board, Game::whiteID))
            {
                return INT_MAX  - (DEPTH - depth); // checkmate
                std::cout << "checkmate found" << std::endl;
            }
            return 0; // stalemate
        }

        for (std::array<int, 2>& move : allMoves)
        {
            std::array<int, 69> newBoard = board;
            Game::movePiece(move[0], move[1], newBoard);
            int score = alphaBeta(newBoard, depth - 1, alpha, beta, true);
            if (score <= alpha)
                return alpha;
            if (score < beta)
                beta = score;
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
    numPositions = 0;
    auto t1 = std::chrono::high_resolution_clock::now();
    std::vector<std::array<int, 2>> allMoves = getAllMoves(board, color);
    std::vector<int> scores;
    scores.shrink_to_fit();
    scores.reserve(allMoves.size());
    scores.push_back(4);
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
        scores.push_back(5);
    }

    // std::array<int, 2> bestMove = allMoves[std::find(scores.begin(), scores.end(), value) - scores.begin()];
    // Game::movePiece(bestMove[0], bestMove[1], board);

    auto t2 = std::chrono::high_resolution_clock::now();
    totalTime = t2 - t1;
    std::cout << "Evaluated " << numPositions << " positions in " << totalTime.count() << " ms" << std::endl;
}

// for testing only
int main()
{
    std::array<int, 69> board = Game::FENtoBoard("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -");
    Game::initDiag();
    // auto t1 = std::chrono::high_resolution_clock::now();
    // for (int i = 0; i < 80000000; i++)
    // {
    //     Game::isCheck(board, Game::whiteID);
    // }
    // auto t2 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double, std::milli> time = t2 - t1;
    // std::cout << time.count() << " ms" << std::endl;
    makeBestMoveAB(board, Game::whiteID);
    std::cout << "eval time: " << evalTime.count() << " (" << (evalTime.count() / totalTime.count()) * 100 << "%)" << std::endl;
    std::cout << "move getting time: " << getMoveTime.count() << " (" << (getMoveTime.count() / totalTime.count()) * 100 << "%)" << std::endl;
}