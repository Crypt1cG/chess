// #include "gamev2better.cpp"
#include "board.hpp"
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
int evaluate(Board& board)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    int whiteScore = 0;
    int blackScore = 0;
    for (int i = 0; i < 64; i++)
    {
        int p = board.squares[i];
        if (p != 0)
        {
            int id = Board::getID(p);
            int score = 0;
            if (id == Board::pawnID)
            {
                score = pawnVal;
                // if (Game::getColor(p) == Game::whiteID)
                //     score += whitePawnMap[i];
                // else score += blackPawnMap[i];
            }
            else if (id == Board::knightID) 
            {
                score = knightVal;
                score += knightMap[i];
            }
            else if (id == Board::bishopID) 
            {
                score = bishopVal;
                if (Board::getColor(p) == Board::whiteID)
                    score += whiteBishopMap[i];
                else score += blackBishopMap[i];
            }
            else if (id == Board::rookID) 
            {
                score = rookVal;
                if (Board::getColor(p) == Board::whiteID)
                    score += whiteRookMap[i];
                else score += blackRookMap[i];
            }
            else if (id == Board::queenID) 
            {
                score = queenVal;
                score += queenMap[i];
            }

            if (Board::getColor(p) == Board::whiteID)
                whiteScore += score;
            else blackScore += score;
        } 
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    evalTime += t2 - t1;
    return blackScore - whiteScore;
}

