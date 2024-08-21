'''makes the ai able to play games directly on lichess using the api
TOKEN: API_TOKEN'''

# TODO: keep track of board using a board array, turn into FEN to give to ai to make move
# NOTE: could try storing an FEN string instead of board, update that for every move
import berserk
import random
import subprocess

session: berserk.session
client: berserk.Client
board = []
TOKEN = "API_TOKEN"
ID: str
COLOR = 'white'
LEVEL = 6


def start_game_ai(level: int, color: str):
    global ID
    info = client.challenges.create_ai(level=level, color=color)
    ID = info['id']

def make_move_lichess(start: int, end: int):
    move_str = index_to_alg(start)
    move_str += index_to_alg(end)
    # promoting adds 'q' for queen? ig adds char representing the piece promoted to

    done = False
    while not done:
        try:
            client.bots.make_move(ID, move_str)
            done = True
        except berserk.exceptions.ApiError:
            print('got that weird error again')
            print(move_str)

def make_move_board(b: list, start: int, dest: int):
    p: str
    p = b[start]
    if dest >= 0:
        dest_x = dest % 8
        dest_y = dest // 8
    else: # NOTE: this might not work, might need to do something diff for negatives
        dest_x = dest % 8
        dest_y = dest // 8

    x = start % 8
    y = start // 8

    new_en_passant = 0

    if p.lower() == 'p':
        if dest_y >= 1 and dest_y < 7: # non promotion moves
            if abs(y - dest_y) == 2: # double move, need to update en passant
                # print("ENPAS TSANT")
                new_en_passant = dest + (-8 * (dest_y - y > 0)) + (8 * (dest_y - y < 0))
            if abs(x - dest_x) == 1: # capture (either en passant or regular)
                if b[dest] == ' ': # moving to an empty square (en passant)
                    b[y * 8 + dest_x] = ' ' # the pawn en passant-ed

        else: # promotion stuff
            if dest_y == -1: # queen promotion (white)
                b[0 * 8 + dest_x] = 'Q'
                b[start] = ' '
                b[66] = not b[66]
                return # don't need to do anything else like en passant bc this can't be a double move
            elif dest_y == 8: # queen promotion (black)
                b[7 * 8 + dest_x] = 'q'
                b[start] = ' '
                b[66] = not b[66]
                return # don't need to do anything else like en passant bc this can't be a double move
            elif dest_y == -2: # knight (white)
                b[0 * 8 + dest_x] = 'N'
                b[start] = ' '
                b[66] = not b[66]
                return # don't need to do anything else like en passant bc this can't be a double move
            elif dest_y == 9: # knight (black)
                b[7 * 8 + dest_x] = 'n'
                b[start] = ' '
                b[66] = not b[66]
                return # don't need to do anything else like en passant bc this can't be a double move
            elif dest_y == -3: # bishop (white)
                b[0 * 8 + dest_x] = 'B'
                b[start] = ' '
                b[66] = not b[66]
                return # don't need to do anything else like en passant bc this can't be a double move
            elif dest_y == 10: # bishop (black)
                b[7 * 8 + dest_x] = 'b'
                b[start] = ' '
                b[66] = not b[66]
                return # don't need to do anything else like en passant bc this can't be a double move
            elif dest_y == -4: # rook (white)
                b[0 * 8 + dest_x] = 'R'
                b[start] = ' '
                b[66] = not b[66]
                return # don't need to do anything else like en passant bc this can't be a double move
            else: # rook (black)
                b[7 * 8 + dest_x] = 'r'
                b[start] = ' '
                b[66] = not b[66]
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
    b[66] = not b[66]

def index_to_alg(pos: int) -> str:
    '''# converts a board index to algebraic notation (ex. 0 -> a8 (tl corner))'''
    promotion = ""
    if pos >= 0 and pos < 64:
        x = pos % 8
        y = 8 - pos // 8
    # for promotions, lichess adds a char indicating piece q = queen, n = knight, r = rook, b = bishop
    elif pos < 0:  # white promotions are negative
        x = pos % 8
        y = pos // 8
        if y == -1: # queen
            promotion = "q"
        elif y == -2: # knight
            promotion = "n"
        elif y == -3: # bishop
            promotion = "b"
        else: # rook
            promotion = "r"
        y = 8 # white promotions are always to the 8th rank
    else: # black promotions
        x = pos % 8
        y = pos // 8
        if y == 8: # queen
            promotion = "q"
        elif y == 9: # knight
            promotion = "n"
        elif y == 10: # bishop
            promotion = "b"
        else: # rook
            promotion = "r"
        y = 1 # black promotions are always to the 1st rank
    pos_str = chr(ord('a') + x)
    pos_str += str(y) # add 1 bc not zero indexed
    pos_str += promotion # adds "" if no promotion
    return pos_str

