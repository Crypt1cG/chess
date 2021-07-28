#include "chessPiecesv2.cpp"
#include <array>
#include <chrono>
#include <string>

#define getColor(x) ((x & 0b0000001000) >> 3)
int getC(int p) {return (p & 0b0000001000) >> 3;}

int main()
{
    // std::array<Piece*, 4> p = {{new Pawn(0, 0, "w"), new Rook(1, 0, "w"), new Queen(2, 0, "W"), new Knight(3, 0, "w")}};
    // // Piece* piece = new Pawn(0, 0, "w");
    // // Piece* piece2;
    // // memcpy(piece2, piece, sizeof(piece));
    // // std::array<Piece*, 4> p2;
    // // for (int i = 0; i < 4; i++)
    // // {
    // //     std::size_t s = sizeof(p[i]);
    // //     memcpy(p2.at(i), p.at(i), s);
    // // }
    
    // int n = 0b1011111100;
    // int n2 = 5714;
    // std::string s = "w";
    // int color = 1;
    // std::array<int, 2> a = {{1, 2}};
    // std::array<int, 2> a1, a2;
    
    // std::cout << n << std::endl;
    // std::cout << "Hello" << std::endl;
    // int id = n & 0b0000000111;
    // int c = (n & 0b0000001000) >> 3;
    // int y = (n & 0b0001110000) >> 4;
    // int x = (n & 0b1110000000) >> 7;
    // std::cout << (5714 % 10) << " " << getID(5714) << id << std::endl;
    // std::cout << (5714 % 100) / 10 << " " << getC(5714) << c << std::endl;
    // std::cout << (5714 % 1000) / 100 << " " << getY(5714) << y << std::endl;
    // std::cout << (5714 / 1000) << " " << getX(5714) << x << std::endl;

    // bool r;
    // auto t1 = std::chrono::high_resolution_clock::now();
    // for (int i = 0; i < 500000000; i++)
    // {
    //     // r = n & 0b0000000111;
    //     // r = (n & 0b0000001000) >> 3;
    //     // r = (n & 0b0001110000) >> 4;
    //     // r = (n & 0b1110000000) >> 7;
    //     // r = getX(n);
    //     a1 = a;

    // }
    // auto t2 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double, std::milli> ms_double = t2 - t1;
    // std::cout << ms_double.count() << " ms" << std::endl;

    // t1 = std::chrono::high_resolution_clock::now();
    // for (int i = 0; i < 500000000; i++)
    // {
    //     // r = n2 % 10;
    //     // r = (n2 % 100) / 10;
    //     // r = (n2 % 1000) / 100;
    //     // r = n2 / 1000;
    //     // r = GetX(n);
    //     memcpy(a2.data(), a.data(), sizeof(a));
    // }

    // t2 = std::chrono::high_resolution_clock::now();
    // ms_double = t2 - t1;
    // std::cout << ms_double.count() << " ms" << std::endl;

    // std::array<int, 2> a = {{1, 2}};
    // std::array<int, 2> b = a;
    // std::array<int, 2> c;
    // memcpy(c.data(), a.data(), sizeof(a));

    // int a = 0b111010;
    // int b = a >> 3;
    // std::cout << a << b << std::endl;

    int x = 0b0000001000;
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100000000; i++)
    {
        getC(x);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = t2 - t1;
    std::cout << ms_double.count() << " ms" << std::endl;
}