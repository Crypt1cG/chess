#include <iostream>
#include <bitset>
void printULL(unsigned long long num)
{
    for (int i = 0; i < 64; i++)
    {
        if (i % 8 == 0) std::cout << std::endl;
        int n = num >> i & 1;
        if (n) std::cout << '1';
        else std::cout << '.';
    }
    std::cout << std::endl;
}
int main()
{
    unsigned long long rayAttacks[64][8] = {};   
    for (int i = 0; i < 64; i++)
    {
        int x = i % 8;
        int y = i / 8;
        unsigned long long masks[8] = {};
        unsigned long long mask = 0;
        /* neg x, pos y (+7) */
        mask = 0;
        int dx = x;
        int dy = 7 - y;
        int newx = x;
        int newy = y;
        for (int j = 0; j < std::min(dx, dy); j++)
        {
            newx--;
            newy++;
            mask |= 1ull << (newy * 8 + newx);
        }
        masks[0] = mask;
        /* pos y (+8) */
        mask = 0;
        for (int j = y + 1; j < 8; j++)
        {
            mask |= 1ull << (j * 8 + x);
        }
        masks[1] = mask;
        /* pos x, pos y (+9) */
        mask = 0;
        dx = 7 - x;
        dy = 7 - y;
        newx = x;
        newy = y;
        for (int j = 0; j < std::min(dx, dy); j++)
        {
            newx++;
            newy++;
            mask |= 1ull << (newy * 8 + newx);
        }
        masks[2] = mask;
        /* pos x (+1) */
        mask = 0;
        for (int j = x + 1; j < 8; j++)
        {
            mask |= 1ull << (y * 8 + j);
        }
        masks[3] = mask;
        /* pos x, neg y (-7) */
        mask = 0;
        dx = 7 - x;
        dy = y;
        newx = x;
        newy = y;
        for (int j = 0; j < std::min(dx, dy); j++)
        {
            newx++;
            newy--;
            mask |= 1ull << (newy * 8 + newx);
        }
        masks[4] = mask;
        /* neg y (-8) */
        mask = 0;
        for (int j = y - 1; j >= 0; j--)
        {
            mask |= 1ull << (j * 8 + x);
        }
        masks[5] = mask;
        /* neg x, neg y (-9) */
        mask = 0;
        dx = x;
        dy = y;
        newx = x;
        newy = y;
        for (int j = 0; j < std::min(dx, dy); j++)
        {
            newx--;
            newy--;
            mask |= 1ull << (newy * 8 + newx);
        }
        masks[6] = mask;
        /* neg x (-1) */
        mask = 0;
        for (int j = x - 1; j >= 0; j--)
        {
            mask |= 1ull << (y * 8 + j);
        }
        masks[7] = mask;
        for (int k = 0; k < 8; k++)
            rayAttacks[i][k] = masks[k];
        // rayAttacks[i] = masks;
    }

    // for (auto m : rayAttacks)
    for (int j = 0; j < 64; j++)
    {
        // std::cout << m << ", ";
        // std::cout << "0b" << std::bitset<64>(m) << std::endl;
        std::cout << '{';
        for (int i = 0; i < 8; i++)
        {
            // std::cout << std::endl << i;
            std::cout << rayAttacks[j][i] << ", ";
            // printULL(m[i]);
        }
        std::cout << "}," << std::endl;
    }
}