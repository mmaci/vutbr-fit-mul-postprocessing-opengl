//--------------------------------------------------------
//
// Projekt: 5. cvicenie k predmetu MUL (DirectX)
// Autor:   Michal Kucis, ikucis@fit.vutbr.cz
// Rok:     2013
// Popis:   Projekt demonstruje jednoduchy program napisany
//          v Direct3D. V ramci cvicenia je nutne napisat
//          vlastny pixel shader v jazyku HLSL Shader model 3.0.
// Odkazy: 
//     Referencna priruka k HLSL
//        http://msdn.microsoft.com/en-us/library/windows/desktop/bb509638(v=vs.85).aspx
//     Zoznam vstavanych prikazov pre HLSL:
//        http://msdn.microsoft.com/en-us/library/windows/desktop/ff471376(v=vs.85).aspx
//     Velmi pekny tutorial k HLSL:
//        http://khason.net/blog/hlsl-pixel-shader-effects-tutorial/
//
//--------------------------------------------------------



#include <windows.h> // <- obsahuje vsetko podstatne pre programovanie vo Win32API
#include <vfw.h>  // <- Video for Windows
#include <d3d9.h>
#include <d3dx9.h>



//=================================================================
// Linkujuce kniznice:

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9d.lib")
#pragma comment(lib, "Vfw32.lib")



//=================================================================
// Makra:

#define USE_VIDEO


// rozmery vytvoreneho okna:
#define DEFAULT_WINDOW_WIDTH	512
#define DEFAULT_WINDOW_HEIGHT	512



// rozmery textury:
#define TEXTURE_SIZE 512



//=================================================================
// Globalne premenne:



// handle hlavneho okna, pomocou neho sa pracuje s oknom
HWND				g_hWnd = NULL;



// Pre podrobnosti o LPDIRECT3D* pouzite http://msdn.microsoft.com/en-us/library/windows/desktop/ff471470(v=vs.85).aspx
LPDIRECT3D9			g_pD3D = NULL;
LPDIRECT3DDEVICE9	g_pd3dDevice = NULL;
LPDIRECT3DTEXTURE9	g_pTexture = NULL;
LPDIRECT3DVERTEXDECLARATION9 g_pVertexDeclaration = NULL;
LPDIRECT3DVERTEXSHADER9	g_vertexShader = NULL;
LPDIRECT3DPIXELSHADER9	g_pixelShader = NULL;
LPD3DXCONSTANTTABLE             g_constantTable  = NULL; 


// Struktury vyuzivane vo Video for Windows:
// - otvoreny avi subor:
PAVIFILE g_pvideofile;
// - hlavicka s informaciami o videu:
BITMAPINFOHEADER *g_pbi;
// - struktura umoznujuca pristup k jednotlivym framom:
PGETFRAME g_pgf;



//=================================================================
// Prototypy funkcii:



// Handler na odchytavanie sprav pre hlavne okno:
LRESULT CALLBACK WindowProc (HWND hWnd, UINT nMsg, WPARAM wparam, LPARAM lparam);

// Funkcia vytvori hlavne okno:
HWND CreateMyWindow ();

// Inicializuje Direct3D zariadenie:
bool InitD3D ();

// Vytvori 'pomocne' Direct3D objekty na vykreslovanie ako texturu, pixel shader, vertex shader a vertex deklaraciu:
bool InitD3DMyObjects ();

// Funkcia obsluhuje vykreslovanie okna:
void Render ();

// Hlavna funkcia programu:
int APIENTRY WinMain (HINSTANCE, HINSTANCE, LPSTR, int);



//=================================================================
// Definicie funkcii:



// Vytvor world-view-projection maticu:
// Zoznam funkcii  pre modifikaciu D3DXMATRIX sa nachadza: 
//		http://msdn.microsoft.com/en-us/library/windows/desktop/bb172972(v=vs.85).aspx
//      pre nas su zaujimave funkcie s prefixom D3DXMATRIX
D3DXMATRIXA16 GetWorldViewProjectionMat ()
{
    D3DXMATRIXA16 matRotX;
	D3DXMATRIXA16 matRotY;
	D3DXMATRIXA16 matPersp;
	D3DXMATRIXA16 matWorldViewProj;

	// Nastavi jednotlive transformacne matice:
	float miliseconds = ((float)GetTickCount()/1000.0f);
	D3DXMatrixRotationX (&matRotX, cos(miliseconds*3)/10);
	D3DXMatrixRotationY (&matRotY, sin(miliseconds*3)/10); 
	D3DXMatrixPerspectiveFovLH (&matPersp, 3.14f/2, 1, 0.1f, 0.9f ); 

	// Zlozime matice:
	matWorldViewProj = matRotX*matRotY*matPersp;
	return matWorldViewProj;
}



