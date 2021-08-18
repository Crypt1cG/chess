#include <array>
#include <utility>
#include <cstdint>
class ZobristTable
{
public:
    static const int SIZE = 128;
    
    std::pair<std::uint64_t, int>& operator[] (std::uint64_t hash)
    {
        std::uint64_t index = hash % SIZE;
        return elems[index];
    }
    uint64_t hash(std::array<int, 69>& b)
    {

    }
private:
    std::array<std::pair<std::uint64_t, int>, SIZE> elems;
};