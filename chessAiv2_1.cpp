#include <string>
#include <vector>
#include <tuple>
// #include "chessPiecesv3.cpp"
#include <array>
#include <bits/stdc++.h>
#include <chrono>
#include <random>
#include "gamev2.cpp"

#define DEPTH 5
/**
 * new features: supports gamev2
 * works pretty well
 * 
 * KNOWN BUGS:
 * castling crashes the ai
 * 
 */

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

    std::vector<uint64_t> castling; // 4 nums for castling rights
    for (int i = 0; i < 4; i++)
        castling.push_back(distrib(generator));
    zobristTable.push_back(castling);

    std::vector<uint64_t> enPassant; // 8 nums for indicating valid en passant files
    for (int i = 0; i < 8; i++)
        enPassant.push_back(distrib(generator));
    zobristTable.push_back(enPassant);

    zobristTable.push_back({distrib(generator)}); // side to move is black
}

uint64_t fullZobristHash(std::array<int, 69>& board)
{
    uint64_t hash = 0;
    for (int i = 0; i < 64; i++)
    {
        if (board[i] != 0)
        {
            int p = board[i];
            int index = (Game::getID(p) - 1) /* 0-5 */ + (Game::getColor(p) * 6); // color = 0, offset is 0, color = 1, starts at index 6

            hash = hash ^ zobristTable[i][index];
        }
    }
    // castling (there might be a better way to do this)
    int castling = board[64];
    if (Game::canCastle("q", Game::whiteID, castling)) // white queen side
        hash = hash ^ zobristTable[64][0];
    if (Game::canCastle("k", Game::whiteID, castling)) // white king side
        hash = hash ^ zobristTable[64][1];
    if (Game::canCastle("q", Game::blackID, castling)) // black queen side
        hash = hash ^ zobristTable[64][2];
    if (Game::canCastle("k", Game::blackID, castling)) // black king side
        hash = hash ^ zobristTable[64][3];

    // en passant shit (assumes only 1 pawn can be en passant-ed at a time)
    int enPassant = board[65];
    if (enPassant != 0)
    {
        // could use log2 here
        if (enPassant == 0b00000001) hash = hash ^ zobristTable[65][7]; // x = 7 file
        else if (enPassant == 0b00000010) hash = hash ^ zobristTable[65][6];
        else if (enPassant == 0b00000100) hash = hash ^ zobristTable[65][5];
        else if (enPassant == 0b00001000) hash = hash ^ zobristTable[65][4];
        else if (enPassant == 0b00010000) hash = hash ^ zobristTable[65][3];
        else if (enPassant == 0b00100000) hash = hash ^ zobristTable[65][2];
        else if (enPassant == 0b01000000) hash = hash ^ zobristTable[65][1];
        else hash = hash ^ zobristTable[65][0];
    }

    if (board[66] == Game::blackID)
        hash = hash ^ zobristTable[66][0];
    
    return hash;
}

