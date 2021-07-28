'''makes the ai able to play games directly on lichess using the api
TOKEN: API_TOKEN'''

#TODO: keep track of board using a board array, turn into FEN to give to ai to make move
#NOTE: could try storing an FEN string instead of board, update that for every move
import berserk

session: berserk.session
client: berserk.Client
board = []
TOKEN = "API_TOKEN"

def startGameAI(level, color):
    client.challenges.create_ai(level=level, color=color)

def makeMove(start: int, end: int):
    start_x = start % 8
    start_y = start // 8
    move_str = 'a' # for the file
    move_str = chr(ord(c) + start_x) # after this we have the letter of the file
    move_str += str(start_y + 1) # add 1 bc not zero indexed
    # now move_str is the starting pos ex. 'e5'
    end_x = end % 8
    end_y = end // 8
    move_str += chr(ord('a') + end_x)
    move_str += str(end_y + 1)
    # promoting adds 'q' for queen? ig adds char representing the piece promoted to

    client.bots.make_move(move_str)

# def FEN_to_board(FEN: str):
#     parts = FEN.split(" ")

def main():
    board = ['r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
             'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
             ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
             ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
             ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
             ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
             'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
             'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R',
             0b1111, # castling
             0b0, # en passant
             0b0] # curr turn
    session = berserk.TokenSession(TOKEN)
    client = berserk.Client(session=session)


if __name__ == "__main__":
    main();