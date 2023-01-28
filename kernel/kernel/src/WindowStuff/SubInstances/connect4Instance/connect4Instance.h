#pragma once
#include "../../../OSDATA/userdata.h"
#include "../defaultInstance/defaultInstance.h"
//#include "../../../customClasses/list.h"
#include "../../Window/window.h"



class Connect4Instance : public DefaultInstance
{
public:
    enum CurrentMode
    {
        NONE,
        WAIT,
        START,
        PLAYER_1_ENTER,
        PLAYER_2_ENTER,
        END
    };

    private:
        Window* window;
        uint8_t sizeX;
        uint8_t sizeY;
        uint8_t* board;
        int winState = 0; // 0 - none, 1 - player1, 2 - player2, 3 - draw

        void SetBoard(uint8_t x, uint8_t y, uint8_t val);
        int validateInput();
        bool DropPiece(uint8_t col, uint8_t piece);
        uint8_t GetBoard(uint8_t x, uint8_t y);
        char playerChars[3] = {'_', 'X', 'O'};
        uint32_t playerCols[3] = {Colors.gray, Colors.bred, Colors.bgreen};
        void CheckWin();
        void CheckWin(uint8_t piece);
        bool CheckSubWin(int x, int y, uint8_t piece, int xd, int yd, int count);


    public:
        CurrentMode currentMode;
        char userInput[260];
        int inputLen = 0;
        uint32_t bgCol;

    public:
    Connect4Instance(Window* window);
    void Init();
    void HandleInput();
    void ClearInput();
    void ClearBoard();
    void DrawBoard();
    void RedrawBoard();
    void Free();

};