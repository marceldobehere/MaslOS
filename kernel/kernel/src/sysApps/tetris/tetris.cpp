#include "tetris.h"
#include "../../cmdParsing/cstrTools.h"
#include "../../memory/heap.h"
#include "../../OSDATA/osdata.h"
#include "../../fsStuff/fsStuff.h"
#include "../../rnd/rnd.h"

namespace SysApps
{
    BasicRenderer* lmaoRenderer = NULL;

    Tetris::Tetris()
    {
        path = StrCopy("bruh:");
        lastSleep = 0;

        //window =

        window = (Window*)_Malloc(sizeof(Window), "Explorer Window");
        //GuiInstance* gui = (GuiInstance*)_Malloc(sizeof(GuiInstance), "Explorer GUI");
        //*gui = GuiInstance(window);
        *(window) = Window(NULL, Size(200, 400), Position(100, 100), "Tetris", true, true, true);
        osData.windows.insertAt(window, 0);
        window->hidden = true;
        //gui->Init();

        window->OnCloseHelp = (void*)this;
        window->OnClose = (void(*)(void*, Window*))(void*)&OnExternalWindowClose;

        window->OnResizeHelp = (void*)this;
        window->OnResize = (void(*)(void*, Window*))(void*)&OnExternalWindowResize;

        window->RenderWhenHidden = true;
        window->OnPartRenderHelp = (void*)this;
        window->OnPartRender =  (void(*)(void*, Window*))(void*)&OnExternalWindowRender;

        window->oldHidden = true;
        window->hidden = false;

        osData.windowsToGetActive.add(window);

        lmaoRenderer = window->renderer;
        lmaoRenderer->Clear(Colors.black);




        tetris_run_init(10, 20);

    }




    void Tetris::OnExternalWindowClose(Window* window)
    {
        //GlobalRenderer->Clear(Colors.purple);
        Free();
    }
    void Tetris::OnExternalWindowResize(Window* window)
    {
        lmaoRenderer = window->renderer;
        lmaoRenderer->Clear(Colors.black);
    }
    void Tetris::OnExternalWindowRender(Window* window)
    {
        uint64_t now = PIT::TimeSinceBootMS();
        if (lastSleep + 50 > now)
            return;
        lastSleep = now;
        tetris_run_loop();

    }

    void Tetris::Free()
    {
        tetris_run_end();
        _Free(path);
        _Free(this);
    }

    void Tetris::Reload()
    {
        AddToStack();


    }
    


    #include "tetris.h"

    struct tetris_level 
    {
        int score;
        int nsec;
    };

    struct tetris_block 
    {
        char data[5][5];
        int w;
        int h;
    };

    struct tetris 
    {
        char **game;
        int w;
        int h;
        int level;
        int gameover;
        int score;
        tetris_block current;
        int x;
        int y;
    };

    tetris_block blocks[] =
    {
        {{"##", 
            "##"},
        2, 2
        },
        {{" X ",
            "XXX"},
        3, 2
        },
        {{"@@@@"},
            4, 1},
        {{"OO",
            "O ",
            "O "},
        2, 3},
        {{"&&",
            " &",
            " &"},
        2, 3},
        {{"ZZ ",
            " ZZ"},
        3, 2}
    };

    struct tetris_level levels[]=
    {
        {0,
            1200000},
        {1500,
            900000},
        {8000,
            700000},
        {20000,
            500000},
        {40000,
            400000},
        {75000,
            300000},
        {100000,
            200000}
    };

    #define TETRIS_PIECES (sizeof(blocks)/sizeof(struct tetris_block))
    #define TETRIS_LEVELS (sizeof(levels)/sizeof(struct tetris_level))

    //struct termios save;

    void
    tetris_cleanup_io() {
        //tcsetattr(fileno(stdin),TCSANOW,&save);
    }

    void
    tetris_signal_quit(int s) {
        tetris_cleanup_io();
    }

    void
    tetris_set_ioconfig() {
        //termios custom;
        //int fd=fileno(stdin);
        //tcgetattr(fd, &save);
        //custom=save;
        //custom.c_lflag &= ~(ICANON|ECHO);
        //tcsetattr(fd,TCSANOW,&custom);
        //fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0)|O_NONBLOCK);
    }

    void
    tetris_init(struct tetris *t,int w,int h) {
        int x, y;
        t->level = 1;
        t->score = 0;
        t->gameover = 0;
        t->w = w;
        t->h = h;
        t->game = (char**)_Malloc(sizeof(char *)*w);
        for (x=0; x<w; x++) {
            t->game[x] = (char*)_Malloc(sizeof(char)*h);
            for (y=0; y<h; y++)
                t->game[x][y] = ' ';
        }
    }

