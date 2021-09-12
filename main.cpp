#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "include/raylib.h"

int generateRandomInt(int lower, int upper){
  int num = (rand() % (upper - lower + 1)) + lower;
  return num;
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 700;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Fishing");
    InitAudioDevice();

    // loading in resources
    Image backgroundImg = LoadImage("resources/background.png");
    Image fishingGuyImg = LoadImage("resources/fishingguy.png");
    Image smallfishImg = LoadImage("resources/smallfish.png");
    Image sharkImg = LoadImage("resources/shark.png");


    // converting images to textures
    Texture2D backgroundTexture = LoadTextureFromImage(backgroundImg);
    Texture2D fishingGuyTexture = LoadTextureFromImage(fishingGuyImg);

    ImageResize(&smallfishImg, smallfishImg.width/2, smallfishImg.height/2);
    Texture2D smallfishFacingRightTexture = LoadTextureFromImage(smallfishImg);
    ImageFlipHorizontal(&smallfishImg);
    Texture2D smallfishFacingLeftTexture = LoadTextureFromImage(smallfishImg);
    ImageRotateCCW(&smallfishImg);
    Texture2D smallfishCaughtTexture = LoadTextureFromImage(smallfishImg);

    Texture2D sharkFacingRightTexture = LoadTextureFromImage(sharkImg);
    ImageFlipHorizontal(&sharkImg);
    Texture2D sharkFacingLeftTexture = LoadTextureFromImage(sharkImg);

    Sound splashSound = LoadSound("resources/splash.wav");
    SetSoundVolume(splashSound, 0.3f);

    Music backgroundWavesSound = LoadMusicStream("resources/waves.mp3");
    PlayMusicStream(backgroundWavesSound);

    //unloads resources now they are used a textures.
    UnloadImage(backgroundImg);
    UnloadImage(fishingGuyImg);
    UnloadImage(smallfishImg);
    UnloadImage(sharkImg);

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    srand(time(NULL));
    //--------------------------------------------------------------------------------------



    struct shark {
      float xPos;
      float yPos;
      int direction; // 1 = moving right, -1 moving left
      float speed;
    };

    struct shark shark = {-300, 750, 1, 1};

    struct fish {
      int points = 0; // 0 points means the fish do not exist.
      int xPos = 0;
      int yPos = 0;
      int direction = 0; // 1 = moving right, -1 moving left
      int speed = 0;
    };

    int MAXFISHCOUNT = 30;
    int MAXCATCH = 10;
    struct fish freeFishies[MAXFISHCOUNT];
    struct fish caughtFishies[MAXCATCH];

    freeFishies[0] = (struct fish){1, 500, 600, -1, 2};
    freeFishies[1] = (struct fish){1, 100, 300, 1, 2};

    int highscore = 0;
    int score = 0;
    int scene = 1;
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
      UpdateMusicStream(backgroundWavesSound);
      if (scene == 1){
        // Update
        //----------------------------------------------------------------------------------
        // update som game variables

        if(highscore < score) highscore = score;
        int mousePosY = GetMouseY();
        if (mousePosY < 170){
          mousePosY = 170;
        }

        if (mousePosY < 200){
          int countCaugthFish = 0;
          for(int i = 0; i < MAXCATCH; i++){
            if(caughtFishies[i].points > 0){
              caughtFishies[i].points = 0;
              score++;
              countCaugthFish++;
            }
          }
          if(countCaugthFish > 0) PlaySound(splashSound);
        }

        // updating fish
        for(int i = 0; i < MAXFISHCOUNT; i++){
          // updating fish positions
          if(freeFishies[i].points > 0){
            freeFishies[i].xPos = freeFishies[i].xPos + (freeFishies[i].speed * freeFishies[i].direction);
          }

          // spawn new fish
          if(freeFishies[i].points == 0){
            int r = generateRandomInt(0,500);
            if(r == 10){
              int newFishPosY = generateRandomInt(220,750);
              int newFishSpeed = generateRandomInt(1,6);
              int newFishDirection = generateRandomInt(0,1);
              if (newFishDirection == 0) newFishDirection = -1;
              int newFishPosX = 0;
              if(newFishDirection < 0){
                newFishPosX = screenWidth+100;
              }else{
                newFishPosX = -100;
              }
              freeFishies[i] = (struct fish){1, newFishPosX, newFishPosY, newFishDirection, newFishSpeed};
            }
          }

          // removing fish that have left the screenWidth
          if(freeFishies[i].points > 0){
            if(freeFishies[i].xPos > screenWidth+200) freeFishies[i].points = 0;
            if(freeFishies[i].xPos < -200) freeFishies[i].points = 0;
          }

          // checking for catching fish
          if(mousePosY < freeFishies[i].yPos+25 && mousePosY > freeFishies[i].yPos){
            if(((screenWidth/2) < freeFishies[i].xPos+50) && ((screenWidth/2) > freeFishies[i].xPos)){
              for(int n = 0; n < MAXCATCH; n++){
                if(caughtFishies[n].points == 0){
                  caughtFishies[n].points = freeFishies[i].points;
                  caughtFishies[n].xPos = screenWidth/2;
                  caughtFishies[n].yPos = mousePosY;
                  caughtFishies[n].speed = 0;
                  caughtFishies[n].direction = 0;
                  freeFishies[i].points = 0;
                }
              }
            }
          }

        }

        // updating caught fish positions
        for(int i = 0; i < MAXCATCH; i++){
          if(caughtFishies[i].points > 0){
            caughtFishies[i].yPos = mousePosY;
          }
        }

        //updating shark
        bool updateShark = false;
        shark.xPos = shark.xPos + (shark.speed * shark.direction);
        if(shark.yPos+63 < mousePosY) shark.yPos = shark.yPos + (shark.speed*score*0.05); // the 63 here is a ofset to make the shark mouth go for the hook
        if(shark.yPos+63 > mousePosY) shark.yPos = shark.yPos - (shark.speed*score*0.05);
        if(shark.yPos < 220) shark.yPos = 220;
        if(shark.xPos > screenWidth+310) updateShark = true;
        if(shark.xPos < -310) updateShark = true;
        if(updateShark){
          int newSharkPosY = generateRandomInt(220,750);
          int newSharkSpeed = 1;
          int newSharkDirection = 0;
          if (newSharkDirection == 0) newSharkDirection = -1;
          int newSharkPosX = 0;
          if(newSharkDirection < 0){
            newSharkPosX = screenWidth+300;
          }else{
            newSharkPosX = -300;
          }
          shark = (struct shark){float(newSharkPosX), float(newSharkPosY), newSharkDirection, float(newSharkSpeed)};
        }
        float mouthPosX;
        if(shark.direction == 1) mouthPosX = 206;
        if(shark.direction == -1) mouthPosX = 0;
        Rectangle sharkMouthRec = { float(shark.xPos+mouthPosX), float(shark.yPos+50), 40, 20 };
        Vector2 hookLoc = { (float)screenWidth/2, (float)mousePosY };
        bool eaten = CheckCollisionPointRec(hookLoc, sharkMouthRec);
        if(eaten) score = 0;

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            DrawTexture(backgroundTexture, 0, 0, WHITE);
            DrawTexture(fishingGuyTexture, screenWidth / 2, 150, WHITE);

            //DrawFPS(10, 10);
            //DrawText("Hej Verden", 190, 200, 20, RED);

            DrawLine(screenWidth / 2, 150, screenWidth / 2, mousePosY, BLACK);

            //render fish
            int count = 0;
            for(int i = 0; i < MAXFISHCOUNT; i++){
              if(freeFishies[i].points == 1){
                if(freeFishies[i].direction > 0) DrawTexture(smallfishFacingLeftTexture, freeFishies[i].xPos, freeFishies[i].yPos, WHITE);
                if(freeFishies[i].direction < 0) DrawTexture(smallfishFacingRightTexture, freeFishies[i].xPos, freeFishies[i].yPos, WHITE);
                count++;
              }
            }
            //DrawText(TextFormat("count: %i", count), 190, 200, 20, RED);

            //render caught fish
            int fishOnHook = 0;
            for(int i = 0; i < MAXCATCH; i++){
              if(caughtFishies[i].points == 1){
                DrawTexture(smallfishCaughtTexture, caughtFishies[i].xPos-(smallfishCaughtTexture.width/2), caughtFishies[i].yPos, WHITE);
                fishOnHook++;
              }
            }
            DrawText(TextFormat("%i", fishOnHook), (screenWidth/2)+15, mousePosY, 18, BLACK);

            // render shark
            if(shark.direction < 0) DrawTexture(sharkFacingLeftTexture, shark.xPos, shark.yPos, WHITE);
            if(shark.direction > 0) DrawTexture(sharkFacingRightTexture, shark.xPos, shark.yPos, WHITE);
            //DrawRectangleRec(sharkMouthRec, RED);

            DrawText(TextFormat("Score:     %i", score), 10, 10, 20, BLACK);
            DrawText(TextFormat("Highscore: %i", highscore), 10, 30, 20, BLACK);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  UnloadTexture(backgroundTexture);
  UnloadTexture(fishingGuyTexture);
  UnloadTexture(smallfishFacingLeftTexture);
  UnloadTexture(smallfishFacingRightTexture);
  UnloadTexture(smallfishCaughtTexture);
  UnloadTexture(sharkFacingLeftTexture);
  UnloadTexture(sharkFacingRightTexture);

  UnloadSound(splashSound);
  StopMusicStream(backgroundWavesSound);
  UnloadMusicStream(backgroundWavesSound);

  CloseAudioDevice();
  CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
