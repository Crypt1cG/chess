// #include <array>
// #include <vector>
// #include <iostream> // for testing
// #include <tuple>

// class Piece
// {
// public:
//     int x, y;
//     std::string color;
    
//     Piece(int x, int y, std::string c);
// };

// class Pawn : public Piece
// {
// public:
//     bool hasMoved;

//     Pawn(int x, int y, std::string c);

//     std::vector<std::tuple<int, int>> GetPossiblePositions(std::array<std::array<std::string, 8>,8>& board);
// };

// class Knight : public Piece
// {
// public:
//     Knight(int x, int y, std::string c);

//     std::vector<std::tuple<int, int>> GetPossiblePositions(std::array<std::array<std::string, 8>,8>& board);
// };

// class Rook : public Piece
// {
// public:
//     Rook(int x, int y, std::string c);

//     std::vector<std::tuple<int, int>> GetPossiblePositions(std::array<std::array<std::string, 8>,8>& board);
// };