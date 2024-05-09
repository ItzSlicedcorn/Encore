//
// Created by marie on 02/05/2024.
//


#include <functional>
#include <random>
#include "game/menus/gameMenu.h"
#include "raylib.h"
#include "raygui.h"
#include "song/songlist.h"
#include "game/lerp.h"
#include "game/settings.h"
#include "game/assets.h"
#include "game/menus/uiUnits.h"
#include "raymath.h"

const float Width = (float)GetScreenWidth();
const float Height = (float)GetScreenHeight();



void Menu::DrawTopOvershell(float TopOvershell) {
    Units u;
    DrawRectangle(0,0,(int)GetScreenWidth(), u.hpct(TopOvershell)+6,WHITE);
    DrawRectangle(0,0,(int)GetScreenWidth(), u.hpct(TopOvershell),GetColor(0x181827FF));
}

void Menu::DrawBottomOvershell() {
    Units u;
    float BottomOvershell = GetScreenHeight() - u.hpct(0.15f);
    DrawRectangle(0,BottomOvershell-6,(float)(GetScreenWidth()), (float)GetScreenHeight(),WHITE);
    DrawRectangle(0,BottomOvershell,(float)(GetScreenWidth()), (float)GetScreenHeight(),GetColor(0x181827FF));
}

void Menu::DrawBottomBottomOvershell() {
    Units u;
    float BottomBottomOvershell = GetScreenHeight() - u.hpct(0.1f);
    DrawRectangle(0,BottomBottomOvershell-6,(float)(GetScreenWidth()), (float)GetScreenHeight(),WHITE);
    DrawRectangle(0,BottomBottomOvershell,(float)(GetScreenWidth()), (float)GetScreenHeight(),GetColor(0x181827FF));
}

void Menu::renderStars(Player player, float xPos, float yPos, Assets assets) {
    Units u;
    int starsval = player.stars(player.songToBeJudged.parts[player.instrument]->charts[player.diff].baseScore,player.diff);
    float starPercent = (float)player.score/(float)player.songToBeJudged.parts[player.instrument]->charts[player.diff].baseScore;

    for (int i = 0; i < 5; i++) {
        bool firstStar = (i == 0);
        DrawTextureEx(assets.emptyStar, {(xPos+(i*50)-125),yPos},0,0.175f,WHITE);
        float yMaskPos = Remap(starPercent, firstStar ? 0 : player.xStarThreshold[i-1], player.xStarThreshold[i], yPos, yPos + 50);
        BeginScissorMode((xPos+(i*50)-125), yMaskPos, 50, 50);
        DrawTextureEx(player.goldStars? assets.goldStar : assets.star, {(xPos+(i*50)-125),yPos},0,0.175f,WHITE);
        EndScissorMode();
    }
};

void Menu::loadMenu(SongList songList, GLFWgamepadstatefun gamepadStateCallbackSetControls, Assets assets) {
    Units u;
    Settings settings;
    float RightBorder = ((float)GetScreenWidth()/2)+((float)GetScreenHeight()/1.20f);
    float RightSide = RightBorder >= (float)GetScreenWidth() ? (float)GetScreenWidth() : RightBorder;
    float LeftBorder = ((float)GetScreenWidth()/2)-((float)GetScreenHeight()/1.20f);
    float LeftSide = LeftBorder <= 0 ? 0 : LeftBorder;



    std::filesystem::path directory = GetPrevDirectoryPath(GetApplicationDirectory());

    std::ifstream splashes;
    splashes.open((directory / "Assets/ui/splashes.txt"));

    static std::string result;
    std::string line;
    if (!stringChosen) {
        std::random_device seed;
        std::mt19937 prng(seed());
        for (std::size_t n = 0; std::getline(splashes, line); n++) {
            std::uniform_int_distribution<> dist(0, n);
            if (dist(prng) < 1)
                result = line;
        }
        stringChosen = true;
    }

    Vector2 StringBox = {RightSide - MeasureTextEx(assets.josefinSansItalic, result.c_str(), 32, 1).x,  u.hpct(0.2f)/2 - 16};
    DrawTopOvershell(0.2f);
    DrawBottomOvershell();
    DrawBottomBottomOvershell();
    DrawTextEx(assets.josefinSansItalic, result.c_str(), StringBox, 32, 1, WHITE);
    DrawTextureEx(assets.encoreWhiteLogo, {LeftSide,
                                               (u.hpct(0.2f)/2 - assets.encoreWhiteLogo.height / 4)}, 0,
                      0.5, WHITE);


        if (GuiButton({((float) GetScreenWidth() / 2) - 100, ((float) GetScreenHeight() / 2) - 120, 200, 60}, "Play")) {

            for (Song &songi: songList.songs) {
                songi.titleScrollTime = GetTime();
                songi.titleTextWidth = assets.MeasureTextRubik(songi.title.c_str(), 24);
                songi.artistScrollTime = GetTime();
                songi.artistTextWidth = assets.MeasureTextRubik(songi.artist.c_str(), 20);
            }
            Menu::SwitchScreen(SONG_SELECT);
        }
        if (GuiButton({((float) GetScreenWidth() / 2) - 100, ((float) GetScreenHeight() / 2) - 30, 200, 60},
                      "Options")) {
            glfwSetGamepadStateCallback(gamepadStateCallbackSetControls);
            Menu::SwitchScreen(SETTINGS);
        }
        if (GuiButton({((float) GetScreenWidth() / 2) - 100, ((float) GetScreenHeight() / 2) + 60, 200, 60}, "Quit")) {
            exit(0);
        }
        if (GuiButton({(float) GetScreenWidth() - 60, (float) GetScreenHeight() - 60, 60, 60}, "")) {
            OpenURL("https://github.com/Encore-Developers/Encore-Raylib");
        }


        if (GuiButton({(float) GetScreenWidth() - 120, (float) GetScreenHeight() - 60, 60, 60}, "")) {
            OpenURL("https://discord.gg/GhkgVUAC9v");
        }
    if (GuiButton({(float) GetScreenWidth() - 180, (float) GetScreenHeight() - 60, 60, 60}, "")) {
        stringChosen = false;
    }
        if (GuiButton({(float) GetScreenWidth() - 180, (float) GetScreenHeight() - 120, 180, 60}, "Rescan Songs")) {
            songsLoaded = false;
        }
        DrawTextureEx(assets.github, {(float) GetScreenWidth() - 54, (float) GetScreenHeight() - 54}, 0, 0.2, WHITE);
        DrawTextureEx(assets.discord, {(float) GetScreenWidth() - 113, (float) GetScreenHeight() - 48}, 0, 0.075,
                      WHITE);
}

