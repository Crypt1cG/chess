#include <string>
#include <vector>
#include <tuple>
// #include "chessPiecesv3.cpp"
#include <array>
#include <bits/stdc++.h>
#include <chrono>
#include <random>
#include "game.cpp"

#define DEPTH 4
/**
 * works pretty well
 * 
 * KNOWN BUGS:
 * castling crashes the ai
 * 
 */

// std::array<std::array<Piece*, 8>,8> board1;
// std::array<std::array<Piece*, 8>,8> board2;
// std::array<std::array<Piece*, 8>,8> board3;
// std::array<std::array<Piece*, 8>,8> board4;

std::vector<std::vector<uint64_t>> zobristTable;
/*
void initZobrist()
{
    std::default_random_engine generator;
    std::uniform_int_distribution<uint64_t> distrib(1, UINT64_MAX);
    for (int i = 0; i < 64; i++)
    {
        std::vector<uint64_t> row;
        for (int j = 0; j < 12; j++)
        {
            uint64_t num = distrib(generator);
            row.push_back(num);
        }
        zobristTable.push_back(row);
    }

    zobristTable.push_back({distrib(generator)}); // side to move is black

    std::vector<uint64_t> castling; // 4 nums for castling rights
    for (int i = 0; i < 4; i++)
        castling.push_back(distrib(generator));
    zobristTable.push_back(castling);

    std::vector<uint64_t> enPassant; // 8 nums for indicating valid en passant files
    for (int i = 0; i < 8; i++)
        enPassant.push_back(distrib(generator));
    zobristTable.push_back(castling);
}

uint64_t fullZobristHash(std::array<std::array<Piece*, 8>,8>& board, std::string color)
{
    uint64_t hash;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board[i][j] != nullptr)
            {
                Piece* p = board[i][j];
                int index;
                if (p->id == "p")
                    index = 0;
                else if (p->id == "k")
                    index = 1;
                else if (p->id == "b")
                    index = 2;
                else if (p->id == "r")
                    index = 3;
                else if (p->id == "Q")
                    index = 4;
                else if (p->id == "K")
                    index = 5;
                if (p->color == "b")
                    index += 6;
                
                int sq = p->y * 8 + p->x;
                hash = hash ^ zobristTable[sq][index];

                if (p->id == "p" && ((Pawn*)p)->canBeEnPassant)
                    hash = hash ^ zobristTable[66][p->y];
            }
        }
    }

    if (color == "b")
        hash = hash ^ zobristTable[64][0];
    
    if (board[7][4] != nullptr && board[7][4]->id == "K" && !((King*)(board[7][4]))->hasMoved)
    {
        if (board[7][7] != nullptr && board[7][7]->id == "r" && !((Rook*)(board[7][7]))->hasMoved)
        {
            // white can castle King side
            hash = hash ^ zobristTable[65][0];
        }
        if (board[7][0] != nullptr && board[7][0]->id == "r" && !((Rook*)(board[7][0]))->hasMoved)
        {
            // white can castle queen side
            hash = hash ^ zobristTable[65][1];
        }
    }
    if (board[0][4] != nullptr && board[0][4]->id == "K" && !((King*)(board[0][4]))->hasMoved)
    {
        if (board[0][7] != nullptr && board[0][7]->id == "r" && !((Rook*)(board[0][7]))->hasMoved)
        {
            // black can castle king side
            hash = hash ^ zobristTable[65][2];
        }
        if (board[0][0] != nullptr && board[0][0]->id == "r" && !((Rook*)(board[0][0]))->hasMoved)
        {
            // black can castle queen side
            hash = hash ^ zobristTable[65][3];
        }
    }
    return hash;
}

uint64_t updateZobrist(std::array<std::array<Piece*, 8>,8>& board, std::tuple<Piece*, std::tuple<int, int>>& move, uint64_t oldHash)
{
    uint64_t hash = oldHash;
    Piece* p = std::get<0>(move);
    int index;
    if (p->id == "p")
        index = 0;
    else if (p->id == "k")
        index = 1;
    else if (p->id == "b")
        index = 2;
    else if (p->id == "r")
        index = 3;
    else if (p->id == "Q")
        index = 4;
    else if (p->id == "K")
        index = 5;
    if (p->color == "b")
        index += 6;
    int sq = p->y * 8 + p->x;

    hash = hash ^ zobristTable[sq][index]; // get rid of piece at old spot

    std::tuple<int, int> newCoords = std::get<1>(move);
    if (board[std::get<1>(newCoords)][std::get<0>(newCoords)] != nullptr)
    {
        int index2;
        Piece* p2 = board[std::get<1>(newCoords)][std::get<0>(newCoords)];
        if (p2->id == "p")
            index2 = 0;
        else if (p2->id == "k")
            index2 = 1;
        else if (p2->id == "b")
            index2 = 2;
        else if (p2->id == "r")
            index2 = 3;
        else if (p2->id == "Q")
            index2 = 4;
        else if (p2->id == "K")
            index2 = 5;
        if (p2->color == "b")
            index2 += 6;
        int sq2 = p2->y * 8 + p2->x;

        hash = hash ^ zobristTable[sq][index2]; // get rid of piece at spot where piece is moving to
    }

    int newSq = std::get<1>(newCoords) * 8 + std::get<0>(newCoords);
    hash = hash ^ zobristTable[newSq][index]; // add hash for piece at new square

    hash = hash ^ zobristTable[64][0];

    return hash;
}
*/

