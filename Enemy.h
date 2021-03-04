#ifndef MAIN_ENEMY_H
#define MAIN_ENEMY_H
#include <vector>
#include "Player.h"
struct Missle
{ 
  Missle(MovementDir dir, Point pos = {.x = 10, .y = 10})
  {
    coords.x = pos.x;
    coords.y = pos.y;

  switch (dir)
  {
    case MovementDir::UP:
      movePattern = {.x = 0*move_speed, .y = 1*move_speed};
      break;
    case MovementDir::DOWN:
      movePattern = {.x = 0*move_speed, .y = -1*move_speed};
      break;
    case MovementDir::LEFT:
      movePattern = {.x = -1*move_speed, .y = 0*move_speed};
      break;
    case MovementDir::RIGHT:
      movePattern = {.x = 1*move_speed, .y = 0*move_speed};
      break;
  }
  coords=pos;
}
  void Draw(Image &screen, Image &background , Image &im);
  bool HitEnemy(Enemy &en);
  bool HitPlayer(Player &pl);
  Point GetPosition(void);


private:
  Point coords {.x = 512, .y = 32};
  int move_speed = 20;
  Point movePattern;
};
struct Enemy
{ 
  Enemy(Point pos = {.x = 32, .y = 32}, int hp = 1) :
                 coords(pos), old_coords(coords), health(hp) {};
  bool Killed();
  void Patrol();
  bool Attacced(Player &player);
  void Draw(Image &screen, Image &background, double frame , Anim enemyAnim);
  Point GetPosition(void);
  void SetPosition(Point p);
  void HitByPlayer(double frame);

private:
  Point coords {.x = 512, .y = 32};
  Point old_coords {.x = 512, .y = 32};
  Point patrolStart {.x =coords.x , .y = coords.y};
  Point patrolEnd {.x = coords.x+128, .y = coords.y+128};
  bool patrolUp = 1;
  double deathTime = 0;
  int health =1;
  int move_speed = 6; 

};
struct BossAnimation
{
  Image *move1;
  Image *move2;
  Image *attac;
  Image *hit;
  Image *death;
};

struct Boss //: public Enemy
{ 
  Boss(Point pos = {.x = 512, .y = 32}, int hp = 15) :
                 coords(pos), old_coords(coords), health(hp) {};
  void ChangeForm();
  void Draw(Image &screen, Image &background, double frame , BossAnimation anim);
  bool Killed();
  void Shoot(std::vector<Missle> &misseles, double frame);
  bool Attacced(Player &player);
  Point GetPosition(void);
  void HitByPlayer(double frame);
  private:
  Point coords {.x = 512, .y = 32};
  Point old_coords {.x = 512, .y = 32};
  Point patrolStart {.x =coords.x , .y = coords.y};
  Point patrolEnd {.x = coords.x+128, .y = coords.y+128};
  double lastAttac =-1;
  double deathTime = -1;
  double hitTime = -1;
  int health =150;
  int move_speed = 1;    

};


#endif //MAIN_ENEMY_H
