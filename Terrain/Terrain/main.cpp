
#if defined(_DEBUG)
#define D3D_DEBUG_INFO
#endif

#include <windows.h>
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <crtdbg.h>
#include "camera.h"
#include "mouse.h"

#include "terrain.h"
#include "skyBox.h"
#include "geomipmapping.h"
#include "QuadTree.h"
#include "Rain.h"
#include "BillBoard.h"

#include "water.h"
#include "Emitter.h"
#include "EmitterConfig.h"
#include "ParticleConfig.h"
#include "SnowParticle.h"
#include "Convert.h"
#include "CPlantCollect.h"
#include "LensFlare.h"
#include "InfTerrain.h"
#include <utility>
#include "CWater.h"
using namespace std;
#define LOG(str) MessageBox(0, str, _T("Error"), MB_ICONSTOP)

const float CAMERA_ORIGIN[3] = {10, 50.0f, 5.0f};
const float CAMERA_SPEED = 50;

const float GROUND_PLANE_DECAL_TILE_FACTOR = 4.0f;
const float GROUND_PLANE_SIZE = 1024.0f;

const DWORD WND_EX_STYLE = 0;
const DWORD WND_STYLE = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;


bool isWire;

SnowParticle *snow = 0;
QuadTree *terrain = 0;
Rain *rain = 0;

InfTerrain *infTerrain = 0;

CWater *cwater = 0;
Emitter *emitter = 0;

SkyBox *skyBox = 0;

CPlantCollect *plantCollect = 0;

BillBoard *tree = 0;
Water *water = 0;

LensFlare *lensFlare = 0;

D3DVERTEXELEMENT9 VERTEX_ELEMENTS[] =
{
    {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
    {0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
    D3DDECL_END()
};

struct Vertex
{
    float x, y, z;
    float u, v;
};

UINT g_windowWidth;
UINT g_windowHeight;
HWND g_hWnd;
IDirect3D9 *g_pDirect3D;
IDirect3DDevice9 *g_pDevice;

IDirect3DTexture9 *g_pDecalMap;
D3DXVECTOR3 g_cameraVelocity;
Camera g_camera;
Mouse g_mouse;
bool g_hasFocus;
bool g_flightSimModeEnabled;

void Cleanup()
{
 

    if (g_pDecalMap)
    {
        g_pDecalMap->Release();
        g_pDecalMap = 0;
    }

    if (g_pDevice)
    {
        g_pDevice->Release();
        g_pDevice = 0;
    }

    if (g_pDirect3D)
    {
        g_pDirect3D->Release();
        g_pDirect3D = 0;
    }

    g_mouse.detach();
}

bool InitD3D()
{
    HRESULT hr = 0;
    D3DPRESENT_PARAMETERS params = {0};
    D3DDISPLAYMODE desktop = {0};

    g_pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);

    if (!g_pDirect3D)
        return false;

    // Just use the current desktop display mode.
    hr = g_pDirect3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &desktop);

    if (FAILED(hr))
    {
        g_pDirect3D->Release();
        g_pDirect3D = 0;
        return false;
    }

    // Setup Direct3D for windowed rendering.
    params.BackBufferWidth = 0;
    params.BackBufferHeight = 0;
    params.BackBufferFormat = desktop.Format;
    params.BackBufferCount = 1;
    params.MultiSampleType = D3DMULTISAMPLE_NONE;
    params.MultiSampleQuality = 0;
    params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    params.hDeviceWindow = g_hWnd;
    params.Windowed = TRUE;
    params.EnableAutoDepthStencil = TRUE;
    params.AutoDepthStencilFormat = D3DFMT_D24S8;
    params.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    params.FullScreen_RefreshRateInHz = 0;
    params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

    // Most modern video cards should have no problems creating pure devices.
    hr = g_pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
            D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,
            &params, &g_pDevice);

    if (FAILED(hr))
    {
        // Fall back to software vertex processing for less capable hardware.
        hr = g_pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
                g_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params, &g_pDevice);
    }

    if (FAILED(hr))
    {
        g_pDirect3D->Release();
        g_pDirect3D = 0;
        return false;
    }    

    // Start by enabling bilinear texture filtering. This will be our default.
    g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    D3DCAPS9 caps;

    // Prefer anisotropic texture filtering if it's supported.
    if (SUCCEEDED(g_pDevice->GetDeviceCaps(&caps)))
    {
        if (caps.RasterCaps & D3DPRASTERCAPS_ANISOTROPY)
        {
            g_pDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, caps.MaxAnisotropy);
            g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
        }

		
    }

    // Setup some initial render states.
    g_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    g_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    return true;
}

