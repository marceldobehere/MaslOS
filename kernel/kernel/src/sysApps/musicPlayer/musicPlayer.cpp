#include "musicPlayer.h"
#include "../../cStdLib/cstrTools.h"
#include "../../memory/heap.h"
#include "../../osData/osData.h"
#include "../../fsStuff/fsStuff.h"
#include "../../rnd/rnd.h"
#include "../openFileExplorer/openFileExplorer.h"
#include "../..//devices/serial/serial.h"
#include "../../tasks/readFile/taskReadFile.h"


namespace SysApps
{
    MusicPlayer::MusicPlayer(const char* path)
    {
        AddToStack();
        //path = StrCopy("");

        //window =

        AddToStack();
        window = (Window*)_Malloc(sizeof(Window), "Music Player Window");
        GuiInstance* gui = new  GuiInstance(window);
        *(window) = Window((DefaultInstance*)gui, Size(400, 300), Position(100, 100), "Music Player", true, true, true);
        osData.windows.InsertAt(window, 0);
        window->hidden = true;
        //window->resizeable = false;
        gui->Init();
        RemoveFromStack();

        AddToStack();
        window->OnCloseHelp = (void*)this;
        window->OnClose = (void(*)(void*, Window*))(void*)&OnExternalWindowClose;
        window->OnResizeHelp = (void*)this;
        window->OnResize = (void(*)(void*, Window*))(void*)&OnExternalWindowResize;
        window->OnPartRenderHelp = (void*)this;
        window->OnPartRender = (void(*)(void*, Window*))(void*)&OnExternalWindowFrame;
        RemoveFromStack();

        AddToStack();
        window->oldHidden = true;
        window->hidden = false;
        osData.windowsToGetActive.Enqueue(window);

        guiInstance = gui;

        guiInstance->screen->backgroundColor = Colors.white;
        RemoveFromStack();

        //guiInstance->CreateComponentWithId(1010, GuiComponentStuff::ComponentType::IMAGE_RECT);
        //testImg1 = (GuiComponentStuff::ImageRectangleComponent*)guiInstance->GetComponentFromId(1010);

        //testImg1->position.x = 0;
        //testImg1->position.y = 20;

        //testImg1->size.FixedX = 100;
        //testImg1->size.FixedY = 100;
        window->newSize = Size(240, 60);

        //_Free(testImg1->imagePath);
        //testImg1->imagePath = StrCopy(path);
        musicPath = StrCopy(path);

        //testImg1->GetImageFromPath(testImg1->imagePath);

        // if (testImg1->image != NULL)
        // {
        //     testImg1->size.FixedX = testImg1->image->width;
        //     testImg1->size.FixedY = testImg1->image->height;
        //     testImg1->size.IsXFixed = true;
        //     testImg1->size.IsYFixed = true;
        //     window->newSize = Size(testImg1->image->width, testImg1->image->height + 20);
        // }

        AddToStack();
        guiInstance->CreateComponentWithId(1011, GuiComponentStuff::ComponentType::BUTTON);
        openBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1011);
        openBtn->position.x = 0;
        openBtn->position.y = 0;
        _Free(openBtn->textComp->text);
        openBtn->textComp->text = StrCopy("Open");
        openBtn->size.FixedX = 50;
        openBtn->size.FixedY = 20;
        openBtn->OnMouseClickHelp = (void*)this;
        openBtn->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnOpenClick;
        RemoveFromStack();

