//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//
// Desc: This file stores the player object class. This class performs tasks
//	   such as player movement, some minor physics as well as rendering.
//
//-----------------------------------------------------------------------------

#ifndef _CPLAYER_H_
#define _CPLAYER_H_

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "Sprite.h"
#include "Bullet.h"

//-----------------------------------------------------------------------------
// Main Class Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CPlayer (Class)
// Desc : Player class handles all player manipulation, update and management.
//-----------------------------------------------------------------------------
class CPlayer
{
public:
	//-------------------------------------------------------------------------
	// Enumerators
	//-------------------------------------------------------------------------
	enum DIRECTION 
	{ 
		DIR_FORWARD	 = 1, 
		DIR_BACKWARD	= 2, 
		DIR_LEFT		= 4, 
		DIR_RIGHT	   = 8, 
	};

	enum ESpeedStates
	{
		SPEED_START,
		SPEED_STOP
	};

	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
			 CPlayer(const BackBuffer *pBackBuffer, int x);
	virtual ~CPlayer();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void					Update( float dt );
	int playerIsDead();
	void					Draw();
	void					Move(ULONG ulDirection);
	void					MoveEnemy(ULONG ulDirection);
	void					RotateLeft();
	void					RotateRight();
	Vec2&					Position();
	Vec2&					Velocity();
	std::string owner;
	DIRECTION mFacingDirection;

	const BackBuffer *mBackBuffer;
	void SetScore(int newScore);
	void SetLives(int newLives);
	int GetLives();
	int GetScore();
	void IncreaseScore();
	void DecreaseLives();
	boolean isDead;
	void					Explode();
	bool					AdvanceExplosion();
	boolean					Shoot(std::string owner);
	int						fireCooldown = 600;
	int						fireCooldown1 = 50;
	Sprite*					m_pSprite;
	int& frameCounter();
private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------
	
	ESpeedStates			m_eSpeedState;
	float					m_fTimer;
	const BackBuffer		*pBackBuffer;
	int mLives;
	int mScore;
	bool					m_bExplosion;
	AnimatedSprite*			m_pExplosionSprite;
	int						m_iExplosionFrame;
	
};

#endif // _CPLAYER_H_