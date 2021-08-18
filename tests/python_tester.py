import sys
sys.path.insert(1, 'N:\VSCodeProjects\wxChess\src')

import lichess_implementation as li

def test_move(board, start, dest, expected) -> bool:
    li.make_move_board(board, start, dest)
    if board == expected:
        return True
    return False

def main():
    b = ['r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
          'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
          'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
          'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R',
          0b1111,  # castling wwbb
          0b0,  # en passant
          0b0]  # curr turn 0 = w 1 = b
    be = ['r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
           'p', 'p', 'p', ' ', 'p', 'p', 'p', 'p',
           ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
           ' ', ' ', ' ', 'p', ' ', ' ', ' ', ' ',
           ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
           ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
           'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
           'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R',
           0b1111,  # castling wwbb
           0b0,  # en passant
           0b0]  # curr turn 0 = w 1 = b
    if test_move(b, 11, 27, be):
        print('test 1 successfull')
    else:
        print('test 1 failed')
        print(b)

    b = ['r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
          'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
          'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
          'R', 'N', 'B', 'Q', 'K', ' ', ' ', 'R',
          0b1111,  # castling wwbb
          0b0,  # en passant
          0b0]  # curr turn 0 = w 1 = b
    be = ['r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
           'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
           ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
           ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
           ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
           ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
           'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
           'R', 'N', 'B', 'Q', ' ', 'R', 'K', ' ',
           0b0011,  # castling wwbb
           0b0,  # en passant
           0b0]  # curr turn 0 = w 1 = b
    if test_move(b, 60, 62, be):
        print('test 2 successfull')
    else:
        print('test 2 failed')
        print(b)

    

    
if __name__ == "__main__":
    main()