        AddToStack();
        guiInstance->CreateComponentWithId(1012, GuiComponentStuff::ComponentType::BUTTON);
        playBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1012);
        playBtn->position.x = 50;
        playBtn->position.y = 0;
        _Free(playBtn->textComp->text);
        playBtn->textComp->text = StrCopy("Play");
        playBtn->size.FixedX = 50;
        playBtn->size.FixedY = 20;
        playBtn->OnMouseClickHelp = (void*)this;
        playBtn->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnPlayClick;
        RemoveFromStack();

        AddToStack();
        guiInstance->CreateComponentWithId(1013, GuiComponentStuff::ComponentType::TEXT);
        timeText = (GuiComponentStuff::TextComponent*)guiInstance->GetComponentFromId(1013);
        timeText->position.x = 0;
        timeText->position.y = 20;
        _Free(timeText->text);
        timeText->text = StrCopy("00:00/00:00");
        timeText->size.FixedX = 100;
        timeText->size.FixedY = 20;
        RemoveFromStack();

        AddToStack();
        guiInstance->CreateComponentWithId(1014, GuiComponentStuff::ComponentType::TEXT);
        pathText = (GuiComponentStuff::TextComponent*)guiInstance->GetComponentFromId(1014);
        pathText->position.x = 0;
        pathText->position.y = 40;
        _Free(pathText->text);
        pathText->text = StrCopy(path);
        pathText->size.FixedX = 100;
        pathText->size.FixedY = 20;
        RemoveFromStack();

        musicSource = NULL;
        musicSwapBufferA = NULL;
        musicSwapBufferB = NULL;
        currentBufferIndex = 0;
        musicFileData = NULL;
        musicFileLen = 0;
        musicFilePos = 0;
        paused = true;


        LoadFile(path);

        



        // guiInstance->CreateComponentWithId(1021, GuiComponentStuff::ComponentType::TEXTFIELD);
        // pathComp = (GuiComponentStuff::TextFieldComponent*)guiInstance->GetComponentFromId(1021);
        // pathComp->position.x = 5*8 + 4;
        // pathComp->position.y = 0;

        // pathComp->AdvancedKeyHitCallBackHelp = (void*)this;
        // pathComp->AdvancedKeyHitCallBack = (bool(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::KeyHitEventInfo))(void*)&PathTypeCallBack;
        // _Free(pathComp->textComp->text);
        // pathComp->textComp->text = StrCopy(path);
        RemoveFromStack();
    }

    void MusicPlayer::LoadFile(const char* path)
    {
        if (path == NULL)
            return;
        
        if (!FS_STUFF::FileExists(path))
        {
            //Serial::Writeln("FILE DOES NOT EXIST");
            return;
        }

        _Free(musicPath);
        musicPath = StrCopy(path);
        _Free(pathText->text);
        pathText->text = StrCopy(path);
        paused = true;
        musicFilePos = 0;
        musicFileData = NULL;
        musicFileLen = NULL;
        _Free(timeText->text);
        timeText->text = StrCopy("00:00 / 00:00");

        //Serial::Writeln("Creating task!");
        guiInstance->OnWaitTaskDoneHelp = (void*)this;
        guiInstance->OnWaitTaskDoneCallback = (void(*)(void*, Task*))(void*)&OnTaskDone;
        guiInstance->waitTask2 = FS_STUFF::ReadFileTask(path);
    }

    void MusicPlayer::LoadFileBuff(void* _tsk)
    {
        TaskReadFile* tsk = (TaskReadFile*)_tsk;

        paused = true;
        _Free(playBtn->textComp->text);
        playBtn->textComp->text = StrCopy("Play");

        if (musicSource != NULL)
        {
            musicSource->Free();
            musicSource = NULL;
        }

        if (musicSwapBufferA != NULL)
        {
            _Free(musicSwapBufferA);
            musicSwapBufferA = NULL;
        }

        if (musicSwapBufferB != NULL)
        {
            _Free(musicSwapBufferB);
            musicSwapBufferB = NULL;
        }

        if (musicFileData != NULL)
        {
            _Free(musicFileData);
            musicFileData = NULL;
        }

        // if (!FS_STUFF::ReadFileFromFullPath(path, &musicFileData, &musicFileLen))
        // {
        //     musicFileData = NULL;
        //     return;
        // }
        musicFileData = tsk->data;

        if (musicFileData == NULL)
        {
            musicFileLen = 0;
            return;
        }

        musicFileLen = tsk->dataLen;

        if (musicFileLen < 50)
        {
            _Free(musicFileData);
            musicFileData = NULL;
            return;
        }

        bool wavValid = true;

        if (!StrEquals(musicFileData + 0, "RIFF", 4))
        {
            //Panic("WAV FILE INVALID RIFF: {}!", to_string(musicFileData[0]), true);
            wavValid = false;
            goto checkWavValid;
        }

        if (!StrEquals(musicFileData + 8, "WAVE", 4))
        {
            //Panic("WAV FILE INVALID WAVE!", true);
            wavValid = false;
            goto checkWavValid;
        }

        if (!StrEquals(musicFileData + 12, "fmt ", 4))
        {
            //Panic("WAV FILE INVALID fmt!", true);
            wavValid = false;
            goto checkWavValid;
        }

        if (!StrEquals(musicFileData + 36, "data", 4))
        {
            //Panic("WAV FILE INVALID data!", true);
            wavValid = false;
            goto checkWavValid;
        }

        checkWavValid:
        if (!wavValid)
        {
            _Free(musicFileData);
            musicFileData = NULL;
            return;
        }

        int channels = *((uint16_t*)(musicFileData + 22));
        int sampleRate = *((uint32_t*)(musicFileData + 24));
        int bitsPerSample = *((uint16_t*)(musicFileData + 34));

        //Serial::Writeln("Channels: {}", to_string(channels));
        //Serial::Writeln("SampleRate: {}", to_string(sampleRate));
        //Serial::Writeln("BitsPerSample: {}", to_string(bitsPerSample));


        //AddToStack();
        if (osData.defaultAudioOutputDevice != NULL)
        {
            int sampleCount = sampleRate / 2;
            //Serial::Writeln("SampleCount: {}", to_string(sampleCount));
            //Serial::Writeln();
            if (musicSource == NULL)
            {
                //Println(window, "> Creating Audiosource");
                musicSwapBufferA = new Audio::AudioBuffer(bitsPerSample, sampleRate, channels, sampleCount);
                musicSwapBufferB = new Audio::AudioBuffer(bitsPerSample, sampleRate, channels, sampleCount);
                currentBufferIndex = 0;
                
                musicSource = (void*)new Audio::BasicAudioSource(
                    musicSwapBufferA
                );

                if (currentBufferIndex == 0)
                    musicSource->buffer = musicSwapBufferA;
                else
                    musicSource->buffer = musicSwapBufferB;
                

                //musicSource->buffer->sampleCount = sampleCount;
                musicSource->OnFinishHelp = (void*)this;
                musicSource->OnFinish = (void(*)(void*, Audio::BasicAudioDestination*))(void*)&OnBufferFinish;
                //Println(window, "> Linking Audiosource to AC97");
                musicSource->ConnectTo(osData.defaultAudioOutputDevice->destination);
            }

            //paused = false;
            HandleMusic();

            // if (!musicSource->readyToSend)
            // {
            //     //Println(window, "> Filling Data");
            //     uint16_t* arr = (uint16_t*)musicSource->buffer->data;
            //     MusicBit16Test::FillArray(arr, 0, sampleCount, 400, sampleRate);
            //     musicSource->buffer->sampleCount = sampleCount;
            //     musicSource->samplesSent = 0;
            //     musicSource->readyToSend = true;
            //     //Println(window, "> Ready To send");
            // }
            // else
            // {
            //     //Print(window, "> Still sending Data. ({}", to_string(src->samplesSent));
            //     //Println(window, " of {} samples)", to_string(src->buffer->sampleCount));
            // }
        }
    }

    void MusicPlayer::OnPlayClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
    {
        if (paused)
        {
            paused = false;
            _Free(playBtn->textComp->text);
            playBtn->textComp->text = StrCopy("Stop");
        }
        else
        {
            paused = true;
            _Free(playBtn->textComp->text);
            playBtn->textComp->text = StrCopy("Play");
        }
    }

    void MusicPlayer::OnOpenClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
    {
        OpenFileExplorer* exp = new OpenFileExplorer();
        
        guiInstance->OnWaitTaskDoneHelp = (void*)this;
        guiInstance->OnWaitTaskDoneCallback = (void(*)(void*, Task*))(void*)&OnTaskDone;
        guiInstance->waitTask = exp->dataTask;
    }

    void MusicPlayer::OnTaskDone(Task* task)
    {
        //Serial::Writeln("TASK DONE!");
        if (task->GetType() == TaskType::SIMPLE_DATA)
        {
            TaskSimpleData* tsk = (TaskSimpleData*)task;
            if (tsk->data != NULL)
            {
                const char* path = (const char*)tsk->data;

                //Serial::Writeln("Opening file: {}", path);
                LoadFile(path);

                _Free(path);
            }
        }
        else if (task->GetType() == TaskType::READ_FILE)
        {
            TaskReadFile* tsk = (TaskReadFile*)task;
            if (tsk->data != NULL)
            {
                //Serial::Writeln("Loading file...");
                LoadFileBuff((void*)tsk);
            }
        }

    }

    void MusicPlayer::OnExternalWindowClose(Window* window)
    {
        //GlobalRenderer->Clear(Colors.purple);
        Free();
    }

    void MusicPlayer::OnExternalWindowResize(Window* window)
    {
        int h = window->size.height;
        if (h < 60)
            h = 60;
        //testImg1->size.FixedY = h - 20;
        //testImg1->size.FixedX = window->size.width;
    }

    void MusicPlayer::HandleMusic()
    {
        // Check if need to load more sound
        if (musicSource != NULL && musicFileData != NULL &&
            !paused)
        {
            // Serial::Writeln("READING");
            // Serial::Writeln("Sample Count: {}", to_string(sampleCount));
            // Serial::Writeln("Bytes Per Sample: {}", to_string(bytesPerSample));
            // Serial::Writeln("Bytes To Read (1): {}", to_string(sampleCount * bytesPerSample * channelCount));
            // Serial::Writeln("File Left: {}", to_string(fileLeft));
            // Serial::Writeln("Bytes To Read (2): {}", to_string(bytesToRead));
            // Serial::Writeln("File Pos: {}", to_string(musicFilePos));
            // Serial::Writeln("File Len: {}", to_string(musicFileLen));
            // Serial::Writeln();

            for (int bufI = 0; bufI < 2; bufI++)
            {
                Audio::AudioBuffer* curBuf = NULL;
                if (bufI == 0)
                    curBuf = musicSwapBufferA;
                else
                    curBuf = musicSwapBufferB;

                if (curBuf->sampleCount != 0)
                    continue;

                int sampleCount = curBuf->totalSampleCount;
                int bytesPerSample = curBuf->bitsPerSample / 8;
                int channelCount = curBuf->channelCount;
                int bytesToRead = sampleCount * bytesPerSample * channelCount;
                int fileLeft = musicFileLen - musicFilePos;
                if (fileLeft < bytesToRead)
                {
                    bytesToRead = fileLeft;
                }


                if (bytesToRead == 0)
                {
                    // check if all buffers are empty
                    // paused = true;
                    // musicFilePos = 0;

                    // _Free(playBtn->textComp->text);
                    // playBtn->textComp->text = StrCopy("Play");
                }
                else
                {
                    int samplesToRead = (bytesToRead / bytesPerSample) / channelCount;
                    _memcpy(musicFileData + musicFilePos, curBuf->data, bytesToRead);

                    curBuf->sampleCount = samplesToRead;
                }

                musicFilePos += bytesToRead;
            }


            //timeText->text = StrCopy(to_string(musicSource->samplesSent));
            if (!musicSource->readyToSend)
            {
                currentBufferIndex = (currentBufferIndex + 1) % 2;

                Audio::AudioBuffer* curBuf = NULL;
                if (currentBufferIndex == 0)
                    curBuf = musicSwapBufferA;
                else
                    curBuf = musicSwapBufferB;

                if (musicSwapBufferA->sampleCount == 0 && 
                    musicSwapBufferB->sampleCount == 0 && musicFilePos >= musicFileLen)
                {
                    paused = true;
                    musicFilePos = 0;
                    currentBufferIndex = 0;

                    _Free(playBtn->textComp->text);
                    playBtn->textComp->text = StrCopy("Play");
                }
                else if (curBuf->sampleCount != 0)
                {
                    Serial::Writelnf("> Switching to buffer %d.", currentBufferIndex);
                    musicSource->buffer = curBuf;
                    musicSource->samplesSent = 0;
                    musicSource->readyToSend = true;
                }
            }
        }
    }

    void MusicPlayer::OnBufferFinish(Audio::BasicAudioDestination* dest)
    {
        //HandleMusic();
    }

    void MusicPlayer::OnExternalWindowFrame(Window* window)
    {
        HandleMusic();

        if (musicSource != NULL && musicFileData != NULL)
        {
            int64_t byteDivisor = (musicSource->buffer->bitsPerSample / 8) * musicSource->buffer->channelCount * musicSource->buffer->sampleRate;
            int64_t musicFilePosSec = musicFilePos / byteDivisor;
            int64_t musicFileLenSec = musicFileLen / byteDivisor;

            int startSec = musicFilePosSec % 60;
            int startMin = (musicFilePosSec / 60); // % 60;

            int endSec = musicFileLenSec % 60;
            int endMin = (musicFileLenSec / 60); // % 60;

            const char* toFree = NULL;
            _Free(timeText->text);
            timeText->text = StrCopy("");

            toFree = StrPadLeft(to_string(startMin), '0', 2, false);
            timeText->text = StrAppend(timeText->text, toFree, true);
            _Free(toFree);
            timeText->text = StrAppend(timeText->text, ":", true);
            toFree = StrPadLeft(to_string(startSec), '0', 2, false);
            timeText->text = StrAppend(timeText->text, toFree, true);
            _Free(toFree);
            
            timeText->text = StrAppend(timeText->text, " / ", true);
            
            toFree = StrPadLeft(to_string(endMin), '0', 2, false);
            timeText->text = StrAppend(timeText->text, toFree, true);
            _Free(toFree);
            timeText->text = StrAppend(timeText->text, ":", true);
            toFree = StrPadLeft(to_string(endSec), '0', 2, false);
            timeText->text = StrAppend(timeText->text, toFree, true);
            _Free(toFree);
        }
    }

    void MusicPlayer::Free()
    {
        AddToStack();

        AddToStack();
        _Free(musicPath);
        RemoveFromStack();

        AddToStack();
        if (musicSwapBufferA != NULL)
        {
            musicSwapBufferA->Free();
            musicSwapBufferA = NULL;
        }
        RemoveFromStack();

        AddToStack();
        if (musicSwapBufferB != NULL)
        {
            musicSwapBufferB->Free();
            musicSwapBufferB = NULL;
        }
        RemoveFromStack();

        AddToStack();
        if (musicSource != NULL)
        {
            musicSource->buffer = NULL;
            musicSource->Free();
            musicSource = NULL;
        }
        RemoveFromStack();

        AddToStack();
        if (musicFileData != NULL)
        {
            _Free(musicFileData);
            musicFileData = NULL;
        }
        RemoveFromStack();

        _Free(this);
        RemoveFromStack();
    }

}