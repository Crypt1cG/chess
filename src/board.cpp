#include <iostream>
#include <chrono>
#include <array>
#include <algorithm>
#include <vector>
#include <string>
#include "board.hpp"

Move::Move(int f, int t)
{
    from = f;
    to = t;
}

/**
 * Makes an int to represent a piece
 * the form is a 10 bit binary number with the following form:
 * yyyxxxci_d - first 3 bits are y (0-7), next 3 are x (0-7) next is color (0/1) next 3 are id (0-7)
 * yyyxxx is equal to the position - example y = 5, x = 3 would give you 101011 which is 43, 5 * 8 + 3 is also 43
 * 
 * @param pos the piece's position on the board
 * @param color the piece's color (whiteID or blackID)
 * @param id the pieces's id
 * @return an int in the form yyyxxxci_d
 */
int Board::makePiece(int pos, int color, int id) {return (color << 3) + id;}

/**
 * returns the color of a piece (yyyxxxci_d)
 * 
 * @param piece an int representing the piece
 * @return the color (whiteID or blackID)
 */
int Board::getColor(int piece) {return (piece >> 3) & 0b1;}

/**
 * returns the id of a piece (yyyxxxci_d)
 * 
 * @param piece an int representing the piece
 * @return the id (based on constants)
 */
int Board::getID(int piece) {return piece & 0b111;}

/**
 * used to tell if a side can castle in a particular direction
 * 
 * @param side either 'q' or 'k' to indicate desired side to be checked
 * @param color whiteID or blackID
 * @return a bool indicating if the given color can castle on the given side
 */
bool Board::canCastle(char side, int color)
{
    int shift = 0;
    if (color == whiteID) shift += 2;
    if (side == 'k') shift++;
    return castling >> shift & 0b1;
}

/**
 * checks if the given file is an en passant target
 * 
 * @param file the file (x coord)
 * @return true if en passant is possible, false otherwise
 */
bool Board::canBeEnPassant(int file) {return (enPassant >> (7 - file)) & 0b1;}

/**
 * returns all moves (legal only by default) that a piece can make
 * 
 * @param pos the location of the piece
 * @param pseudoLegal if true, returns pseudo-legal moves (doesn't check if a move will result in check)
 * @return a vector of all the moves
 */