class ChessAi
{
public:
    Game g;
    int color;
    int numPositions;
    std::chrono::duration<double, std::milli> getMovesTime;
    std::chrono::duration<double, std::milli> getMovesTime2;

    std::chrono::duration<double, std::milli> evalTime;
    int numDuplicates;
    std::vector<std::tuple<uint64_t, int>> checkedBoards;

    ChessAi(int c, Game& g)
    {
        this->g = g;
        color = c;
        numPositions = 0;
        numDuplicates = 0;
    }

    std::vector<std::array<int, 4>> GetMoves(std::array<int, 67>& b, int c)
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        std::vector<std::array<int, 4>> allMoves; // currX, currY, destX, destY
        for (int i = 0; i < 64; i++)
        {
            if (b[i] != 0 && Game::getColor(b[i]) == c)
            {
                int p = b[i];
                auto t3 = std::chrono::high_resolution_clock::now();
                std::vector<std::array<int, 2>> moves = g.getMoves(b, Game::getX(p), Game::getY(p));
                auto t4 = std::chrono::high_resolution_clock::now();
                getMovesTime2 += t4-t3;
                for (std::array<int, 2> m : moves)
                {
                    allMoves.push_back({{Game::getX(p), Game::getY(p), m[0], m[1]}});
                }
            }
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        getMovesTime += t2 - t1;
        return allMoves;
    }

    [[deprecated("replaced by MakeBestMove, which makes better moves (not random)")]]
    void MakeRandomMove(std::array<int, 67>& b)
    {
        std::vector<std::array<int, 4>> allMoves = GetMoves(b, color);
        int ind = rand() % allMoves.size();
        std::array<int, 4> move = allMoves[ind];
        g.movePiece(move[0], move[1], move[2], move[3], b);
    }

    void MakeBestMove(std::array<int, 67>& b)
    {
        std::vector<std::array<int, 4>> allMoves = GetMoves(b, color);
        std::vector<int> scores;
        std::array<int, 67> newBoard;
        for (std::array<int, 4> m : allMoves)
        {   
            newBoard = b;
            g.movePiece(m[0], m[1], m[2], m[3], newBoard);
            // scores.push_back(alphaBeta(newBoard, "w", 4, INT_MIN, INT_MAX)); // we got all of black's moves, look for white's responses
            scores.push_back(scoreLayersOld(newBoard, Game::whiteID, DEPTH));
            // stop memory leaks (i think this works)
        }

        int ind = rand() % scores.size();
        int max = scores[ind];
        for (int i = 0; i < scores.size(); i++)
        {
            if (scores[i] > max)
            {
                max = scores[i];
                ind = i;
            }
        }
        std::cout << "evaluated " << numPositions << " moves/positons" << std::endl;
        std::cout << numDuplicates << " duplicates" << std::endl;
        std::cout << checkedBoards.size() << std::endl;
        g.movePiece(allMoves[ind][0], allMoves[ind][1], allMoves[ind][2], allMoves[ind][3], b);
    }

