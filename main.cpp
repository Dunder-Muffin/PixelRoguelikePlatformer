#include "common.h"
#include "Image.h"
#include "Player.h"
#include "Enemy.h"
#include "Treasure.h"
#define GLFW_DLL  
#include <GLFW/glfw3.h>

#include <cmath>
#include <cstring>


struct InputState
{
  bool keys[1024]{};
  GLfloat lastX = 400, lastY = 300;
  bool firstMouse = true;
  bool captureMouse         = true;  
  bool capturedMouseJustNow = false;
} static Input;


bool spawnEnemies = 1;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
int totalScore;
void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		//if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
  case GLFW_KEY_1:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case GLFW_KEY_2:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
	default:
		if (action == GLFW_PRESS)
      Input.keys[key] = true;
		else if (action == GLFW_RELEASE)
      Input.keys[key] = false;
	}
}

void processPlayerMovement(Player &player)
{
  if (Input.keys[GLFW_KEY_W])
    player.ProcessInput(MovementDir::UP);
  else if (Input.keys[GLFW_KEY_S])
    player.ProcessInput(MovementDir::DOWN);
  if (Input.keys[GLFW_KEY_A])
    player.ProcessInput(MovementDir::LEFT);
  else if (Input.keys[GLFW_KEY_D])
    player.ProcessInput(MovementDir::RIGHT);

}
void processAdditionalKeyboardInteraction(Player &player, Image &screenBuffer, std::vector<Missle> &pMisseles)
{
  static double lastShot = -2;
  Point pos = player.GetPosition();
  pos.x+=16;
  if (Input.keys[GLFW_KEY_F12])
     screenBuffer.Save(std::string("screenshot.jpg"));
  if (Input.keys[GLFW_KEY_LEFT_CONTROL] ||Input.keys[GLFW_KEY_RIGHT_CONTROL])
    player.Attac(lastFrame);
  if (lastFrame  - lastShot >0.8)
  {
    if (Input.keys[GLFW_KEY_RIGHT])
    {
      lastShot = lastFrame;
      pMisseles.push_back(Missle({MovementDir::RIGHT, pos}));
      return;
    }
    if (Input.keys[GLFW_KEY_LEFT])
    {
      lastShot = lastFrame;      
      pMisseles.push_back(Missle({MovementDir::LEFT, pos}));
      return;
    }
    if (Input.keys[GLFW_KEY_UP])
    {
      lastShot = lastFrame;
      pMisseles.push_back(Missle({MovementDir::UP, pos}));
      return;
    }
    if (Input.keys[GLFW_KEY_DOWN])
    {
      lastShot = lastFrame;
      pMisseles.push_back(Missle({MovementDir::DOWN, pos}));
      return;   
    }
  }
}
int findStartRoomCoord(std::ifstream &mapFile, size_t &x, size_t &y)
{
  std::vector <char> rooms;
  char room = 0;
  if (!mapFile.is_open()) 
  {
    std::cerr << "Could not open the file'"<< std::endl;
    return EXIT_FAILURE;
  }
  while (mapFile.get(room))
  {   
    if(room == '\n')
    {
      y++;
      x = 0;
    }
    if (room == 'a')
      break;
    x++;
  }
  x--;
  //y++;
  return 0;
}
std::vector<std::string> readRoomFile(char roomType)
{

  std::string room = "levels/room";
  std::string txt = ".txt";
  std::ifstream roomFile(room + roomType + txt);
  std::vector<std::string> roomMap;
  std::string line;
  while (1) 
  {
  if (!getline(roomFile, line))
    break;
  roomMap.push_back(line);
  }
  roomFile.close();
  return roomMap;
}