std::vector<Move> Board::getMoves(int pos, bool pseudoLegal)
{
    int piece = squares[pos];
    // int y = getY(piece); // faster than / 8? (idk)
    // int x = getX(piece); // faster than % 8? (idk)
    int x = pos % 8;
    // int x = pos & 0b111;
    int y = pos / 8;
    // int y = pos >> 3;
    int id = getID(piece);
    int color = getColor(piece);
    std::vector<Move> results;

    if (id == pawnID)
    {
        results.reserve(4); 
        int inc;
        if (color == whiteID) inc = -8; // decrease y coord by 1
        else inc = 8; // increase y coord by 1
        if (pos + inc >= 8 && pos + inc < 56) // ranks 3-6 (ranks 2 and 7 are promotion, 1 and 8 will never have pawns)
        {
            if (squares[pos + inc] == empty)
            {
                results.emplace_back(pos, pos + inc);
                if ((color == whiteID && y == 6) || (color == blackID && y == 1))
                {
                    if (squares[pos + 2 * inc] == empty) results.emplace_back(pos, pos + 2 * inc);
                }
            }
            /* captures */
            if (x - 1 >= 0 && squares[pos + inc - 1] != 0 && getColor(squares[pos + inc - 1]) != color)
                results.emplace_back(pos, pos + inc - 1);
            if (x + 1 < 8 && squares[pos + inc + 1] != 0 && getColor(squares[pos + inc + 1]) != color)
                results.emplace_back(pos, pos + inc + 1);
            
            /* en passant */
            if((color == whiteID && y == 3) || (color == blackID && y == 4))
            {
                if (x >= 1 && getID(squares[pos - 1]) == pawnID && getColor(squares[pos - 1]) != color && canBeEnPassant(x - 1))
                    results.emplace_back(pos, pos + inc - 1);
                if (x < 7 && getID(squares[pos + 1]) == pawnID && getColor(squares[pos + 1]) != color && canBeEnPassant(x + 1))
                    results.emplace_back(pos, pos + inc + 1);
            }
        }
        else // last rank, promotion time
        {
            if (squares[pos + inc] == empty)
            {
                results.emplace_back(pos, pos + 2 * inc); // queen promotion, should be y=-1 or y=8
                results.emplace_back(pos, pos + 3 * inc); // knight promotion
                results.emplace_back(pos, pos + 4 * inc); // bishop promotion
                results.emplace_back(pos, pos + 5 * inc); // rook promotion
            }
            if (x < 7 && squares[pos + inc + 1] != empty && getColor(squares[pos + inc + 1]) != color)
            {
                results.emplace_back(pos, pos + 2 * inc + 1); // queen promotion, should be y=-1 or y=8
                results.emplace_back(pos, pos + 3 * inc + 1); // knight promotion
                results.emplace_back(pos, pos + 4 * inc + 1); // bishop promotion
                results.emplace_back(pos, pos + 5 * inc + 1); // rook promotion
            }
            if (x > 0 && squares[pos + inc - 1] != empty && getColor(squares[pos + inc - 1]) != color)
            {
                results.emplace_back(pos, pos + 2 * inc - 1); // queen promotion, should be y=-1 or y=8
                results.emplace_back(pos, pos + 3 * inc - 1); // knight promotion
                results.emplace_back(pos, pos + 4 * inc - 1); // bishop promotion
                results.emplace_back(pos, pos + 5 * inc - 1); // rook promotion
            }
        }
    }
    else if (id == knightID)
    {
        results.reserve(8);
        std::vector<int> moves = knightMoves[pos];
        for (int dest : moves)
        {
            if (squares[dest] == empty || getColor(squares[dest]) != color)
                results.emplace_back(pos, dest);
        }
    }
    else if (id == bishopID)
    {
        results.reserve(14);
        for (std::vector<int> dir : bishopMoves[pos])
        {
            for (int dest : dir)
            {
                int p = squares[dest];
                if (p == empty || getColor(p) != color)
                    results.emplace_back(pos, dest);
                if (p != empty)
                    break;
            }
        }
    }
    else if (id == rookID)
    {
        results.reserve(15);
        for (std::vector<int> dir : rookMoves[pos])
        {
            for (int dest : dir)
            {
                int p = squares[dest];
                if (p == empty || getColor(p) != color)
                    results.emplace_back(pos, dest);
                if (p != empty)
                    break;
            }
        }
    }
    else if (id == queenID)
    {
        results.reserve(29);
        // bishop code
        for (std::vector<int> dir : bishopMoves[pos])
        {
            for (int dest : dir)
            {
                int p = squares[dest];
                if (p == empty || getColor(p) != color)
                    results.emplace_back(pos, dest);
                if (p != empty)
                    break;
            }
        }
        // rook code
        for (std::vector<int> dir : rookMoves[pos])
        {
            for (int dest : dir)
            {
                int p = squares[dest];
                if (p == empty || getColor(p) != color)
                    results.emplace_back(pos, dest);
                if (p != empty)
                    break;
            }
        }
    }
    else if (id == kingID)
    {
        results.reserve(10);
        std::vector<int> moves = kingMoves[pos];
        for (int dest : moves)
        {
            if (squares[dest] == empty || getColor(squares[dest]) != color)
                results.emplace_back(pos, dest);
        }

        // castling
        // TODO: i think this can be optimized (i think color check is unecess bc would be check, maybe don't copy board until canCastle is called)
        if (!isCheck(color) && x == 4)
        {
            if (getID(squares[pos + 3]) == rookID && getColor(squares[pos + 3]) == color && squares[pos + 1] == empty && squares[pos + 2] == empty)
            {
                Board tmp = *this;
                tmp.movePiece(Move(pos, pos + 1)); // check the in-between spot to ensure we're not castling thru check
                // movePiece(Move(pos, pos + 1));
                if (canCastle('k', color) && !tmp.isCheck(color)) 
                    results.emplace_back(pos, pos + 2);
                // unMakeMove();
            }
            if (getID(squares[pos - 4]) == rookID && getColor(squares[pos - 4]) == color && squares[pos - 1] == empty && squares[pos - 2] == empty && squares[pos - 3] == empty)
            {
                Board tmp = *this;
                tmp.movePiece(Move(pos, pos - 1)); // check the in-between spot to ensure we're not castling thru check
                // movePiece(Move(pos, pos - 1));
                if (canCastle('q', color) && !tmp.isCheck(color))
                    results.emplace_back(pos, pos - 2);
                // unMakeMove();
            }
        }
    }

    if (pseudoLegal) return results;

    std::vector<Move> finalMoves;
    // finalMoves.push_back(Move(10, -6));
    finalMoves.reserve(results.size());
    // for (int i = 0; i < results.size(); i++)
    // {
    //     Board newBoard = *this;
    //     newBoard.movePiece(results[i]);
    //     if (!newBoard.isCheck(color))
    //         finalMoves.push_back(results[i]);
    // }
    for (Move& m : results)
    {
        Board newBoard = *this;
        newBoard.movePiece(m);
        // movePiece(m);
        if (!newBoard.isCheck(color))
            // Move newMove = Move(m.from, m.to);
            finalMoves.push_back(m);
            // finalMoves.emplace_back(Move(m.from, m.to));
        // unMakeMove();
    }
    return finalMoves;
}