// Handler hlavneho okna:
LRESULT CALLBACK WindowProc (HWND hWnd, UINT nMsg, WPARAM wparam, LPARAM lparam)
{
	switch (nMsg)
	{
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage (0);
		break;
	}
	return DefWindowProc(hWnd, nMsg, wparam, lparam);
}




// Vytvori hlavne okno:
HWND CreateMyWindow ()
{
	// Zaregistruje okno:
    WNDCLASSEX wc;
    wc.cbSize        = sizeof (WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WindowProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = GetModuleHandle (NULL);
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "MY_WINDOW";
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

	// Vytvori okno:
    HWND hWnd = CreateWindowEx (
        WS_EX_CLIENTEDGE,
        "MY_WINDOW",
        "5. cvicenie z MUL",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
        NULL, NULL, GetModuleHandle(NULL), NULL);
    if (hWnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

	// Zobrazi okno:
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	g_hWnd = hWnd;
	
	return hWnd;
}



// Vytvori a nastavi Direct3D zariadenie pre spravne vykreslovanie:
bool InitD3D ()
{
	HRESULT hr;

	if (NULL == (g_pD3D = Direct3DCreate9 (D3D_SDK_VERSION)))
		return false;

	D3DDISPLAYMODE d3ddm; 
	if (FAILED (hr = g_pD3D->GetAdapterDisplayMode (D3DADAPTER_DEFAULT, &d3ddm)))    
        return false;     
    
	// Vytvori d3d zariadenie:
    D3DPRESENT_PARAMETERS d3dpp;   
    ZeroMemory( &d3dpp, sizeof(d3dpp) );   
    d3dpp.Windowed               = TRUE;   
    d3dpp.SwapEffect             = D3DSWAPEFFECT_COPY;   
    d3dpp.BackBufferFormat       = d3ddm.Format;   
    d3dpp.EnableAutoDepthStencil = TRUE;   
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;   
    hr = g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,   
                               D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,   
                               &d3dpp, &g_pd3dDevice );                                        
    if (FAILED(hr))                                          
        return false;     
          
	// vypne odstranovanie odvratenych trojuholnikov pri renderingu
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );   
       
    return true; 
}



// Vytvori pixel a vertex shader, vertex deklaraciu a nacita texturu
bool InitD3DMyObjects ()
{
	HRESULT hr;

	// Vytvori objekt Vertex declaration:
	D3DVERTEXELEMENT9 dwDecl[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, 
										  D3DDECLUSAGE_POSITION, 0},
		{0,  12, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, 
										  D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	g_pd3dDevice->CreateVertexDeclaration(dwDecl, &g_pVertexDeclaration);

	
#ifndef USE_VIDEO
	// Nacita texturu zo suboru:
	hr = D3DXCreateTextureFromFile(g_pd3dDevice, "DH.png", &g_pTexture);
	if (FAILED(hr))
		return false;
#else
	// Vytvori texturu, do ktorej sa budu nahravat framy z videa:
	hr = g_pd3dDevice->CreateTexture (TEXTURE_SIZE, TEXTURE_SIZE, 0, 
			D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8,
			D3DPOOL_DEFAULT, &g_pTexture, NULL);
	if (FAILED(hr))
		return false;
#endif

	// Vytvori vertex buffer:
    LPD3DXBUFFER buffer = NULL;
	LPD3DXBUFFER errors = NULL;
	hr = D3DXCompileShaderFromFile( "stud.shaders",   //filepath
                                        NULL,          //macro's            
                                        NULL,          //includes           
                                        "vs_main",     //main function      
                                        "vs_3_0",      //shader profile     
                                        0,             //flags              
                                        &buffer,         //compiled operations
                                        &errors,          //errors
                                        &g_constantTable);         //constants
	if (FAILED(hr))
	{
		char *ErrorMessage = (char*) errors->GetBufferPointer();
		MessageBox(g_hWnd, ErrorMessage, "Shader: Error!", MB_ICONEXCLAMATION | MB_OK);
        return false;
	}	

	// Vytvori pixel shader:
	g_pd3dDevice->CreateVertexShader((DWORD*)buffer->GetBufferPointer(), &g_vertexShader);
    buffer->Release();

    hr = D3DXCompileShaderFromFile( "stud.shaders",   //filepath
                                        NULL,          //macro's            
                                        NULL,          //includes           
                                        "ps_main",     //main function      
                                        "ps_3_0",      //shader profile     
                                        0,             //flags              
                                        &buffer,         //compiled operations
                                        &errors,          //errors
                                        NULL);         //constants
    if (FAILED(hr))
 	{
		char *ErrorMessage = (char*) errors->GetBufferPointer();
		MessageBox(g_hWnd, ErrorMessage, "Shader: Error!", MB_ICONEXCLAMATION | MB_OK);
		return false;
	}	
	g_pd3dDevice->CreatePixelShader ((DWORD*)buffer->GetBufferPointer(), &g_pixelShader);
    buffer->Release();

	return true;
}




// struktura, ktora popisuje vertex vo vertex buffery:
struct CUSTOMVERTEX
{
	float x,y,z,u,v;
};



// Obsluhuje vykreslovanie:
void Render ()
{
	// Vymaze vykreslovany color buffer a z-buffer
    g_pd3dDevice->Clear (0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,128,255), 1.0f, 0);

	// Zacne vykreslovanie:
    g_pd3dDevice->BeginScene(); 

	// Nastavi vertex bufferu tranformacnu maticu:
	g_constantTable->SetMatrix(g_pd3dDevice, "WorldViewProj", &GetWorldViewProjectionMat());
  
	// Jasne a priamociare:
	g_pd3dDevice->SetVertexShader(g_vertexShader);
    g_pd3dDevice->SetPixelShader(g_pixelShader);
	g_pd3dDevice->SetTexture (0, g_pTexture);
	g_pd3dDevice->SetVertexDeclaration (g_pVertexDeclaration);
	
	// Textura ma iny rozmer ako ma rozmery video, preto sa vyuzije z textury len ta cast,
	// ktora je vyuzita na video..
	// Vykresli quad do sceny:
	float texCoordMult = 321.0f/TEXTURE_SIZE;
	CUSTOMVERTEX pQuad[] = {{-0.5f,-0.5f,0.5f,0,texCoordMult},
                     { 0.5f,-0.5f,0.5f,texCoordMult,texCoordMult},
                     {-0.5f,0.5f,0.5f,0,0},
                     { 0.5f,0.5f,0.5f,texCoordMult,0}};
    g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pQuad,sizeof(CUSTOMVERTEX));

	// Ukoncime vykreslovanie a nastavime vykreslovanie novo vykresleneho back bufferu:
    g_pd3dDevice->EndScene();   
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );   
}


