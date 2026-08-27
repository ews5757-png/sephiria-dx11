#pragma once
#include "../GameInfo.h"

/*
	델타 타임이란?
	프레임 가느이 시간 차이를 의미한다.
	fps는 1초에 몇번 화면이 갱신 됭었는가를 의미한다.
	60fps ->1초에 화면이 60번  그려진 거다.
	1프레임은 60fps는 1/60 초 이다. 0.0166...초
*/
struct FTimerElement
{
	float TimerTime;
	std::function<void()> Callback;
	void* Owner;
};
class CTimer
{
	friend class CGameManager;

private:
	static LARGE_INTEGER mSecond;
	static LARGE_INTEGER mTime;
	static float mDeltaTime;

	static std::list<FTimerElement> mTimerList;


private:
	static void Init();
	static float Update();

public:
	static float GetDeltaTime();
	static float mRuntime;
	static void AddTimer(float Duration, std::function<void()> Callback,void* Owner)
	{
		FTimerElement Element;
		Element.TimerTime = Duration;
		Element.Callback = Callback;
		Element.Owner = Owner;
		mTimerList.push_back(Element);
	}
	static void ClearTimer(void* Owner);
};

