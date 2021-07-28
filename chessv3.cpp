#include <wx/wx.h> // https://docs.wxwidgets.org/3.1.5/annotated.html
#include <wx/graphics.h>
#include <wx/dcgraph.h>
#include <wx/dcbuffer.h>
#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <chrono>
// #include "chessPiecesv2.cpp" // included in chessai
#include "chessAiRewritev2.cpp"
// #include "game.cpp"

#define AI_ENABLED true
#define DEBUG false

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

    // initZobrist();

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

    // game.CheckForChecks();

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

        if (game.board[i] != 0)
        {
            if (!DEBUG)
            {
                wxBitmap bmp;
                // make path
                std::string path = "res/png/"; // im using the pngs on windows, bmp not working
                if (Game::getColor(game.board[i]) == Game::whiteID)
                    path += "w";
                else path += "b";
                switch (Game::getID(game.board[i]))
                {
                case Game::pawnID:
                    path += "p";
                    break;
                case Game::knightID:
                    path += "kn";
                    break;
                case Game::bishopID:
                    path += "b";
                    break;
                case Game::rookID:
                    path += "r";
                    break;
                case Game::queenID:
                    path += "q";
                    break;
                case Game::kingID:
                    path += "k";
                    break;
                }
                path += ".png"; // change to .bmp for bmp

                bmp.LoadFile(path, wxBITMAP_TYPE_PNG);
                gcdc.DrawBitmap(bmp, tlX, tlY);
            }
            else
                gcdc.DrawText(std::to_string(Game::getColor(game.board[i]) * 10 + Game::getID(game.board[i])), tlX + sqWidth / 2, tlY + sqHeight / 2);

            // if (game.board[row][col]->id == "K" && ((King*)game.board[row][col])->inCheck)
            // {
            //     gcdc.SetBrush(*wxTRANSPARENT_BRUSH); // so we can do an outline
            //     gcdc.SetPen(*wxRED_PEN);
            //     gcdc.DrawRectangle(tlX, tlY, sqWidth, sqHeight);
            // }
        }
    }

    // std::cout << fullZobristHash(game.board, *(game.currTurn)) << std::endl;
    if (AI_ENABLED && *(game.currTurn) == Game::blackID) // the ai will play as black
    {
        makeBestMoveAB(game.board, Game::blackID);
        // game.CheckForChecks();
        *(game.currTurn) = Game::whiteID;
    }
    // std::cout << fullZobristHash(game.board, *(game.currTurn)) << std::endl;
    // ok. why is this here?
    if (game.currSelectedPiece != -1) 
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
        for (int m : game.currSelectedPossibleCoords)
        {
            int x = m % 8;
            int y = m / 8;
            if (y >= 0 && y < 8)
            {
                gcdc.SetBrush(*wxLIGHT_GREY_BRUSH);
                gcdc.DrawCircle(x * sqWidth + sqWidth / 2, y * sqHeight + sqHeight / 2, sqWidth / 6);
                // dc.DrawRectangle(x * sqWidth, y * sqHeight, sqWidth, sqHeight);
            }
            else // pawn promotion garbage
            {
                if (y < 0) // white promoting
                {
                    int newY = (y + 1) * -1;
                    wxBitmap bmp;
                    // make path
                    std::string path = "res/png/w"; // im using the pngs on windows, bmp not working
                    // if (Game::getColor(game.board[i]) == Game::whiteID)
                    //     path += "w";
                    // else path += "b";
                    if (y == -1) path += "Q";
                    else if (y == -2) path += "kn";
                    else if (y == -3) path += "b";
                    else if (y == -4) path += "r";
                    path += "p"; // indicates it's a promotion image
                    path += ".png"; // change to .bmp for bmp
                    bmp.LoadFile(path, wxBITMAP_TYPE_PNG);
                    gcdc.DrawBitmap(bmp, x * sqWidth, newY * sqHeight);
                }
                else // black promoting
                {
                    int newY = 7 - ((y - 1) - 7);
                    wxBitmap bmp;
                    // make path
                    std::string path = "res/png/b"; // im using the pngs on windows, bmp not working
                    // if (Game::getColor(game.board[i]) == Game::whiteID)
                    //     path += "w";
                    // else path += "b";
                    if (y == 8) path += "Q";
                    else if (y == 9) path += "kn";
                    else if (y == 10) path += "b";
                    else if (y == 11) path += "r";
                    path += "p"; // indicates it's a promotion image
                    path += ".png"; // change to .bmp for bmp
                    bmp.LoadFile(path, wxBITMAP_TYPE_PNG);
                    gcdc.DrawBitmap(bmp, x * sqWidth, newY * sqHeight);
                }
            }
        }
    }
}