/**
 * checks if the player given by color is in check
 * 
 * @param color the side in question
 * @return true if in check, false otherwise
 */
bool Board::isCheck(int color)
{
    int kingPos;
    if (color == whiteID) kingPos = whiteKingPos;
    else kingPos = blackKingPos;

    // check for knights attacking
    std::vector<int>& knMoves = knightMoves[kingPos];
    for (int dest : knMoves)
    {
        if (getID(squares[dest]) == knightID && getColor(squares[dest]) != color)
            return true;
    }

    // check for rooks/queens attacking
    for (std::vector<int>& dir : rookMoves[kingPos])
    {
        for (int dest : dir)
        {
            if ((getID(squares[dest]) == rookID || getID(squares[dest]) == queenID) && getColor(squares[dest]) != color)
                return true;
            if (squares[dest] != empty)
                break;
        }
    }
    
    // check for bishops/queens attacking
    for (std::vector<int>& dir : bishopMoves[kingPos])
    {
        for (int dest : dir)
        {
            if ((getID(squares[dest]) == bishopID || getID(squares[dest]) == queenID) && getColor(squares[dest]) != color)
                return true;
            if (squares[dest] != empty)
                break;
        }
    }

    // check for kings attacking
    std::vector<int> kMoves = kingMoves[kingPos];
    for (int dest : kMoves)
    {
        if (getID(squares[dest]) == kingID) // don't have to check for opposite color cause only 1 other king (1 is at kingpos)
            return true;
    }
    
    // check for pawns attacking
    int kingX = kingPos % 8;
    if (color == whiteID && kingPos > 7) // add > 7 bc can't get attacked on first rank by pawns (would have to be off the board)
    {
        if (kingX > 0 && getID(squares[kingPos - 9]) == pawnID && getColor(squares[kingPos - 9]) != color)
            return true;
        if (kingX < 7 && getID(squares[kingPos - 7]) == pawnID && getColor(squares[kingPos - 7]) != color)
            return true;
    }
    else if (color == blackID && kingPos < 56) // see comment above for < 56
    {
        if (kingX > 0 && getID(squares[kingPos + 7]) == pawnID && getColor(squares[kingPos + 7]) != color)
            return true;
        if (kingX < 7 && getID(squares[kingPos + 9]) == pawnID && getColor(squares[kingPos + 9]) != color)
            return true;
    }
    return false;
}

/**
 * moves a piece
 * 
 * @param move the move to be made
 */
