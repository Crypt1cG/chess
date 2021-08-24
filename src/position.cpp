#include "position.hpp"

Position::Position(std::string fen)
{
    // for (int i = 0; i < 64; i++) squares[i] = 0;
    
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
        if (c == 'P') whitePawns |= U64(1) << index;
        else if (c == 'p') blackPawns |= U64(1) << index;
        else if (c == 'N') whiteKnights |= U64(1) << index;
        else if (c == 'n') blackKnights |= U64(1) << index;
        else if (c == 'B') whiteBishops |= U64(1) << index;
        else if (c == 'b') blackBishops |= U64(1) << index;
        else if (c == 'R') whiteRooks |= U64(1) << index;
        else if (c == 'r') blackRooks |= U64(1) << index;
        else if (c == 'Q') whiteQueens |= U64(1) << index;
        else if (c == 'q') blackQueens |= U64(1) << index;
        else if (c == 'K') whiteKing = U64(1) << index;
        else if (c == 'k') blackKing = U64(1) << index;

        /* numbers */
        else if (c == '1') index += 0;
        else if (c == '2') index += 1;
        else if (c == '3') index += 2;
        else if (c == '4') index += 3;
        else if (c == '5') index += 4;
        else if (c == '6') index += 5;
        else if (c == '7') index += 6;
        else if (c == '8') index += 7;
        
        if (c >= 'A' && c <= 'Z')
        {
            whitePieces |= U64(1) << index;
            occupency |= U64(1) << index;
        }
        else if (c >= 'a' && c <= 'z')
        {
            blackPieces |= U64(1) << index;
            occupency |= U64(1) << index;
        }
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

Position::Position() : Position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -") {};

void Position::print()
{
    std::string sep = "+---+---+---+---+---+---+---+---+";
    std::cout << sep << std::endl;
    for (int i = 0; i < 8; i++)
    {
        std::cout << "| ";
        for (int j = 0; j < 8; j++)
        {
            std::string piece;
            if ((occupency >> (i * 8 + j)) & 1) // not empty
            {
                if ((whitePieces >> (i * 8 + j)) & 1) // white piece
                {
                    if ((whitePawns >> (i * 8 + j)) & 1) piece = "P";
                    else if ((whiteKnights >> (i * 8 + j)) & 1) piece = "N";
                    else if ((whiteBishops >> (i * 8 + j)) & 1) piece = "B";
                    else if ((whiteRooks >> (i * 8 + j)) & 1) piece = "R";
                    else if ((whiteQueens >> (i * 8 + j)) & 1) piece = "Q";
                    else if ((whiteKing >> (i * 8 + j)) & 1) piece = "K";
                }
                else
                {
                    if ((blackPawns >> (i * 8 + j)) & 1) piece = "p";
                    else if ((blackKnights >> (i * 8 + j)) & 1) piece = "n";
                    else if ((blackBishops >> (i * 8 + j)) & 1) piece = "b";
                    else if ((blackRooks >> (i * 8 + j)) & 1) piece = "r";
                    else if ((blackQueens >> (i * 8 + j)) & 1) piece = "q";
                    else if ((blackKing >> (i * 8 + j)) & 1) piece = "k";
                }
            }
            else piece = " ";
            piece += " | ";
            std::cout << piece;
        }
        std::cout << i + 1 << "\n" << sep << std::endl;
    }
    std::cout << "  a   b   c   d   e   f   g   h" << std::endl;
}

bool Position::canCastle(int color, char side)
{
    int mask = color == whiteID ? 0b1000 : 0b10;
    if (side == 'q') mask >>= 1;
    return castling & mask;
}
// int main()
// {
//     Position p = Position("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -");
//     p.print();
// }