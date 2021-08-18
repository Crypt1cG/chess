#include "chessAiRewrite.cpp"

int main()
{
    std::string FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";
    std::array<int, 69> board = Game::FENtoBoard(FEN);
    // makeBestMove(board, Game::blackID);
    // long long depth1 = 46;
    // long long depth2 = 2079;
    // long long depth3 = 89890;
    // long long depth4 = 3894594;
    // long long depth5 = 164075551;

    // DEPTH = 1;
    // makeBestMove(board, Game::whiteID);
    // if (numPositions == depth1)
    //     std::cout << "Depth 1 passed (" << depth1 << ")" << std::endl;

    // board = Game::FENtoBoard(FEN);
    // DEPTH = 2;
    // makeBestMove(board, Game::whiteID);
    // if (numPositions == depth2)
    //     std::cout << "Depth 2 passed (" << depth2 << ")" << std::endl;

    // board = Game::FENtoBoard(FEN);
    // DEPTH = 3;
    // makeBestMove(board, Game::whiteID);
    // if (numPositions == depth3)
    //     std::cout << "Depth 3 passed (" << depth3 << ")" << std::endl;
    
    // board = Game::FENtoBoard(FEN);
    // DEPTH = 4;
    // makeBestMove(board, Game::whiteID);
    // if (numPositions == depth4)
    //     std::cout << "Depth 4 passed (" << depth4 << ")" << std::endl;
    
    // board = Game::FENtoBoard(FEN);
    // DEPTH = 5;
    // makeBestMove(board, Game::whiteID);
    // if (numPositions == depth5)
    //     std::cout << "Depth 5 passed (" << depth5 << ")" << std::endl;
}