uint64_t updateZobrist(std::array<int, 69>& board, std::array<int, 4>& move, uint64_t oldHash, int newEnPassant, int newCastling)
{
    uint64_t hash = oldHash;
    int p = board[(move[1]) * 8 + move[0]];
    int index = (Game::getID(p) - 1) /* 0-5 */ + (Game::getColor(p) * 6); // color = 0, offset is 0, color = 1, starts at index 6

    int sq = move[1] * 8 + move[0];

    hash = hash ^ zobristTable[sq][index]; // get rid of piece at old spot

    // remove piece at new spot
    int sq2 = move[3] * 8 + move[2];
    int p2 = board[sq2];
    if (p2 != 0)
    {
        int index2 = (Game::getID(p2) - 1) /* 0-5 */ + (Game::getColor(p2) * 6); // color = 0, offset is 0, color = 1, starts at index 6
        hash = hash ^ zobristTable[sq2][index2];
    }

    // add piece at new spot
    hash = hash ^ zobristTable[sq2][index];

    // switch current turn (if was black, remove black, if was white, add black)
    hash = hash ^ zobristTable[66][0];

    // castling (there might be a better way to do this)
    // this gets rid of all old castling
    int castling = board[64];
    if (Game::canCastle("q", Game::whiteID, castling)) // white queen side
        hash = hash ^ zobristTable[64][0];
    if (Game::canCastle("k", Game::whiteID, castling)) // white king side
        hash = hash ^ zobristTable[64][1];
    if (Game::canCastle("q", Game::blackID, castling)) // black queen side
        hash = hash ^ zobristTable[64][2];
    if (Game::canCastle("k", Game::blackID, castling)) // black king side
        hash = hash ^ zobristTable[64][3];

    // add new castling
    castling = newCastling;
    if (Game::canCastle("q", Game::whiteID, castling)) // white queen side
        hash = hash ^ zobristTable[64][0];
    if (Game::canCastle("k", Game::whiteID, castling)) // white king side
        hash = hash ^ zobristTable[64][1];
    if (Game::canCastle("q", Game::blackID, castling)) // black queen side
        hash = hash ^ zobristTable[64][2];
    if (Game::canCastle("k", Game::blackID, castling)) // black king side
        hash = hash ^ zobristTable[64][3];

    // en passant shit (assumes only 1 pawn can be en passant-ed at a time)
    // remove old enPassant shit
    int enPassant = board[65];
    if (enPassant != 0)
    {
        // could use log2 here
        if (enPassant == 0b00000001) hash = hash ^ zobristTable[65][7]; // x = 7 file
        else if (enPassant == 0b00000010) hash = hash ^ zobristTable[65][6];
        else if (enPassant == 0b00000100) hash = hash ^ zobristTable[65][5];
        else if (enPassant == 0b00001000) hash = hash ^ zobristTable[65][4];
        else if (enPassant == 0b00010000) hash = hash ^ zobristTable[65][3];
        else if (enPassant == 0b00100000) hash = hash ^ zobristTable[65][2];
        else if (enPassant == 0b01000000) hash = hash ^ zobristTable[65][1];
        else hash = hash ^ zobristTable[65][0];
    }

    // add new enPassant shit
    enPassant = newEnPassant;
    if (enPassant != 0)
    {
        // could use log2 here
        if (enPassant == 0b00000001) hash = hash ^ zobristTable[65][7]; // x = 7 file
        else if (enPassant == 0b00000010) hash = hash ^ zobristTable[65][6];
        else if (enPassant == 0b00000100) hash = hash ^ zobristTable[65][5];
        else if (enPassant == 0b00001000) hash = hash ^ zobristTable[65][4];
        else if (enPassant == 0b00010000) hash = hash ^ zobristTable[65][3];
        else if (enPassant == 0b00100000) hash = hash ^ zobristTable[65][2];
        else if (enPassant == 0b01000000) hash = hash ^ zobristTable[65][1];
        else hash = hash ^ zobristTable[65][0];
    }

    return hash;
}


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

    std::vector<std::array<int, 4>> GetMoves(std::array<int, 69>& b, int c)
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
    void MakeRandomMove(std::array<int, 69>& b)
    {
        std::vector<std::array<int, 4>> allMoves = GetMoves(b, color);
        int ind = rand() % allMoves.size();
        std::array<int, 4> move = allMoves[ind];
        g.movePiece(move[0], move[1], move[2], move[3], b);
    }

    void MakeBestMove(std::array<int, 69>& b)
    {
        std::vector<std::array<int, 4>> allMoves = GetMoves(b, color);
        std::vector<int> scores;
        std::array<int, 69> newBoard;
        // uint64_t hash = fullZobristHash(b);
        int alpha = INT_MIN;
        int beta = INT_MAX;
        int score;
        for (std::array<int, 4> m : allMoves)
        {   
            // newBoard = b;
            // g.movePiece(m[0], m[1], m[2], m[3], newBoard);
            // // uint64_t newHash = updateZobrist(b, m, hash, newBoard[65], newBoard[64]);
            // // scores.push_back(alphaBeta(newBoard, "w", 4, INT_MIN, INT_MAX)); // we got all of black's moves, look for white's responses
            // scores.push_back(scoreLayersOld(newBoard, Game::whiteID, DEPTH));
            // // scores.push_back(scoreLayers(newBoard, Game::whiteID, DEPTH, newHash));

            // alpha beta version:
            newBoard = b;
            g.movePiece(m[0], m[1], m[2], m[3], newBoard);

            int val = alphaBeta(newBoard, Game::whiteID, DEPTH, alpha, beta);
            scores.push_back(val);
            // numPositions++;
            if (/*(c == Game::blackID &&*/ val > score/*) || (c == Game::whiteID && val < score)*/) // just need that cause it will always be black ???
                score = val;

            // if (c == Game::blackID)
            // {
                if (val > alpha) alpha = val;
            // }
            // else
            // {
            //     if (val > beta) beta = val;
            // }
            // if (beta <= alpha) 
            // {
            //     std::cout << "pruning" << std::endl;
            //     break;
            // }
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
    int evalBoard(std::array<int, 69>& b)
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

    int scoreLayers(std::array<int, 69>& b, int c, int layersLeft, uint64_t oldHash)
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
                if (Game::isCheck(b, Game::whiteID))
                    return INT_MAX; // white is in checkmate
                return 0; // stalemate
            }
            else
            {
                if (Game::isCheck(b, Game::blackID))
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
            std::array<int, 69> newBoard = b;
            g.movePiece(m[0], m[1], m[2], m[3], newBoard);
            uint64_t hash = updateZobrist(b, m, oldHash, newBoard[65], newBoard[64]);
            int ind = findBoard(hash);
            int val;
            if (ind != -1)
            {
                numDuplicates++;
                val = std::get<1>(checkedBoards[ind]);
            }
            else
            {
                int newColor = !c;
                val = scoreLayers(newBoard, newColor, layersLeft, hash);
                checkedBoards.push_back(std::make_tuple(hash, val));
            }
            if ((c == Game::blackID && val > score) || (c == Game::whiteID && val < score))
                score = val;
        }
        return score;
    }
    
    int scoreLayersOld(std::array<int, 69>& b, int c, int layersLeft)
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
                if (Game::isCheck(b, Game::whiteID))
                    return INT_MAX; // white is in checkmate
                return 0; // stalemate
            }
            else
            {
                if (Game::isCheck(b, Game::blackID))
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
            std::array<int, 69> newBoard = b;
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

    int miniMax(std::array<int, 69>& b, int depth, bool maximisingPlayer)
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
                if (Game::isCheck(b, Game::blackID)) // checkmate, give lowest possible score
                    return INT_MIN;
                return 0; // stalemate
            }

        }
        else // white
        {
            std::vector<std::array<int, 4>> allMoves = GetMoves(b, Game::whiteID);
            if (allMoves.size() == 0) // checkmate or stalemate
            {
                if (Game::isCheck(b, Game::whiteID)) // white checkmated, give highest possible score
                    return INT_MAX;
                return 0; // stalemate
            }
        }
    }

    // alpha is best option for max, beta is best for min
    int alphaBeta(std::array<int, 69>& b, int c, int layersLeft, int alpha, int beta)
    {
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
                if (Game::isCheck(b, Game::whiteID))
                    return INT_MAX; // white is in checkmate
                return 0; // stalemate
            }
            else
            {
                if (Game::isCheck(b, Game::blackID))
                    return INT_MIN; // black is in checkmate
                return 0; // stalemate
            }
        }

        int score;
        if (c == Game::blackID) score = INT_MIN;
        else score = INT_MAX;

        std::array<int, 69> newBoard;
        for (std::array<int, 4> m : allMoves)
        {   
            newBoard = b;
            g.movePiece(m[0], m[1], m[2], m[3], newBoard);
            int newColor = !c;

            int val = alphaBeta(newBoard, newColor, layersLeft, alpha, beta);
            // numPositions++;
            if ((c == Game::blackID && val > score) || (c == Game::whiteID && val < score))
                score = val;

            if (c == Game::blackID)
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
        }
        return score;
    }
    


};


