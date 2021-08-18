def rook_moves():
    moves = []
    for i in range(64):
        this_sq_moves = []
        x = i % 8
        y = i // 8
        # up
        up = []
        for j in range(y - 1, -1, -1):
            up.append(j * 8 + x)
        this_sq_moves.append(up)
        # right
        right = []
        for j in range(x, 8):
            right.append(y * 8 + j)
        this_sq_moves.append(right)
        # down
        down = []
        for j in range(y, 8):
            down.append(j * 8 + x)
        this_sq_moves.append(down)
        # left
        left = []
        for j in range(x - 1, -1, -1):
            left.append(y * 8 + j)
        this_sq_moves.append(left)
        moves.append(this_sq_moves)
    print(moves)
        
def main():
    rook_moves()
    
if __name__ == "__main__":
    main()