    void
    tetris_clean(struct tetris *t) {
        int x;
        for (x=0; x<t->w; x++) {
            _Free(t->game[x]);
        }
        _Free(t->game);
    }

    void funkyPrint(char chr)
    {
        for (int i = 0; i < 2; i++)
        {
            if (chr == '#')
            {
                lmaoRenderer->putChar(' ', 
                lmaoRenderer->CursorPosition.x, 
                lmaoRenderer->CursorPosition.y, 
                Colors.black, Colors.red);
                lmaoRenderer->CursorPosition.x += 8;
            }
            else if (chr == 'X') // @ O Z
            {
                lmaoRenderer->putChar(' ', 
                lmaoRenderer->CursorPosition.x, 
                lmaoRenderer->CursorPosition.y, 
                Colors.black, Colors.blue);
                lmaoRenderer->CursorPosition.x += 8;
            }
            else if (chr == '@') // @ O Z
            {
                lmaoRenderer->putChar(' ', 
                lmaoRenderer->CursorPosition.x, 
                lmaoRenderer->CursorPosition.y, 
                Colors.black, Colors.green);
                lmaoRenderer->CursorPosition.x += 8;
            }
            else if (chr == 'O') // @ O Z
            {
                lmaoRenderer->putChar(' ', 
                lmaoRenderer->CursorPosition.x, 
                lmaoRenderer->CursorPosition.y, 
                Colors.black, Colors.yellow);
                lmaoRenderer->CursorPosition.x += 8;
            }
            else if (chr == 'Z') // @ O Z
            {
                lmaoRenderer->putChar(' ', 
                lmaoRenderer->CursorPosition.x, 
                lmaoRenderer->CursorPosition.y, 
                Colors.black, Colors.purple);
                lmaoRenderer->CursorPosition.x += 8;
            }
            else if (chr == '&') // @ O Z
            {
                lmaoRenderer->putChar(' ', 
                lmaoRenderer->CursorPosition.x, 
                lmaoRenderer->CursorPosition.y, 
                Colors.black, Colors.orange);
                lmaoRenderer->CursorPosition.x += 8;
            }
            else 
                lmaoRenderer->Print(chr);
            }
    }

    void
    tetris_print(struct tetris *t) {
        int x,y;
        lmaoRenderer->Clear(Colors.black);
        lmaoRenderer->Print("[LEVEL: {}", to_string(t->level));
        lmaoRenderer->Print(" | SCORE: {}]\n\r", to_string(t->score));
        //lmaoRenderer->Print
        for (x=0; x<2*t->w+2; x++)
            lmaoRenderer->Print("~");
        lmaoRenderer->Print("\n\r");
        for (y=0; y<t->h; y++) {
            lmaoRenderer->Print ("!");
            for (x=0; x<t->w; x++) 
            {
                char chr = '?';
                if (x>=t->x && y>=t->y 
                        && x<(t->x+t->current.w) && y<(t->y+t->current.h) 
                        && t->current.data[y-t->y][x-t->x]!=' ')
                    chr = t->current.data[y-t->y][x-t->x];
                else
                    chr = t->game[x][y];
                funkyPrint(chr);
            }
            lmaoRenderer->Print ("!\n\r");
        }
        for (x=0; x<2*t->w+2; x++)
            lmaoRenderer->Print("~");
        lmaoRenderer->Print("\n\r");
    }

    int
    tetris_hittest(struct tetris *t) {
        int x,y,X,Y;
        struct tetris_block b=t->current;
        for (x=0; x<b.w; x++)
            for (y=0; y<b.h; y++) {
                X=t->x+x;
                Y=t->y+y;
                if (X<0 || X>=t->w)
                    return 1;
                if (b.data[y][x]!=' ') {
                    if ((Y>=t->h) || 
                            (X>=0 && X<t->w && Y>=0 && t->game[X][Y]!=' ')) {
                        return 1;
                    }
                }
            }
        return 0;
    }

    void
    tetris_new_block(struct tetris *t) {
        t->current=blocks[RND::lehmer64()%TETRIS_PIECES];
        t->x=(t->w/2) - (t->current.w/2);
        t->y=0;
        if (tetris_hittest(t)) {
            t->gameover=1;
        }
    }

    void
    tetris_print_block(struct tetris *t) {
        int x,y,X,Y;
        struct tetris_block b=t->current;
        for (x=0; x<b.w; x++)
            for (y=0; y<b.h; y++) {
                if (b.data[y][x]!=' ')
                    t->game[t->x+x][t->y+y]=b.data[y][x];
            }
    }