void drawBackground(Image &screenBuffer, const std::vector<std::string> &roomMap, 
                    Image& tiles,Anim treasure, int globalCoordX, int globalCoordY,
                    std::vector<Point> &torchPosition, double frame, std::vector<Enemy> &enemies)
{
  size_t texCoordX =- 1;
  size_t texCoordY =- 1;
  int x,y;
  Point torch;
  Pixel a;
  Image *tile;
  for(int TileY = 0; TileY < WINDOW_HEIGHT/tileSize; TileY++)
  {
    for(int TileX = 0; TileX < WINDOW_WIDTH/tileSize; TileX++)
    {
      switch(roomMap[TileY][TileX])
      {
        case '#':
          tile = &tiles;
          texCoordY = 128;/*wall texture coord in all Tex file*/
          texCoordX = 256;
          if (TileX%20==1 || TileY%20==1)
          {
          torch.x = TileX;
          torch.y = TileY;
          torchPosition.push_back(torch);
          }          
          break;
        case 'G':
          treasureCoord.x = globalCoordX;
          treasureCoord.y = globalCoordY;
          if(!treasureEmpty[treasureCoord])
          {
            texCoordY = 0;
            texCoordX = 0;
            if ((int)(5*frame)%3==0)
              tile = treasure.f1;
            else if ((int)(5*frame)%3==1)
              tile = treasure.f2;
            else
              tile = treasure.f3;
          }
          break;
        case 'e':
          if (spawnEnemies)
            enemies.push_back(Enemy({TileX*tileSize,TileY*tileSize}));
          break;
        case 'b':            
        case 'x':
        case '@':
        case '.':
          tile = &tiles;
          texCoordY = 128;/*floor texture coord in all Tex file*/
          texCoordX = 64;
          break;
        case ' ':
          tile = &tiles;
          texCoordY = 128;/*void texture coord in all Tex file*/
          texCoordX = 448;
          break;
          default:
            std::cerr << "Failed to draw room texture: '"<<roomMap[TileY][TileX]<<"'" <<std::endl;
            exit(-1);
      }
      for( y= 0; y < tileSize; ++y)
      {
        for( x= 0; x < tileSize; ++x)
        {
          a =  tile->GetPixel(texCoordX + x , texCoordY + y);
          screenBuffer.PutPixel( TileX*tileSize + x, TileY*tileSize + y,a);
        }
      }
    }
  }
  spawnEnemies=0;
}

void putTorches(Image &screen, Anim torch, std::vector<Point> p,float frame)
{
  int Xstart, Ystart;
  Image l("./resources/light2.png");
  Pixel pix1, pix2;
  Point coord;
  double a=0.2;
  while (!p.empty())
  {
    coord = p.back();
    p.pop_back();

    Xstart = coord.x*tileSize+10 -200;
    Ystart = coord.y*tileSize -200;

    for(int y= Ystart; y <((Ystart+400<WINDOW_HEIGHT)?Ystart+400 : WINDOW_HEIGHT); ++y)
    {
      for(int x= Xstart; x <((Xstart+400<WINDOW_WIDTH)?Xstart+400 : WINDOW_WIDTH); ++x)
      {
        if (x>=0 && y>=0)
        {
          pix1 = screen.GetPixel(x,y);
          pix2 = l.GetPixel(x-Xstart ,y-Ystart);
          if (pix2.r != 0 || pix2.g != 0 || pix2.b !=0)
          {
          pix1.r = pix1.r * (1 - a) + pix2.r * a;
          pix1.g = pix1.g * (1 - a) + pix2.g * a;
          pix1.b = pix1.b * (1 - a) + pix2.b * a;
            screen.PutPixel(x,y,pix1);
        }
        }
      }
    }
    for(int y= Ystart+94+100; y <Ystart+126+100; ++y)
    {
      for(int x= Xstart+84+100; x <Xstart+116+100; ++x)
      {
        if ((int)(8*frame)%3 ==2)
          pix1 = torch.f1->GetPixel(x- Xstart-84-100 ,y-Ystart-94-100);
        else if ((int)(8*frame)%3 ==1)
          pix1 = torch.f2->GetPixel(x-Xstart-84-100 ,y-Ystart-94-100);
        else
          pix1 = torch.f3->GetPixel(x-Xstart-84-100,y-Ystart-94-100);
        if (pix1.r != 0 || pix1.g != 0 || pix1.b !=0)
          screen.PutPixel(x,y,pix1);

      }
    }
  }
}
void parseScore (Image &screen, int n, std::vector<int>& vec) {
    vec.push_back (n%10);
    int temp = n / 10;  
    if (temp == 0 ) return;
    parseScore (screen, temp, vec);
}

