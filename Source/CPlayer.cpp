//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//
// Desc: This file stores the player object class. This class performs tasks
//       such as player movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "CPlayer.h"
#include "CGameApp.h"

extern CGameApp	g_App;

//-----------------------------------------------------------------------------
// Name : CPlayer () (Constructor)
// Desc : CPlayer Class Constructor
//-----------------------------------------------------------------------------
CPlayer::CPlayer(const BackBuffer *pBackBuffer, int x) :
	mBackBuffer(pBackBuffer), mFacingDirection(DIRECTION::DIR_FORWARD), mLives(3), mScore(0)
{
	//m_pSprite = new Sprite("data/planeimg.bmp", "data/planemask.bmp");
	if (x == 2)
	{
		m_pSprite = new Sprite("data/en.bmp", RGB(0xff, 0x00, 0xff));
		m_pSprite->setBackBuffer(pBackBuffer);
	}
	else
	{
		m_pSprite = new Sprite("data/planeimgandmask.bmp", RGB(0xff, 0x00, 0xff));
		m_pSprite->setBackBuffer(pBackBuffer);
	}

	isDead = false;
	m_pSprite->setBackBuffer( pBackBuffer );
	m_eSpeedState = SPEED_STOP;
	m_fTimer = 0;

	// Animation frame crop rectangle
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 128;
	r.bottom = 128;

	m_pExplosionSprite	= new AnimatedSprite("data/explosion.bmp", "data/explosionmask.bmp", r, 16);
	m_pExplosionSprite->setBackBuffer( pBackBuffer );
	m_bExplosion		= false;
	m_iExplosionFrame	= 0;
}

//-----------------------------------------------------------------------------
// Name : ~CPlayer () (Destructor)
// Desc : CPlayer Class Destructor
//-----------------------------------------------------------------------------
CPlayer::~CPlayer()
{
	delete m_pSprite;
	delete m_pExplosionSprite;
}