bool InitScene()
{
    HRESULT hr = 0;
    
	rain = new Rain(g_pDevice);

	plantCollect = new CPlantCollect(g_pDevice);

	ParticleConfig pConfig;
	pConfig.color = 0xffffffff;
	pConfig.lifeTime = 10.0f;
	pConfig.textureNames.push_back("shine.bmp");

	EmitterConfig eConfig;
	eConfig.maxNumParticles = 5000;
	eConfig.numParticleToAdd = 1024;
	eConfig.position = D3DXVECTOR3(0, 10, 0);

	emitter = new Emitter(g_pDevice, &eConfig, &pConfig);


	ParticleConfig psConfig;
	psConfig.color = 0xffffffff;
	psConfig.lifeTime = 10.0f;

	for (int i = 0; i < 6; i++)
	{
		psConfig.textureNames.push_back("snow"+Convert::Int2Str(i+1)+".jpg");
	}
	

	lensFlare = new LensFlare(g_pDevice, &g_camera);

	snow = new SnowParticle(g_pDevice, &psConfig, &eConfig);

	infTerrain = new InfTerrain(g_pDevice, &g_camera);

	
	terrain = new QuadTree(g_pDevice, "media/heightmap.raw", 1, 0.25);

	terrain->LoadDetailTexture("media/detailMap.tga");

	terrain->LoadTile(LOWEST_TILE, "media/lowestTile.tga");
	terrain->LoadTile(LOW_TILE, "media/lowTile.tga");
	terrain->LoadTile(HIGH_TILE, "media/highTile.tga");
	terrain->LoadTile(HIGHEST_TILE, "media/highestTile.tga");

	terrain->GenerateTextureMap(256);

	infTerrain->SetTerrain(terrain);
	

	water = new Water(g_pDevice, "media/water2.tga");


	cwater = new CWater(g_pDevice, &g_camera);
	cwater->Create(129, 129, 0.0f, 0.0f, 40.0f);


	tree = new BillBoard(g_pDevice);

    hr = D3DXCreateTextureFromFile(g_pDevice, _T("floor_decal_map.dds"), &g_pDecalMap);

    if (FAILED(hr))
    {
        LOG(_T("Failed to load texture!"));
        return false;
    }

	//terrain->SetLightColor(D3DXVECTOR3(1.0f, 1, 1));
	//terrain->SlopeLighting(1, 1, 0.2f, 0.9f, 15);
	//terrain->CalculateLighting();

  
	//terrain->LoadTexture("media/grass_1.tga");

    g_camera.setMode(Camera::CAMERA_MODE_FIRST_PERSON);

    g_camera.perspective(Camera::DEFAULT_FOVX,
        static_cast<float>(g_windowWidth) / static_cast<float>(g_windowHeight),
        1.0f, GROUND_PLANE_SIZE * 2.0f);

    g_camera.setPosition(CAMERA_ORIGIN[0], CAMERA_ORIGIN[1], CAMERA_ORIGIN[2]);

	skyBox = new SkyBox(g_pDevice, "media/top.jpg", "media/skybox_bottom.tga", 
		"media/left.jpg", "media/right.jpg",
		"media/front.jpg", "media/back.jpg");


	plantCollect->Create(50, 100, 10);

	return true;
}