    /*void MakeActualMove(std::array<std::array<Piece*, 8>,8>& board, std::tuple<Piece*, std::tuple<int, int>> move)
    {
        // Piece* p = new Piece(std::get<0>(move)->x, std::get<0>(move)->y, std::get<0>(move)->color);
        // p->id = std::get<0>(move)->id;
        // std::cout << p->id << std::endl;
        // if (std::get<0>(move)->id == "p")
        // {
        //     ((Pawn*)p)->canBeEnPassant = ((Pawn*)(std::get<0>(move)))->canBeEnPassant;
        //     ((Pawn*)p)->hasMoved = ((Pawn*)(std::get<0>(move)))->hasMoved;
        // }
        // else if (std::get<0>(move)->id == "r")
        //     ((Rook*)p)->hasMoved = ((Rook*)(std::get<0>(move)))->hasMoved;
        // else if (std::get<0>(move)->id == "K")
        // {
        //     ((King*)p)->hasMoved = ((King*)(std::get<0>(move)))->hasMoved;
        //     ((King*)p)->inCheck = ((King*)(std::get<0>(move)))->inCheck;
        // }
        
        Piece* p = board[std::get<0>(move)->y][std::get<0>(move)->x];
        
        int x = std::get<0>(std::get<1>(move));
        int y = std::get<1>(std::get<1>(move));
        // std::cout << p->id << x << y << std::endl;
        // delete board[p->y][p->x];
        board[p->y][p->x] = nullptr;
        

        if (p->id == "p") // it's a pawn, need to get rid of moving 2
        {
            if (abs(p->y - y) == 2) // double move
            {
                ((Pawn*)p)->canBeEnPassant = true;
                // game.pawnsToUpdate.push_back(game.currSelectedPiece);
            }
            if (abs(p->x - x) == 1) // it moved horizontally - either regular capture or en passant
            {
                if (board[y][x] == nullptr) // its moving into an empty square (not a regular capture)
                {
                    delete board[p->y][x];
                    board[p->y][x] = nullptr; // the pawn en passant -ed
                }
            }
        }
        if (p->id == "K") // it's a king, need to get rid of castling
        {
            if (abs(p->x - x) == 2) // castling - the king moved 2 squares
            {
                // move the rook
                if (x == 6) // castled king side
                {
                    Piece* tmpR = ((Rook*)(board[y][7]))->Clone();
                    delete board[y][7];
                    board[y][7] = nullptr;
                    board[y][5] = tmpR;
                    tmpR->x = 5;
                    ((Rook*)tmpR)->hasMoved = true;
                }
                else if (x == 2)
                {
                    Piece* tmpR = ((Rook*)(board[y][0]))->Clone();
                    delete board[y][0];
                    board[y][0] = nullptr;
                    board[y][3] = tmpR;
                    tmpR->x = 3;
                    ((Rook*)tmpR)->hasMoved = true;
                }
            }
            ((King*)p)->hasMoved = true;
        }
        if (p->id == "r") // it's a rook, need to get rid of castling
            ((Rook*)p)->hasMoved = true;

        if (board[y][x] != nullptr) // if something is being captured, need to delete the captured piece otherwise the pointer is lost and memory is leaked
            delete board[y][x];

        if (p->color == "w") // clear black canBeEnPassant pawns
        {
            for (int i = 0; i < 8; i++)
            {
                if (board[3][i] != nullptr && board[3][i]->id == "p")
                    ((Pawn*)(board[3][i]))->canBeEnPassant = false;
            }
        }
        else
        {
            for (int i = 0; i < 8; i++)
            {
                if (board[4][i] != nullptr && board[4][i]->id == "p")
                    ((Pawn*)(board[4][i]))->canBeEnPassant = false;
            }
        }
        
        board[y][x] = p;
        p->x = x;
        p->y = y;
    }*/
    
    /**
     * returns a numerical score for a given chess board
     * 
     * NOTE: currently gives black's score ONLY because it is used for the ai
     * which currently only plays as black
     * 
     * @param board the board to be evaluated
     * @return integer representing score
     */
    int evalBoard(std::array<int, 67>& b)
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        int whiteScore = 0;
        int blackScore = 0;