    void
    tetris_rotate(struct tetris *t) {
        struct tetris_block b=t->current;
        struct tetris_block s=b;
        int x,y;
        b.w=s.h;
        b.h=s.w;
        for (x=0; x<s.w; x++)
            for (y=0; y<s.h; y++) {
                b.data[x][y]=s.data[s.h-y-1][x];
            }
        x=t->x;
        y=t->y;
        t->x-=(b.w-s.w)/2;
        t->y-=(b.h-s.h)/2;
        t->current=b;
        if (tetris_hittest(t)) {
            t->current=s;
            t->x=x;
            t->y=y;
        }
    }

    void
    tetris_gravity(struct tetris *t) {
        int x,y;
        t->y++;
        if (tetris_hittest(t)) {
            t->y--;
            tetris_print_block(t);
            tetris_new_block(t);
        }
    }

    void
    tetris_fall(struct tetris *t, int l) {
        int x,y;
        for (y=l; y>0; y--) {
            for (x=0; x<t->w; x++)
                t->game[x][y]=t->game[x][y-1];
        }
        for (x=0; x<t->w; x++)
            t->game[x][0]=' ';
    }

    void
    tetris_check_lines(struct tetris *t) {
        int x,y,l;
        int p=100;
        for (y=t->h-1; y>=0; y--) {
            l=1;
            for (x=0; x<t->w && l; x++) {
                if (t->game[x][y]==' ') {
                    l=0;
                }
            }
            if (l) {
                t->score += p;
                p*=2;
                tetris_fall(t, y);
                y++;
            }
        }
    }

    int
    tetris_level(struct tetris *t) {
        int i;
        for (i=0; i<TETRIS_LEVELS; i++) {
            if (t->score>=levels[i].score) {
                t->level = i+1;
            } else break;
        }
        return levels[t->level-1].nsec;
    }

    struct tetris X_t;
    int X_count;
    void tetris_run_init(int w, int h)
    {
        //tetris_set_ioconfig();
        tetris_init(&X_t, w, h);
        //srand(time(NULL));
        X_count = 0;
        //tm.tv_sec=0;
        //tm.tv_nsec=1000000;

        tetris_new_block(&X_t);
    }
    void tetris_run_loop()
    {
        //tetris_print(&X_t);
        //return;
        if (X_count%1 == 0) 
        {
            if (KeyboardScancodeState[0x1E]) // A
            {
                X_t.x--;
                if (tetris_hittest(&X_t))
                    X_t.x++;
            }
            if (KeyboardScancodeState[0x20]) // D
            {
                X_t.x++;
                if (tetris_hittest(&X_t))
                    X_t.x--;
            }
            if (KeyboardScancodeState[0x11]) // W
            {
                tetris_rotate(&X_t);
            }

            if (KeyboardScancodeState[0x1F]) // S
            {
                tetris_gravity(&X_t);
            }   
        }

        X_count++;
        tetris_print(&X_t);
        if (X_count%4 == 0) {
            tetris_gravity(&X_t);
            tetris_check_lines(&X_t);
        }
    }
    void tetris_run_end()
    {
        tetris_print(&X_t);
        lmaoRenderer->Print("*** GAME OVER ***\n");

        tetris_clean(&X_t);
        tetris_cleanup_io();
    }


    // void
    // tetris_run(int w, int h) {
    //     //struct timespec tm;
    //     struct tetris t;
    //     char cmd;
    //     int count=0;
    //     tetris_set_ioconfig();
    //     tetris_init(&t, w, h);
    //     //srand(time(NULL));

    //     //tm.tv_sec=0;
    //     //tm.tv_nsec=1000000;

    //     tetris_new_block(&t);
    //     while (!t.gameover) {
    //         //nanosleep(&tm, NULL);
    //         count++;
    //         if (count%50 == 0) {
    //             tetris_print(&t);
    //         }
    //         if (count%350 == 0) {
    //             tetris_gravity(&t);
    //             tetris_check_lines(&t);
    //         }
    //         if (KeyboardScancodeState[0x1E]) // A
    //         {
    //             t.x--;
    //             if (tetris_hittest(&t))
    //                 t.x++;
    //         }
    //         if (KeyboardScancodeState[0x20]) // D
    //         {
    //             t.x++;
    //             if (tetris_hittest(&t))
    //                 t.x--;
    //         }
    //         if (KeyboardScancodeState[0x11]) // W
    //         {
    //             tetris_rotate(&t);
    //         }

    //         if (KeyboardScancodeState[0x1F]) // S
    //         {
    //             tetris_gravity(&t);
    //         }   
    //     }

    //     tetris_print(&t);
    //     lmaoRenderer->Print("*** GAME OVER ***\n");

    //     tetris_clean(&t);
    //     tetris_cleanup_io();
    // }

















}