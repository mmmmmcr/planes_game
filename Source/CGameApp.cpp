//-----------------------------------------------------------------------------
// File: CGameApp.cpp
//
// Desc: Game Application class, this is the central hub for all app processing
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CGameApp Specific Includes
//-----------------------------------------------------------------------------
#include "CGameApp.h"
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <fstream>
extern HINSTANCE g_hInst;

//-----------------------------------------------------------------------------
// CGameApp Member Functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CGameApp () (Constructor)
// Desc : CGameApp Class Constructor
//-----------------------------------------------------------------------------
CGameApp::CGameApp()
{
	// Reset / Clear all required values
	m_hWnd			= NULL;
	m_hIcon			= NULL;
	m_hMenu			= NULL;
	m_pBBuffer		= NULL;
	m_pPlayer		= NULL;
	m_pPlayer1		= NULL;
	m_LastFrameRate = 0;
}

//-----------------------------------------------------------------------------
// Name : ~CGameApp () (Destructor)
// Desc : CGameApp Class Destructor
//-----------------------------------------------------------------------------
CGameApp::~CGameApp()
{
	// Shut the engine down
	ShutDown();
}

//-----------------------------------------------------------------------------
// Name : InitInstance ()
// Desc : Initialises the entire Engine here.
//-----------------------------------------------------------------------------
bool CGameApp::InitInstance( LPCTSTR lpCmdLine, int iCmdShow )
{
	// Create the primary display device
	if (!CreateDisplay()) { ShutDown(); return false; }

	// Build Objects
	if (!BuildObjects()) 
	{ 
		MessageBox( 0, _T("Failed to initialize properly. Reinstalling the application may solve this problem.\nIf the problem persists, please contact technical support."), _T("Fatal Error"), MB_OK | MB_ICONSTOP);
		ShutDown(); 
		return false; 
	}

	// Set up all required game states
	SetupGameState();

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : CreateDisplay ()
// Desc : Create the display windows, devices etc, ready for rendering.
//-----------------------------------------------------------------------------
bool CGameApp::CreateDisplay()
{
	LPTSTR			WindowTitle		= _T("GameFramework");
	LPCSTR			WindowClass		= _T("GameFramework_Class");
					Width			= 800;
					Height			= 600;
	RECT			rc;
	WNDCLASSEX		wcex;


	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= CGameApp::StaticWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInst;
	wcex.hIcon			= LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= WindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

	if(RegisterClassEx(&wcex)==0)
		return false;

	// Retrieve the final client size of the window
	::GetClientRect( m_hWnd, &rc );
	m_nViewX		= rc.left;
	m_nViewY		= rc.top;
	m_nViewWidth	= rc.right - rc.left;
	m_nViewHeight	= rc.bottom - rc.top;

	m_hWnd = CreateWindow(WindowClass, WindowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, NULL, NULL, g_hInst, this);

	if (!m_hWnd)
		return false;

	// Show the window
	ShowWindow(m_hWnd, SW_MAXIMIZE);
	//ShowWindow(m_hWnd, SW_MAXIMIZE);

	// Success!!
	return true;
}

//-----------------------------------------------------------------------------
// Name : BeginGame ()
// Desc : Signals the beginning of the physical post-initialisation stage.
//		From here on, the game engine has control over processing.
//-----------------------------------------------------------------------------
int CGameApp::BeginGame()
{
	MSG		msg;

	// Start main loop
	while(true) 
	{
		// Did we recieve a message, or are we idling ?
		if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) 
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		} 
		else 
		{
			// Advance Game Frame.
			FrameAdvance();

		} // End If messages waiting
	
	} // Until quit message is receieved

	return 0;
}

//-----------------------------------------------------------------------------
// Name : ShutDown ()
// Desc : Shuts down the game engine, and frees up all resources.
//-----------------------------------------------------------------------------
bool CGameApp::ShutDown()
{
	// Release any previously built objects
	ReleaseObjects ( );
	
	// Destroy menu, it may not be attached
	if ( m_hMenu ) DestroyMenu( m_hMenu );
	m_hMenu		 = NULL;

	// Destroy the render window
	SetMenu( m_hWnd, NULL );
	if ( m_hWnd ) DestroyWindow( m_hWnd );
	m_hWnd		  = NULL;
	
	// Shutdown Success
	return true;
}

