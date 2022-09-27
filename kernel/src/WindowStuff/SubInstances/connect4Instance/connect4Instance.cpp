#include "connect4Instance.h"
#include "../../../cmdParsing/cstrTools.h"
#include "../../../memory/heap.h"
#include "../../../kernelStuff/stuff/cstr.h"

Connect4Instance::Connect4Instance(Window* window)
{
    this->window = window;
    this->instanceType = InstanceType::Connect4;
    currentMode = CurrentMode::NONE;
    ClearInput();
}


void Connect4Instance::Init()
{
    currentMode = CurrentMode::NONE;
    bgCol = 0xFF000022;
    window->renderer->Clear(bgCol);
    window->renderer->putStr("CONNECT FOUR", (window->size.width-(12*8))/2, 30);
    window->renderer->putStr("________________", (window->size.width-(16*8))/2, 36);
    
    window->renderer->putStr("PRESS SPACE TO START", (window->size.width-(20*8))/2, 80);

    sizeX = 7;
    sizeY = 6;
    board = (uint8_t*)malloc(sizeX * sizeY);
    winState = 0;

    ClearInput();
    ClearBoard();
    currentMode = CurrentMode::START;
}


void Connect4Instance::ClearInput()
{
    inputLen = 0;
    for (int i = 0; i < 257; i++)
        userInput[i] = 0;
}

int Connect4Instance::validateInput()
{
    if (inputLen == 0)
        return -1;
    
    int col = to_int(userInput) - 1;
    if (col < 0 || col >= sizeX)
        return -1;
    
    return col;
}

bool Connect4Instance::DropPiece(uint8_t col, uint8_t piece)
{
    if (col < 0 || col >= sizeX)
        return false;
    if (GetBoard(col, 0) != 0)
        return false;
    for (int y = sizeY - 1; y >= 0; y--)
        if (GetBoard(col, y) == 0)
        {
            SetBoard(col, y, piece);
            return true;
        }
    return true;
}

void Connect4Instance::HandleInput()
{
    CurrentMode mode = currentMode;
    currentMode = CurrentMode::WAIT;
    window->renderer->CursorPosition.x -= inputLen*8;
    for (int i = 0; i < inputLen; i++)
        window->renderer->delChar(window->renderer->CursorPosition.x + i*8, window->renderer->CursorPosition.y, bgCol);

    if (mode == CurrentMode::START)
    {
        if (StrEquals(userInput, " "))
        {
            currentMode = CurrentMode::PLAYER_1_ENTER;
            RedrawBoard();
        }
    }
    else if (mode == CurrentMode::PLAYER_1_ENTER)
    {
        currentMode = CurrentMode::PLAYER_1_ENTER;    
        int col = validateInput();
        if (col != -1)
            if (DropPiece(col, 1))
            {
                currentMode = CurrentMode::PLAYER_2_ENTER;
                RedrawBoard();  
            }
        
    }
    else if (mode == CurrentMode::PLAYER_2_ENTER)
    {
        currentMode = CurrentMode::PLAYER_2_ENTER;    
        int col = validateInput();
        if (col != -1)
            if (DropPiece(col, 2))
            {
                currentMode = CurrentMode::PLAYER_1_ENTER;
                RedrawBoard();  
            }
    }

    CheckWin();
    if (winState != 0)
    {
        RedrawBoard();
    }
    ClearInput();
}