void Canvas::OnMouseClick(wxMouseEvent& evt)
{
    if (!AI_ENABLED || *(game.currTurn) == Game::whiteID)
    {
        if (evt.m_x <= 800 && evt.m_y <= 800)
        {
            int sqX = evt.m_x / 100;
            int sqY = evt.m_y / 100;

            if (Game::getID(game.currSelectedPiece) == Game::pawnID && 
                ((Game::getY(game.currSelectedPiece) == 6 && *(game.currTurn) == Game::blackID) || 
                    (Game::getY(game.currSelectedPiece) == 1 && *(game.currTurn) == Game::whiteID))) // if curr piece is a pawn on its 2nd to last rank
            {   
                int newY;
                if (Game::getY(game.currSelectedPiece) == 1) // white promoting
                    newY = (sqY * -1) - 1;
                else // black promoting
                    newY = 15 - sqY; // it just works
                    
                int newPos = newY * 8 + sqX;
                if (std::find(game.currSelectedPossibleCoords.begin(), game.currSelectedPossibleCoords.end(), newPos) != game.currSelectedPossibleCoords.end())
                {
                    game.movePiece(Game::getX(game.currSelectedPiece), Game::getY(game.currSelectedPiece), sqX, newY, game.board);
                    *(game.currTurn) = !*(game.currTurn);
                    game.currSelectedPiece = -1;
                    game.currSelectedPossibleCoords = {};
                    this->Refresh();
                    return;
                }

            }
            if (game.currSelectedPiece == -1 || (sqX != Game::getX(game.currSelectedPiece) || sqY != Game::getY(game.currSelectedPiece))) // no piece selected
            {   
                if (game.board[sqY * 8 + sqX] != 0 && Game::getColor(game.board[sqY * 8 + sqX]) == *(game.currTurn))
                {
                    game.currSelectedPiece = game.board[sqY * 8 + sqX];
                    game.currSelectedPossibleCoords = game.getMoves(game.board, Game::getX(game.currSelectedPiece), Game::getY(game.currSelectedPiece));
                    this->Refresh();
                }
                else
                {
                    int pos = sqY * 8 + sqX;
                    if (game.currSelectedPossibleCoords.size() != 0 && 
                        std::find(game.currSelectedPossibleCoords.begin(), 
                                  game.currSelectedPossibleCoords.end(), 
                                  pos) != game.currSelectedPossibleCoords.end())
                    {
                        game.movePiece(Game::getX(game.currSelectedPiece), Game::getY(game.currSelectedPiece), sqX, sqY, game.board);
                        *(game.currTurn) = !*(game.currTurn);
                    }
                    // else if (Game::getID(game.currSelectedPiece) == Game::pawnID && 
                    //     ((Game::getY(game.currSelectedPiece) == 6 && *(game.currTurn) == Game::blackID) || 
                    //      (Game::getY(game.currSelectedPiece) == 1 && *(game.currTurn) == Game::whiteID))) // if curr piece is a pawn on its 2nd to last rank
                    // {   
                    //     int newY;
                    //     if (Game::getY(game.currSelectedPiece) == 1) // white promoting
                    //         newY = (sqY * -1) - 1;
                    //     else // black promoting
                    //         newY = 15 - sqY; // it just works
                    //     std::cout << newY << std::endl;
                    //     std::array<int, 2> newCoords = {{sqX, newY}};
                    //     if (std::find(game.currSelectedPossibleCoords.begin(), game.currSelectedPossibleCoords.end(), newCoords) != game.currSelectedPossibleCoords.end())
                    //     {
                    //         std::cout << "g" << std::endl;
                    //         game.movePiece(Game::getX(game.currSelectedPiece), Game::getY(game.currSelectedPiece), sqX, newY, game.board);
                    //         *(game.currTurn) = !*(game.currTurn);
                    //     }
                    // }
                    // else // not one of the possible moves
                    // {
                        game.currSelectedPiece = -1;
                        game.currSelectedPossibleCoords = {};
                        this->Refresh();
                    // }
                }
            }
            else if (game.currSelectedPiece != -1 && sqX == Game::getX(game.currSelectedPiece) && sqY == Game::getY(game.currSelectedPiece))
            {
                game.currSelectedPiece = -1;
                game.currSelectedPossibleCoords = {};
                this->Refresh();
            }
        }
    }  
}