// int main()
// {
//     // std::array<int, 69> board;

//     // board = {{Game::makePiece(0, 0, blackID, rookID), Game::makePiece(1, 0, blackID, knightID), Game::makePiece(2, 0, blackID, bishopID), Game::makePiece(3, 0, blackID, queenID), Game::makePiece(4, 0, blackID, kingID), Game::makePiece(5, 0, blackID, bishopID), Game::makePiece(6, 0, blackID, knightID), Game::makePiece(7, 0, blackID, rookID),
//     //           Game::makePiece(0, 1, blackID, pawnID), Game::makePiece(1, 1, blackID, pawnID),   Game::makePiece(2, 1, blackID, pawnID),   Game::makePiece(3, 1, blackID, pawnID),  Game::makePiece(4, 1, blackID, pawnID), Game::makePiece(5, 1, blackID, pawnID),   Game::makePiece(6, 1, blackID, pawnID),   Game::makePiece(7, 1, blackID, pawnID),
//     //           0,                                      0,                                        0,                                        0,                                       0,                                      0,                                        0,                                        0,
//     //           0,                                      0,                                        0,                                        0,                                       0,                                      0,                                        0,                                        0,
//     //           0,                                      0,                                        0,                                        0,                                       0,                                      0,                                        0,                                        0,
//     //           0,                                      0,                                        0,                                        0,                                       0,                                      0,                                        0,                                        0,
//     //           Game::makePiece(0, 6, whiteID, pawnID), Game::makePiece(1, 6, whiteID, pawnID),   Game::makePiece(2, 6, whiteID, pawnID),   Game::makePiece(3, 6, whiteID, pawnID),  Game::makePiece(4, 6, whiteID, pawnID), Game::makePiece(5, 6, whiteID, pawnID),   Game::makePiece(6, 6, whiteID, pawnID),   Game::makePiece(7, 6, whiteID, pawnID),
//     //           Game::makePiece(0, 7, whiteID, rookID), Game::makePiece(1, 7, whiteID, knightID), Game::makePiece(2, 7, whiteID, bishopID), Game::makePiece(3, 7, whiteID, queenID), Game::makePiece(4, 7, whiteID, kingID), Game::makePiece(5, 7, whiteID, bishopID), Game::makePiece(6, 7, whiteID, knightID), Game::makePiece(7, 7, whiteID, rookID),
//     //           0b1111, // castling perms
//     //           0, // en passant targets
//     //           0}}; // curr turn