//-----------------------------------------------------------------------------
// Name : StaticWndProc () (Static Callback)
// Desc : This is the main messge pump for ALL display devices, it captures
//		the appropriate messages, and routes them through to the application
//		class for which it was intended, therefore giving full class access.
// Note : It is VITALLY important that you should pass your 'this' pointer to
//		the lpParam parameter of the CreateWindow function if you wish to be
//		able to pass messages back to that app object.
//-----------------------------------------------------------------------------
LRESULT CALLBACK CGameApp::StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	// If this is a create message, trap the 'this' pointer passed in and store it within the window.
	if ( Message == WM_CREATE ) SetWindowLong( hWnd, GWL_USERDATA, (LONG)((CREATESTRUCT FAR *)lParam)->lpCreateParams);

	// Obtain the correct destination for this message
	CGameApp *Destination = (CGameApp*)GetWindowLong( hWnd, GWL_USERDATA );
	
	// If the hWnd has a related class, pass it through
	if (Destination) return Destination->DisplayWndProc( hWnd, Message, wParam, lParam );
	
	// No destination found, defer to system...
	return DefWindowProc( hWnd, Message, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Name : DisplayWndProc ()
// Desc : The display devices internal WndProc function. All messages being
//		passed to this function are relative to the window it owns.
//-----------------------------------------------------------------------------
LRESULT CGameApp::DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	static UINT			fTimer;	

	// Determine message type
	switch (Message)
	{
		case WM_CREATE:
			break;
		
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		
		case WM_SIZE:
			if ( wParam == SIZE_MINIMIZED )
			{
				// App is inactive
				m_bActive = false;
			
			} // App has been minimized
			else
			{
				// App is active
				m_bActive = true;

				// Store new viewport sizes
				m_nViewWidth  = LOWORD( lParam );
				m_nViewHeight = HIWORD( lParam );
		
			
			} // End if !Minimized

			break;

		case WM_LBUTTONDOWN:
			// Capture the mouse
			SetCapture( m_hWnd );
			GetCursorPos( &m_OldCursorPos );
			break;

		case WM_LBUTTONUP:
			// Release the mouse
			ReleaseCapture( );
			break;

		case WM_KEYDOWN:
			switch(wParam)
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			case 'Q':
				fTimer = SetTimer(m_hWnd, 1, 50, NULL);
				m_pPlayer->Explode();
				break;
			case 'N':
				m_pPlayer1->RotateLeft();
				break;
			case 'M':
				m_pPlayer1->RotateRight();
				break;
			case 'R':
				m_pPlayer->RotateLeft();
				break;
			case 'T':
				m_pPlayer->RotateRight();
				break;
			case VK_RETURN:
				fTimer = SetTimer(m_hWnd, 1, 50, NULL);
				m_pPlayer1->Explode();
				break;
			case 'F':
				m_pPlayer->Shoot("player");
				break;
			case 'L':
				m_pPlayer1->Shoot("player");
				break;
			case '1':
				saveGame();
			case '2':
				loadGame();
			}
			

			break;

		case WM_TIMER:
			switch(wParam)
			{
			case 1:
				if(!m_pPlayer->AdvanceExplosion())
					KillTimer(m_hWnd, 1);
				if (!m_pPlayer1->AdvanceExplosion())
					KillTimer(m_hWnd, 1);
			}
			break;

		case WM_COMMAND:
			break;

		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);

	} // End Message Switch
	
	return 0;
}

