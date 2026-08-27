#include "Animation2DSequence.h"

#include "../Asset/Animation/Animation2DData.h"

CAnimation2DSequence::CAnimation2DSequence()
{

}

CAnimation2DSequence::CAnimation2DSequence(const CAnimation2DSequence& Anim)
{
	// 얕은복사를 한다.
	*this = Anim;

	mNotifyList.clear();
}

CAnimation2DSequence::~CAnimation2DSequence()
{
	size_t	Size = mNotifyList.size();

	for (size_t i = 0; i < Size; ++i)
	{
		SAFE_DELETE(mNotifyList[i]);
	}
}

const std::string& CAnimation2DSequence::GetName() const
{
	return mAsset->GetName();
}

void CAnimation2DSequence::SetFrameRange(int Start, int End)
{

	mUseRange = true;
	mStartFrame = Start;
	mEndFrame = End;

	// 시작 프레임으로 세팅(정방향)
	mFrame = mStartFrame;
	mTime = 0.f;
	mEndFunctionEnable = true;

	// 프레임타임도 범위 기준으로 다시 계산
	if (mAsset)
	{
		int total = mAsset->GetFrameCount();
		int end = (mEndFrame < 0) ? (total - 1) : mEndFrame;
		if (mStartFrame < 0) mStartFrame = 0;
		if (end >= total) end = total - 1;
		if (end < mStartFrame) end = mStartFrame;

		int count = end - mStartFrame + 1;
		if (count < 1) count = 1;

		mFrameTime = mPlayTime / count;
	}
}

void CAnimation2DSequence::ClearFrameRange()
{
	mUseRange = false;
	mStartFrame = 0;
	mEndFrame = -1;
}


CAnimation2DSequence* CAnimation2DSequence::Clone()
{
	return new CAnimation2DSequence(*this);
}

void CAnimation2DSequence::SetAsset(class CAnimation2DData* Asset)
{
	mAsset = Asset;

	// 1프레임이 변경되어야 하는 시간 설정
	// mPlayTime이 1초이고 프레임이 10프레임이면
	// 1프레임이 변경되어야 하는 시간은 0.1초가 된다.
	mFrameTime = mPlayTime / mAsset->GetFrameCount();

}

void CAnimation2DSequence::SetPlayTime(float PlayTime)
{
	mPlayTime = PlayTime;

	if (mAsset)
	{
		mFrameTime = mPlayTime / mAsset->GetFrameCount();
	}

}

void CAnimation2DSequence::CallNotify()
{
	size_t	Size = mNotifyList.size();

	for (size_t i = 0; i < Size; ++i)
	{
		if (mNotifyList[i]->Frame == mFrame)
		{
			if (mNotifyList[i]->Function)
				mNotifyList[i]->Function();
		}
	}
}

void CAnimation2DSequence::Update(float DeltaTime)
{
	mTime += DeltaTime * mPlayRate;

	if (mTime < mFrameTime)
		return;

	mTime -= mFrameTime;

	const int total = mAsset->GetFrameCount();

	int start = 0;
	int end = total - 1;

	if (mUseRange)
	{
		start = mStartFrame;
		end = (mEndFrame < 0) ? (total - 1) : mEndFrame;

		// 안전 보정
		if (start < 0) start = 0;
		if (start >= total) start = total - 1;
		if (end < 0) end = 0;
		if (end >= total) end = total - 1;
		if (end < start) end = start;
	}

	// 프레임 진행
	++mFrame;

	// 범위 끝을 넘으면 처리
	if (mFrame > end)
	{
		if (mEndFunction && mEndFunctionEnable)
		{
			mEndFunctionEnable = false;
			mEndFunction();
		}

		if (mLoop)
		{
			mEndFunctionEnable = true;
			mFrame = start;     // 범위 시작으로 루프
		}
		else
		{
			mFrame = end;       // 범위 끝에서 정지
		}
	}

	CallNotify();
}