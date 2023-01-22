#pragma once
#include <Vfw.h>
#include <string>
#include <iostream>

#define YYAVI_AUDIO 1
#define YYAVI_VIDEO 2
#define YYAVI_OK 0

class YYVideoPlayer
{
private:
	PAVIFILE pAviFile = NULL;
	AVIFILEINFO pAviInfo; // set in openFile
	
	// streams
	PAVISTREAM pAudio = NULL; // set in openFile
	PAVISTREAM pVideo = NULL; // set in openFile
	
	AVISTREAMINFO sAudioInfo; // Allocated in initStream
	AVISTREAMINFO sVideoInfo; // Allocated in initStream

	// Flags
	bool done = false;
	bool cleaned = false;
	
	void openFile(const char* filename);
	void initStream(PAVISTREAM &pStream, AVISTREAMINFO* info, int avitype);

	void grabFrame(int avitype, LONG lsize, PAVISTREAM &pStream);
	
	void CleanUp(PGETFRAME& pgf, PAVISTREAM &pStream);

	void debug(std::string s)
	{
		std::cout << s << std::endl;
	}
public: 
	YYVideoPlayer();
	
};