void Connect4Instance::RedrawBoard()
{
    window->renderer->Clear(bgCol);
    window->renderer->CursorPosition = {0,0};
    window->renderer->color = Colors.white;
    window->renderer->Println("Board:");

    DrawBoard();

    window->renderer->color = Colors.yellow;

    if (currentMode == CurrentMode::PLAYER_1_ENTER || currentMode == CurrentMode::PLAYER_2_ENTER)
    {
        if (currentMode == CurrentMode::PLAYER_1_ENTER)
            window->renderer->Println("Enter Column, Player 1.");
        else// if (currentMode == CurrentMode::PLAYER_2_ENTER)
            window->renderer->Println("Enter Column, Player 2.");
        window->renderer->color = Colors.white;
        window->renderer->Print("> ");
    }
    else
    {
        if (currentMode == CurrentMode::END)
        {
            if (winState == 0 || winState == 3)
            {
                window->renderer->color = Colors.gray;
                window->renderer->Println("It's a draw.");
            }
            else
            {
                if (winState == 1)
                {
                    window->renderer->color = playerCols[1];
                    window->renderer->Println("Player 1 won!");
                }
                else// if (currentMode == CurrentMode::PLAYER_2_ENTER)
                {
                    window->renderer->color = playerCols[2];
                    window->renderer->Println("Player 2 won!");
                }
            }
        }
    }

}



void Connect4Instance::ClearBoard()
{
    for (int i = 0; i < sizeX * sizeY; i++)
        board[i] = 0;
}

void Connect4Instance::DrawBoard()
{
    window->renderer->CursorPosition.y = 16*2;
    int startX = 8;

    for (int y = 0; y < sizeY; y++)
    {
        window->renderer->CursorPosition.x = startX;
        for (int x = 0; x < sizeX; x++)
        {
            uint8_t thing = board[x + y * sizeX];
            
            if (thing >= 0 && thing < 3)
            {
                window->renderer->color = playerCols[thing];
                window->renderer->Print(playerChars[thing]);
            }
            else
            {
                window->renderer->color = Colors.yellow;
                window->renderer->Print("?");
            }

            window->renderer->Print(" ");
        }    
        window->renderer->Println();
    }
    window->renderer->CursorPosition.y += 16;
    window->renderer->CursorPosition.x = 0;
    window->renderer->color = Colors.white;
}

void Connect4Instance::SetBoard(uint8_t x, uint8_t y, uint8_t val)
{
    if (x < 0 || x >= sizeX)
        return;
    if (y < 0 || y >= sizeY)
        return;
    
    board[x + y * sizeX] = val;
}

uint8_t Connect4Instance::GetBoard(uint8_t x, uint8_t y)
{
    if (x < 0 || x >= sizeX)
        return 0;
    if (y < 0 || y >= sizeY)
        return 0;
    
    return board[x + y * sizeX];
}

void Connect4Instance::CheckWin()
{
    for (int i = 1; i < 3 && winState == 0; i++)
        CheckWin(i);

    if (winState != 0)
        return;

    for (int i = 0; i < sizeX; i++)
        if (board[i] == 0)
            return;
    
    winState = 3;
    currentMode = CurrentMode::END;
}

void Connect4Instance::CheckWin(uint8_t piece)
{
    // Vertical
    for (int mainX = 0; mainX < sizeX; mainX++)
        for (int mainY = 0; mainY < sizeY - 3; mainY++)
            if (CheckSubWin(mainX, mainY, piece, 0, 1, 4))
                return;

    // Horizonatl
    for (int mainX = 0; mainX < sizeX - 3; mainX++)
        for (int mainY = 0; mainY < sizeY; mainY++)
            if (CheckSubWin(mainX, mainY, piece, 1, 0, 4))
                return;

    // Diag 1
    for (int mainX = 0; mainX < sizeX - 3; mainX++)
        for (int mainY = 0; mainY < sizeY - 3; mainY++)
            if (CheckSubWin(mainX, mainY, piece, 1, 1, 4))
                return;

    // Diag 2
    for (int mainX = 0; mainX < sizeX - 3; mainX++)
        for (int mainY = 0; mainY < sizeY - 3; mainY++)
            if (CheckSubWin(((sizeX - 1) - mainX), mainY, piece, -1, 1, 4))
                return;
}

bool Connect4Instance::CheckSubWin(int x, int y, uint8_t piece, int xd, int yd, int count)
{
    for (int i = 0; i < count; i++)
        if (GetBoard(x + i*xd, y + i*yd) != piece)
            return false;
    winState = piece;
    currentMode = CurrentMode::END;
    return true;
}

