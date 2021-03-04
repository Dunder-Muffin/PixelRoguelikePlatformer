#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"

struct Enemy;
struct Boss; 
struct Point
{
  int x;
  int y;
};

enum class MovementDir
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

struct PlayerAnim
{
  Image *right;
  Image *rightMove;
  Image *rightAttac;
  Image *left;
  Image *leftMove;
  Image *leftAttac;
  Image *swipeAttac;
};
struct Player
{
  explicit Player(Point pos = {.x = 10, .y = 10}) :
                 coords(pos), old_coords(coords) {};
  void Attac(double frame);
  bool MovedUp() const;
  bool MovedDown() const;
  bool MovedLeft() const;
  bool MovedRight() const;
  bool Moved() const;
  void StopUp();
  void StopDown();
  void StopLeft();
  void StopRight();
  void HitEnemy(Enemy &en, double frame);
  void HitEnemy(Boss &en, double frame);
  void ProcessInput(MovementDir dir);
  void Draw(Image &screen, Image &background, double frame, PlayerAnim anim);
  Point GetPosition(void);
  void SetPosition(Point p);

private:
  Point coords {.x = 10, .y = 10};
  Point old_coords {.x = 10, .y = 10};
  Pixel color {.r = 255, .g = 255, .b = 0, .a = 255};
  int move_speed = 4; 
  bool attaced = 0;
  double lastAttacTime = -10;

};


#endif //MAIN_PLAYER_H
