#include <wx/wx.h> // https://docs.wxwidgets.org/3.1.5/annotated.html
#include <wx/graphics.h>
#include <wx/dcgraph.h>
#include <wx/dcbuffer.h>
#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <chrono>
// #include "chessPiecesv2.cpp" // included n chessai
#include "chessAi.cpp"

#define AI_ENABLED true
#define DEBUG false

class Game
{
public:
    std::array<std::array<Piece*, 8>,8> board;
    int turn; // even for white, odd for black (add 1 each time something moves)
    Piece* currSelectedPiece;
    std::vector<std::tuple<int, int>> currSelectedPossibleCoords;
    std::string currTurn;
    std::vector<Piece*> pawnsToUpdate; // crusty thing to make en passant work (only have 1 move to do it)

    Game()
    {
        // board representing the game. first char is piece color, second is what piece it is
        // see here for curly braces: https://stackoverflow.com/questions/12844475/why-cant-simple-initialize-with-braces-2d-stdarray
        //TODO: automate?
        board = {{  {{new Rook(0, 0, "b"), new Knight(1, 0, "b"), new Bishop(2, 0, "b"), new Queen(3, 0, "b"), new King(4, 0, "b"), new Bishop(5, 0, "b"), new Knight(6, 0, "b"), new Rook(7, 0, "b")}},
                    {{new Pawn(0, 1, "b"), new Pawn(1, 1, "b"), new Pawn(2, 1, "b"), new Pawn(3, 1, "b"), new Pawn(4, 1, "b"), new Pawn(5, 1, "b"), new Pawn(6, 1, "b"), new Pawn(7, 1, "b")}},
                    {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
                    {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
                    {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
                    {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
                    {{new Pawn(0, 6, "w"), new Pawn(1, 6, "w"), new Pawn(2, 6, "w"), new Pawn(3, 6, "w"), new Pawn(4, 6, "w"), new Pawn(5, 6, "w"), new Pawn(6, 6, "w"), new Pawn(7, 6, "w")}},
                    {{new Rook(0, 7, "w"), new Knight(1, 7, "w"), new Bishop(2, 7, "w"), new Queen(3, 7, "w"), new King(4, 7, "w"), new Bishop(5, 7, "w"), new Knight(6, 7, "w"), new Rook(7, 7, "w")}} }};

        // board = {{  {{new Rook(0, 0, "b"), nullptr, nullptr, new Knight(3, 0, "b"), nullptr, nullptr, nullptr, nullptr}},
        //             {{new Pawn(0, 1, "b"), nullptr, new Rook(2, 1, "w"), nullptr, nullptr, nullptr, nullptr, nullptr}},
        //             {{nullptr, new Pawn(1, 2, "b"), nullptr, new Pawn(3, 2, "w"), nullptr, new King(5, 2, "b"), nullptr, new Pawn(7, 2, "b")}},
        //             {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
        //             {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
        //             {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
        //             {{new Pawn(0, 6, "w"), new Pawn(1, 6, "w"), nullptr, nullptr, nullptr, new Pawn(5, 6, "w"), new Pawn(6, 6, "w"), new Pawn(7, 6, "w")}},
        //             {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, new King(6, 7, "w"), nullptr}} }};

        // this caused a crash before
        // board = {{  {{new King(0, 0, "b"), nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
        //             {{new Pawn(0, 1, "b"), new Rook(1, 1, "b"), nullptr, nullptr, nullptr, new Pawn(5, 1, "b"), nullptr, new Pawn(7, 1, "b")}},
        //             {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
        //             {{nullptr, nullptr, nullptr, nullptr, new King(4, 3, "w"), new Knight(5,3, "b"), nullptr, new Pawn(7, 3, "w")}},
        //             {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
        //             {{nullptr, nullptr, new Pawn(2, 5, "b"), nullptr, nullptr, new Rook(5, 5, "b"), new Pawn(6, 5, "w"), nullptr}},
        //             {{new Pawn(0, 6, "w"), nullptr, nullptr, nullptr, nullptr, new Queen(5, 6, "b"), nullptr, nullptr}},
        //             {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}} }};
        // ((King*)(board[0][0]))->hasMoved = true;
        // ((King*)(board[3][4]))->hasMoved = true;

        // ((Rook*)(board[1][1]))->hasMoved = true;
        // ((Rook*)(board[5][5]))->hasMoved = true;


        // board = {{  {{new Rook(0, 0, "b"), new Knight(1, 0, "b"), new Bishop(2, 0, "b"), nullptr, new King(4, 0, "b"), nullptr, nullptr, new Rook(7, 0, "b")}},
        //         {{new Pawn(0, 1, "b"), new Pawn(1, 1, "b"), nullptr, nullptr, new Pawn(4, 1, "b"), nullptr, nullptr, new Pawn(7, 1, "b")}},
        //         {{nullptr, nullptr, new Pawn(2, 2, "b"), nullptr, nullptr, nullptr, new Pawn(6, 2, "b"), new Knight(7, 2, "b")}},
        //         {{nullptr, nullptr, nullptr, nullptr, new Pawn(4, 3, "w"), nullptr, nullptr, nullptr}},
        //         {{nullptr, nullptr, new Bishop(2, 4, "w"), nullptr, new Pawn(4, 4, "w"), nullptr, nullptr, nullptr}},
        //         {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
        //         {{new Pawn(0, 6, "w"), new Pawn(1, 6, "w"), new Pawn(2, 6, "w"), new Knight(3, 6, "w"), nullptr, new Pawn(5, 6, "w"),  new Pawn(6, 6, "w"),  new Pawn(7, 6, "w")}},
        //         {{new Rook(0, 7, "w"), nullptr, nullptr, nullptr, new King(4, 7, "w"), nullptr, nullptr, new Rook(7, 7, "w")}} }};

        // for testing
        // board = {{  {{" ", " ", " ", " ", " ", " ", " ", " "}},
        //             {{" ", " ", "bQ", " ", " ", " ", " ", " "}},
        //             {{" ", " ", " ", " ", " ", "wb", " ", " "}},
        //             {{" ", " ", " ", "wr", " ", " ", " ", " "}},
        //             {{" ", "wp", " ", " ", " ", " ", " ", " "}},
        //             {{" ", " ", " ", " ", "wk", " ", "wK", " "}},
        //             {{" ", " ", "wQ", " ", " ", " ", " ", " "}},
        //             {{" ", " ", " ", " ", " ", " ", " ", " "}} }};

        currTurn = "w";
        
    }

    void CheckForChecks()
    {
        std::vector<std::tuple<int, int>> allMoves;
        int kingX;
        int kingY;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (this->board[i][j] != nullptr)
                {
                    if (this->board[i][j]->color != this->currTurn)
                    {
                        std::vector<std::tuple<int, int>> moves = board[i][j]->GetPossiblePositions(this->board, true);
                        allMoves.insert(allMoves.end(), moves.begin(), moves.end());
                    }
                    else if (this->board[i][j]->id == "K") // this team's king
                    {
                        kingX = j;
                        kingY = i;
                    }
                }
            }
        }
        for (std::tuple<int, int> t : allMoves)
        {
            if (std::get<0>(t) == kingX && std::get<1>(t) == kingY)
            {
                ((King*)(this->board[kingY][kingX]))->inCheck = true;
                return;
            }
        }
        ((King*)(this->board[kingY][kingX]))->inCheck = false;
    }
    
    bool CheckForChecks(std::array<std::array<Piece*, 8>,8> b, std::string color)
    {
        std::vector<std::tuple<int, int>> allMoves;
        int kingX;
        int kingY;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (b[i][j] != nullptr)
                {
                    if (b[i][j]->color != color)
                    {
                        std::vector<std::tuple<int, int>> moves = b[i][j]->GetPossiblePositions(b, true);
                        allMoves.insert(allMoves.end(), moves.begin(), moves.end());
                    }
                    else if (b[i][j]->id == "K") // this team's king
                    {
                        kingX = j;
                        kingY = i;
                    }
                }
            }
        }
        for (std::tuple<int, int> t : allMoves)
        {
            if (std::get<0>(t) == kingX && std::get<1>(t) == kingY)
                return true;
        }
        return false;
    }
};

