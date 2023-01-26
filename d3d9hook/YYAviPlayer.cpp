#include "pch.h"
#include "YYAviPlayer.h"

void CAviTexture::Open(LPDIRECT3DDEVICE9 pDevice, LPCSTR szFile)
{
	// Opens The AVIFile Library
	AVIFileInit();
	// Opens The AVI Stream
	AVIStreamOpenFromFileA(&m_pavi, szFile, streamtypeVIDEO, 0, OF_READ, NULL); 
	// Get stream information  
	AVIStreamInfo(m_pavi, &m_si, sizeof(m_si)); 
	// Prepare to decompress video frames 
	m_pgf = AVIStreamGetFrameOpen(m_pavi, NULL); 
	if (!m_pgf)
	{
		std::cout << "ERROR in Open: AVIStreamGetFrameOpen!" << std::endl;
	}
	// Get the width and height 
	m_width = m_si.rcFrame.right - m_si.rcFrame.left;  m_height = m_si.rcFrame.bottom - m_si.rcFrame.top;  
	// Get the last frame of the stream 
	m_lastFrame = AVIStreamLength(m_pavi);  
	// Calculate rough milliseconds per frame
	m_mpf = AVIStreamSampleToTime(m_pavi, 1);  
	m_bmih.biSize        = sizeof (BITMAPINFOHEADER); 
	// Size of the BitmapInfoHeader 
	m_bmih.biPlanes      = 1;                         
	// Bitplanes  
	m_bmih.biBitCount    = 32;                        
	// Bits Format  
	m_bmih.biWidth       = 512; 
	// Width
	m_bmih.biHeight      = -512;  
	// Height (Invert) 
	m_bmih.biCompression = BI_RGB;        
	// Requested Mode = RGB 
	// Create a device context 
	m_hDC = CreateCompatibleDC(0);  
	// Create a DIB  
	m_hBitmap = CreateDIBSection(m_hDC, (BITMAPINFO*)(&m_bmih), DIB_RGB_COLORS, (void**)(&m_pData), NULL, NULL);  
	SelectObject(m_hDC, m_hBitmap); 
	// Select the bitmap into the device context  
	// Enable Dib Drawing  
	m_hdd = DrawDibOpen();  
	// Create the D3D texture
	/*if (FAILED(pDevice->CreateTexture(512, 512, 0, D3DUSAGE_AUTOGENMIPMAP, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_pTexture, NULL)))
	{
		m_pTexture = NULL;
		std::cout << " Couldn't create the texture for avi!" << std::endl;
	}*/

	
		
	// Zero data
	m_next  = 0; 
	m_frame = 0;
}

void CAviTexture::Close()
{  // Release the D3D texture 
	if( m_pTexture )  
	{   
		m_pTexture->Release();  
		m_pTexture = NULL; 
	}  
	DrawDibClose(m_hdd); 
	// Disable Dib Drawing 
	DeleteObject(m_hBitmap);
	// Delete the DIB object 
	AVIStreamGetFrameClose(m_pgf); 
	// Deallocate the GetFrame resources  
	AVIStreamRelease(m_pavi);  
	// Release the stream  
	AVIFileExit();                
	// Exit the AVIFile Library
}

void CAviTexture::Update(float frameTime){ 
	LPBITMAPINFOHEADER lpbi = NULL;  
	// Increase next based on frame time (milliseconds) 
	m_next += static_cast<int>(frameTime);	m_frame = m_next/m_mpf; 
	// Calculate the current frame
	// If the video is complete  
	std::cout << "mnext is " << m_next << std::endl;
	/*
	if (m_frame >= m_lastFrame)
	{  
		m_frame = 0; // Reset the frame back to zero (start of video) 
		m_next = 0;  // Reset the animation timer  
	}  */
	PGETFRAME pgf;
	pgf = AVIStreamGetFrameOpen(m_pavi, NULL);
	if (!pgf)
	{
		std::cout << ("Error in AVIStreamGetFrameOpen!") << std::endl;
		return;
	}

	LONG lEndTime = AVIStreamEndTime(m_pavi);
	LONG lTime = 0;
	LONG lFrame = 0;
	// Calculated just before next frame is blitted
	if (lTime <= lEndTime)
	{
		lFrame = AVIStreamTimeToSample(m_pavi, lTime);
		std::cout << "lframe: " << lFrame  << "endt " << lEndTime << std::endl;

	}
	else return;// the video is done
	// Get data from the AVI Stream  

	std::cout << "Test a" << std::endl;
	//lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(m_pgf, lFrame);
	lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pgf, lFrame);
	// Get a pointer to the data returned by AVIStreamGetFrame 


	std::cout << "Test a" << std::endl;
	DWORD* pAviData = (DWORD*)lpbi + lpbi->biSize + lpbi->biClrUsed * sizeof(RGBQUAD);  
	// Convert data to requested bitmap format 

	std::cout << "Test a" << std::endl;
	DrawDibDraw(m_hdd, m_hDC, 0, 0, 512, 512, lpbi, pAviData, 0, 0, m_width, m_height, 0);   
	// Lock the texture and get a pointer to the destination surface
	D3DLOCKED_RECT rect; 
	m_pTexture->LockRect(0, &rect, NULL, 0);
	// Get DWORD pointers to dest and src data 
	DWORD* pDestData = (DWORD*)rect.pBits;  
	DWORD* pSrcData  = (DWORD*)m_pData; 
	// Copy bitmap data to texture  
	for( int y = 0 ; y < 512 ; y++ )  
	{	   
		// Copy this row of pixels 
		memcpy( pDestData, pSrcData, 512 * sizeof( DWORD ) );   
		// Skip past row of pixels
		pDestData += ( rect.Pitch / 4 );   
		pSrcData += 512; 
	} 
	// Unlock the texture buffer  
	m_pTexture->UnlockRect(0);
}

void CAviTexture::SetTexture(LPDIRECT3DDEVICE9 pDevice)
{
	pDevice->SetTexture(0, m_pTexture);
}

bool CAviTexture::Draw(LPDIRECT3DDEVICE9 pDevice, float x, float y, float width, float height)
{
	aviVertex vertices[] = {  
		{       
			x, y+height, 0.5f, 1.0f, 0.0f, 1.0f
		},  
		{   
			x,y, 0.5f, 1.0f, 0.0f, 0.0f 
		},   
		{ 
			x+width, y+height, 0.5f, 1.0f, 1.0f, 1.0f 
		},    
		{
			x+width,        y, 0.5f, 1.0f, 1.0f, 0.0f 
		} 
	};
	
	pDevice->SetTexture(0, m_pTexture);
	pDevice->SetFVF(D3DFVF_AVIVERTEX); 
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vertices, sizeof(aviVertex));
	
	return true;
}

bool CAviTexture::Draw(LPD3DXSPRITE pSprite, float x, float y)
{
	const D3DXVECTOR3 cvec1 = D3DXVECTOR3(0, 0, 0);
	const D3DXVECTOR3 cvec2 = D3DXVECTOR3(x, y, 0);

	pSprite->Draw(m_pTexture, NULL, &cvec1, &cvec2, D3DCOLOR_RGBA(255, 255, 255, 255));

	return true;
}