void Board::movePiece(Move move)
{
    int dest = move.to;
    int pos = move.from;
    int destX, destY;
    if (dest >= 0)
    {
        destY = dest / 8;
        // destY = dest >> 3;
        destX = dest % 8;
        // destX = dest & 0b111;
    }
    else
    {
        destY = (dest + 1) / 8 - 1;
        // destY = (dest + 1) >> 3 - 1;
        destX = (dest + 1) % 8 + 8 - 1;
        // destY = (dest + 1) & 0b111 + 7;
    }
    int currX = pos % 8;
    int currY = pos / 8;
    int p = squares[pos];
    int color = getColor(p);
    int id = getID(p);
    int newEnPassant = 0;
    if (id == pawnID)
    {
        if (destY > 0 && destY < 7) // non promotion moves
        {
            if (abs(currY - destY) == 2)
                newEnPassant = 0b1 << (7 - destX);
            if (abs(currX - destX) == 1) // it moved horizontally, either regular or en passant capture
            {
                if (squares[dest] == empty) // moving to empty square == en passant
                    squares[currY * 8 + destX] = empty; // the pawn en passant -ed
            }
        }
        else // promotion stuff
        {
            if (destY == -1) // white queen promotion
                squares[0 * 8 + destX] = makePiece((0 * 8 + destX), color, queenID);
            else if (destY == 8) // black queen promotion
                squares[7 * 8 + destX] = makePiece((56 + destX), color, queenID);
            else if (destY == -2) // white knight
                squares[0 * 8 + destX] = makePiece((0 + destX), color, knightID);
            else if (destY == 9) // black knight
                squares[7 * 8 + destX] = makePiece((56 + destX), color, knightID);
            else if (destY == -3) // white bishop
                squares[0 * 8 + destX] = makePiece((0 + destX), color, bishopID);
            else if (destY == 10) // black bishop
                squares[7 * 8 + destX] = makePiece((56 + destX), color, bishopID);
            else if (destY == -4) // white rook
                squares[0 * 8 + destX] = makePiece((0 + destX), color, rookID);
            else if (destY == 11) // black rook
                squares[7 * 8 + destX] = makePiece((56 + destX), color, rookID);
            squares[pos] = empty;
            enPassant = 0; // get rid of any old en passants
            currTurn = !currTurn;
            return; // don't need to do anything else
        } 
    }
    else if (id == kingID)
    {
        if (abs(currX - destX) == 2) // castling
        {
            if (destX == 6) // castled king side
            {
                // move the rook
                squares[destY * 8 + 5] = squares[destY * 8 + 7];
                squares[destY * 8 + 7] = empty;
                // setX(squares[destY * 8 + 5], 5);
            }
            else // castled queen side
            {
                // move the rook
                squares[destY * 8 + 3] = squares[destY * 8 + 0];
                squares[destY * 8 + 0] = empty;
                // setX(squares[destY * 8 + 3], 3);
            }
        }
        if (color == whiteID)
        {
            castling = castling & 0b0011; // remove white castling rights
            whiteKingPos = dest;
        }
        else
        {
            castling = castling & 0b1100; // remove black castling rights
            blackKingPos = dest;
        }
    }
    else if (id == rookID)
    {
        if (color == whiteID)
        {
            if (pos == 63) // king side
                castling = castling & 0b0111;
            else if (pos == 56) // queen side
                castling = castling & 0b1011;
        }
        else
        {
            if (pos == 7) // king side
                castling = castling & 0b1101;
            else if (pos == 0) // queen side
                castling = castling & 0b1110;
        }
    }
    
    // int first = (pos << 4) + p;
    // int second = (dest << 4) + squares[dest];
    squares[dest] = squares[pos];
    squares[pos] = empty;
    // setX(squares[dest], destX);
    // setY(squares[dest], destY);
    currTurn = !currTurn;
    enPassant = newEnPassant;
    // moves.push({first, second});
}

/**
 * unmakes the most recent moves (uses stack)
 */
void Board::unMakeMove()
{
    // each int is yyyxxxci_d
    std::pair<int, int> move = moves.top();
    moves.pop();

    int firstPos = move.first >> 4;
    int firstPiece = move.first & 0b1111;
    squares[firstPos] = firstPiece;

    int secondPos = move.second >> 4;
    int secondPiece = move.second & 0b1111;
    squares[secondPos] = secondPiece;
    currTurn = !currTurn;
};