Game game;

class App : public wxApp
{
public:
    virtual bool OnInit();
};

class Canvas : public wxPanel
{
public:
    Canvas(wxFrame* parent);

    void paintEvent(wxPaintEvent& evt);
    void paintNow();
    void render(wxGCDC& gcdc);
    void OnMouseClick(wxMouseEvent& evt);
    void OnErase(wxEraseEvent& evt);
};

wxIMPLEMENT_APP(App);

bool App::OnInit() 
{
    wxInitAllImageHandlers(); // does something for loading in images

    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    wxFrame* mainFrame = new wxFrame(nullptr, wxID_ANY, "Chess", wxPoint(30, 30), wxSize(900, 900));
    Canvas* ChessBoard = new Canvas((wxFrame*) mainFrame);

    // hmmm for these 2 lines i need the & on linux but not on windows "invalid use of non-static member function" on linux w/o it
    ChessBoard->Bind(wxEVT_PAINT, &Canvas::paintEvent, ChessBoard); // IMPORTANT (btw last arg is telling it which object to send events to)
    ChessBoard->Bind(wxEVT_LEFT_DOWN, &Canvas::OnMouseClick, ChessBoard);
    ChessBoard->Bind(wxEVT_ERASE_BACKGROUND, &Canvas::OnErase, ChessBoard);
    this->Bind(wxEVT_ERASE_BACKGROUND, &Canvas::OnErase, ChessBoard); // i dont think this does anything
    mainFrame->Bind(wxEVT_ERASE_BACKGROUND, &Canvas::OnErase, ChessBoard); // nor does this?

    sizer->Add(ChessBoard, 1, wxEXPAND);

    mainFrame->SetSizer(sizer);
    mainFrame->SetAutoLayout(true);

    initZobrist();

    mainFrame->Show();

    return true;
}

