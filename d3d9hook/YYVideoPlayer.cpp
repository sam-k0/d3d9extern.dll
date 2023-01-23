#include "pch.h"
#include "YYVideoPlayer.h"

YYVideoPlayer::YYVideoPlayer()
{
}

void YYVideoPlayer::openFile(const char* filename)
{
	if (AVIFileOpenA(&pAviFile, filename, OF_READ, NULL) == YYAVI_OK)
	{
		AVIFileInfo(pAviFile, &pAviInfo, sizeof(pAviInfo)); // set the info struct

		
		// Open only audio and video streams

		if (AVIFileGetStream(pAviFile, &pAudio, streamtypeAUDIO, 0) != YYAVI_OK)
		{
			// Error
			debug("Error getting stream AUDIO");
		}

		if (AVIFileGetStream(pAviFile, &pVideo, streamtypeVIDEO, 0) != YYAVI_OK)
		{
			// Error
			debug("Error getting stream VIDEO");
		}
	}
	else
	{
		// Error
		debug("Could not open file " + std::string(filename));
		return;
	}
}



void YYVideoPlayer::initStream(PAVISTREAM &pStream, AVISTREAMINFO* info, int avitype)
{
	LONG lsize = NULL; // in bytes
	

	if (AVIStreamInfo(pStream, info, sizeof(info)) != YYAVI_OK)
	{
		//ERR
		debug("Something went wrong initializing the stream.");
		return;
	}

	
	if (AVIStreamReadFormat(pStream, AVIStreamStart(pStream), NULL, &lsize) != YYAVI_OK)
	{
		// Err
		debug("Something went wrong reading the stream format. ");
		return;
	}


}


void YYVideoPlayer::grabFrame(int avitype, LONG lsize, PAVISTREAM &pStream)
{
	LPBYTE pChunk;

	if (avitype == YYAVI_AUDIO) // AUDIO stream
	{
		pChunk = new BYTE[lsize];
		if (!pChunk) { debug("Could not allocate the byte."); } // Allocation error.

		if (AVIStreamReadFormat(pStream, AVIStreamStart(pStream), &pChunk, &lsize) != YYAVI_OK)
		{
			// Error
			debug("Error in allocating the pChunk.");
			return;
		}

		LPWAVEFORMAT pWaveFormat = (LPWAVEFORMAT)pChunk;

		LONG laudiosize;
		if (AVIStreamRead(pStream, 0, AVISTREAMREAD_CONVENIENT, NULL, 0, &laudiosize, NULL) != YYAVI_OK)
		{
			// ERR
			debug("Could not read avi audio stream.");
			return;
		}

		// Load sound into buffer
		LPBYTE pBuffer = new BYTE[laudiosize];
		if (!pBuffer) { debug("Could not allocate the buffer."); } // Allocation error.

		if (AVIStreamRead(pStream, 0, AVISTREAMREAD_CONVENIENT, pBuffer, laudiosize, NULL, NULL))
		{
			debug("Could not read avi audio stream: 2");
			return;
		}



	}
	else if (avitype == YYAVI_VIDEO) // VIDEO stream
	{
		pChunk = new BYTE[lsize];
		if (!pChunk) { debug("Could not allocate the byte."); } // Allocation error.

		if (AVIStreamReadFormat(pStream, AVIStreamStart(pStream), pChunk, &lsize) != YYAVI_OK)
		{
			// ERR
			debug("Error in reading stream format in video stream.");
			return;
		}

		PGETFRAME pgf;
		pgf = AVIStreamGetFrameOpen(pStream, NULL);
		if (!pgf)
		{
			debug("Error in AVIStreamGetFrameOpen!");
			return;
		}

		LONG lEndTime = AVIStreamEndTime(pStream);
		LONG lTime = 0;
		LONG lFrame = 0;
		// Calculated just before next frame is blitted
		if (lTime <= lEndTime)
			lFrame = AVIStreamTimeToSample(pStream, lTime);
		else // the video is done
		{
			debug("Done!");
			// set flag to mark as done!
			done = true;
			if(!cleaned)
			{
				CleanUp(pgf, pStream);
			}
			
			return;
		}

		LPBITMAPINFOHEADER lpbi;
		lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pgf, lFrame);

		// DISPLAY TO SCREEN HERE
		
	}

}


void YYVideoPlayer::CleanUp(PGETFRAME &pgf, PAVISTREAM &pStream)
{
	cleaned = true;
	if (AVIStreamGetFrameClose(pgf))
	{
		debug("Error in AVISTREAMGETCLOSE");
	}

	// Release all streams
	AVIStreamRelease(pStream);
	debug("Releasing pStream");

	AVIFileRelease(pAviFile);
	debug("Releasing pAviFile");

	AVIFileExit();
	debug("Exiting AVI File");
	debug("Done!");
	return;
}