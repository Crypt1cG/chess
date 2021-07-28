#include <wx/wx.h> // https://docs.wxwidgets.org/3.1.5/annotated.html
#include <iostream>
#include <array>
#include <string>
#include <vector>
#include "chessPieces.cpp"

class Game
{
public:
    std::array<std::array<std::string, 8>,8> board;
    int turn; // even for white, odd for black (add 1 each time something moves)
    std::vector<Piece*> pieces;
    Piece* currSelectedPiece;
    std::vector<std::tuple<int, int>> currSelectedPossibleCoords;
    std::string currTurn;

    Game()
    {
        // board representing the game. first char is piece color, second is what piece it is
        // see here for curly braces: https://stackoverflow.com/questions/12844475/why-cant-simple-initialize-with-braces-2d-stdarray
        board = {{  {{"br", "bk", "bb", "bQ", "bK", "bb", "bk", "br"}},
                    {{"bp", "bp", "bp", "bp", "bp", "bp", "bp", "bp"}},
                    {{" ", " ", " ", " ", " ", " ", " ", " "}},
                    {{" ", " ", " ", " ", " ", " ", " ", " "}},
                    {{" ", " ", " ", " ", " ", " ", " ", " "}},
                    {{" ", " ", " ", " ", " ", " ", " ", " "}},
                    {{"wp", "wp", "wp", "wp", "wp", "wp", "wp", "wp"}},
                    {{"wr", "wk", "wb", "wQ", "wK", "wb", "wk", "wr"}} }};

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
        
        // make pieces
        pieces.push_back(new Rook(0, 7, "w"));
        pieces.push_back(new Rook(7, 7, "w"));
        pieces.push_back(new Knight(1, 7, "w"));
        pieces.push_back(new Knight(6, 7, "w"));
        pieces.push_back(new Bishop(2, 7, "w"));
        pieces.push_back(new Bishop(5, 7, "w"));
        pieces.push_back(new Queen(3, 7, "w"));
        pieces.push_back(new King(4, 7, "w"));

        for (int i = 0; i < 8; i++)
        {
            pieces.push_back(new Pawn(i, 6, "w"));
        }

        // black pieces
        pieces.push_back(new Rook(0, 0, "b"));
        pieces.push_back(new Rook(7, 0, "b"));
        pieces.push_back(new Knight(1, 0, "b"));
        pieces.push_back(new Knight(6, 0, "b"));
        pieces.push_back(new Bishop(2, 0, "b"));
        pieces.push_back(new Bishop(5, 0, "b"));
        pieces.push_back(new Queen(3, 0, "b"));
        pieces.push_back(new King(4, 0, "b"));

        for (int i = 0; i < 8; i++)
        {
            pieces.push_back(new Pawn(i, 1, "b"));
        }
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
    void render(wxDC& dc);
    void OnMouseClick(wxMouseEvent& evt);
};

wxIMPLEMENT_APP(App);

bool App::OnInit() 
{
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    wxFrame* mainFrame = new wxFrame(nullptr, wxID_ANY, "Chess", wxPoint(30, 30), wxSize(900, 900));
    Canvas* ChessBoard = new Canvas((wxFrame*) mainFrame);

    ChessBoard->Bind(wxEVT_PAINT, Canvas::paintEvent, ChessBoard); // IMPORTANT (btw last arg is telling it which object to send events to)
    ChessBoard->Bind(wxEVT_LEFT_DOWN, Canvas::OnMouseClick, ChessBoard);
    sizer->Add(ChessBoard, 1, wxEXPAND);

    mainFrame->SetSizer(sizer);
    mainFrame->SetAutoLayout(true);

    wxInitAllImageHandlers(); // does something for loading in images

    mainFrame->Show();

    return true;
}

Canvas::Canvas(wxFrame* parent) : wxPanel(parent)
{
}

void Canvas::paintEvent(wxPaintEvent& evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void Canvas::paintNow()
{
    wxClientDC dc(this);
    render(dc);
}

void Canvas::render(wxDC& dc)
{
    const int BoardWidth = 800;
    const int BoardHeight = 800;
    const int sqWidth = BoardWidth / 8;
    const int sqHeight = BoardHeight / 8;

    // std::cout << this->g->board[0][0] << std::endl;

    wxFontInfo fi(12);
    fi.Family(wxFONTFAMILY_SWISS);
    fi.Style(wxFONTSTYLE_NORMAL);
    fi.Weight(wxFONTWEIGHT_NORMAL);
    wxFont font(fi);

    dc.SetFont(font);
    dc.SetBackgroundMode(wxBRUSHSTYLE_SOLID);
    dc.SetTextForeground(*wxBLACK);
    dc.SetTextBackground(*wxWHITE);

    std::cout << "bruh" << std::endl;

    for (int i = 0; i < 64; i++)
    {
        int row = i / 8;
        int col = i % 8;
        int tlX = col * sqWidth;
        int tlY = row * sqHeight;
        if ((row + col) % 2 == 0) // white square
            dc.SetBrush(*wxWHITE_BRUSH);
        else dc.SetBrush(*wxBLACK_BRUSH); // black square

        dc.DrawRectangle(tlX, tlY, sqWidth, sqHeight);
        // std::cout << *(this->g)->board[0][0] << std::endl;

        wxBitmap bmp;
        bmp.LoadFile("N:\\VSCodeProjects\\wxChess\\bp.bmp", wxBITMAP_TYPE_BMP);
        if (game.board[row][col] != " ")
            dc.DrawBitmap(bmp, tlX, tlY);
            // dc.DrawText(game.board[row][col], tlX + sqWidth / 2, tlY + sqHeight / 2);
    }

    if (game.currSelectedPiece != nullptr)
    {
        std::cout << "drawing stuf?" << std::endl;
        game.currSelectedPossibleCoords = game.currSelectedPiece->GetPossiblePositions(game.board);
        std::cout << game.currSelectedPossibleCoords.size() << std::endl;
        for (std::tuple<int, int> t : game.currSelectedPossibleCoords)
        {
            int x = std::get<0>(t);
            int y = std::get<1>(t);
            std::cout << x << y << std::endl;
            dc.SetBrush(*wxLIGHT_GREY_BRUSH);
            dc.DrawCircle(x * sqWidth + sqWidth / 2, y * sqHeight + sqHeight / 2, sqWidth / 5);
            // dc.DrawRectangle(x * sqWidth, y * sqHeight, sqWidth, sqHeight);
        }
    }
}

void Canvas::OnMouseClick(wxMouseEvent& evt)
{
    std::cout << "Mouse x: " << evt.m_x << " Mouse y: " << evt.m_y << std::endl;
    if (evt.m_x <= 800 && evt.m_y <= 800)
    {
        int sqX = evt.m_x / 100;
        int sqY = evt.m_y / 100;
        std::cout << "Square x: " << sqX << " Square y: " << sqY << std::endl;
        if (game.board[sqY][sqX].substr(0, 1) == game.currTurn)
        {
            for (int i = 0; i < game.pieces.size(); i++)
            {
                if (game.pieces[i]->x == sqX && game.pieces[i]->y == sqY)
                {
                    game.currSelectedPiece = game.pieces[i];
                    this->Refresh();
                }
            }
        }
        else
        {
            if (game.currSelectedPossibleCoords.size() != 0 && 
                std::find(game.currSelectedPossibleCoords.begin(),
                          game.currSelectedPossibleCoords.end(),
                          std::make_tuple(sqX, sqY)) != game.currSelectedPossibleCoords.end())
            {
                // move the piece
                std::string tmp = game.board[game.currSelectedPiece->y][game.currSelectedPiece->x];
                game.board[game.currSelectedPiece->y][game.currSelectedPiece->x] = " ";
                
                
                if (game.board[sqY][sqX] != " ") // piece captured
                {
                    
                    int index;
                    // find the index of the piece where capture is taking place
                    for (int i = 0; i < game.pieces.size(); i++)
                    {
                        if (game.pieces[i]->x == sqX && game.pieces[i]->y == sqY)
                        {
                            index = i;
                            break;
                        }
                    }
                    game.pieces.erase(game.pieces.begin() + index);
                }

                game.currSelectedPiece->x = sqX;
                game.currSelectedPiece->y = sqY;

                game.board[sqY][sqX] = tmp;

                if (dynamic_cast<Pawn*>(game.currSelectedPiece) != nullptr) // it's a pawn, need to get rid of moving 2
                {
                    ((Pawn*)game.currSelectedPiece)->hasMoved = true;
                }

                game.currSelectedPossibleCoords = {};
                game.currSelectedPiece = nullptr;
                if (game.currTurn == "w") game.currTurn = "b";
                else game.currTurn = "w";
                this->Refresh();
            }
            else // not in the possible moves
            {
                game.currSelectedPiece = nullptr;
                game.currSelectedPossibleCoords = {};
                this->Refresh(); // should prob just have one of these later
            }
        }
    }  
}