def alg_to_index(pos: str) -> int:
    x = pos[0]
    y = pos[1]
    x = ord(x) - ord('a')
    y = 8 - int(y)
    if len(pos) == 3: # promotion
        if y == 0: # white promoting
            if pos[2] == "q":
                y = -1
            elif pos[2] == "n":
                y = -2
            elif pos[2] == "b":
                y = -3
            else:
                y = -4
        else: # black promoting
            if pos[2] == "q":
                y = 8
            elif pos[2] == "n":
                y = 9
            elif pos[2] == "b":
                y = 10
            else:
                y = 11
    index = y * 8 + x
    return index

def game_loop():
    gen = client.bots.stream_game_state(ID)
    for event in gen: # just keeps going...
        # print(event)
        if event['type'] == 'gameState':
            moves = event['moves']
            moves = moves.split(" ")
        elif 'state' in event: # this is the first move, its formatted differently and stuff
            moves = event['state']['moves']
            if moves != '':
                moves = moves.split(" ")
            else:
                moves = []
        # print(moves)
        if event['type'] == 'gameState' or 'state' in event:
            if (COLOR == 'white' and len(moves) % 2 == 1) or (COLOR == 'black' and len(moves) % 2 == 0): # we just made a move
                # prev_move = moves[-1]
                # start = prev_move[:2]
                # end = prev_move[2:]
                # start = alg_to_index(start)
                # end = alg_to_index(end)
                # make_move_board(board, start, end)
                pass
            else:
                # while True:
                try:
                    # print('trying a move...')
                    # # TODO: make the fen string for the current board, send it to the exe
                    # proc = subprocess.run([".\\test.exe"], stdout=subprocess.PIPE) 
                    # output = str(proc.stdout) # this will look like b'.....'
                    # output = output[2, -1] # get rid of the b''
                    # output = tuple(output.split(" ")) # int, int - start, dest

                    # # start = random.randint(0, 63)
                    # # end = random.randint(0, 63)
                    # # print(index_to_alg(start), " to ", index_to_alg(end))
                    # start = index_to_alg(output[0])
                    # end = index_to_alg(output[1])
                    # print(start, 'to', end)

                    # make opponent's move
                    if len(moves) != 0:
                        prev_move = moves[-1]
                        start = prev_move[:2]
                        end = prev_move[2:]
                        start = alg_to_index(start)
                        end = alg_to_index(end)
                        make_move_board(board, start, end)

                    fen_str = board_to_FEN(board)
                    print(fen_str)
                    proc = subprocess.run(["/home/cryptic/Coding/wxChess/bin/ai", fen_str], stdout=subprocess.PIPE, text=True)
                    move = proc.stdout.split(" ")
                    start = int(move[0])
                    end = int(move[1])
                    make_move_lichess(start, end)
                    make_move_board(board, start, end)

                    # client.bots.make_move(ID, start + end)
                    # break
                except berserk.exceptions.ResponseError:
                    pass
        

def board_to_FEN(b: list) -> str:
    '''NOTE: does not return a fully valid FEN string - en passant target does
    not include a rank (uses 0), also omits the halfmove clock and fullmove number
    this string is however compatible with my chess ai'''
    num_blank_squares = 0
    result = ""
    for i in range(64):
        if i % 8 == 0 and i != 0: # every new rank
            num_blank_squares = 0
        if b[i] != ' ': # a piece is there
            if num_blank_squares != 0:
                result += str(num_blank_squares)
                num_blank_squares = 0
            result += b[i]
        else:
            num_blank_squares += 1;
        if i % 8 == 7: # last square
            if num_blank_squares != 0:
                result += str(num_blank_squares)
            if i != 63: # no slash at end
                result += '/'
            num_blank_squares = 0

    result += ' '
    if b[66] == 0:
        result += 'w'
    else:
        result += 'b'

    result += ' '
    castling = b[64]
    if castling & 0b0100: # white king side
        result += 'K'
    if castling & 0b1000: # white queen side
        result += "Q"
    if castling & 0b0001: # black king side
        result += 'k'
    if castling & 0b0010: # black queen side
        result += 'q'
    if castling == 0:
        result += '-'

    result += ' '
    if b[65] != 0:
        en_passant = b[65]
        x = en_passant % 8
        y = en_passant // 8
        en_passant_str = chr(ord('a') + x)
        en_passant_str += str(8 - y)
        result += en_passant_str
    else:
        result += '-'

    return result