//-----------------------------------------------------------------------------
// Name : BuildObjects ()
// Desc : Build our demonstration meshes, and the objects that instance them
//-----------------------------------------------------------------------------
bool CGameApp::BuildObjects()
{
	m_pBBuffer = new BackBuffer(m_hWnd, m_nViewWidth, m_nViewHeight);
	m_pPlayer = new CPlayer(m_pBBuffer, 1);
	m_pPlayer1 = new CPlayer(m_pBBuffer, 1);
	 noEnemies = 14; 
	addEnemies(noEnemies);

	//Enemy en(m_pBBuffer);
	//enemyOnScreen.push_back(en);


	if(!m_imgBackground.LoadBitmapFromFile("data/background.bmp", GetDC(m_hWnd)))
		return false;

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : SetupGameState ()
// Desc : Sets up all the initial states required by the game.
//-----------------------------------------------------------------------------
void CGameApp::SetupGameState()
{
	m_pPlayer->Position() = Vec2(100, 500);
	m_pPlayer1->Position() = Vec2(300, 500);

}

//-----------------------------------------------------------------------------
// Name : ReleaseObjects ()
// Desc : Releases our objects and their associated memory so that we can
//		rebuild them, if required, during our applications life-time.
//-----------------------------------------------------------------------------
void CGameApp::ReleaseObjects( )
{
	if(m_pPlayer != NULL)
	{
		delete m_pPlayer;
		m_pPlayer = NULL;
	}

	if(m_pBBuffer != NULL)
	{
		delete m_pBBuffer;
		m_pBBuffer = NULL;
	}
}

//-----------------------------------------------------------------------------
// Name : FrameAdvance () (Private)
// Desc : Called to signal that we are now rendering the next frame.
//-----------------------------------------------------------------------------
void CGameApp::FrameAdvance()
{
	static TCHAR FrameRate[ 50 ];
	static TCHAR TitleBuffer[ 255 ];

	// Advance the timer
	m_Timer.Tick( );


	// Skip if app is inactive
	if ( !m_bActive ) return;
	
	// Get / Display the framerate
	if ( m_LastFrameRate != m_Timer.GetFrameRate() )
	{
		m_LastFrameRate = m_Timer.GetFrameRate( FrameRate, 50 );
		sprintf_s( TitleBuffer, _T("Game : %s	Lives: %d-%d		Score: Player1:%d Player2:%d "), FrameRate, m_pPlayer->GetLives(), m_pPlayer1->GetLives(), m_pPlayer->GetScore(), m_pPlayer1->GetScore());
		SetWindowText( m_hWnd, TitleBuffer );
		
		if (!m_pPlayer->GetLives()) {
			::MessageBox(m_hWnd, "Second  Wins", "Game over", MB_OK);
			::PostQuitMessage(0);
		}
		else if (!m_pPlayer1->GetLives()) {
			::MessageBox(m_hWnd, "First  Wins", "Game over", MB_OK);
			::PostQuitMessage(0);
		}

	} // End if Frame Rate Altered

	


	// Poll & Process input devices
	ProcessInput();

	// Animate the game objects
	AnimateObjects();

	removeDead();

	// Drawing the game objects
	DrawObjects();
}

//-----------------------------------------------------------------------------
// Name : ProcessInput () (Private)
// Desc : Simply polls the input devices and performs basic input operations
//-----------------------------------------------------------------------------
void CGameApp::ProcessInput( )
{
	static UCHAR pKeyBuffer[ 256 ];
	ULONG		Direction = 0;
	ULONG		DirectionP2 = 0;
	POINT		CursorPos;
	float		X = 0.0f, Y = 0.0f;

	// Retrieve keyboard state
	if ( !GetKeyboardState( pKeyBuffer ) ) return;

	// Check the relevant keys
	if ( pKeyBuffer[ VK_UP	] & 0xF0 ) Direction |= CPlayer::DIR_FORWARD;
	if ( pKeyBuffer[ VK_DOWN  ] & 0xF0 ) Direction |= CPlayer::DIR_BACKWARD;
	if ( pKeyBuffer[ VK_LEFT  ] & 0xF0 ) Direction |= CPlayer::DIR_LEFT;
	if ( pKeyBuffer[ VK_RIGHT ] & 0xF0 ) Direction |= CPlayer::DIR_RIGHT;

	if (pKeyBuffer['W'] & 0xF0) DirectionP2 |= CPlayer::DIR_FORWARD;
	if (pKeyBuffer['S'] & 0xF0) DirectionP2 |= CPlayer::DIR_BACKWARD;
	if (pKeyBuffer['A'] & 0xF0) DirectionP2 |= CPlayer::DIR_LEFT;
	if (pKeyBuffer['D'] & 0xF0) DirectionP2 |= CPlayer::DIR_RIGHT;

	
	// Move the player
	
	m_pPlayer1->Move(Direction);
	m_pPlayer->Move(DirectionP2);
	for (auto enemy : m_enemies) {
		enemy->MoveEnemy(CPlayer::DIR_BACKWARD);
	}


	// Now process the mouse (if the button is pressed)
	if ( GetCapture() == m_hWnd )
	{
		// Hide the mouse pointer
		SetCursor( NULL );

		// Retrieve the cursor position
		GetCursorPos( &CursorPos );

		// Reset our cursor position so we can keep going forever :)
		SetCursorPos( m_OldCursorPos.x, m_OldCursorPos.y );

	} // End if Captured
}

short int CGameApp::Sprite_Collide(Sprite * p1, Sprite * p2) {

	RECT r;
	r.left = p1->mPosition.x - p1->width() / 2;
	r.right = p1->mPosition.x + p1->width() / 2;
	r.top = p1->mPosition.y - p1->height() / 2;
	r.bottom = p1->mPosition.y + p1->height() / 2;

	RECT r2;
	r2.left = p2->mPosition.x - p2->width() / 2;
	r2.right = p2->mPosition.x + p2->width() / 2;
	r2.top = p2->mPosition.y - p2->height() / 2;
	r2.bottom = p2->mPosition.y + p2->height() / 2;


	if (r.right > r2.left && r.left < r2.right && r.bottom>r2.top && r.top < r2.bottom) {
		return true;
	}
	if (r.left > r2.right && r.right < r2.left && r.bottom>r2.top && r.top < r2.bottom) {
		return true;
	}

	return false;

};

//-----------------------------------------------------------------------------
// Name : AnimateObjects () (Private)
// Desc : Animates the objects we currently have loaded.
//-----------------------------------------------------------------------------
void CGameApp::AnimateObjects()
{
	m_pPlayer->Update(m_Timer.GetTimeElapsed());
	m_pPlayer1->Update(m_Timer.GetTimeElapsed());
	for (auto enem : m_enemies) {
		enem->Update(m_Timer.GetTimeElapsed());
	}
}

//-----------------------------------------------------------------------------
// Name : DrawObjects () (Private)
// Desc : Draws the game objects
//-----------------------------------------------------------------------------
void CGameApp::DrawBackground()
{
	static int currentY = m_imgBackground.Height();

	static size_t lastTime = ::GetTickCount();
	size_t currentTime = ::GetTickCount();

	if (currentTime - lastTime > 250)
	{
		lastTime = currentTime;
		currentY -= 10;
		if (currentY < 0)
			currentY = m_imgBackground.Height();
	}

	m_imgBackground.Paint(m_pBBuffer->getDC(), 0, currentY);
}


void CGameApp::DrawObjects()
{
	static UINT			fTimer;
	m_pBBuffer->reset();

	DrawBackground();
		
	for (auto enem : m_enemies)
	{
		enem->Draw();
		enem->frameCounter()++;
	}

	//Draw Players
	m_pPlayer->Draw();
	
	m_pPlayer1->Draw();

	

	srand(time(NULL));

	for (auto enem : m_enemies)
	{
		if (enem->frameCounter() == 1200)
		{
			enem->frameCounter() = rand() % 1000;
			enem->Shoot("enemy");
		}
	}

	for (auto en : m_enemies) {
		if (Sprite_Collide(en->m_pSprite, m_pPlayer1->m_pSprite)) {
			fTimer = SetTimer(m_hWnd, 1, 70, NULL);
			m_pPlayer1->Explode();
			m_pPlayer1->m_pSprite->mPosition = Vec2(300, 500);
			en->Explode();
			en->m_pSprite->mPosition.x = 100;
		}
		if (Sprite_Collide(en->m_pSprite, m_pPlayer->m_pSprite)) {
			fTimer = SetTimer(m_hWnd, 1, 70, NULL);
			m_pPlayer->Explode();
			m_pPlayer->m_pSprite->mPosition = Vec2(600, 500);
			en->Explode();
			en->m_pSprite->mPosition.x = 100;
			
		}
	}


	if (Sprite_Collide(m_pPlayer->m_pSprite, m_pPlayer1->m_pSprite)) {
		fTimer = SetTimer(m_hWnd, 1, 70, NULL);
		m_pPlayer1->Explode();
		m_pPlayer1->m_pSprite->mPosition = Vec2(50, 600);
		m_pPlayer->Explode();
		m_pPlayer->m_pSprite->mPosition = Vec2(200, 600);
	}

	for (auto it : bulletsOnScreen) {
		it.m_pSprite->draw();
		it.Move();
		if (Sprite_Collide(it.m_pSprite, m_pPlayer1->m_pSprite)) {
				fTimer = SetTimer(m_hWnd, 1, 70, NULL);
				m_pPlayer1->Explode();
				it.m_pSprite->mPosition.x = -100;
				m_pPlayer->IncreaseScore();
				m_pPlayer1->m_pSprite->mPosition = Vec2(50, 600);
		}
		if (Sprite_Collide(it.m_pSprite, m_pPlayer->m_pSprite)) {
			fTimer = SetTimer(m_hWnd, 1, 70, NULL);
			m_pPlayer->Explode();
			it.m_pSprite->mPosition.x = -100;
			m_pPlayer1->IncreaseScore();
			m_pPlayer->m_pSprite->mPosition = Vec2(200, 600);
			
		}

		for (auto en : m_enemies) {
			if (Sprite_Collide(it.m_pSprite, en->m_pSprite)) {
				fTimer = SetTimer(m_hWnd, 1, 70, NULL);
				en->Explode();
				en->m_pSprite->mPosition.x = -100;
				it.m_pSprite->mPosition.x = 100;
				m_pPlayer->IncreaseScore();
				m_pPlayer1->IncreaseScore();
			}
			
		}

	}
	for (auto it : bulletsOnScreen1) {
		it.m_pSprite->draw();
		it.EnemyBullet();
		if (Sprite_Collide(it.m_pSprite, m_pPlayer1->m_pSprite)) {
			fTimer = SetTimer(m_hWnd, 1, 70, NULL);
			m_pPlayer1->Explode();
			it.m_pSprite->mPosition.y = -100;
			m_pPlayer1->m_pSprite->mPosition = Vec2(300, 500);
		}
		if (Sprite_Collide(it.m_pSprite, m_pPlayer->m_pSprite)) {
			fTimer = SetTimer(m_hWnd, 1, 70, NULL);
			m_pPlayer->Explode();
			it.m_pSprite->mPosition.y = -100;
			m_pPlayer->m_pSprite->mPosition = Vec2(100, 500);
		}

	}

	bulletsOnScreen.remove_if([](Bullet c) {
		return c.m_pSprite->mPosition.y < 10 ? true : false;
	});

	m_pBBuffer->present();
}

void CGameApp::saveGame()
{
	std::ofstream save("savegame/savegame.save");
	
	save << m_pPlayer->Position().x << " " << m_pPlayer->Position().y << " " << m_pPlayer->GetLives() << " ";
	save << m_pPlayer->GetScore() << "\n";
	save << m_pPlayer1->Position().x << " " << m_pPlayer1->Position().y << " " << m_pPlayer1->GetLives() << " ";
	save << m_pPlayer1->GetScore() << "\n";

	save.close();
}

void CGameApp::loadGame()
{
	std::ifstream save("savegame/savegame.save");
	
	double cdx, cdy;
	int livesP1, livesP2, score, noEn;

	save >> cdx >> cdy >> livesP1 >> score;
	m_pPlayer->Position() = Vec2(cdx, cdy);
	m_pPlayer->SetScore(score);
	m_pPlayer->SetLives(livesP1);

	save >> cdx >> cdy >> livesP2 >> score;
	m_pPlayer1->Position() = Vec2(cdx, cdy);
	m_pPlayer1->SetScore(score);
	m_pPlayer1->SetLives(livesP2);

}

void CGameApp::addEnemies(int noEnemies)
{
	srand(time(NULL));
	Vec2 position = Vec2(300, 50);

	for (int it = 0; it != noEnemies; ++it) {
		m_enemies.push_back(new CPlayer(m_pBBuffer, 2));
		m_enemies.back()->Position() = position;
		m_enemies.back()->Velocity() = Vec2(0, 0);
		m_enemies.back()->frameCounter() = rand() % 1000;

		position.x += 150;
		if (position.x > GetSystemMetrics(SM_CXSCREEN) - 300) {
			position.x = 300;
			position.y += 150;
		}

	}
}


void CGameApp::removeDead()
{
	for (auto enem : m_enemies)
	{
		if (enem->playerIsDead())
		{
			m_enemies.remove(enem);
			break;
		}
	}
}

