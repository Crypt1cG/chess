'''makes the ai able to play games directly on lichess using the api
TOKEN: API_TOKEN'''

# TODO: keep track of board using a board array, turn into FEN to give to ai to make move
# NOTE: could try storing an FEN string instead of board, update that for every move
import berserk
import random

session: berserk.session
client: berserk.Client
board = []
TOKEN = "API_TOKEN"
ID: str


def start_game_ai(level: int, color: str):
    global ID
    info = client.challenges.create_ai(level=level, color=color)
    ID = info['id']

def make_move_lichess(start: int, end: int):
    move_str = index_to_alg(start)
    move_str += index_to_alg(end)
    # promoting adds 'q' for queen? ig adds char representing the piece promoted to

    client.bots.make_move(ID, move_str)

def make_move_board(b: list[str], start: int, dest: int):
    p: str
    p = b[start]
    if dest > 0:
        dest_x = dest % 8
        dest_y = dest // 8
    else:
        dest_x = 8 - dest % 8
        dest_y = dest / 8 - 1

    x = start % 8
    y = start / 8

    new_en_passant = 0

    if p.lower() == 'p':
        if dest_y >= 1 and dest_y < 7: # non promotion moves
            if abs(y - dest_y) == 2: # double move, need to update en passant
                new_en_passant = (0b1 << 8) >> dest_x
            if abs(x - dest_x) == 1: # capture (either en passant or regular)
                if b[dest] == ' ': # moving to an empty square (en passant)
                    b[y * 8 + x] = ' ' # the pawn en passant-ed

        else: # promotion stuff
            if dest_y == -1: # queen promotion (white)
                b[0 * 8 + dest_x] = 'Q'
                b[start] = ' '
                return # don't need to do anything else like en passant bc this can't be a double move
            elif destY == 8: # queen promotion (black)
                b[7 * 8 + dest_x] = 'q'
                b[start] = ' '
                return # don't need to do anything else like en passant bc this can't be a double move
            elif dest_y == -2: # knight (white)
                b[0 * 8 + dest_x] = 'N'
                b[start] = ' '
                return # don't need to do anything else like en passant bc this can't be a double move
            elif dest_y == 9: # knight (black)
                b[7 * 8 + dest_x] = 'n'
                b[start] = ' '
                return # don't need to do anything else like en passant bc this can't be a double move
            elif dest_y == -3: # bishop (white)
                b[0 * 8 + dest_x] = 'B'
                b[start] = ' '
                return # don't need to do anything else like en passant bc this can't be a double move
            elif dest_y == 10: # bishop (black)
                b[7 * 8 + dest_x] = 'b'
                b[start] = ' '
                return # don't need to do anything else like en passant bc this can't be a double move
            elif dest_y == -4: # rook (white)
                b[0 * 8 + dest_x] = 'R'
                b[start] = ' '
                return # don't need to do anything else like en passant bc this can't be a double move
            else: # rook (black)
                b[7 * 8 + dest_x] = 'r'
                b[start] = ' '
                return # don't need to do anything else like en passant bc this can't be a double move

    elif p.lower() == 'k':
        if abs(x - dest_x) == 2: # castling
            if dest_x == 6: # castled king side
                # move the rook
                b[dest_y * 8 + 5] = b[dest_y * 8 + 7]
                b[dest_y * 8 + 7] = ' '
            else: # castled queen side
                # move the rook
                b[dest_y * 8 + 3] = b[dest_y * 8 + 0]
                b[dest_y * 8 + 0] = ' '
        if p == "K": # white
            b[64] = b[64] & 0b0011 # remove white castling rights
        else:
            b[64] = b[64] & 0b1100 # remove black castling rights

    elif p.lower() == 'r':
        if p == "R": # white
            if x == 7: # king side
                b[64] = b[64] & 0b1011
            elif x == 0: # queen side
                b[64] = b[64] & 0b0111
        else:
            if x == 7: # king side
                b[64] = b[64] & 0b1110
            elif x == 0: # queen side
                b[64] = b[64] & 0b1101

    b[dest] = b[start]
    b[start] = ' '
    b[65] = new_en_passant

def index_to_alg(pos: int) -> str:
    '''# converts a board index to algebraic notation (ex. 0 -> a8 (tl corner))'''
    if pos > 0:
        x = pos % 8
        y = pos // 8
    else:  # white promotions are negative
        x = 8 - pos % 8
        y = pos / 8 - 1

    pos_str = chr(ord('a') + x)
    pos_str += str(y + 1)  # add 1 bc not zero indexed
    return pos_str

def alg_to_index(pos: str) -> int:
    pass

def game_loop():
    gen = client.bots.stream_game_state(ID)
    for event in gen: # just keeps going...
        print(event)
        if event['type'] == 'gameState':
            print('here')
            moves = event['moves']
            moves = moves.split(" ")
            if len(moves) % 2 == 0: # we just made a move
                pass
            else:
                while True:
                    try:
                        print('trying a move...')
                        # start = random.randint(0, 63)
                        # end = random.randint(0, 63)
                        # print(index_to_alg(start), " to ", index_to_alg(end))
                        start = 'g7'
                        end = 'g5'
                        print(start, 'to', end)
                        client.bots.make_move(ID, start + end)
                        break;
                    except berserk.exceptions.ResponseError:
                        pass

def board_to_FEN(b: list[str]) -> str:
    pass
def main():
    global board, client, session
    board = ['r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
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
    session = berserk.TokenSession(TOKEN)
    client = berserk.Client(session=session)
    start_game_ai(1, 'black')
    input("waiting")
    game_loop()
    # make_move_board(board, 10, 18)
    # print(board)

if __name__ == "__main__":
    main()