'''
    # CPP CODE
    # {
    #     std::string result;
    #     std::string enPassant = "-";
    #     for (int i = 0; i < 8; i++)
    #     {
    #         int numBlankSquares = 0;
    #         for (int j = 0; j < 8; j++)
    #         {
    #             if (board[i][j] != nullptr)
    #             {
    #                 if (board[i][j]->id == "p" && ((Pawn*)(board[i][j]))->canBeEnPassant)
    #                 {
    #                     if (enPassant == "-")
    #                         enPassant = std::to_string(board[i][j]->x) + "," + std::to_string(board[i][j]->y);
    #                     else
    #                         enPassant += std::to_string(board[i][j]->x) + "," + std::to_string(board[i][j]->y);
    #                 }
    #                 if (numBlankSquares != 0)
    #                     result += std::to_string(numBlankSquares);
    #                 numBlankSquares = 0;
    #                 std::string piece = board[i][j]->id;
    #                 if (piece == "k")
    #                     piece = "n"; // knights are n in FEN notation
    #                 else if (piece == "K")
    #                     piece = "k";
    #                 else if (piece == "Q")
    #                     piece = "q";
    #                 if (board[i][j]->color == "w") // white pieces are uppercase
    #                     piece[0] = std::toupper(piece[0]);
    #                 result += piece;
    #             }
    #             else
    #             {
    #                 numBlankSquares++;
    #                 if (j == 7)
    #                     result += std::to_string(numBlankSquares);
    #             }
    #         }
    #         if (i != 7) // no slash at end
    #             result += "/";
    #     }
    #     result += " " + currTurn;

    #     result += " ";
    #     std::string castling = "-";
    #     if (board[7][4] != nullptr && board[7][4]->id == "K" && !((King*)(board[7][4]))->hasMoved)
    #     {
    #         if (board[7][7] != nullptr && board[7][7]->id == "r" && !((Rook*)(board[7][7]))->hasMoved)
    #         {
    #             if (castling == "-") castling = "";
    #             result += "K"; // white can castle kingside
    #         }
    #         if (board[7][0] != nullptr && board[7][0]->id == "r" && !((Rook*)(board[7][0]))->hasMoved)
    #         {
    #             if (castling == "-") castling = "";
    #             result += "Q"; // white can castle queenside
    #         }
    #     }
    #     if (board[0][4] != nullptr && board[0][4]->id == "K" && !((King*)(board[0][4]))->hasMoved)
    #     {
    #         if (board[0][7] != nullptr && board[0][7]->id == "r" && !((Rook*)(board[0][7]))->hasMoved)
    #         {
    #             if (castling == "-") castling = "";
    #             result += "k"; // black can castle kingside
    #         }
    #         if (board[0][0] != nullptr && board[0][0]->id == "r" && !((Rook*)(board[0][0]))->hasMoved)
    #         {
    #             if (castling == "-") castling = "";
    #             result += "q"; // black can castle queenside
    #         }
    #     }
    #     result += castling;

    #     result += " " + enPassant;
    #     return result;
    # }
    '''

def main():
    global board, client, session, ID, COLOR
    board = ['r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
             'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
             ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
             ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
             ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
             ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
             'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
             'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R',
             0b1111,  # castling wwbb qkqk
             0,  # en passant - index of square behind a pawn that made a double move
             0b0]  # curr turn 0 = w 1 = b
    # print(board_to_FEN(board))

    #######################
    # CODE TO PLAY A GAME #
    #######################
    
    session = berserk.TokenSession(TOKEN)
    client = berserk.Client(session=session)
    for event in client.bots.stream_incoming_events():
        print(event)
        if event['type'] == 'challenge' and event['challenge']['variant']['key'] == 'standard':
            client.bots.accept_challenge(event['challenge']['id'])
            color = event['challenge']['color']
            if (color == 'black'):
                COLOR = 'white'
            else:
                COLOR = 'black'
        elif event['type'] == 'gameStart':
            ID = event['game']['id']
            game_loop()
    # start_game_ai(LEVEL, COLOR)
    # input("waiting")
    # game_loop()

    ###########################
    # END IMPORTANT GAME CODE #
    ###########################

    # move = 'g7g8n'
    # start = move[:2]
    # end = move[2:]
    # print(alg_to_index(start))
    # print(alg_to_index(end))
    # make_move_board(board, 10, 18)
    # print(board)
    # move = subprocess.run(["/home/cryptic/Coding/wxChess/bin/chessAiRewritev2", "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -"], stdout=subprocess.PIPE, text=True)
    # print(move.stdout)
    # locs = move.stdout.split(" ")
    # print(index_to_alg(int(locs[0])), index_to_alg(int(locs[1])))
    # print(index_to_alg(11))
    # print(index_to_alg(-6))
    # print(index_to_alg(64))

if __name__ == "__main__":
    main()
