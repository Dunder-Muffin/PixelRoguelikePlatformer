#include "Enemy.h"

int sgn(int val) 
{
  return (0 < val) - (val < 0);
}


bool Enemy::Killed()
{
  if (health <0)
    health =0;
  return !health;
} 

bool Boss::Killed()
{
  if (health <0)
    health =0;
  return !health;
} 
bool Enemy::Attacced(Player &player)
{
  int move_dist = move_speed * 1;
  Point playerPos = player.GetPosition();
  if (!Killed())
  {
    old_coords = coords;
    coords.x += sgn(playerPos.x - old_coords.x)*move_dist;
    coords.y += sgn(playerPos.y - old_coords.y)*move_dist;
    if (abs(playerPos.x - old_coords.x) <=move_dist && 
        abs(playerPos.y - old_coords.y)<=move_dist)
      return 1;
  }
  return 0;
}
bool Boss::Attacced(Player &player)
{
  int move_dist = move_speed * 1;
  Point playerPos = player.GetPosition();
  if (!Killed())
  {
    old_coords = coords;
    coords.x += sgn(playerPos.x - old_coords.x)*move_dist;
    coords.y += sgn(playerPos.y - old_coords.y)*move_dist;
    if (abs(playerPos.x - old_coords.x-40) <=60 && 
        abs(playerPos.y - old_coords.y-32)<=70)
      return 1;
  }
  return 0;
}
void Enemy::HitByPlayer(double frame)
{
  health--;
  if (health ==0)
  {
    deathTime = frame;
  }
}

void Enemy::Patrol()
{
  int move_dist = move_speed * 1;
  old_coords = coords;
  if (patrolUp)
  {
    if (coords.x<patrolEnd.x)
    {
      coords.x += move_dist;
      return;
    } else 
    if (coords.x>=patrolEnd.x)
    {
      if (coords.y<patrolEnd.y)
      {
        coords.y += move_dist;
        return;
      } else 
      if (coords.y>=patrolEnd.y)
      {
        patrolUp = 0;
      }
    }
  }
  else
  {
    if (coords.x>patrolStart.x)
    {
      coords.x -= move_dist;
      return;
    } else 
    if (coords.x<=patrolStart.x)
    {
      if (coords.y>patrolStart.y)
      {
        coords.y -= move_dist;
        return;
      } else 
      if (coords.y<=patrolStart.y)
      {
        patrolUp = 1;
        this->Patrol();
      }
    }
  }
}
void Enemy::Draw(Image &screen, Image &background, double frame , Anim anim)
{
/*  for(int y = old_coords.y; y <= old_coords.y + tileSize; ++y)
  {
    for(int x = old_coords.x; x <= old_coords.x + tileSize; ++x)
    {
      screen.PutPixel( x,  y, 
      background.GetPixel(x, y));
    }
  }*/
  Pixel pix;
  Image *enemyTex;
  if (Killed())
    enemyTex = anim.f3;
  else
  if ((int)(frame*3)%2)
    enemyTex = anim.f1;
  else 
    enemyTex = anim.f2;
  if (!Killed()||(abs(deathTime - frame )< 0.01))
  {
    for(int y = 0; y < enemyTex->Height(); ++y)
    {
      for(int x = 0; x < enemyTex->Width(); ++x)
      {
        pix = enemyTex->GetPixel(x ,y);
        if (pix.r != 0 || pix.g != 0 || pix.b !=0)
          screen.PutPixel(coords.x + x, coords.y + y, pix);
      }
    }
    old_coords = coords;
  }
}
Point Enemy::GetPosition(void)
{
  Point position;
  position.x = coords.x;
  position.y = coords.y;
  return position;
}

void Enemy::SetPosition(Point p)
{
  coords.x = p.x;
  coords.y = p.y; 
  old_coords = coords;
}

Point Missle::GetPosition(void)
{
  Point position;
  position.x = coords.x;
  position.y = coords.y;
  return position;
}
void Missle::Draw(Image &screen, Image &background , Image &im)
{
  Pixel a;
  if (coords.x<WINDOW_WIDTH-16 && coords.x>0
  && coords.y<WINDOW_HEIGHT-16 && coords.y>0)
  {
    for(int y = 0; y <tileSize/2; ++y)
    {
      for(int x = 0; x <tileSize/2; ++x)
      {
        a = im.GetPixel(x ,y);
        if (a.r != 0 || a.g != 0 || a.b !=0)
          screen.PutPixel(coords.x+ x, coords.y + y, a);
      }
    }
    coords.x += movePattern.x;
    coords.y += movePattern.y;
  }
}
Point Boss::GetPosition(void)
{
  Point position;
  position.x = coords.x; 
  position.y = coords.y;
  return position;
}
void Boss::HitByPlayer(double frame)
{
  hitTime = frame;
  health--;
  if (health ==0)
  {
    deathTime = frame;
  }
}
void Boss::Draw(Image &screen, Image &background, double frame , BossAnimation anim)
{
/*  for(int y = old_coords.y; y <= old_coords.y + tileSize; ++y)
  {
    for(int x = old_coords.x; x <= old_coords.x + tileSize; ++x)
    {
      screen.PutPixel( x,  y, 
      background.GetPixel(x, y));
    }
  }*/
  int xshift= 0;
  Pixel pix;
  Image *enemyTex;

  if (Killed())
    enemyTex = anim.death;
  else
  if (frame - hitTime<0.4)
  {
    enemyTex = anim.hit;
  } else
  if (frame - lastAttac<0.25 && frame - lastAttac>0.02)
  {
    enemyTex = anim.attac;
  } else
  if ((int)(frame*2)%2)
  {
    enemyTex = anim.move1;
  }
  else
  {
    enemyTex = anim.move2;
    xshift = 30;
  } 
  if (!Killed()||(abs(deathTime - frame )< 0.4))
  { 
    for(int y = 0; y < enemyTex->Height(); ++y)
    {
      for(int x = 0; x < enemyTex->Width(); ++x)
      {
        pix = enemyTex->GetPixel(x ,y);
        if (pix.r != 0 || pix.g != 0 || pix.b !=0)
          screen.PutPixel(coords.x + x - xshift, coords.y + y, pix);
      }
    }
    old_coords = coords;
  }
}
void Boss::Shoot(std::vector<Missle> &misseles, double frame)
{
  lastAttac = frame;
  misseles.clear();
  misseles.push_back(Missle({MovementDir::DOWN, {coords.x+16, coords.y+16}}));
  misseles.push_back(Missle({MovementDir::UP, {coords.x+16, coords.y+16}}));
  misseles.push_back(Missle({MovementDir::LEFT, {coords.x+16, coords.y+16}}));
  misseles.push_back(Missle({MovementDir::RIGHT, {coords.x+16, coords.y+16}}));
}