        for (int i = 0; i < 64; i++)
        {
            if (b[i] != 0)
            {
                if (Game::getColor(b[i]) == Game::whiteID)
                {   
                    switch (Game::getID(b[i]))
                    {
                    case Game::pawnID:
                        whiteScore += 1;
                        break;
                    case Game::knightID:
                        whiteScore += 3;
                        break;
                    case Game::bishopID:
                        whiteScore += 3;
                        break;
                    case Game::rookID:
                        whiteScore += 5;
                        break;
                    case Game::queenID:
                        whiteScore += 9;
                        break;
                    }
                }
                else
                {
                    switch (Game::getID(b[i]))
                    {
                    case Game::pawnID:
                        blackScore += 1;
                        break;
                    case Game::knightID:
                        blackScore += 3;
                        break;
                    case Game::bishopID:
                        blackScore += 3;
                        break;
                    case Game::rookID:
                        blackScore += 5;
                        break;
                    case Game::queenID:
                        blackScore += 9;
                        break;
                    }
                }
            }
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        evalTime += t2 - t1;
        // whiteScore += GetMoves(board, "w").size() * 0.01;
        // blackScore += GetMoves(board, "b").size() * 0.01;

        return blackScore - whiteScore;
    }

    /*int scoreLayers(std::array<std::array<Piece*, 8>,8>& board, std::string color, int layersLeft)
    {
        
        layersLeft--; // we are doing a layer, one less layer left
        if (layersLeft == 0) // last layer doesn't look at further moves, just evaluates the board position
        {
            return evalBoard(board);
        }
        std::vector<std::tuple<Piece*, std::tuple<int, int>>> allMoves = GetMoves(board, color);
        std::vector<int> scores;

        for (std::tuple<Piece*, std::tuple<int, int>> t : allMoves)
        {   
            std::array<std::array<Piece*, 8>,8> newBoard = cloneBoard(board);
            MakeActualMove(newBoard, t);
            std::string newColor;
            if (color == "w") newColor = "b";
            else newColor = "w";

            uint64_t boardHash = fullZobristHash(newBoard, newColor);
            int ind = findBoard(boardHash);
            if (ind == -1)
            {
                numPositions++;
                int score = scoreLayers(newBoard, newColor, layersLeft);
                scores.push_back(score);
                checkedBoards.push_back(std::make_tuple(boardHash, score));
            }
            else
            {
                scores.push_back(std::get<1>(checkedBoards[ind]));
                numDuplicates++;
            }
            // stop memory leaks (i think this works)
            clearBoard(newBoard);
        }
        
        // white goes for min score, black goes for max
        if (color == "w")
        {
            if (scores.size() == 0) // either stalemate or checkmate
            {
                if (Piece::isCheck(board, "w")) // white is checkmated
                {
                    return INT_MAX; // good for black (black won), return largest int possible
                }
                else // stalemate (draw)
                    return 0;
            }
            int ind = rand() % scores.size();
            int min = scores[ind];
            for (int i = 0; i < scores.size(); i++)
            {
                if (scores[i] < min)
                {
                    min = scores[i];
                    ind = i;
                }
            }
            return min;
        }
        else
        {
            if (scores.size() == 0) // either stalemate or checkmate
            {
                if (Piece::isCheck(board, "b")) // black is checkmated
                {
                    return INT_MIN; // good for white (white won), return smallest int possible
                }
                else // stalemate (draw)
                    return 0;
            }
            int ind = rand() % scores.size();
            int max = scores[ind];
            for (int i = 0; i < scores.size(); i++)
            {
                if (scores[i] > max)
                {
                    max = scores[i];
                    ind = i;
                }
            }
            return max;
        }
    }
    */

