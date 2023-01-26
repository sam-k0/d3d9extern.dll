#pragma once

// Used to decode the Movie to bitmaps, hopefully
// https://www.codeproject.com/Articles/1353/Extracting-bitmaps-from-movies-using-DirectShow
#include "AtlBase.h"	// For atl smart pointers

#include <dshow.h>
#include <dvdmedia.h>
#include <dmodshow.h>

/*
        Thx to the genuis at Microsoft who didn't include the SampleGrabber to the direct show SDK...
        https://chromium.googlesource.com/webm/udpsample/+/059ab729a4639e3fdf9923eb8eab617216bb0386/qedit.h
        https://social.msdn.microsoft.com/Forums/vstudio/en-US/df2dd8ab-055e-44c3-931c-0f4778a17561/why-clsidsamplegrabber-is-undefined-how-can-i-resolve-it-?forum=vcgeneral
        https://stackoverflow.com/questions/64912751/how-to-include-qedit-h-from-directshow-to-project
*/


static const CLSID CLSID_SampleGrabber = { 0xC1F400A0, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };
static const IID IID_ISampleGrabber = { 0x6B652FFF, 0x11FE, 0x4fce, { 0x92, 0xAD, 0x02, 0x66, 0xB5, 0xD7, 0xC7, 0x8F } };
static const IID IID_ISampleGrabberCB = { 0x0579154A, 0x2B53, 0x4994, { 0xB0, 0xD0, 0xE7, 0x73, 0x14, 0x8E, 0xFF, 0x85 } };

class __declspec(uuid("{C1F400A4-3F08-11D3-9F0B-006008039E37}")) NullRenderer;
class __declspec(uuid("{C1F400A0-3F08-11D3-9F0B-006008039E37}")) SampleGrabber;

#pragma region SampleGrabber

struct __declspec(uuid("0579154a-2b53-4994-b0d0-e773148eff85"))
    ISampleGrabberCB : IUnknown
{
    //
    // Raw methods provided by interface
    //

    virtual HRESULT __stdcall SampleCB(
        double SampleTime,
        struct IMediaSample* pSample) = 0;
    virtual HRESULT __stdcall BufferCB(
        double SampleTime,
        unsigned char* pBuffer,
        long BufferLen) = 0;
};

struct __declspec(uuid("6b652fff-11fe-4fce-92ad-0266b5d7c78f"))
    ISampleGrabber : IUnknown
{
    //
    // Raw methods provided by interface
    //

    virtual HRESULT __stdcall SetOneShot(
        long OneShot) = 0;
    virtual HRESULT __stdcall SetMediaType(
        struct _AMMediaType* pType) = 0;
    virtual HRESULT __stdcall GetConnectedMediaType(
        struct _AMMediaType* pType) = 0;
    virtual HRESULT __stdcall SetBufferSamples(
        long BufferThem) = 0;
    virtual HRESULT __stdcall GetCurrentBuffer(
        /*[in,out]*/ long* pBufferSize,
        /*[out]*/ long* pBuffer) = 0;
    virtual HRESULT __stdcall GetCurrentSample(
        /*[out,retval]*/ struct IMediaSample** ppSample) = 0;
    virtual HRESULT __stdcall SetCallback(
        struct ISampleGrabberCB* pCallback,
        long WhichMethodToCallback) = 0;
};

struct __declspec(uuid("c1f400a0-3f08-11d3-9f0b-006008039e37"))
    SampleGrabber;
// [ default ] interface ISampleGrabber

#pragma endregion



CComPtr<IGraphBuilder> pGraphBuilder;

HRESULT setup(LPCWSTR filepath)
{
	
	HRESULT hr = ::CoCreateInstance(CLSID_FilterGraph, NULL,
		CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraphBuilder);
	CComPtr<IBaseFilter> pGrabberBaseFilter;
	CComPtr<ISampleGrabber> pSampleGrabber;
	AM_MEDIA_TYPE mt;
	hr = ::CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (LPVOID*)&pGrabberBaseFilter);
	if (FAILED(hr))
		return hr;
	pGrabberBaseFilter->QueryInterface(IID_ISampleGrabber, (void**)&pSampleGrabber);
	if (pSampleGrabber == NULL)
		return E_NOINTERFACE;
	hr = pGraphBuilder->AddFilter(pGrabberBaseFilter, L"Grabber");
	if (FAILED(hr))
		return hr;

    // Step 2
    ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
    mt.majortype = MEDIATYPE_Video;
    mt.subtype = MEDIASUBTYPE_RGB24;
    mt.formattype = FORMAT_VideoInfo;
    hr = pSampleGrabber->SetMediaType(&mt);
    if (FAILED(hr))
        return hr;
    hr = pGraphBuilder->RenderFile(filepath, NULL);
    if (FAILED(hr))
        return hr;

    // Step 3
    hr = pSampleGrabber->SetBufferSamples(TRUE);
    if (FAILED(hr))
        return hr;
    hr = pSampleGrabber->SetOneShot(TRUE);
    if (FAILED(hr))
        return hr;

    // Step 4
    /*hr = pMediaControl->Run();
    if (FAILED(hr))
        return hr;
    long evCode;
    hr = pMediaEventEx->WaitForCompletion(INFINITE, &evCode);
    if (FAILED(hr))
        return hr;*/

    AM_MEDIA_TYPE MediaType;
    ZeroMemory(&MediaType, sizeof(MediaType));
    hr = pSampleGrabber->GetConnectedMediaType(&MediaType);
    if (FAILED(hr))
        return hr;
    // Get a pointer to the video header. 
    VIDEOINFOHEADER* pVideoHeader = (VIDEOINFOHEADER*)MediaType.pbFormat;
    if (pVideoHeader == NULL)
        return E_FAIL;
    // The video header contains the bitmap information. 
    // Copy it into a BITMAPINFO structure. 
    BITMAPINFO BitmapInfo;
    ZeroMemory(&BitmapInfo, sizeof(BitmapInfo));
    CopyMemory(&BitmapInfo.bmiHeader, &(pVideoHeader->bmiHeader),
        sizeof(BITMAPINFOHEADER));

    // Create a DIB from the bitmap header, and get a pointer to the buffer. 
    void* buffer = NULL;
    HBITMAP hBitmap = ::CreateDIBSection(0, &BitmapInfo, DIB_RGB_COLORS, &buffer,
        NULL, 0);
    GdiFlush();
}