/**
 * Constructor for making a board from an FEN string
 * 
 * @param fen the fen string to make the board from
 */
Board::Board(std::string fen)
{
    for (int i = 0; i < 64; i++) squares[i] = 0;

    size_t firstSpace = fen.find(" "); // first space separates actual board from other stuff
    size_t secondSpace = fen.find(" ", firstSpace + 1); // separates toMove and castling
    size_t thirdSpace = fen.find(" ", secondSpace + 1); // separates castling and en pasant

    std::string boardStr = fen.substr(0, firstSpace);
    std::string toMove = fen.substr(firstSpace + 1, 1); // "w" or "b"
    std::string castlingStr = fen.substr(secondSpace + 1, thirdSpace - secondSpace); // KQkq means both can castle king and queen side
    std::string enPassant = fen.substr(thirdSpace + 1); // the rest of the string

    int index = 0;
    for (char c : boardStr)
    {
        char id = tolower(c);
        int piece = 0;
        if (id == 'p') piece = makePiece(index, whiteID, pawnID);
        else if (id == 'n') piece = makePiece(index, whiteID, knightID);
        else if (id == 'b') piece = makePiece(index, whiteID, bishopID);
        else if (id == 'r') piece = makePiece(index, whiteID, rookID);
        else if (id == 'q') piece = makePiece(index, whiteID, queenID);
        else if (id == 'k') 
        {
            piece = makePiece(index, whiteID, kingID);
            if (c == 'K') whiteKingPos = index;
            else blackKingPos = index;
        }
        /* numbers */
        else if (c == '1') index += 0;
        else if (c == '2') index += 1;
        else if (c == '3') index += 2;
        else if (c == '4') index += 3;
        else if (c == '5') index += 4;
        else if (c == '6') index += 5;
        else if (c == '7') index += 6;
        else if (c == '8') index += 7;
        
        if (c >= 'a' && c <= 'z') // lowercase - this means it's a black piece
            piece = (piece & 0b1111110111) + 0b1000; // change the color bit
        
        squares[index] = piece;

        if (c != '/') index++;
    }

    if (toMove == "w") currTurn = whiteID;
    else currTurn = blackID;

    int castlingNum = 0;
    for (char c : castlingStr)
    {
        if (c == '-') break;
        else if (c == 'K') // white king side
            castlingNum += 0b1000;
        else if (c == 'Q') // white queen side
            castlingNum += 0b0100;
        else if (c == 'k') // black king side
            castlingNum += 0b0010;
        else if (c == 'q') // white queen side
            castlingNum += 0b0001;
    }
    castling = castlingNum;

    if (enPassant != "-")
    {
        char file = enPassant.at(0);
        if (file == 'a') enPassant = 0b10000000;
        else if (file == 'b') enPassant = 0b01000000;
        else if (file == 'c') enPassant = 0b00100000;
        else if (file == 'd') enPassant = 0b00010000;
        else if (file == 'e') enPassant = 0b00001000;
        else if (file == 'f') enPassant = 0b00000100;
        else if (file == 'g') enPassant = 0b00000010;
        else if (file == 'h') enPassant = 0b00000001;
    }
}

/**
 * Default constructor - makes a board from default fen
 */
Board::Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -") {};

/**
 * prints the board
 */
void Board::print()
{
    int lowerDiff = 'a' - 'A';
    std::string line = "+---+---+---+---+---+---+---+---+";
    std::cout << line << std::endl;
    for (int i = 0; i < 8; i++)
    {
        std::string row = "| ";
        for (int j = 0; j < 8; j++)
        {
            char c;
            if (squares[i * 8 + j] != 0)
            {
                int id = getID(squares[i * 8 + j]);
                switch (id)
                {
                case pawnID:
                    c = 'P';
                    break;
                case knightID:
                    c = 'N';
                    break;
                case bishopID:
                    c = 'B';
                    break;
                case rookID:
                    c = 'R';
                    break;
                case queenID:
                    c = 'Q';
                    break;
                case kingID:
                    c = 'K';
                    break;
                }
                if (getColor(squares[i * 8 + j]) == blackID) c += lowerDiff;
            }
            else c = ' ';
            row += c;
            row += " | ";
        }
        std::cout << row << std::endl;
        std::cout << line << std::endl;
    }
}

