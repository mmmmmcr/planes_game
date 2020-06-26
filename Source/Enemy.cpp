#include "Enemy.h"
#include "CGameApp.h"

extern CGameApp g_App;
Enemy::Enemy(const BackBuffer *pBackBuffer)
{
	m_pSprite = new Sprite("data/en.bmp", RGB(0xff, 0x00, 0xff));
	m_pSprite->setBackBuffer(pBackBuffer);
	m_pSprite->mPosition = Vec2(100, 100);

	m_pSprite1 = new Sprite("data/en.bmp", RGB(0xff, 0x00, 0xff));
	m_pSprite1->setBackBuffer(pBackBuffer);
	m_pSprite1->mPosition = Vec2(300, 100);
}


Enemy::~Enemy()
{
}

void Enemy::move()
{
	this->m_pSprite->mPosition.y += 1;
	this->m_pSprite1->mPosition.y += 1;
}

void Enemy:: Shoot()
{
	if (shootCooldown < 5) {
		Bullet bullet(g_App.m_pBBuffer, "down");
		bullet.Move();
		bullet.m_pSprite->mPosition.y = this->m_pSprite->mPosition.y + m_pSprite->height() / 1.5;
		bullet.m_pSprite->mPosition.x = this->m_pSprite->mPosition.x;
		g_App.bulletsOnScreen.push_back(bullet);
		shootCooldown = 500;
	}
}