std::vector<Move> getAllMoves(Board& board, bool pseudoLegal = false)
{
    int color = board.currTurn;
    auto t1 = std::chrono::high_resolution_clock::now();
    std::vector<Move> allMoves;
    for (int i = 0; i < 64; i++)
    {
        int p = board.squares[i];
        if (p != 0 && Board::getColor(p) == color)
        {
            // std::vector<int> moves = Game::getMoves(board, Game::getX(p), Game::getY(p), pseudoLegal);
            // for (int m : moves)
            //     allMoves.push_back({i, m});
            std::vector<Move> moves = board.getMoves(i, pseudoLegal);
            allMoves.reserve(moves.size());
            allMoves.insert(allMoves.end(), moves.begin(), moves.end());
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    getMoveTime += t2 - t1;
    return allMoves;
}

int miniMax(Board& board, int depth, bool maximisingPlayer)
{
    if (depth == 0) 
    {
        numPositions++; 
        return evaluate(board);
    }

    if (maximisingPlayer) // black
    {
        std::vector<Move> allMoves = getAllMoves(board);
        if (allMoves.size() == 0)
        {
            if (board.isCheck(Board::blackID))
                return INT_MIN + (DEPTH - depth); // checkmate - make sooner ones worth more
            return 0; // stalemate
        }

        int value = INT_MIN;
        for (Move& move : allMoves)
        {
            Board newBoard = board;
            newBoard.movePiece(move);
            value = std::max(value, miniMax(newBoard, depth - 1, false));
        }
        return value;
    }
    else
    {
        std::vector<Move> allMoves = getAllMoves(board, Board::whiteID);
        if (allMoves.size() == 0)
        {
            if (board.isCheck(Board::whiteID))
                return INT_MAX - (DEPTH - depth); // checkmate - make sooner ones worth more
            return 0; // stalemate
        }

        int value = INT_MAX;
        for (Move& move : allMoves)
        {
            Board newBoard = board;
            newBoard.movePiece(move);
            value = std::min(value, miniMax(newBoard, depth - 1, true));
        }
        return value;
    }
}

int alphaBeta(Board& board, int depth, int alpha, int beta, bool maximisingPlayer)
{
    if (maximisingPlayer) // black
    {
        if (depth == 0) 
        {
            numPositions++; 
            return evaluate(board);
        }

        std::vector<Move> allMoves = getAllMoves(board, true); // adding true makes it pseudolegal moves
        if (allMoves.size() == 0)
        {
            if (board.isCheck(Board::blackID))
                return INT_MIN + (DEPTH - depth); // checkmate add DEPTH - depth to make further moves worse
            return 0; // stalemate
        }

        for (Move& move : allMoves)
        {
            Board newBoard = board;
            newBoard.movePiece(move);
            if (!newBoard.isCheck(Board::blackID)) // since doing pseudolegal moves, have to check for check
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

        std::vector<Move> allMoves = getAllMoves(board, true); // adding true makes it pseudolegal moves
        if (allMoves.size() == 0)
        {
            if (board.isCheck(Board::whiteID))
            {
                return INT_MAX  - (DEPTH - depth); // checkmate sub DEPTH - depth to make further moves worse
                std::cout << "checkmate found" << std::endl;
            }
            return 0; // stalemate
        }

        for (Move& move : allMoves)
        {
            Board newBoard = board;
            newBoard.movePiece(move);
            if (!newBoard.isCheck(Board::whiteID)) // since doing pseudolegal moves, have to check for check
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
void makeBestMove(Board& board, int color)
{
    numPositions = 0;
    auto t1 = std::chrono::high_resolution_clock::now();
    std::vector<Move> allMoves = getAllMoves(board, color);
    std::vector<int> scores;

    if (allMoves.size() == 0) // checkmate or stalemate, can't make any moves
        return;

    int value;
    if (color == Board::blackID)
        value = INT_MIN;
    else value = INT_MAX;
    for (Move& move : allMoves)
    {
        Board newBoard = board;
        newBoard.movePiece(move);
        int score = miniMax(newBoard, DEPTH - 1, !color);
        if (color == Board::blackID)
            value = std::max(value, score);
        else value = std::min(value, score);
        scores.push_back(score);
    }

    Move bestMove = allMoves[std::find(scores.begin(), scores.end(), value) - scores.begin()];
    board.movePiece(bestMove);

    auto t2 = std::chrono::high_resolution_clock::now();
    totalTime = t2 - t1;
    std::cout << "Evaluated " << numPositions << " positions in " << totalTime.count() << " ms" << std::endl;
}

void makeBestMoveAB(Board& board, int color)
{
    int numPieces = 0;
    for (int i = 0; i < 64; i++)
        if (board.squares[i] != 0) numPieces++;
    if (numPieces < 5) DEPTH = ENDGAME_DEPTH + 1; // when there's not that many pieces, can increase depth
    else if (numPieces < 9) DEPTH = ENDGAME_DEPTH;

    numPositions = 0;
    auto t1 = std::chrono::high_resolution_clock::now();
    std::vector<Move> allMoves = getAllMoves(board, color);
    std::vector<int> scores;
    scores.reserve(allMoves.size());

    if (allMoves.size() == 0) // checkmate or stalemate, can't make any moves
        return;

    int value;
    if (color == Board::blackID)
        value = INT_MIN;
    else value = INT_MAX;
    for (Move& move : allMoves)
    {
        Board newBoard = board;
        newBoard.movePiece(move);
        int score = alphaBeta(newBoard, DEPTH - 1, INT_MIN, INT_MAX, !color);
        if (color == Board::blackID)
            value = std::max(value, score);
        else value = std::min(value, score);
        // // int score = 5;
        scores.push_back(score);
    }
    Move bestMove = allMoves[std::find(scores.begin(), scores.end(), value) - scores.begin()];
    std::cout << bestMove.from << " " << bestMove.to << std::endl;
    board.movePiece(bestMove);

    auto t2 = std::chrono::high_resolution_clock::now();
    totalTime = t2 - t1;
    // std::cout << "Evaluated " << numPositions << " positions in " << totalTime.count() << " ms" << std::endl;
}

long long countPositions(Board& board, int depth)
{
    int color = board.currTurn;
    if (depth == 0) return 1;
    else
    {
        long long count = 0;
        std::vector<Move> allMoves = getAllMoves(board);
        for (Move& move : allMoves)
        {
            Board newBoard = board;
            newBoard.movePiece(move);
            // if (!Game::isCheck(newBoard, color))
            // {
                int c = countPositions(newBoard, depth - 1); 
                // if (depth == 1)
                //     std::cout << Board::indexToAlg(move.from) << "->" << Board::indexToAlg(move.to) << ": " << c << std::endl;
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

int main(int argc, char *argv[])
{
    std::string FEN = argv[1]; // 0 is call to exe, 1 is first arg
    // for (int i = 0; i < argc; i++)
    //     std::cout << argv[i] << std::endl;

    // Board board = Board("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -"); // position 5

    // Board board = Board("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");

    // Board board = Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
    // Board board = Board("r3k2r/p1pNqpb1/bn2pnp1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq -"); // e5d7
    // Board board = Board();
    Board board = Board(FEN);
    Board::initMoves();

    /* FOR REGULAR USE */
    // auto t1 = std::chrono::high_resolution_clock::now();
    // makeBestMoveAB(board, board[66]); // need to make this use the id from the fen string
    // auto t2 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double, std::milli> time = t2 - t1;
    // std::cout << time.count() << " ms" << std::endl;

    /* FOR PERFT TESTS */
    int d = std::stoi(argv[2]);
    long long num = countPositions(board, d);
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