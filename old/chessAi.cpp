#include <string>
#include <vector>
#include <tuple>
#include "chessPiecesv2.cpp"
#include <array>
#include <bits/stdc++.h>
#include <chrono>
#include <random>


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


class ChessAi
{
public:
    std::string color;
    int numPositions;
    std::chrono::duration<double, std::milli> getMovesTime;
    std::chrono::duration<double, std::milli> evalTime;
    std::chrono::duration<double, std::milli> cloneTime;
    std::chrono::duration<double, std::milli> clearTime;
    int numDuplicates;
    std::vector<std::tuple<uint64_t, int>> checkedBoards;

    ChessAi(std::string c)
    {
        color = c;
        numPositions = 0;
        numDuplicates = 0;
    }

    std::vector<std::tuple<Piece*, std::tuple<int, int>>> GetMoves(std::array<std::array<Piece*, 8>,8>& board, std::string c)
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        std::vector<std::tuple<Piece*, std::tuple<int, int>>> allMoves;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (board[i][j] != nullptr && board[i][j]->color == c)
                {
                    Piece* p = board[i][j];
                    std::vector<std::tuple<int, int>> moves = p->GetPossiblePositions(board);
                    for (std::tuple<int, int> t : moves)
                    {
                        allMoves.push_back(std::make_tuple(p, std::make_tuple(std::get<0>(t), std::get<1>(t))));
                    }
                }
            }
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        getMovesTime += t2 - t1;
        return allMoves;
    }

    [[deprecated("replaced by MakeBestMove, which makes better moves (not random)")]]
    void MakeRandomMove(std::array<std::array<Piece*, 8>,8>& board)
    {
        std::vector<std::tuple<Piece*, std::tuple<int, int>>> allMoves = GetMoves(board, "b");
        int ind = rand() % allMoves.size();
        std::tuple<Piece*, std::tuple<int, int>> move = allMoves[ind];
        MakeActualMove(board, move);
    }

    void MakeBestMove(std::array<std::array<Piece*, 8>,8>& board)
    {
        std::vector<std::tuple<Piece*, std::tuple<int, int>>> allMoves = GetMoves(board, "b");
        std::vector<int> scores;
        for (std::tuple<Piece*, std::tuple<int, int>> t : allMoves)
        {   
            std::array<std::array<Piece*, 8>,8> newBoard = cloneBoard(board);
            MakeActualMove(newBoard, t);
            // scores.push_back(alphaBeta(newBoard, "w", 4, INT_MIN, INT_MAX)); // we got all of black's moves, look for white's responses
            scores.push_back(scoreLayersOld(newBoard, "w", 4));
            // stop memory leaks (i think this works)
            clearBoard(newBoard);
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
        MakeActualMove(board, allMoves[ind]);
    }

    void MakeBestMoveOld(std::array<std::array<Piece*, 8>,8>& board)
    {
        std::vector<std::tuple<Piece*, std::tuple<int, int>>> allMoves = GetMoves(board, "b");
        std::vector<int> scores;
        for (std::tuple<Piece*, std::tuple<int, int>> t : allMoves)
        {   
            std::array<std::array<Piece*, 8>,8> newBoard = cloneBoard(board);
            MakeActualMove(newBoard, t);
            scores.push_back(evalBoard(newBoard));
            // stop memory leaks (i think this works)
            clearBoard(newBoard);
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
        MakeActualMove(board, allMoves[ind]);
    }

    [[deprecated("make a clone board and use MakeActualMove")]]
    void MakeTestMove(std::array<std::array<Piece*, 8>,8>& board, std::tuple<Piece*, std::tuple<int, int>> move)
    {
        return;
        Piece* p = new Piece(std::get<0>(move)->x, std::get<0>(move)->y, std::get<0>(move)->color);
        p->id = std::get<0>(move)->id;
        // std::cout << p->id << std::endl;
        if (std::get<0>(move)->id == "p")
        {
            ((Pawn*)p)->canBeEnPassant = ((Pawn*)(std::get<0>(move)))->canBeEnPassant;
        }
        else if (std::get<0>(move)->id == "r")
            ((Rook*)p)->hasMoved = ((Rook*)(std::get<0>(move)))->hasMoved;
        else if (std::get<0>(move)->id == "K")
        {
            ((King*)p)->hasMoved = ((King*)(std::get<0>(move)))->hasMoved;
            ((King*)p)->inCheck = ((King*)(std::get<0>(move)))->inCheck;
        }

        // Piece* p = board[std::get<0>(move)->y][std::get<0>(move)->x];

        int x = std::get<0>(std::get<1>(move));
        int y = std::get<1>(std::get<1>(move));
        
        delete board[p->y][p->x];
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

        board[y][x] = p;
        p->x = x;
        p->y = y;
    }

    void MakeActualMove(std::array<std::array<Piece*, 8>,8>& board, std::tuple<Piece*, std::tuple<int, int>> move)
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
    }
    
    /**
     * returns a numerical score for a given chess board
     * 
     * NOTE: currently gives black's score ONLY because it is used for the ai
     * which currently only plays as black
     * 
     * @param board the board to be evaluated
     * @return integer representing score
     */
    int evalBoard(std::array<std::array<Piece*, 8>,8>& board)
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        int whiteScore = 0;
        int blackScore = 0;

        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (board[i][j] != nullptr)
                {
                    if (board[i][j]->color == "w")
                    {   
                        std::string id = board[i][j]->id;
                        if (id == "K")
                            whiteScore += 0;
                        else if (id == "p")
                            whiteScore += 1;
                        else if (id == "k")
                            whiteScore += 3;
                        else if (id == "b")
                            whiteScore += 3;
                        else if (id == "r")
                            whiteScore += 5;
                        else if (id == "Q")
                            whiteScore += 9;
                    }
                    else
                    {
                        std::string id = board[i][j]->id;
                        if (id == "K")
                            blackScore += 0;
                        else if (id == "p")
                            blackScore += 1;
                        else if (id == "k")
                            blackScore += 3;
                        else if (id == "b")
                            blackScore += 3;
                        else if (id == "r")
                            blackScore += 5;
                        else if (id == "Q")
                            blackScore += 9;
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

    std::array<std::array<Piece*, 8>,8> cloneBoard(std::array<std::array<Piece*, 8>,8>& board)
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        std::array<std::array<Piece*, 8>,8> newBoard;

        memcpy(newBoard.data(), board.data(), sizeof(board));

        // **OLD CODE**
        // for (int i = 0; i < 8; i++)
        // {
        //     for (int j = 0; j < 8; j++)
        //     {
        //         if (board[i][j] == nullptr)
        //             newBoard[i][j] = nullptr;                
        //         else
        //             newBoard[i][j] = board[i][j]->Clone();
        //     }
        // }
        auto t2 = std::chrono::high_resolution_clock::now();
        cloneTime += t2 - t1;
        return newBoard;
    }

    void clearBoard(std::array<std::array<Piece*, 8>,8>& board)
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (board[i][j] != nullptr)
                    delete board[i][j];
            }
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        clearTime += t2 - t1;
    }

    int scoreLayers(std::array<std::array<Piece*, 8>,8>& board, std::string color, int layersLeft)
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

    int scoreLayersOld(std::array<std::array<Piece*, 8>,8>& board, std::string color, int layersLeft)
    {
        // std::cout << "good";
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

        for (std::tuple<Piece*, std::tuple<int, int>> t : allMoves)
        {   
            std::array<std::array<Piece*, 8>,8> newBoard = cloneBoard(board);
            MakeActualMove(newBoard, t);
            std::string newColor;
            if (color == "w") newColor = "b";
            else newColor = "w";

            // numPositions++;
            int val = scoreLayersOld(newBoard, newColor, layersLeft);
            if ((color == "b" && val > score) || (color == "w" && val < score))
                score = val;
            
            // stop memory leaks (i think this works)
            clearBoard(newBoard);
        }
        return score;
        // white goes for min score, black goes for max
        // if (color == "w")
        // {
        //     if (scores.size() == 0) // either stalemate or checkmate
        //     {
        //         if (Piece::isCheck(board, "w")) // white is checkmated
        //         {
        //             return INT_MAX; // good for black (black won), return largest int possible
        //         }
        //         else // stalemate (draw)
        //             return 0;
        //     }
        //     int ind = rand() % scores.size();
        //     int min = scores[ind];
        //     for (int i = 0; i < scores.size(); i++)
        //     {
        //         if (scores[i] < min)
        //         {
        //             min = scores[i];
        //             ind = i;
        //         }
        //     }
        //     return min;
        // }
        // else
        // {
        //     if (scores.size() == 0) // either stalemate or checkmate
        //     {
        //         if (Piece::isCheck(board, "b")) // black is checkmated
        //         {
        //             return INT_MIN; // good for white (white won), return smallest int possible
        //         }
        //         else // stalemate (draw)
        //             return 0;
        //     }
        //     int ind = rand() % scores.size();
        //     int max = scores[ind];
        //     for (int i = 0; i < scores.size(); i++)
        //     {
        //         if (scores[i] > max)
        //         {
        //             max = scores[i];
        //             ind = i;
        //         }
        //     }
        //     return max;
        // }
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

    // alpha is best option for max, beta is best for min
    int alphaBeta(std::array<std::array<Piece*, 8>,8>& board, std::string color, int layersLeft, int alpha, int beta)
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
};