    int scoreLayersOld(std::array<int, 67>& b, int c, int layersLeft)
    {
        // std::cout << "good";
        layersLeft--; // we are doing a layer, one less layer left
        if (layersLeft == 0) // last layer doesn't look at further moves, just evaluates the board position
        {
            numPositions++;
            return evalBoard(b);
        }

        std::vector<std::array<int, 4>> allMoves = GetMoves(b, c);

        if (allMoves.size() == 0)
        {
            if (c == Game::whiteID)
            {
                if (g.isCheck(b, Game::whiteID))
                    return INT_MAX; // white is in checkmate
                return 0; // stalemate
            }
            else
            {
                if (g.isCheck(b, Game::blackID))
                    return INT_MIN; // black is in checkmate
                return 0; // stalemate
            }
        }
        
        int score;
        // std::cout << score << std::endl;
        // i think i need this, w/o it score starts at 0
        if (c == Game::blackID) score = INT_MIN;
        else score = INT_MAX;

        for (std::array<int, 4> m : allMoves)
        {   
            std::array<int, 67> newBoard = b;
            g.movePiece(m[0], m[1], m[2], m[3], newBoard);
            int newColor = !c;

            int val = scoreLayersOld(newBoard, newColor, layersLeft);
            if ((c == Game::blackID && val > score) || (c == Game::whiteID && val < score))
                score = val;
        }
        return score;
    }

    /**
     * looks to see if a board has already been evaluated
     * 
     * if it has, returns the index of it in checkBoards
     * if not found, returns -1
     */
    // int findBoardOld(std::array<std::array<Piece*, 8>,8>& board, std::string color)
    // {
    //     std::string s = Piece::boardToFEN(board, color);
    //     for (int i = 0; i < checkedBoards.size(); i++)
    //     {
    //         if (s == std::get<0>(checkedBoards[i])) return i;
    //     }
    //     return -1;
    // }

    int findBoard(uint64_t h)
    {
        for (int i = 0; i < checkedBoards.size(); i++)
        {
            if (h == std::get<0>(checkedBoards[i])) return i;
        }
        return -1;
    }

    int miniMax(std::array<int, 67>& b, int depth, bool maximisingPlayer)
    {
        if (depth == 0)
        {
            numPositions++;
            return evalBoard(b);
        }

        if (maximisingPlayer) // black for now
        {
            std::vector<std::array<int, 4>> allMoves = GetMoves(b, Game::blackID);
            if (allMoves.size() == 0) // checkmate or stalemate
            {
                if (g.isCheck(b, Game::blackID)) // checkmate, give lowest possible score
                    return INT_MIN;
                return 0; // stalemate
            }

        }
        else // white
        {
            std::vector<std::array<int, 4>> allMoves = GetMoves(b, Game::whiteID);
            if (allMoves.size() == 0) // checkmate or stalemate
            {
                if (g.isCheck(b, Game::whiteID)) // white checkmated, give highest possible score
                    return INT_MAX;
                return 0; // stalemate
            }
        }
    }

    // alpha is best option for max, beta is best for min
    /*int alphaBeta(std::array<std::array<Piece*, 8>,8>& board, std::string color, int layersLeft, int alpha, int beta)
    {
        layersLeft--; // we are doing a layer, one less layer left
        if (layersLeft == 0) // last layer doesn't look at further moves, just evaluates the board position
        {
            numPositions++;
            return evalBoard(board);
        }

        std::vector<std::tuple<Piece*, std::tuple<int, int>>> allMoves = GetMoves(board, color);

        if (allMoves.size() == 0)
        {
            if (color == "w")
            {
                if (Piece::isCheck(board, "w"))
                    return INT_MAX; // white is in checkmate
                return 0; // stalemate
            }
            if (color == "b")
            {
                if (Piece::isCheck(board, "b"))
                    return INT_MIN; // black is in checkmate
                return 0; // stalemate
            }
        }

        int score;
        if (color == "b") score = INT_MIN;
        else score = INT_MAX;

        for (std::tuple<Piece*, std::tuple<int, int>> t : allMoves)
        {   
            std::array<std::array<Piece*, 8>,8> newBoard = cloneBoard(board);
            MakeActualMove(newBoard, t);
            std::string newColor;
            if (color == "w") newColor = "b";
            else newColor = "w";
            int val = alphaBeta(newBoard, newColor, layersLeft, alpha, beta);
            // numPositions++;
            if ((color == "b" && val > score) || (color == "w" && val < score))
                score = val;

            if (color == "b")
            {
                if (val > alpha) alpha = val;
            }
            else
            {
                if (val > beta) beta = val;
            }
            if (beta <= alpha) 
            {
                std::cout << "pruning" << std::endl;
                break;
            }
            // stop memory leaks (i think this works)
            clearBoard(newBoard);
        }
        return score;
    }
    */


};