void endOfTheGame(Image & screen)
{
  Image win("./resources/win.png");
  //screen = win;
  for(int y= 0; y < WINDOW_HEIGHT; ++y)
  {
    for(int x= 0; x < WINDOW_WIDTH; ++x)
    {
      screen.PutPixel(x,  y, win.GetPixel(x ,y));
    }
  }
  Image numbers("./resources/numbers.png");
/*  Image i1("./resources/1.png");
  Image i2("./resources/2.png");
  Image i3("./resources/3.png");
  Image i4("./resources/4.png");
  Image i5("./resources/5.png");
  Image i6("./resources/6.png");
  Image i7("./resources/7.png");
  Image i8("./resources/8.png");
  Image i9("./resources/9.png");*/
  std::vector <int> vec;
  parseScore (screen, totalScore, vec);
  if (vec.size()>=WINDOW_WIDTH/50)
  {
    std::cout<<"Your score is too big git fit in End Screen, great job! Here it is:\
    "<<totalScore<<std::endl;
    return;
  }
  Point pCoord;
  Pixel pix;
  int i=0;
  for (std::vector<int>::reverse_iterator it = vec.rbegin() ; it != vec.rend(); ++it)
  {
    switch (*it)
    {
      case 6:
       pCoord.x = 0;
       pCoord.y = 0;
        break;
      case 7: 
        pCoord.x = 100;
        pCoord.y = 0;
        break;
      case 8:
        pCoord.x = 200;
        pCoord.y = 0;
        break;
      case 9:
        pCoord.x = 300;
        pCoord.y = 0;
        break;
      case 5:
        pCoord.x = 400;
        pCoord.y = 105;
        break;
      case 1:
        pCoord.x = 0;
        pCoord.y = 105;
        break;
      case 2:
        pCoord.x = 100;
        pCoord.y = 105;
        break;
      case 3:
        pCoord.x = 200;
        pCoord.y = 105;
        break;
      case 4:
        pCoord.x = 300;
        pCoord.y = 105;
        break;
      case 0:
        pCoord.x = 415;
        pCoord.y = 0;
        break;      
    }
    for(int y= 0; y < 90; ++y)
    {
      for(int x= 0; x < 50; ++x)
      {
        pix = numbers.GetPixel(pCoord.x + x , pCoord.y + y);
        if (pix.r != 0 || pix.g != 0 || pix.b !=0)
          if (i*50+x<WINDOW_WIDTH &&y-2<WINDOW_HEIGHT)
            screen.PutPixel(i*50 + x,  y-2, pix); 
      }
    }
    i++; 
  }
}