// Likvidacia D3D objektov:
void DestroyD3DMyObjects ()
{
	g_constantTable->Release ();
	g_constantTable = NULL;
	g_pixelShader->Release ();
	g_pixelShader = NULL;
	g_vertexShader->Release ();
	g_vertexShader = NULL;
	g_pTexture->Release ();
	g_pTexture = NULL;
	g_pVertexDeclaration->Release ();
	g_pVertexDeclaration = NULL;
}


void DestroyD3D ()
{
	g_pd3dDevice->Release ();
	g_pd3dDevice = NULL;
	g_pD3D->Release ();
	g_pD3D = NULL;
}



#ifdef USE_VIDEO
bool InitVfW ()
{
	// Aky subor sa pouzije:
	LPCTSTR szFile = "clock.avi";

	// Otvori avi subor:
	if(AVIFileOpen(&g_pvideofile, szFile, OF_READ, NULL))
		return false;
	
	// Ziska video stream:
	PAVISTREAM pavi;
	if(AVIFileGetStream(g_pvideofile, &pavi, streamtypeVIDEO, 0))
		return false;

	// Ziska info o video streame:
	AVISTREAMINFO si;
	if(AVIStreamInfo(pavi, &si, sizeof(AVISTREAMINFO)))
		return false;
	LONG lStreamSize;
	AVIStreamFormatSize(pavi, 0, &lStreamSize);
	g_pbi = (BITMAPINFOHEADER *)GlobalAlloc(GMEM_FIXED, lStreamSize);
	if(!g_pbi)
		return false;

	// Skontroluje, ci je video vhodne:
	if(AVIStreamReadFormat(pavi, 0, g_pbi, &lStreamSize))
		return false;
	if(g_pbi->biBitCount != 24)
		return false;

	// Vsetko je ok, ziskame strukturu PGETFRAME:
	g_pbi->biCompression = BI_RGB;
	g_pgf = AVIStreamGetFrameOpen(pavi, g_pbi);
	if(!g_pgf)
		return false;

	return true;
}

// hack:
#define CALC_BI_STRIDE(width,bitcount)  ((((width * bitcount) + 31) & ~31) >> 3)

