#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "../includes.h"
#include <map>
#include "../framework.h"
#include "../include/BASS/bass.h"

class SoundManager {
public:
	SoundManager();
	 ~SoundManager() {};

	static SoundManager * getInstance() {
		if (instance == NULL) instance = new SoundManager();
		return instance;
	}

	std::map<std::string, HSAMPLE*> sample;
	std::map<std::string, HSTREAM*> stream;
	std::vector<HCHANNEL> channels;
	int channels_num;

	//sound 
	
	//El handler para un sample						
	HSAMPLE hSample1;
	HSAMPLE hSample2;	
	// Handle for open strea
	HSTREAM hStream_menu;
	HSTREAM hStream_inGame;
	//El handler para un canal
	HCHANNEL hSampleChannel1;	
	HCHANNEL hSampleChannel2;

	void startStream(const std::string& s, bool restart);
	void stopStream(const std::string& s);
	void startSound(const std::string& s, bool restart, const int &ch);
	bool loadSample(const std::string& s);
	bool loadStream(const std::string& s);
	void streamVol(const std::string& s, const float &vol);
	void soundVol(const std::string& s, const float &vol);

private:
	static SoundManager * instance;
};

#endif // 