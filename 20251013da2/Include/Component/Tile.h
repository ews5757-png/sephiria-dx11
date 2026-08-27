#pragma once

#include "../Gameinfo.h"

enum class ETileLayer : uint8_t
{
	Ground = 0,
	Overlay,
	End
};

struct FTileLayer
{
	int Frame = -1;         // -1이면 비어있음
	ETileType Type = ETileType::Normal; // 타입을 레이어별로 둘거면 유지, 아니면 Ground만 써도 됨
};

//자료구조의 노드와 같은 존재이다. 
class CTile
{
	friend class CTileMapComponent;

private:
	CTile();
	~CTile();

private:
	ETileType mType = ETileType::Normal;
	FVector2D mPos;
	FVector2D mSize;
	FVector2D mCenter;
	//타일은 스프라이트 이미지를 사용할거기에 텍스쳐 프레임
	//int mTextureFrame = -1;

	FVector2D mRenderOffset = { 0.f, 0.f };


	int mLayerFrame[(int)ETileLayer::End] = { -1, -1 }; // [Ground, Overlay]

public:

	void SetFrame(int a = -1, int b = -1)
	{
		mLayerFrame[0] = a;
		mLayerFrame[1] = b;
	}
	ETileType GetTileType() const
	{
		return mType;
	}

	FVector2D GetPos() const
	{
		return mPos;
	}

	FVector2D GetSize() const
	{
		return mSize;
	}

	FVector2D GetCenter() const
	{
		return mCenter;
	}

	//int GetTextureFrame() const
	//{
	//	return mTextureFrame;
	//}
	int GetTextureFrame() const { return mLayerFrame[(int)ETileLayer::Ground]; }
	int GetLayerFrame(ETileLayer Layer) const { return mLayerFrame[(int)Layer]; }
public:
	void SetTileType(ETileType Type)
	{
		mType = Type;
	}

	//void SetTextureFrame(int Frame)
	//{
	//	mTextureFrame = Frame;
	//}
	void SetTextureFrame(int Frame) { mLayerFrame[(int)ETileLayer::Ground] = Frame; }
	void SetLayerFrame(ETileLayer Layer, int Frame) { mLayerFrame[(int)Layer] = Frame; }


public:
	void Save(FILE* File);
	void Load(FILE* File);

};

