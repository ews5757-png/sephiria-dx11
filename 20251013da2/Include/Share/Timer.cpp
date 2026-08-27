#include "Timer.h"


LARGE_INTEGER CTimer::mSecond = {};
LARGE_INTEGER CTimer::mTime = {};
float CTimer::mDeltaTime = 0.f;
float CTimer::mRuntime = 0.f;

std::list<FTimerElement> CTimer::mTimerList = {};

void CTimer::Init()
{
	//시작할때 초기 시간값을 가져와야한다.
	QueryPerformanceFrequency(&mSecond);	//초당 몇 프레임 (카운트) 수를 얻어온다.

	QueryPerformanceCounter(&mTime);		//
}

float CTimer::Update()
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time); //현재 시간을 얻어온다.

	//time 은 현재 프레임
	//mtime 은 이전 프레임 

	//현재 프레임 카운트 - 이전 프레임 카운트 / 초당 카운트 수
	//       105                 100             100(1초동안 100번) 
	//          5 :  x  초  :    100 : 1초
	mDeltaTime = (time.QuadPart - mTime.QuadPart) / (float)mSecond.QuadPart;
	mRuntime += mDeltaTime;
	mTime = time; // 시간 갱신

    auto iter = mTimerList.begin();
    auto iterEnd = mTimerList.end();

    for (; iter != iterEnd; )
    {
        iter->TimerTime -= mDeltaTime;

        if (iter->TimerTime <= 0.f)
        {
            // 시간이 다 됐으므로 예약된 함수 실행
            if (iter->Callback)
                iter->Callback();

            // 리스트에서 삭제 후 다음 타이머로
            iter = mTimerList.erase(iter);
            iterEnd = mTimerList.end();
        }
        else
        {
            ++iter;
        }
    }


	return mDeltaTime;
}

float CTimer::GetDeltaTime()
{
	return mDeltaTime;
}

void CTimer::ClearTimer(void* Owner)
{
    if (!Owner) return;

    auto iter = mTimerList.begin();
    auto iterEnd = mTimerList.end();

    for (; iter != iterEnd; )
    {
        if (iter->Owner == Owner)
        {
            iter = mTimerList.erase(iter);
            iterEnd = mTimerList.end(); 
        }
        else
        {
            ++iter;
        }
    }
}