Canvas::Canvas(wxFrame* parent) : wxPanel(parent)
{
    // fix flickering
    wxWindow::SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void Canvas::OnErase(wxEraseEvent& evt)
{
    // stops flickering on windows
}

void Canvas::paintEvent(wxPaintEvent& evt)
{
    wxAutoBufferedPaintDC dc(this);
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    wxGCDC gcdc(gc);

    // wxFontInfo fi(12);
    // fi.Family(wxFONTFAMILY_SWISS);
    // fi.Style(wxFONTSTYLE_NORMAL);
    // fi.Weight(wxFONTWEIGHT_NORMAL);
    // wxFont font(fi);

    // gcdc.SetFont(font);
    render(gcdc);
}

void Canvas::paintNow()
{
    // wxClientDC dc(this);
    // wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    // wxGCDC gcdc(gc);
    // // wxBufferedDC dc(this);
    // render(gcdc);
}

void Canvas::render(wxGCDC& gcdc)
{
    const int BoardWidth = 800;
    const int BoardHeight = 800;
    const int sqWidth = BoardWidth / 8;
    const int sqHeight = BoardHeight / 8;
    
    if (DEBUG) // debugger doesn't work with images apparently, have to use text
    {
        wxFontInfo fi(12);
        fi.Family(wxFONTFAMILY_SWISS);
        fi.Style(wxFONTSTYLE_NORMAL);
        fi.Weight(wxFONTWEIGHT_NORMAL);
        wxFont font(fi);

        gcdc.SetFont(font);
        gcdc.SetBackgroundMode(wxBRUSHSTYLE_SOLID);
        gcdc.SetTextForeground(*wxBLACK);
        gcdc.SetTextBackground(*wxWHITE);
    }

    game.CheckForChecks();

    for (int i = 0; i < 64; i++)
    {
        int row = i / 8;
        int col = i % 8;
        int tlX = col * sqWidth;
        int tlY = row * sqHeight;
        gcdc.SetPen(*wxBLACK_PEN);

        if ((row + col) % 2 == 0) // white square
            gcdc.SetBrush(*wxWHITE_BRUSH);
        else gcdc.SetBrush(wxBrush(wxColor(94, 145, 70))); // black square

        gcdc.DrawRectangle(tlX, tlY, sqWidth, sqHeight);
        // std::cout << *(this->g)->board[0][0] << std::endl;

        if (game.board[row][col] != nullptr)
        {
            if (!DEBUG)
            {
                wxBitmap bmp;
                // make path
                std::string path = "res/png/"; // im using the pngs on windows, bmp not working
                path += game.board[row][col]->color;
                path += game.board[row][col]->id;
                if (path.substr(path.length() - 1, 1) == "k") // checks if last char is k - it is a knight, file name is _kn.bmp (_K.bmp is king)
                    path += "n";
                path += ".png"; // change to .bmp for bmp

                bmp.LoadFile(path, wxBITMAP_TYPE_PNG);
                gcdc.DrawBitmap(bmp, tlX, tlY);
            }
            else
                gcdc.DrawText(game.board[row][col]->color + game.board[row][col]->id, tlX + sqWidth / 2, tlY + sqHeight / 2);

            if (game.board[row][col]->id == "K" && ((King*)game.board[row][col])->inCheck)
            {
                gcdc.SetBrush(*wxTRANSPARENT_BRUSH); // so we can do an outline
                gcdc.SetPen(*wxRED_PEN);
                gcdc.DrawRectangle(tlX, tlY, sqWidth, sqHeight);
            }
        }
    }

    if (AI_ENABLED && game.currTurn == "b") // the ai will play as black
    {
        ChessAi ai("b");
        auto t1 = std::chrono::high_resolution_clock::now();
        ai.MakeBestMove(game.board);
        auto t2 = std::chrono::high_resolution_clock::now();
        auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
        std::cout << "that move took " << ms_int.count() << " milliseconds" << std::endl;
        game.CheckForChecks();
        game.currTurn = "w";
    }

    // ok. why is this here?
    if (game.currSelectedPiece != nullptr) 
    {
        // int kingX;
        // int kingY;
        // for (int i = 0; i < 8; i++)
        // {
        //     for (int j = 0; j < 8; j++)
        //     {
        //         if (game.board[i][j] != nullptr)
        //         {
        //             if (game.board[i][j]->id == "K" && game.board[i][j]->color == game.currTurn)
        //             {
        //                 kingX = j;
        //                 kingY = i;
        //             }
        //         }
        //     }
        // }

        // std::vector<std::tuple<int, int>> initialMoves = game.currSelectedPiece->GetPossiblePositions(game.board);
        // std::vector<std::tuple<int, int>> finalMoves;
        // for (std::tuple<int, int> t : initialMoves)
        // {
        //     std::array<std::array<Piece*, 8>,8> newBoard = game.board;
        //     newBoard[game.currSelectedPiece->y][game.currSelectedPiece->x] = nullptr;
        //     newBoard[std::get<1>(t)][std::get<0>(t)] = game.currSelectedPiece;
        //     if (!game.CheckForChecks(newBoard, game.currTurn))
        //     {
        //         finalMoves.push_back(std::make_tuple(std::get<0>(t), std::get<1>(t)));
        //     }
        // }
        game.currSelectedPossibleCoords = game.currSelectedPiece->GetPossiblePositions(game.board);
        
        for (std::tuple<int, int> t : game.currSelectedPossibleCoords)
        {
            int x = std::get<0>(t);
            int y = std::get<1>(t);
            gcdc.SetBrush(*wxLIGHT_GREY_BRUSH);
            gcdc.DrawCircle(x * sqWidth + sqWidth / 2, y * sqHeight + sqHeight / 2, sqWidth / 6);
            // dc.DrawRectangle(x * sqWidth, y * sqHeight, sqWidth, sqHeight);
        }
    }
}

void Canvas::OnMouseClick(wxMouseEvent& evt)
{
    if (!AI_ENABLED || game.currTurn == "w")
    {
        if (evt.m_x <= 800 && evt.m_y <= 800)
        {
            int sqX = evt.m_x / 100;
            int sqY = evt.m_y / 100;
            if (game.currSelectedPiece == nullptr || (sqX != game.currSelectedPiece->x || sqY != game.currSelectedPiece->y))
            {
                if (game.board[sqY][sqX] != nullptr && game.board[sqY][sqX]->color == game.currTurn) // if nullptr should short circuit
                {
                    game.currSelectedPiece = game.board[sqY][sqX];
                    this->Refresh();
                }
                else
                {
                    if (game.currSelectedPossibleCoords.size() != 0 && 
                        std::find(game.currSelectedPossibleCoords.begin(),
                                game.currSelectedPossibleCoords.end(),
                                std::make_tuple(sqX, sqY)) != game.currSelectedPossibleCoords.end())
                    {
                        // move the piece NOTE: i think no memory is leaked because we retain a pointer to the data
                        Piece* tmp = game.board[game.currSelectedPiece->y][game.currSelectedPiece->x];
                        // delete game.board[game.currSelectedPiece->y][game.currSelectedPiece->x];
                        game.board[game.currSelectedPiece->y][game.currSelectedPiece->x] = nullptr;
                        
                        // if (game.board[sqY][sqX] != nullptr) // piece captured
                        // {
                        //     int index;
                        //     // find the index of the piece where capture is taking place
                        //     for (int i = 0; i < game.pieces.size(); i++)
                        //     {
                        //         if (game.pieces[i]->x == sqX && game.pieces[i]->y == sqY)
                        //         {
                        //             index = i;
                        //             break;
                        //         }
                        //     }
                        //     game.pieces.erase(game.pieces.begin() + index);
                        // }

                        for (Piece* p : game.pawnsToUpdate)
                        {
                            ((Pawn*)p)->canBeEnPassant = false;
                        }
                        game.pawnsToUpdate.clear();

                        if (game.currSelectedPiece->id == "p") // it's a pawn, need to get rid of moving 2
                        {
                            if (abs(game.currSelectedPiece->y - sqY) == 2) // double move
                            {
                                ((Pawn*)game.currSelectedPiece)->canBeEnPassant = true;
                                game.pawnsToUpdate.push_back(game.currSelectedPiece);
                            }
                            if (abs(game.currSelectedPiece->x - sqX) == 1) // it moved horizontally - either regular capture or en passant
                            {
                                if (game.board[sqY][sqX] == nullptr) // its moving into an empty square (not a regular capture)
                                {
                                    delete game.board[game.currSelectedPiece->y][sqX];
                                    game.board[game.currSelectedPiece->y][sqX] = nullptr; // the pawn en passant -ed
                                }
                            }
                        }
                        if (game.currSelectedPiece->id == "K") // it's a king, need to get rid of castling
                        {
                            if (abs(game.currSelectedPiece->x - sqX) == 2) // castling - the king moved 2 squares
                            {
                                // move the rook
                                if (sqX == 6) // castled king side
                                {
                                    Piece* tmpR = ((Rook*)(game.board[sqY][7]))->Clone();
                                    delete (Rook*)(game.board[sqY][7]); // delete old rook
                                    game.board[sqY][7] = nullptr;
                                    game.board[sqY][5] = tmpR;
                                    tmpR->x = 5;
                                    ((Rook*)tmpR)->hasMoved = true;
                                }
                                else if (sqX == 2)
                                {
                                    Piece* tmpR = ((Rook*)(game.board[sqY][0]))->Clone();
                                    delete (Rook*)(game.board[sqY][0]); // delete old rook
                                    game.board[sqY][0] = nullptr;
                                    game.board[sqY][3] = tmpR;
                                    tmpR->x = 3;
                                    ((Rook*)tmpR)->hasMoved = true;
                                }
                            }
                            ((King*)game.currSelectedPiece)->hasMoved = true;
                        }
                        if (game.currSelectedPiece->id == "r") // it's a rook, need to get rid of castling
                            ((Rook*)game.currSelectedPiece)->hasMoved = true;

                        game.currSelectedPiece->x = sqX;
                        game.currSelectedPiece->y = sqY;

                        if (game.board[sqY][sqX] != nullptr)  // if something is being captured, need to delete the captured piece otherwise the pointer is lost and memory is leaked
                            delete game.board[sqY][sqX];

                        game.board[sqY][sqX] = tmp;

                        game.currSelectedPossibleCoords = {};
                        // delete game.currSelectedPiece;
                        game.currSelectedPiece = nullptr;

                        //check if the move blocked a check 
                        game.CheckForChecks();
                        if (game.currTurn == "w") game.currTurn = "b";
                        else game.currTurn = "w";
                        this->Refresh();
                    }
                    else // not in the possible moves
                    {
                        // delete game.currSelectedPiece;
                        game.currSelectedPiece = nullptr;
                        game.currSelectedPossibleCoords = {};
                        this->Refresh(); // should prob just have one of these later
                    }
                }
            }
            else if (game.currSelectedPiece != nullptr && sqX == game.currSelectedPiece->x && sqY == game.currSelectedPiece->y)
            {
                game.currSelectedPiece = nullptr;
                this->Refresh();
            }
        }
    }  
}