/**
 * populates move arrays
 */
void Board::initMoves()
{
    for (int i = 0; i < 64; i++)
    {
        int x = i % 8;
        int y = i / 8;

        /*King moves*/
        std::vector<int> king = {-9, -8, -7, -1, 1, 7, 8, 9}; // list of possible king moves (offsets)
        if (x == 0)
        {
            auto it = std::find(king.begin(), king.end(), -9);
            if (it != king.end()) king.erase(it);
            it = std::find(king.begin(), king.end(), -1);
            if (it != king.end()) king.erase(it);
            it = std::find(king.begin(), king.end(), 7);
            if (it != king.end()) king.erase(it);
        }
        else if (x == 7)
        {
            auto it = std::find(king.begin(), king.end(), -7);
            if (it != king.end()) king.erase(it);
            it = std::find(king.begin(), king.end(), 1);
            if (it != king.end()) king.erase(it);
            it = std::find(king.begin(), king.end(), 9);
            if (it != king.end()) king.erase(it);
        }
        if (y == 0)
        {
            auto it = std::find(king.begin(), king.end(), -7);
            if (it != king.end()) king.erase(it);
            it = std::find(king.begin(), king.end(), -8);
            if (it != king.end()) king.erase(it);
            it = std::find(king.begin(), king.end(), -9);
            if (it != king.end()) king.erase(it);
        } 
        else if (y == 7)
        {
            auto it = std::find(king.begin(), king.end(), 7);
            if (it != king.end()) king.erase(it);
            it = std::find(king.begin(), king.end(), 8);
            if (it != king.end()) king.erase(it);
            it = std::find(king.begin(), king.end(), 9);
            if (it != king.end()) king.erase(it);
        }
        std::vector<int> finalKing;
        finalKing.reserve(king.size());
        for (int m : king) finalKing.push_back(i + m);
        kingMoves[i] = finalKing;
        
        /*Knight moves*/
        // x and y offsets for all possible knight moves
        std::vector<std::array<int, 2>> knight = {{-1, -2}, {1, -2}, {-1, 2}, {1, 2}, {-2, -1}, {-2, 1}, {2, -1}, {2, 1}};
        for (std::array<int, 2> m: knight)
        {
            if (x + m[0] >= 0 && x + m[0] <= 7 && y + m[1] >= 0 && y + m[1] <= 7)
                knightMoves[i].push_back(i + (m[1] * 8 + m[0]));
        }

        /*Rook moves*/
        std::array<std::vector<int>, 4> rook;
        // up moves
        std::vector<int> up;
        for (int j = y - 1; j >= 0; j--) up.push_back(j * 8 + x);
        rook[0] = up;

        // right moves
        std::vector<int> right;
        for (int j = x + 1; j <= 7; j++) right.push_back(y * 8 + j);
        rook[1] = right;
        
        // down moves
        std::vector<int> down;
        for (int j = y + 1; j <= 7; j++) down.push_back(j * 8 + x);
        rook[2] = down;

        // left moves
        std::vector<int> left;
        for (int j = x - 1; j >= 0; j--) left.push_back(y * 8 + j);
        rook[3] = left;

        rookMoves[i] = rook;

        /*Bishop Moves*/
        // positive positive
        int newX = x + 1;
        int newY = y + 1;
        while (newX < 8 && newY < 8)
        {
            bishopMoves[i][0].push_back(newY * 8 + newX);
            newX++;
            newY++;
        }

        // positive negative
        newX = x + 1;
        newY = y - 1;
        while (newX < 8 && newY >= 0)
        {
            bishopMoves[i][1].push_back(newY * 8 + newX);
            newX++;
            newY--;
        }

        // negative negative
        newX = x - 1;
        newY = y - 1;
        while (newX >= 0 && newY >= 0)
        {
            bishopMoves[i][2].push_back(newY * 8 + newX);
            newX--;
            newY--;
        }

        // negative positive
        newX = x - 1;
        newY = y + 1;
        while (newX >= 0 && newY < 8)
        {
            bishopMoves[i][3].push_back(newY * 8 + newX);
            newX--;
            newY++;
        }
    }
}