void Menu::showResults(const Player& player, Assets assets) {
    Units u;
    float RightBorder = ((float)GetScreenWidth()/2)+((float)GetScreenHeight()/1.20f);
    float RightSide = RightBorder >= (float)GetScreenWidth() ? (float)GetScreenWidth() : RightBorder;


    float LeftBorder = ((float)GetScreenWidth()/2)-((float)GetScreenHeight()/1.20f);
    float LeftSide = LeftBorder <= 0 ? 0 : LeftBorder;


    Song songToBeJudged = player.songToBeJudged;


    float ResultsWidth = (RightSide - LeftSide);
    float PlayerWidth = (ResultsWidth*0.2275f);

    float InnerMargins = (ResultsWidth*0.03f);

    float Player2 = LeftSide + PlayerWidth + InnerMargins;

    float Player4 = RightSide - PlayerWidth;
    float Player3 = Player4 - PlayerWidth - InnerMargins;

    float middle = (PlayerWidth/2) + LeftSide;


    // resultsRenderer.IndivPlayerResultsBox(LeftSide, RightSide, player, assets, );

    DrawRectangle(LeftSide-4, 0, PlayerWidth+8, (float)GetScreenHeight(), WHITE);
    DrawRectangle(LeftSide, 0, PlayerWidth, (float)GetScreenHeight(), GetColor(0x181827FF));
    // renderStars(player, 0,0);

    //assets.DrawTextRHDI("Results", 70,7, WHITE);
    assets.DrawTextRubik(songToBeJudged.artist.c_str(), middle - (assets.MeasureTextRubik(songToBeJudged.artist.c_str(), 24) / 2), 48, 24, WHITE);
    // if (player.FC) {
    //    assets.DrawTextRubik("Flawless!", middle- (assets.MeasureTextRubik("Flawless!", 24) / 2), 7, 24, GOLD);
    // }
    float scorePos = middle - MeasureTextEx(assets.redHatDisplayItalic, scoreCommaFormatter(player.score).c_str(), 64, 1).x /2;
    DrawTextEx(
            assets.redHatDisplayItalic,
            scoreCommaFormatter(player.score).c_str(),
            {
                scorePos,
                (float)GetScreenHeight()/2},
            64,
            1,
            GetColor(0x00adffFF));

    // assets.DrawTextRHDI(scoreCommaFormatter(player.score).c_str(), middle - assets.MeasureTextRHDI(scoreCommaFormatter(player.score).c_str())/2, 120, Color{107, 161, 222,255});
    assets.DrawTextRubik(TextFormat("Perfect Notes : %01i/%02i", player.perfectHit, songToBeJudged.parts[player.instrument]->charts[player.diff].notes.size()), (middle - assets.MeasureTextRubik(TextFormat("Perfect Notes: %01i/%02i", player.perfectHit, songToBeJudged.parts[player.instrument]->charts[player.diff].notes.size()), 24) / 2), 192, 24, WHITE);
    assets.DrawTextRubik(TextFormat("Good Notes : %01i/%02i", player.notesHit - player.perfectHit, songToBeJudged.parts[player.instrument]->charts[player.diff].notes.size()), (middle - assets.MeasureTextRubik(TextFormat("Good Notes: %01i/%02i", player.notesHit - player.perfectHit, songToBeJudged.parts[player.instrument]->charts[player.diff].notes.size()), 24) / 2), 224, 24, WHITE);
    assets.DrawTextRubik(TextFormat("Missed Notes: %01i/%02i", player.notesMissed, songToBeJudged.parts[player.instrument]->charts[player.diff].notes.size()), (GetScreenWidth() / 2 - assets.MeasureTextRubik(TextFormat("Missed Notes: %01i/%02i", player.notesMissed, songToBeJudged.parts[player.instrument]->charts[player.diff].notes.size()), 24) / 2), 256, 24, WHITE);
    assets.DrawTextRubik(TextFormat("Strikes: %01i", player.playerOverhits), (middle - assets.MeasureTextRubik(TextFormat("Strikes: %01i", player.playerOverhits), 24) / 2), 288, 24, WHITE);
    assets.DrawTextRubik(TextFormat("Longest Streak: %01i", player.maxCombo), (middle - assets.MeasureTextRubik(TextFormat("Longest Streak: %01i", player.maxCombo), 24) / 2), 320, 24, WHITE);
    renderStars(player, middle, (float)GetScreenHeight()/2 - 50, assets);


    //DrawRectangle(Player2-4, 0, PlayerWidth+8, (float)GetScreenHeight(), WHITE);
    //DrawRectangle(Player2, 0, PlayerWidth, (float)GetScreenHeight(), GetColor(0x181827FF));

    //DrawRectangle(Player3-4, 0, PlayerWidth+8, (float)GetScreenHeight(), WHITE);
    //DrawRectangle(Player3, 0, PlayerWidth, (float)GetScreenHeight(), GetColor(0x181827FF));

    //DrawRectangle(Player4-4, 0, PlayerWidth+8, (float)GetScreenHeight(), WHITE);
    //DrawRectangle(Player4, 0, PlayerWidth, (float)GetScreenHeight(), GetColor(0x181827FF));
    // DrawLine(LeftSide, 0, LeftSide, (float)GetScreenHeight(), WHITE);

    float TopOvershellHeight = (float)GetScreenHeight() * 0.2f;

    float SongNameFontSizePre = (TopOvershellHeight / 2) - ((float)GetScreenHeight() * 0.01f);
    float SongNameFontSize = (MeasureTextEx(assets.redHatDisplayBlack,player.songToBeJudged.title.c_str(), SongNameFontSizePre, 1).x >= GetScreenWidth()*0.75f ? TopOvershellHeight / 2 - ((float)GetScreenWidth() * 0.25f) : TopOvershellHeight / 2 - ((float)GetScreenHeight() * 0.01f));
    float SongNameHeightPos = ((float)GetScreenHeight() * 0.007f);
    float ScoreFontSize = (TopOvershellHeight / 2) - ((float)GetScreenHeight() * 0.009f);
    float ScoreHeightPos = (TopOvershellHeight / 2 + (float)GetScreenHeight() * 0.01f);

    DrawTopOvershell(TopOvershellHeight);
    DrawBottomOvershell();
    DrawBottomBottomOvershell();



    float songNamePos = (float)GetScreenWidth()/2 - MeasureTextEx(assets.redHatDisplayBlack,player.songToBeJudged.title.c_str(), SongNameFontSize, 1).x/2;
    float bigScorePos = (float)GetScreenWidth()/2 - 15 - MeasureTextEx(assets.redHatDisplayItalicLarge,scoreCommaFormatter(player.score).c_str(), ScoreFontSize, 1).x;
    float bigStarPos = (float)GetScreenWidth()/2 + 140;

    DrawTextEx(assets.redHatDisplayBlack, player.songToBeJudged.title.c_str(), {songNamePos,SongNameHeightPos},SongNameFontSize,1,WHITE);
    DrawTextEx(assets.redHatDisplayItalicLarge, scoreCommaFormatter(player.score).c_str(), {bigScorePos,ScoreHeightPos},64,1, GetColor(0x00adffFF));
    renderStars(player, bigStarPos, 105, assets);
    // assets.DrawTextRHDI(player.songToBeJudged.title.c_str(),songNamePos, 50, WHITE);

}

void Menu::SwitchScreen(Screens screen){
    currentScreen = screen;
    switch (screen) {
        case MENU:
            // reset lerps
            stringChosen = false;
            break;
        case SONG_SELECT:
            break;
        case INSTRUMENT_SELECT:
            break;
        case DIFFICULTY_SELECT:
            break;
        case GAMEPLAY:
            break;
        case RESULTS:
            break;
        case SETTINGS:
            break;
    }
}