bool Init()
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int halfScreenWidth = screenWidth ;
    int halfScreenHeight = screenHeight;
    int left = (screenWidth - halfScreenWidth) / 2;
    int top = (screenHeight - halfScreenHeight) / 2;
    RECT rc = {0};


    SetRect(&rc, left, top, left + halfScreenWidth, top + halfScreenHeight);
    AdjustWindowRectEx(&rc, WND_STYLE, FALSE, WND_EX_STYLE);
    MoveWindow(g_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
    
    GetClientRect(g_hWnd, &rc);
    g_windowWidth = (rc.right - rc.left) / 2;
    g_windowHeight = (rc.bottom - rc.top)/2;

    if (!InitD3D())
    {
        LOG(_T("Direct3D initialization failed!"));
        return false;
    }

    if (!InitScene())
    {
        LOG(_T("Scene initialization failed!"));
        return false;
    }
 
    if (!g_mouse.attach(g_hWnd))
    {
        LOG(_T("Mouse initialization failed!"));
        return false;
    }

    g_mouse.setPosition(g_windowWidth / 2, g_windowHeight / 2);
    
	

	infTerrain->Init();

	//terrain->Init();

	bool ds = emitter->Init();

	tree->LoadTexture("media/tree.dds");
	tree->SetPos(D3DXVECTOR3(50, 0, 50));

	snow->Init();

	lensFlare->Create(D3DXVECTOR3(-160, 700, 600), D3DXVECTOR2(1600, 875));


    return true;
}

void UpdateFrame(float elapsedTimeSec)
{
    
    switch (g_camera.mode)
    {
    case Camera::CAMERA_MODE_FIRST_PERSON:
        g_camera.mouseRotate(g_mouse.getXPosRelative(), g_mouse.getYPosRelative(), 0.0f);
        break;

    case Camera::CAMERA_MODE_FLIGHT_SIM:
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
            g_camera.mouseRotate(g_mouse.getXPosRelative(), -g_mouse.getYPosRelative(), 0.0f);
        else
            g_camera.mouseRotate(0.0f, -g_mouse.getYPosRelative(), g_mouse.getXPosRelative());
        break;

    default:
        break;
    }
    
	int ro_SPEED = 2;
    g_mouse.setPosition(g_windowWidth / 2, g_windowHeight / 2);

   
	if (GetAsyncKeyState(VK_UP) & 0x8000)
		g_camera.rotate(0, -ro_SPEED, 0);

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		g_camera.rotate(0, ro_SPEED, 0);

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		g_camera.rotate(ro_SPEED, 0, 0);

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		g_camera.rotate(-ro_SPEED, 0, 0);
	
    if (GetAsyncKeyState(_T('W')) & 0x8000)
        g_cameraVelocity.z = CAMERA_SPEED;

    if ( GetAsyncKeyState(_T('S')) & 0x8000)
        g_cameraVelocity.z = -CAMERA_SPEED;

    if ( GetAsyncKeyState(_T('D')) & 0x8000)
        g_cameraVelocity.x = CAMERA_SPEED;

    if ( GetAsyncKeyState(_T('A')) & 0x8000)
        g_cameraVelocity.x = -CAMERA_SPEED;

    if (GetAsyncKeyState(_T('E')) & 0x8000)
        g_cameraVelocity.y = CAMERA_SPEED;

    if (GetAsyncKeyState(_T('Q')) & 0x8000)
        g_cameraVelocity.y = -CAMERA_SPEED;

	if (GetAsyncKeyState(_T('F')) & 0x8000)
		isWire = isWire ? false : true;

    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        PostMessage(g_hWnd, WM_CLOSE, 0, 0);

    g_cameraVelocity *= elapsedTimeSec;
    g_camera.move(g_cameraVelocity.x, 0.0f, g_cameraVelocity.z);

    if (g_cameraVelocity.y != 0.0f)
        g_camera.move(Camera::WORLD_YAXIS, g_cameraVelocity.y);

	D3DXVECTOR3 pos = infTerrain->GetCameraLogicPos(&g_camera, terrain);

	//g_camera.setPosition(pos.x, terrain->GetHeight(pos.x, pos.z) + 10, pos.z);

    g_cameraVelocity.x = g_cameraVelocity.y = g_cameraVelocity.z = 0.0f;

	skyBox->Update(elapsedTimeSec * 50);

	//g_camera
	infTerrain->Update();

	//terrain->UpdateQuatTree(&g_camera);

	water->Update(elapsedTimeSec * 50);
	rain->Update(elapsedTimeSec * 10);

	tree->Update(elapsedTimeSec, g_camera);

	emitter->Update(elapsedTimeSec * 100);

	snow->Update(elapsedTimeSec);
}