void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    Input.captureMouse = !Input.captureMouse;

  if (Input.captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Input.capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (Input.firstMouse)
  {
    Input.lastX = float(xpos);
    Input.lastY = float(ypos);
    Input.firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - Input.lastX;
  GLfloat yoffset = Input.lastY - float(ypos);

  Input.lastX = float(xpos);
  Input.lastY = float(ypos);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  // ...
}


int initGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  std::cout << "Controls: "<< std::endl;
  std::cout << "press right mouse button to capture/release mouse cursor  "<< std::endl;
  std::cout << "W, A, S, D - movement  "<< std::endl;
  std::cout << "CTRL - attack  "<< std::endl;
  std::cout << "arrows - throw axe" << std::endl;
  std::cout << "F12 - screenshot  "<< std::endl;
  std::cout << "press ESC to exit" << std::endl;

	return 0;
}
int main(int argc, char** argv)
{
  if(!glfwInit())
    return -1;
//  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow*  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "task1 base project", nullptr, nullptr);
  if (window == nullptr)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  
  glfwMakeContextCurrent(window); 

  glfwSetKeyCallback        (window, OnKeyboardPressed);  
  glfwSetCursorPosCallback  (window, OnMouseMove); 
  glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
  glfwSetScrollCallback     (window, OnMouseScroll);

  if(initGL() != 0) 
    return -1;
  
  //Reset any OpenGL errors which could be present for some reason
  GLenum gl_error = glGetError();
  while (gl_error != GL_NO_ERROR)
    gl_error = glGetError();

  Point pos, pos1, starting_pos{.x = WINDOW_WIDTH / 2, .y = WINDOW_HEIGHT / 2};
  Player player{starting_pos};
  Image gameover("./resources/gameoverrev.png");
  Image specialRoom("./resources/nineAndThreeQuarters.png");
  Image treasureFound("./resources/200.png");
  Image fireBall("./resources/FireBoll.png");
  Image axe("./resources/PlayerMissle.png");
  Image tiles("./resources/TileA3.png");
  Image chest1("./resources/crate.png");
  Image chest2("./resources/crate2.png");
  Image chest3("./resources/crate3.png");
  Anim treasureAnim{.f1 = &chest1, .f2 = &chest2, .f3 =&chest3 };
  Image birb1("./resources/Bird1.png");
  Image birb2("./resources/Bird2.png");
  Image birb3("./resources/Bird3.png");
  Anim birbAnim{.f1 = &birb1, .f2 = &birb2, .f3 =&birb3 };    
  Image right("./resources/charSmall.png");
  Image rightMove("./resources/charSmallmoveR.png");
  Image rightAttac("./resources/AttacRight.png");
  Image left("./resources/charSmallLeft.png");
  Image leftMove("./resources/charSmallmove.png");
  Image leftAttac("./resources/AttacLeft.png");
  Image swipeAttac("./resources/AttacSwipe.png");
  PlayerAnim Panim{.right = &right, .rightMove = &rightMove,.rightAttac = &rightAttac,
  .left = &left, .leftMove = &leftMove, .leftAttac = &leftAttac, .swipeAttac = &swipeAttac};
  Image torch1("./resources/torch1.png");
  Image torch2("./resources/torch2.png");
  Image torch3("./resources/torch3.png");
  Anim torchAnim{.f1 = &torch1, .f2 = &torch2, .f3 =&torch3 };
  Image move1("./resources/Boss.png");
  Image move2("./resources/BossMove.png");
  Image attac("./resources/BossAttac.png");
  Image hit("./resources/BossDamaged.png");
  Image death("./resources/BossDead.png");
  Image form2("./resources/Slime.png");
  Image form2move("./resources/SlimeMove.png");
  Image SlimeD("./resources/SlimeDead.png");
  BossAnimation gishFlex{.move1 = &move1, .move2 = &move2, .attac = &attac,
   .hit = &hit, .death = &death}; 
  Anim slimeA = { .f1 = &form2, .f2 = &form2move, .f3 = &SlimeD};
  Image screenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
  

  Missle frBoll();
  std::vector<Missle> plMisseles;
  std::vector<Missle> enMisseles;
  std::vector<Enemy> enemies;
  Boss gish({900,900});
  Enemy slime1(starting_pos,5), slime2(starting_pos,5);

  std::ifstream mapFile("levels/plan.txt");
  size_t globalCoordX = 0, globalCoordY = 0;
  findStartRoomCoord(mapFile, globalCoordX, globalCoordY);
  char roomType = 'a';
  std::vector<std::string> roomMap = readRoomFile(roomType);
  size_t y,x;
  Pixel pix;
  std::string line;
  bool gameEnded =0;
  bool youWin = 0;
  double treasureFoundTime= -100;
  std::vector<Point> torchPosition;
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;
 
  //game loop  
  while (!glfwWindowShouldClose(window))
  {
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    glfwPollEvents();

    processPlayerMovement(player);
    processAdditionalKeyboardInteraction(player, screenBuffer, plMisseles);
    pos = player.GetPosition();

  switch (roomMap[round(pos.y/(double)tileSize)][round(pos.x/(double)tileSize)])
    {
      case '@':
      case '.':
      case 'b':
      case 'e':
        break;
      case ' ':
        gameEnded = 1;
        break;
      case '#':
        if (player.MovedLeft())
        {
          player.StopLeft();
        }
          else if (player.MovedUp())
          {
            player.StopUp();
          }
            else if (player.MovedRight())
            {
              player.StopRight();
            }
              else if (player.MovedDown())
              {
                player.StopDown();
              }
        break;
      case 'G':
        treasureCoord.x = globalCoordX;
        treasureCoord.y = globalCoordY;
        if (!treasureEmpty[treasureCoord])
        {
          totalScore += 200;
          treasureFoundTime = currentFrame;
        }
        if (globalCoordX!=7 || globalCoordY!=5)
          treasureEmpty[treasureCoord]=1;

        break;
      case 'x':
        if (player.MovedLeft())
        {
          globalCoordX--;
        }
          else if (player.MovedUp())
          {
            globalCoordY--;
          }
            else if (player.MovedRight())
            {
              globalCoordX++;
            }
              else if (player.MovedDown())
              {
                globalCoordY++;
              }
        mapFile.seekg (0, std::ios::beg);      
        for (y = 0; y <= globalCoordY; ++y)
        {
          getline(mapFile, line);
        }
        roomType = line[globalCoordX];
        if (roomType  ==' ')
        {
          roomType = 'S';
          globalCoordX = 7;
          globalCoordY = 5; 
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
          glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, specialRoom.Data()); GL_CHECK_ERRORS;
          glfwSwapBuffers(window);
          sleep(4);
        }
        enemies.clear();
        spawnEnemies = 1;
        roomMap = readRoomFile(roomType);
        player.SetPosition(starting_pos);
        break;
      default:
        std::cerr << "Failed to identify room texture " <<pos.y<<pos.x<<std::endl;
        return -1;
    }

    Point plPos = player.GetPosition();
    Point  enemyP,playerM,enemyP1, enemyP2;
    if (!gameEnded)
    {      
      drawBackground(screenBuffer, roomMap, tiles, treasureAnim, globalCoordX,
                 globalCoordY, torchPosition, currentFrame, enemies);
      if (roomType  =='Q')
      {
        if (!gish.Killed())
        {
          player.HitEnemy(gish, currentFrame);
          if(gish.Attacced(player))
          {
            gameEnded = 1;
          }
          if ((int)currentFrame%2==1)
            gish.Shoot(enMisseles, currentFrame);
          for (std::vector<Missle>::iterator it = enMisseles.begin(); it != enMisseles.end(); ++it)
          {
            pos1 = it->GetPosition();
            //std::cout<<pos1.x<<"  "<<pos.x<<"\n";
            if (abs(pos1.x - plPos.x)<= 16 && abs(pos1.y - plPos.y) <= 16)
            {
              gameEnded =1;
              break;
            }
            it->Draw(screenBuffer, tiles , fireBall);
          }
          gish.Draw(screenBuffer, tiles, currentFrame, gishFlex);
        
        for (std::vector<Missle>::iterator axeM = plMisseles.begin(); axeM != plMisseles.end(); ++axeM)
        {
          enemyP = gish.GetPosition();
          playerM = axeM->GetPosition();
          axeM->Draw(screenBuffer, tiles, axe);
          if (abs(enemyP.x - playerM.x)< 64 && abs(enemyP.y - playerM.y) < 64)
          {
            gish.HitByPlayer(currentFrame);
            break;
          }
        }
        slime1.SetPosition({enemyP.x-40,enemyP.y+10});
        slime2.SetPosition({enemyP.x+20,enemyP.y+10});  
      } else
      {
        slime1.Patrol();
        if(slime2.Attacced(player))
        {
          gameEnded = 1;
        }
        slime2.Draw(screenBuffer, tiles, currentFrame, slimeA);
        slime1.Draw(screenBuffer, tiles, currentFrame, slimeA);

          player.HitEnemy(slime1, currentFrame);
          player.HitEnemy(slime2, currentFrame);

       for (std::vector<Missle>::iterator axeM = plMisseles.begin(); axeM != plMisseles.end(); ++axeM)
        {
          axeM->Draw(screenBuffer, tiles, axe);
          enemyP1 = slime1.GetPosition();
          enemyP2 = slime2.GetPosition();
          playerM = axeM->GetPosition();
          if (abs(enemyP1.x - playerM.x-16)< 32 && abs(enemyP1.y - playerM.y+16) < 42)
          {
            slime1.HitByPlayer(currentFrame);
            break;
          }
          if (abs(enemyP2.x - playerM.x-16)< 42 && abs(enemyP2.y - playerM.y+16) < 42)
          {
            slime2.HitByPlayer(currentFrame);
            break;
          }
        }  
      }

      } else
      {
        for (std::vector<Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it)
        {
          player.HitEnemy(*it, currentFrame);
          pos1 = it->GetPosition();
          if (abs(pos1.x - plPos.x)< 350 && abs(pos1.y - plPos.y) < 350)
          {
            if(it->Attacced(player))
            {
              gameEnded = 1;
            }
          }
          else
          {
            it->Patrol();
          }
          it->Draw(screenBuffer, tiles, currentFrame, birbAnim);
        }
        for (std::vector<Missle>::iterator axeM = plMisseles.begin(); axeM != plMisseles.end(); ++axeM)
        {
          Point playerM, enemyP;
          axeM->Draw(screenBuffer, tiles, axe);
          for (std::vector<Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it)
          {
            enemyP = it->GetPosition();
            playerM = axeM->GetPosition();
            if (abs(enemyP.x - playerM.x)< 20 && abs(enemyP.y - playerM.y) < 20)
            {
              it->HitByPlayer(currentFrame);
              break;
            }
          }
        }  
      }
      player.Draw(screenBuffer, tiles, currentFrame, Panim);

      putTorches(screenBuffer,torchAnim,torchPosition,currentFrame);
      torchPosition.clear();
      if (currentFrame-treasureFoundTime<0.7)
      {
        for( y= 0; y < tileSize; ++y)
        {
          for( x= 0; x < tileSize; ++x)
          { 
            pix = treasureFound.GetPixel(x, tileSize-y);
            if (pix.r != 0 || pix.g != 0 || pix.b !=0)
            {
              screenBuffer.PutPixel( pos.x + x, pos.y+tileSize + y,pix);
            }
          }
        } 
      }
    }
    if (slime1.Killed() && slime2.Killed())
    {
      endOfTheGame(screenBuffer);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
      glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;

      glfwSwapBuffers(window);
      sleep(4); 
      glfwSetWindowShouldClose(window, GL_TRUE);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
    glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
    glfwSwapBuffers(window);

    if (gameEnded)
    {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
      glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, gameover.Data()); GL_CHECK_ERRORS;

      glfwSwapBuffers(window);
      sleep(4); 
      glfwSetWindowShouldClose(window, GL_TRUE);
    }
  }
  mapFile.close();
  glfwTerminate();
  return 0;
}