/**
 * converts a board index to algebraic notation 
 * ex. 0 -> a8
 * 
 * @param index the board index
 * @return a string that is the square expressed in algebraic notation
 */
std::string Board::indexToAlg(int index)
{
    std::string result = "";
    if (index >= 0 && index < 64)
    {
        char file = 'a';
        int x = index % 8;
        file += x;
        result += file;
        int y = 8 - index / 8;
        result += std::to_string(y);
        return result;
    }
    else if (index >= 64)
    {
        char file = 'a';
        int x = index % 8;
        file += x;
        result += file;
        result += '1';
        int y = index / 8;
        switch (y)
        {
        case 8:
            result += 'q';
            break;
        case 9:
            result += 'n';
            break;
        case 10:
            result += 'b';
            break;
        case 11:
            result += 'r';
            break;
        }
        return result;
    }
    else return std::to_string(index);
}

// int main()
// {
//     Board::initMoves();
//     // std::array<int, 69> board = Game::FENtoBoard("8/1K6/5B2/3R4/2N3P1/4Q3/8/k7 w - -");
//     Board board = Board("8/1K6/5B2/3R4/2N3P1/4Q3/8/k7 w - -");
//     int pawnX = 6;
//     int pawnY = 4;
//     int bishopX = 5;
//     int bishopY = 2;
//     int rookX = 3;
//     int rookY = 3;
//     int knightX = 2;
//     int knightY = 4;
//     int kingX = 1;
//     int kingY = 1;
//     int queenX = 4;
//     int queenY = 5;
//     bool allmoves = false;
//     int numTrials = 1000000;
//     // king perf test
//     auto t1 = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < numTrials; i++)
//     {
//         // Game::getMoves(board, kingX, kingY, allmoves);
//         board.getMoves(kingY * 8 + kingX, allmoves);
//     }
//     auto t2 = std::chrono::high_resolution_clock::now();
//     std::chrono::duration<double, std::milli> time = t2 - t1;
//     std::cout << "King: " << time.count() << std::endl;

//     // pawn perf test
//     t1 = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < numTrials; i++)
//     {
//         // Game::getMoves(board, pawnX, pawnY, allmoves);
//         board.getMoves(pawnY * 8 + pawnX, allmoves);
//     }
//     t2 = std::chrono::high_resolution_clock::now();
//     time = t2 - t1;
//     std::cout << "Pawn: " << time.count() << std::endl;

//     // bishop perf test
//     t1 = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < numTrials; i++)
//     {
//         // Game::getMoves(board, bishopX, bishopY, allmoves);
//         board.getMoves(bishopY * 8 + bishopX, allmoves);
//     }
//     t2 = std::chrono::high_resolution_clock::now();
//     time = t2 - t1;
//     std::cout << "Bishop: " << time.count() << std::endl;

//     // rook perf test
//     t1 = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < numTrials; i++)
//     {
//         // Game::getMoves(board, rookX, rookY, allmoves);
//         board.getMoves(rookY * 8 + rookX, allmoves);
//     }
//     t2 = std::chrono::high_resolution_clock::now();
//     time = t2 - t1;
//     std::cout << "Rook: " << time.count() << std::endl;

//     // knight perf test
//     t1 = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < numTrials; i++)
//     {
//         // Game::getMoves(board, knightX, knightY, allmoves);
//         board.getMoves(knightY * 8 + knightX, allmoves);
//     }
//     t2 = std::chrono::high_resolution_clock::now();
//     time = t2 - t1;
//     std::cout << "Knight: " << time.count() << std::endl;

//     // queen perf test
//     t1 = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < numTrials; i++)
//     {
//         // Game::getMoves(board, queenX, queenY, allmoves);
//         board.getMoves(queenY * 8 + queenX, allmoves);
//     }
//     t2 = std::chrono::high_resolution_clock::now();
//     time = t2 - t1;
//     std::cout << "Queen: " << time.count() << std::endl;

//     std::cout << "done" << std::endl;
// }