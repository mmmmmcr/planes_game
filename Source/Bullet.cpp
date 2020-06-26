#include "Bullet.h"

Bullet::Bullet(const BackBuffer *pBackBuffer, std::string owner):
	mBackBuffer(pBackBuffer)
{
	this->owner = owner;
	m_pSprite = new Sprite("data/bullet.bmp", RGB(0xff, 0x00, 0xff));
	m_pSprite->setBackBuffer(pBackBuffer);

}

Bullet::~Bullet()
{
}

void Bullet::Move()
{
	
		m_pSprite->mPosition.y -= 0.7;
	
}

void Bullet::EnemyBullet(){
	
	m_pSprite->mPosition.y += 0.5;
}

void Bullet::Stop()
{
	this->m_pSprite->mPosition.y -= .001;
}