// obsahuje podstatne informacie o frame
struct VIDEOFRAME
{
	char* data;
	int height, width;
	size_t stride;
	bool valid;
};
// ziska frame z videa na pozicii pos
VIDEOFRAME GetFrameVfW (int pos)
{
	// ziska frame:
	char* frame = (char*) AVIStreamGetFrame(g_pgf, pos);
	VIDEOFRAME vf;
	// ak mame NULL pointer, ziadame frame, ktory neexistuje, vratime chybu:
	if (!frame)
	{
		vf.valid = false;
		return vf;
	}
	// ziskame data:
	void *data = (char *)frame + sizeof(BITMAPINFOHEADER);
	int width = g_pbi->biWidth;
	int height = g_pbi->biHeight;
	// offset na dalsi riadok vo frame v bytoch:
	size_t stride = CALC_BI_STRIDE(g_pbi->biWidth,24);
	if(height > 0) // video moze mat nastavenu zapornu vysku, je nutne to osetrit
	{
		data = (char *)data + (height-1)*stride;
		stride *= -1;
	}
	else
	{
		height *= -1;
	}

	// nastavime clenske premenne vystupnej premennej
	vf.data = (char*) data;
	vf.width = width;
	vf.height = height;
	vf.stride = stride;
	vf.valid = true;
	return vf;
}

// uvolni naalokovane zdroje vyzadovane pre Video for Windows
void DestroyVfW ()
{
	GlobalFree(g_pbi);
	AVIFileRelease(g_pvideofile);
	AVIFileExit();
}

// nakopiruje data z VIDEOFRAME do Direct3DTextury
// toto nie je trivialna zalezitost, pretoze z VfW ziskavame data, ktore obsahuju RGB zlozku,
//   kde kazda pixel ma 24 bitov. Vytvorena Direct3DTextura obsahuje ale RGBA zlozky,
//   teda kazdy texel ma 32 bitov. Z tohto dovodu je nutne sa postarat o spravne kopirovanie
//   a je nutne kopirovat kazdy pixel zvlast...
void FillTexture (VIDEOFRAME* pvf, LPDIRECT3DTEXTURE9 pTex)
{
	// ziskame pocet riadkov a stlpcov, ktore musime prekopirovat:
	D3DSURFACE_DESC desc;
	pTex->GetLevelDesc (0, &desc);
	int w = min ((int)pvf->width, (int)desc.Width);
	int h = min ((int)pvf->height, (int)desc.Height);

	D3DLOCKED_RECT rect;
	
	// Do texelov Direct3DTextury nemozeme priamo pristupovat, pred pristupom, zamkneme:
	pTex->LockRect (0, &rect, NULL, D3DLOCK_DISCARD);
	for (int y = 0; y < h; y++)
	{
		// ziskame pociatok riadkov v texture a videoframe 
		char* pOutRGBA = ((char*)rect.pBits) + rect.Pitch*y;
		char* pInRGB = ((char*)pvf->data) + pvf->stride*y;
		for (int x = 0; x < w; x++)
		{
			// ziskame pozicie texelu a pixelu z textury a videoframu:
			char* in = pInRGB + 3*x;
			char* out = pOutRGBA + 4*x;

			memcpy (out, in, 3);
		}
	}
	// Pred pouzitim je nutne odomknut texturu
	pTex->UnlockRect(0);

}
#endif USE_VIDEO







int APIENTRY WinMain (HINSTANCE, HINSTANCE, LPSTR, int)
{
	if (! CreateMyWindow ())
		return -1;

	if (! InitD3D ())
		return -1;

	if (! InitD3DMyObjects ())
		return -1;

	MSG msg;

#ifdef USE_VIDEO
	if (! InitVfW ())
		return -1;
	// cislo videoframe:
	int pos = 0;
	// obsahuje kolko milisekund prebehlo:
	int tickCount = GetTickCount ();
	// obsahuje kolko tickov je zobrazovany jeden videoframe:
	int tickCountRatio = 800;
#endif
	while(TRUE)
	{
		// slucka spracovavania sprav, ktore prestane cyklik, ak sa pozaduje ukoncenie aplikacie
		// alebo sa ziadna sprava nenachadza vo fronte sprav
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if(msg.message == WM_QUIT)
			break;

#ifdef USE_VIDEO
		// Zisti o kolko snimkov sa video posunulo:
		int add = (GetTickCount() - tickCount) / tickCountRatio;
		tickCount += add * tickCountRatio;
		pos += add;

		// Ziska video frame:	
		VIDEOFRAME vf = GetFrameVfW (pos);
		if (! vf.valid)
		{
			// ak sa nepodari ziskat video frame, prehravanie videa sa dostalo na koniec
			// zaciname vykreslovat video od zaciatku:
			pos = 0;
			vf = GetFrameVfW (pos);
		}
		// Naplni texturu datami:
		FillTexture (&vf, g_pTexture);
#endif
		// Vykresli 'scenu':
		Render ();
	}

	// Upratanie:
#ifdef USE_VIDEO
	DestroyVfW ();
#endif
	DestroyD3DMyObjects ();
	DestroyD3D ();
	return 0;
}