void DrawScene()
{
	//plantCollect->Draw();

	infTerrain->Draw();
	//skyBox->Render();

	//snow->Render();

	//lensFlare->Draw();

}
void DrawFrame()
{
    g_pDevice->Clear(0, 0,
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_ZBUFFER,
        D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

    if (SUCCEEDED(g_pDevice->BeginScene()))
    {
        g_pDevice->SetTransform(D3DTS_PROJECTION, &g_camera.getProjMatrix());
        g_pDevice->SetTransform(D3DTS_VIEW, &g_camera.getViewMatrix());
        
        //g_pDevice->SetTexture(0, g_pDecalMap);
       

		//rain->Render();

		terrain->SetWireframe(true);
		//cwater->BeginReflect();
		//DrawScene();
		//cwater->EndReflect();

		g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(100, 149, 237, 255), 1.0f, 0);
		
		DrawScene();
		// 绘制水体表面
		//cwater->Draw(100);

		//water->Render();
		//emitter->Render();

		
        g_pDevice->EndScene();
        g_pDevice->Present(0, 0, 0, 0);
    }
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_ACTIVATE:
        switch (wParam)
        {
        default:
            break;

        case WA_ACTIVE:
        case WA_CLICKACTIVE:
            g_mouse.hideCursor(true);
            g_hasFocus = true;
            break;

        case WA_INACTIVE:
            g_mouse.hideCursor(false);
            g_hasFocus = false;
            break;
        }
        break;

    case WM_CHAR:
        if (wParam == _T(' '))
        {
            g_flightSimModeEnabled = !g_flightSimModeEnabled;

            if (g_flightSimModeEnabled)
            {
                g_camera.setMode(Camera::CAMERA_MODE_FLIGHT_SIM);
                SetWindowText(hWnd, _T("D3D Camera Demo 1 - [flight mode]"));
            }
            else
            {
                g_camera.setMode(Camera::CAMERA_MODE_FIRST_PERSON);
                g_camera.setPosition(g_camera.m_pos.x, CAMERA_ORIGIN[1], g_camera.m_pos.z);
                SetWindowText(hWnd, _T("D3D Camera Demo 1 - [first person mode]"));
            }
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        g_mouse.handleMsg(hWnd, msg, wParam, lParam);
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#if defined _DEBUG
    _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif

    MSG msg = {0};
    WNDCLASSEX wcl = {0};

    wcl.cbSize = sizeof(wcl);
    wcl.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wcl.lpfnWndProc = WindowProc;
    wcl.cbClsExtra = 0;
    wcl.cbWndExtra = 0;
    wcl.hInstance = hInstance;
    wcl.hIcon = 0;
    wcl.hCursor = LoadCursor(0, IDC_ARROW);
    wcl.hbrBackground = 0;
    wcl.lpszMenuName = 0;
    wcl.lpszClassName = _T("D3DWindowClass");
    wcl.hIconSm = 0;

    if (!RegisterClassEx(&wcl))
        return 0;

    g_hWnd = CreateWindowEx(WND_EX_STYLE, wcl.lpszClassName,
                _T("D3D Camera Demo 1 - [first person mode]"), WND_STYLE,
                0, 0, 0, 0, 0, 0, wcl.hInstance, 0);

    if (g_hWnd)
    {
        if (Init())
        {
            // Run the current thread on the same processor. For single core
            // CPUs this will have no affect. On multi-core CPUs this ensures
            // all timing is performed on the same core and prevents any timing
            // anomalies from occurring.
            //SetProcessorAffinity();

            INT64 freq = 0;
            INT64 lastTime = 0;
            INT64 currentTime = 0;
            float elapsedTimeSec = 0.0f;
            float timeScale = 0.0f;

            QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&freq));
            QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&lastTime));
            timeScale = 1.0f / freq;

            ShowWindow(g_hWnd, nShowCmd);
            UpdateWindow(g_hWnd);

            while (true)
            {
                if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
                {
                    if (msg.message == WM_QUIT)
                        break;

                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                else if (g_hasFocus)
                {
                    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));
                    elapsedTimeSec = (currentTime - lastTime) * timeScale;
                    lastTime = currentTime;

                    UpdateFrame(elapsedTimeSec);
                    DrawFrame();
                }
                else
                {
                    WaitMessage();
                }
            }

            Cleanup();
        }
        
        UnregisterClass(wcl.lpszClassName, hInstance);
    }

    return static_cast<int>(msg.wParam);
}