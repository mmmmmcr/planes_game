//-----------------------------------------------------------------------------
// File: Bullet.cpp
//
// Desc: This file stores the bullet object class.
//
//-----------------------------------------------------------------------------

#ifndef _BULLET_H_
#define _BULLET_H_

#include <string>

#include "Main.h"
#include "Sprite.h"


class Bullet
{
public:
	enum DIRECTION
	{
		DIR_FORWARD = 1,
		DIR_BACKWARD = 2,
		DIR_LEFT = 4,
		DIR_RIGHT = 8,
	};
	Bullet(const BackBuffer *pBackBuffer, std::string owner = "");
	~Bullet();
	
	Sprite*					m_pSprite;
	Sprite*					m_pSprite1;
	DIRECTION facingBullet;
	std::string owner;
	const BackBuffer *mBackBuffer;
	void Move();
	void EnemyBullet();
	void Stop();
};

#endif // !_BULLET_H_



