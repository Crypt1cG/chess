#pragma once

class Move
{
public:
    int from, to;
    int piece;
    Move(const int f, const int t, const int p) {from = f; to = t; piece = p;}
};