//     Game game;
//     ChessAi ai(Game::blackID, game);

//     auto t1 = std::chrono::high_resolution_clock::now();
//     ai.MakeBestMove(game.board);
//     auto t2 = std::chrono::high_resolution_clock::now();
//     std::chrono::duration<double, std::milli> ms_double = t2 - t1;
//     std::cout << ms_double.count() << " ms" << std::endl;

//     std::cout << ai.getMovesTime.count() << " ms" << std::endl;
//     std::cout << ai.getMovesTime2.count() << " ms" << std::endl;
//     std::cout << ai.evalTime.count() << " ms" << std::endl;
//     // auto t1 = std::chrono::high_resolution_clock::now();
//     // uint64_t result = ai.fullZobristHash(board, "w");
//     // auto t2 = std::chrono::high_resolution_clock::now();
//     // std::chrono::duration<double, std::milli> ms_double = t2 - t1;
//     // std::cout << ms_double.count() << "ms" << std::endl;
//     // std::cout << result << std::endl;
//     // uint64_t num = result ^ ai.zobristTable[9][6];
//     // std::cout << num << std::endl;
//     // // auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
//     // // std::cout << "that move took " << ms_int.count() << " milliseconds" << std::endl;


//     // t1 = std::chrono::high_resolution_clock::now();
//     // Piece::boardToFEN(board, "w");
//     // t2 = std::chrono::high_resolution_clock::now();
//     // ms_double = t2 - t1;
//     // std::cout << ms_double.count() << "ms" << std::endl;
//     // ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
//     // std::cout << "that move took " << ms_int.count() << " milliseconds" << std::endl;


//     // auto t1 = std::chrono::high_resolution_clock::now();
//     // ai.MakeBestMove(board);
//     // // board[0][2]->GetPossiblePositions(board, true);
//     // // board[0][2]->GetPossiblePositions(board);
//     // auto t2 = std::chrono::high_resolution_clock::now();
//     // auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
//     // std::cout << "that move took " << ms_int.count() << " milliseconds" << std::endl;
// }