#include "Player.h"
#include "Enemy.h"
bool Player::Moved() const
{
  if(coords.x == old_coords.x && coords.y == old_coords.y)
    return false;
  else
    return true;
}
bool Player::MovedUp() const
{
  if(coords.y <= old_coords.y)
    return false;
  else
    return true;
}
bool Player::MovedDown() const
{
  if(coords.y >= old_coords.y)
    return false;
  else
    return true;
}
bool Player::MovedLeft() const
{
  if(coords.x >= old_coords.x)
    return false;
  else
    return true;
}
bool Player::MovedRight() const
{
  if(coords.x <= old_coords.x)
    return false;
  else
    return true;
}

Point Player::GetPosition(void)
{
  Point position;
  position.x = coords.x;
  position.y = coords.y;
  return position;
}

void Player::SetPosition(Point p)
{
  coords.x = p.x;
  coords.y = p.y; 
  old_coords = coords;
}


void Player::StopLeft()
{
  if (coords.x < old_coords.x)
  {
    coords.y = old_coords.y;
    coords.x = old_coords.x;
  }
}
void Player::StopRight()
{
  if (coords.x > old_coords.x)
  {
    coords.y = old_coords.y;
    coords.x = old_coords.x;
  }
}
void Player::StopUp()
{
  if (coords.y > old_coords.y)
  {
    coords.y = old_coords.y;
    coords.x = old_coords.x;
  }
}
void Player::StopDown()
{
  if (coords.y < old_coords.y)
  {
    coords.x = old_coords.x;
    coords.y = old_coords.y;
  }
}

void Player::ProcessInput(MovementDir dir)
{
  int move_dist = move_speed * 1;
  switch(dir)
  {
    case MovementDir::UP:
      old_coords.y = coords.y;
      coords.y += move_dist;
      break;
    case MovementDir::DOWN:
      old_coords.y = coords.y;
      coords.y -= move_dist;
      break;
    case MovementDir::LEFT:
      old_coords.x = coords.x;
      coords.x -= move_dist;
      break;
    case MovementDir::RIGHT:
      old_coords.x = coords.x;
      coords.x += move_dist;
      break;
    default:
      break;
  }
}
 
void Player::HitEnemy(Enemy &en, double frame)
{
  Point enPos = en.GetPosition();
  if (attaced)
  {
    if (MovedRight() && abs(coords.y - enPos.y)<=1
        && (enPos.x - coords.x <= tileSize*2))
    {
      en.HitByPlayer(frame);
      return;      
    } else
    if (MovedLeft()  && abs(coords.y - enPos.y)<=1
        &&(coords.x - enPos.x <= tileSize*2))
    {
      en.HitByPlayer(frame);
      return;
    } else
    if (abs(coords.y - enPos.y) <= tileSize && 
        abs(coords.x - enPos.x) <= tileSize)
    {
      en.HitByPlayer(frame);
      return;
    }
  }
}
void Player::HitEnemy(Boss &en, double frame)
{
  Point enPos = en.GetPosition();
  if (attaced)
  {
    if (MovedRight() && abs(coords.y - enPos.y)<=1
        && (enPos.x - coords.x <= tileSize*4))
    {
      en.HitByPlayer(frame);
      return;      
    } else
    if (MovedLeft()  && abs(coords.y - enPos.y)<=1
        &&(coords.x - enPos.x <= tileSize*4))
    {
      en.HitByPlayer(frame);
      return;
    } else
    if (abs(coords.y - enPos.y) <= tileSize*2 && 
        abs(coords.x - enPos.x) <= tileSize)
    {
      en.HitByPlayer(frame);
      return;
    }
  }
}
void Player::Attac(double frame)
{
  if (frame - lastAttacTime>0.7)
  {
    attaced = 1;
    lastAttacTime = frame;
  }
}

void Player::Draw(Image &screen, Image &background, double frame , PlayerAnim anim)
{
/*    for(int y = old_coords.y; y <= old_coords.y + tileSize; ++y)
    {
      for(int x = old_coords.x; x <= old_coords.x + tileSize; ++x)
      {
        screen.PutPixel( x,  y, 
        background.GetPixel(x, y));
      }
    }
*/
  Pixel a;
  Image *charFile;

  if (MovedLeft())
    if(frame - lastAttacTime<0.3)
    {
      charFile = anim.leftAttac;
      if(frame - lastAttacTime>0.1)
        attaced =0;
    }else
    {
      if ((int)(10*frame)%2)
        charFile = anim.left;
      else
       charFile = anim.leftMove; 
    } else 
    if (MovedRight())
      if(frame - lastAttacTime<0.3)
      {
        charFile = anim.rightAttac;
        if(frame - lastAttacTime>0.1)
          attaced =0;
      }else
      {
        if ((int)(10*frame)%2)
          charFile = anim.right;
        else
          charFile = anim.rightMove;
      }
  else
    if(frame - lastAttacTime<0.3)
    {
      charFile = anim.swipeAttac;
      if(frame - lastAttacTime>0.1)
        attaced =0;
    }else
    {
      if ((int)frame%2)
        charFile = anim.right;
      else 
        charFile = anim.left;
    }

  for(int y = 0; y <tileSize-1; ++y)
  {
    for(int x = 0; x < tileSize; ++x)
    {
      a = charFile->GetPixel(x ,y);
      if (a.r != 0 || a.g != 0 || a.b !=0)
        screen.PutPixel(coords.x+ x, coords.y + y, a);
    }
  }
  old_coords = coords;
}
