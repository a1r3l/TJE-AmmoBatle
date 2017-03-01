#include "SoundManager.h"
#include <iostream>
#include "../utils.h"

SoundManager * SoundManager::instance = NULL;

SoundManager::SoundManager() {
	BASS_Init(1, 44100, 0, 0, NULL);
	channels_num = 0;
	channels.resize(20 * sizeof(HCHANNEL));
}

void SoundManager::startStream(const std::string& s, bool restart) {
	std::map<std::string, HSTREAM*>::iterator it = SoundManager::stream.find(s);
	if (it == stream.end()) {
		std::cout << "Could not find stream " << std::endl;
	}
	else BASS_ChannelPlay(*(it->second), restart);
}

void SoundManager::stopStream(const std::string& s) {

	std::map<std::string, HSTREAM*>::iterator it = SoundManager::stream.find(s);
	if (it == stream.end()) {
		std::cout << "Could not find stream "<< std::endl;
	}
	else BASS_ChannelStop(*(it->second));
}

void SoundManager::startSound(const std::string& s, bool restart, const int &ch) {
	std::map<std::string, HSAMPLE*>::iterator it = SoundManager::sample.find(s);
	if (it == sample.end()) {
		std::cout << "Could not find sample " << std::endl;
	}
	else	BASS_ChannelPlay(channels[ch], restart);
}

bool SoundManager::loadSample(const std::string& s) {
	//Buscamos el sample dentro de nuestro map 
	std::map<std::string, HSAMPLE*>::iterator it = SoundManager::sample.find(s);
	//SI no esta dentro de nuestro map lo cargamos
	if (it == sample.end())
	{
		std::string path = "data/sounds/" + s;
		HSAMPLE* aux = new HSAMPLE();		
		if (BASS_SampleLoad(false, path.c_str(), 0, 0.1, 3, 0) != 0)
		{
			*aux = BASS_SampleLoad(false, path.c_str(), 0, 0, 3, 0);
			sample.insert(std::pair<std::string, HSAMPLE*>(s, aux));
			channels[channels_num] = BASS_SampleGetChannel(*aux, false);
			channels_num++;
			if (channels_num > 10) std::cout << "You just have 10 channels it will fail.\nSorry!" << std::endl;
			return true;
		}
		std::cout << "Could not load sample " << s << std::endl;
		return false;
	}
	else return true;
}

bool SoundManager::loadStream(const std::string& s) {
	//Busco mi stream dentro del map
	std::map<std::string, HSTREAM*>::iterator it = SoundManager::stream.find(s);
	//Si no esta lo cargo 
	if (it == stream.end())
	{
		std::cout << "Loading stream " << s << std::endl;
		std::string path = "data/sounds/" + s;
		HSTREAM* aux = new HSTREAM();
		if (BASS_StreamCreateFile(false, path.c_str(), 0, 0, 0) != 0)
		{
			*aux = BASS_StreamCreateFile(false, path.c_str(), 0, 0, 0);
			stream.insert(std::pair<std::string, HSTREAM*>(s, aux));
			return true;
		}
		std::cout << "Could not load stream " << s << std::endl;
		return false;
	}
	else return true;
}

void SoundManager::streamVol(const std::string& s, const float &vol) {
	//Busco el stream al cual bajarle el volumen
	std::map<std::string, HSTREAM*>::iterator it = SoundManager::stream.find(s);
	//Si no he llegado al final esque lo he encontrado
	if (it != stream.end()) {
		BASS_ChannelSetAttribute(*(it->second), BASS_ATTRIB_VOL, vol);
		return;
	}
	//SI he llegado al final prrinto error
	else std::cout << "Could not find stream " << s << ".\nSorry!" << std::endl;
}


void SoundManager::soundVol(const std::string& s, const float &vol) {
	//Busco el stream al cual bajarle el volumen
	std::map<std::string, HSTREAM*>::iterator it = SoundManager::sample.find(s);
	//Si no he llegado al final esque lo he encontrado
	if (it != sample.end()) {
		BASS_ChannelSetAttribute(*(it->second), BASS_ATTRIB_VOL, vol);
		return;
	}
	//SI he llegado al final prrinto error
	else std::cout << "Could not find stream " << s << ".\nSorry!" << std::endl;
}