#pragma once

#include "Sound.h"

class CSoundManager
{
	friend class CAssetManager;

private:
	CSoundManager();
	~CSoundManager();

private:
	//1
	// 사운드 관련 관리자 
	FMOD::System* mSystem = nullptr;
	//채널 최상위 그룹 
	FMOD::ChannelGroup* mMasterGroup = nullptr;
	//채널 하위그룹
	std::unordered_map<std::string, FMOD::ChannelGroup*> mChannelGroupMap;
	//사운드 종류 
	std::unordered_map<std::string, CSharedPtr<CSound>> mSoundMap;

public:
	bool Init();
	void Update();

	bool CreateChannelGroup(const std::string& Name);

	//사운드 불러오기
	bool LoadSound(const std::string& Name, const std::string& GroupName, bool Loop, const char* FileName);
	bool LoadSoundFullPath(const std::string& Name, const std::string& GroupName, bool Loop, const char* FullPath);

	//기능
	void Play(const std::string& Name);
	void Stop(const std::string& Name);
	void Pause(const std::string& Name);
	void Resume(const std::string& Name);

	void SetMasterVolume(int Volume);
	void SetGroupVolume(const std::string& GroupName, int Volume);



private:
	FMOD::ChannelGroup* FindChannelGroup(const std::string& Name);

public:
	void ReleaseSound(class CAsset* Sound);

	CSound* FindSound(const std::string& Name);


};