int main()
{
    // std::array<int, 67> board;

    // board = {{Game::makePiece(0, 0, blackID, rookID), Game::makePiece(1, 0, blackID, knightID), Game::makePiece(2, 0, blackID, bishopID), Game::makePiece(3, 0, blackID, queenID), Game::makePiece(4, 0, blackID, kingID), Game::makePiece(5, 0, blackID, bishopID), Game::makePiece(6, 0, blackID, knightID), Game::makePiece(7, 0, blackID, rookID),
    //           Game::makePiece(0, 1, blackID, pawnID), Game::makePiece(1, 1, blackID, pawnID),   Game::makePiece(2, 1, blackID, pawnID),   Game::makePiece(3, 1, blackID, pawnID),  Game::makePiece(4, 1, blackID, pawnID), Game::makePiece(5, 1, blackID, pawnID),   Game::makePiece(6, 1, blackID, pawnID),   Game::makePiece(7, 1, blackID, pawnID),
    //           0,                                      0,                                        0,                                        0,                                       0,                                      0,                                        0,                                        0,
    //           0,                                      0,                                        0,                                        0,                                       0,                                      0,                                        0,                                        0,
    //           0,                                      0,                                        0,                                        0,                                       0,                                      0,                                        0,                                        0,
    //           0,                                      0,                                        0,                                        0,                                       0,                                      0,                                        0,                                        0,
    //           Game::makePiece(0, 6, whiteID, pawnID), Game::makePiece(1, 6, whiteID, pawnID),   Game::makePiece(2, 6, whiteID, pawnID),   Game::makePiece(3, 6, whiteID, pawnID),  Game::makePiece(4, 6, whiteID, pawnID), Game::makePiece(5, 6, whiteID, pawnID),   Game::makePiece(6, 6, whiteID, pawnID),   Game::makePiece(7, 6, whiteID, pawnID),
    //           Game::makePiece(0, 7, whiteID, rookID), Game::makePiece(1, 7, whiteID, knightID), Game::makePiece(2, 7, whiteID, bishopID), Game::makePiece(3, 7, whiteID, queenID), Game::makePiece(4, 7, whiteID, kingID), Game::makePiece(5, 7, whiteID, bishopID), Game::makePiece(6, 7, whiteID, knightID), Game::makePiece(7, 7, whiteID, rookID),
    //           0b1111, // castling perms
    //           0, // en passant targets
    //           0}}; // curr turn

    Game game;
    ChessAi ai(Game::blackID, game);

    auto t1 = std::chrono::high_resolution_clock::now();
    ai.MakeBestMove(game.board);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = t2 - t1;
    std::cout << ms_double.count() << " ms" << std::endl;

    std::cout << ai.getMovesTime.count() << " ms" << std::endl;
    std::cout << ai.getMovesTime2.count() << " ms" << std::endl;
    std::cout << ai.evalTime.count() << " ms" << std::endl;
    // auto t1 = std::chrono::high_resolution_clock::now();
    // uint64_t result = ai.fullZobristHash(board, "w");
    // auto t2 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double, std::milli> ms_double = t2 - t1;
    // std::cout << ms_double.count() << "ms" << std::endl;
    // std::cout << result << std::endl;
    // uint64_t num = result ^ ai.zobristTable[9][6];
    // std::cout << num << std::endl;
    // // auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    // // std::cout << "that move took " << ms_int.count() << " milliseconds" << std::endl;


    // t1 = std::chrono::high_resolution_clock::now();
    // Piece::boardToFEN(board, "w");
    // t2 = std::chrono::high_resolution_clock::now();
    // ms_double = t2 - t1;
    // std::cout << ms_double.count() << "ms" << std::endl;
    // ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    // std::cout << "that move took " << ms_int.count() << " milliseconds" << std::endl;


    // auto t1 = std::chrono::high_resolution_clock::now();
    // ai.MakeBestMove(board);
    // // board[0][2]->GetPossiblePositions(board, true);
    // // board[0][2]->GetPossiblePositions(board);
    // auto t2 = std::chrono::high_resolution_clock::now();
    // auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    // std::cout << "that move took " << ms_int.count() << " milliseconds" << std::endl;
}