void CPlayer::Update(float dt)
{
	// Update sprite
	m_pSprite->update(dt);


	// Get velocity
	double v = m_pSprite->mVelocity.Magnitude();

	// NOTE: for each async sound played Windows creates a thread for you
	// but only one, so you cannot play multiple sounds at once.
	// This creation/destruction of threads also leads to bad performance
	// so this method is not recommanded to be used in complex projects.

	// update internal time counter used in sound handling (not to overlap sounds)
	m_fTimer += dt;

	// A FSM is used for sound manager 
	switch(m_eSpeedState)
	{
	case SPEED_STOP:
		if(v > 35.0f)
		{
			m_eSpeedState = SPEED_START;
			PlaySound("data/jet-start.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		break;
	case SPEED_START:
		if(v < 25.0f)
		{
			m_eSpeedState = SPEED_STOP;
			PlaySound("data/jet-stop.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		else
			if(m_fTimer > 1.f)
			{
				PlaySound("data/jet-cabin.wav", NULL, SND_FILENAME | SND_ASYNC);
				m_fTimer = 0;
			}
		break;
	}

	// NOTE: For sound you also can use MIDI but it's Win32 API it is a bit hard
	// see msdn reference: http://msdn.microsoft.com/en-us/library/ms711640.aspx
	// In this case you can use a C++ wrapper for it. See the following article:
	// http://www.codeproject.com/KB/audio-video/midiwrapper.aspx (with code also)
}

void CPlayer::Draw()
{	
	//Cooldown of bullets
	if (fireCooldown1 > 1)
		fireCooldown1--;

	if (fireCooldown > 1)
		fireCooldown -- ;

	if(!m_bExplosion)
		m_pSprite->draw();
	else
		m_pExplosionSprite->draw();
}

void CPlayer::MoveEnemy(ULONG ulDirection) {
	auto x = m_pSprite->mPosition.x;
	m_pSprite->mPosition.y += .09;
}

void CPlayer::Move(ULONG ulDirection)
{
	if (ulDirection & CPlayer::DIR_LEFT)
		m_pSprite->mVelocity.x -= 3.1;

	if (m_pSprite->mPosition.x - m_pSprite->width() / 2 <= 0) {
		m_pSprite->mVelocity.x = 0;
		m_pSprite->mPosition.x += 1;
	}

	if (ulDirection & CPlayer::DIR_RIGHT)
		m_pSprite->mVelocity.x += 3.1;

	if (m_pSprite->mPosition.x + m_pSprite->width() / 2 >= GetSystemMetrics(SM_CXSCREEN)) {
		m_pSprite->mVelocity.x = 0;
		m_pSprite->mPosition.x -= 1;
	}

	if (ulDirection & CPlayer::DIR_FORWARD)
		m_pSprite->mVelocity.y -= 3.1;

	if (m_pSprite->mPosition.y - m_pSprite->height() / 2 <= 0) {
		m_pSprite->mVelocity.y = 0;
		m_pSprite->mPosition.y += 1;
	}

	if (ulDirection & CPlayer::DIR_BACKWARD)
		m_pSprite->mVelocity.y += 3.1;

	if (m_pSprite->mPosition.y + m_pSprite->height() >= GetSystemMetrics(SM_CYSCREEN)) {
		m_pSprite->mVelocity.y = 0;
		m_pSprite->mPosition.y -= 1;
	}

}


void CPlayer::RotateLeft()
{
	auto position = m_pSprite->mPosition;
	auto velocity = m_pSprite->mVelocity;

	delete m_pSprite;

	switch (mFacingDirection)
	{
	case DIRECTION::DIR_FORWARD:
		m_pSprite = new Sprite("data/planeimgandmaskleft.bmp", RGB(0xff, 0x00, 0xff));
		mFacingDirection = DIRECTION::DIR_LEFT;
		owner = "left";
		break;
	case DIRECTION::DIR_BACKWARD:
		m_pSprite = new Sprite("data/planeimgandmaskright.bmp", RGB(0xff, 0x00, 0xff));
		mFacingDirection = DIRECTION::DIR_RIGHT;
		owner = "right";
		break;
	case DIRECTION::DIR_LEFT:
		mFacingDirection = DIRECTION::DIR_BACKWARD;
		owner = "down";
		m_pSprite = new Sprite("data/planeimgandmaskdown.bmp", RGB(0xff, 0x00, 0xff));
		break;
	case DIRECTION::DIR_RIGHT:
		m_pSprite = new Sprite("data/planeimgandmask.bmp", RGB(0xff, 0x00, 0xff));
		owner = "forward";
		mFacingDirection = DIRECTION::DIR_FORWARD;
		break;
	}
	m_pSprite->mPosition = position;
	m_pSprite->mVelocity = velocity;
	m_pSprite->setBackBuffer(mBackBuffer);
}

void CPlayer::RotateRight()
{
	auto position = m_pSprite->mPosition;
	auto velocity = m_pSprite->mVelocity;

	delete m_pSprite;

	switch (mFacingDirection)
	{
	case DIRECTION::DIR_FORWARD:
		m_pSprite = new Sprite("data/planeimgandmaskright.bmp", RGB(0xff, 0x00, 0xff));
		mFacingDirection = DIRECTION::DIR_RIGHT;
		owner = "right";
		break;
	case DIRECTION::DIR_BACKWARD:
		m_pSprite = new Sprite("data/planeimgandmaskleft.bmp", RGB(0xff, 0x00, 0xff));
		mFacingDirection = DIRECTION::DIR_LEFT;
		owner = "left";
		break;
	case DIRECTION::DIR_LEFT:
		m_pSprite = new Sprite("data/planeimgandmask.bmp", RGB(0xff, 0x00, 0xff));
		mFacingDirection = DIRECTION::DIR_FORWARD;
		owner = "forward";
		break;
	case DIRECTION::DIR_RIGHT:
		m_pSprite = new Sprite("data/planeimgandmaskdown.bmp", RGB(0xff, 0x00, 0xff));
		mFacingDirection = DIRECTION::DIR_BACKWARD;
		owner = "down";
		break;
	}

	m_pSprite->mPosition = position;
	m_pSprite->mVelocity = velocity;
	m_pSprite->setBackBuffer(mBackBuffer);
}



Vec2& CPlayer::Position()
{
	return m_pSprite->mPosition;
}

Vec2& CPlayer::Velocity()
{
	return m_pSprite->mVelocity;
}



void CPlayer::Explode()
{
	mLives--;
	m_pExplosionSprite->mPosition = m_pSprite->mPosition;
	m_pExplosionSprite->SetFrame(0);
	PlaySound("data/explosion.wav", NULL, SND_FILENAME | SND_ASYNC);
	m_bExplosion = true;
}

int& CPlayer::frameCounter() {
	return m_pSprite->frameCounter;
}

int CPlayer::playerIsDead()
{
	return isDead;
}

bool CPlayer::AdvanceExplosion()
{
	if(m_bExplosion)
	{
		m_pExplosionSprite->SetFrame(m_iExplosionFrame++);
		if(m_iExplosionFrame==m_pExplosionSprite->GetFrameCount())
		{
			isDead = true;
			m_bExplosion = false;
			m_iExplosionFrame = 0;
			m_pSprite->mVelocity = Vec2(0,0);
			m_eSpeedState = SPEED_STOP;
			return false;
		}
	}

	return true;
}


boolean CPlayer::Shoot(std::string owner)
{
	if (fireCooldown1 < 5 && owner == "player") {

		Bullet bullet(g_App.m_pBBuffer);
		bullet.Move();
		bullet.m_pSprite->mPosition.y = this->m_pSprite->mPosition.y - m_pSprite->height() / 1.5;
		bullet.m_pSprite->mPosition.x = this->m_pSprite->mPosition.x;
		g_App.bulletsOnScreen.push_back(bullet);

	}
		 if (owner == "enemy" && fireCooldown < 5) {
			Bullet bullet(g_App.m_pBBuffer);
			bullet.EnemyBullet();
			bullet.m_pSprite->mPosition.y = this->m_pSprite->mPosition.y + m_pSprite->height() / 1.5;
			bullet.m_pSprite->mPosition.x = this->m_pSprite->mPosition.x;
			g_App.bulletsOnScreen1.push_back(bullet);
		}
		fireCooldown1 = 100;
		fireCooldown = 700;
		return true;
	
}

void CPlayer::SetScore(int newScore) {
	  mScore = newScore ;
}

void CPlayer :: SetLives(int newLives) {
	mLives = newLives;
}

void CPlayer::DecreaseLives()
{
	mLives--;
}

int CPlayer::GetScore()
{
	return mScore;
}

void CPlayer::IncreaseScore()
{
	mScore += 100;
}

int CPlayer::GetLives()
{
	return mLives;
}