int main()
{
    std::array<std::array<Piece*, 8>,8> board;

    board = {{  {{new Rook(0, 0, "b"), new Knight(1, 0, "b"), new Bishop(2, 0, "b"), new Queen(3, 0, "b"), new King(4, 0, "b"), new Bishop(5, 0, "b"), new Knight(6, 0, "b"), new Rook(7, 0, "b")}},
                {{new Pawn(0, 1, "b"), new Pawn(1, 1, "b"), new Pawn(2, 1, "b"), new Pawn(3, 1, "b"), new Pawn(4, 1, "b"), new Pawn(5, 1, "b"), new Pawn(6, 1, "b"), new Pawn(7, 1, "b")}},
                {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
                {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
                {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
                {{nullptr, nullptr, nullptr, new Pawn(3, 5, "w"), nullptr, nullptr, nullptr, nullptr}},
                {{new Pawn(0, 6, "w"), new Pawn(1, 6, "w"), new Pawn(2, 6, "w"), nullptr, new Pawn(4, 6, "w"), new Pawn(5, 6, "w"), new Pawn(6, 6, "w"), new Pawn(7, 6, "w")}},
                {{new Rook(0, 7, "w"), new Knight(1, 7, "w"), new Bishop(2, 7, "w"), new Queen(3, 7, "w"), new King(4, 7, "w"), new Bishop(5, 7, "w"), new Knight(6, 7, "w"), new Rook(7, 7, "w")}} }};

    ChessAi ai("b");

    auto t1 = std::chrono::high_resolution_clock::now();
    ai.MakeBestMove(board);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = t2 - t1;
    std::cout << ms_double.count() << " ms" << std::endl;

    std::cout << ai.getMovesTime.count() << " ms" << std::endl;
    std::cout << ai.evalTime.count() << " ms" << std::endl;
    std::cout << ai.cloneTime.count() << " ms" << std::endl;
    std::cout << ai.clearTime.count() << " ms" << std::endl;
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