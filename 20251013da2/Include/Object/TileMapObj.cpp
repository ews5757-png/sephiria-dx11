#include "TileMapObj.h"

#include "../Scene/Input.h"
#include "../Scene/Scene.h"
#include "../Share/Log.h"
#include "../Device.h"
#include "../Scene/SceneUIManager.h"
CTileMapObj::CTileMapObj()
{
}

CTileMapObj::CTileMapObj(const CTileMapObj& Obj)
{
}

CTileMapObj::CTileMapObj(CTileMapObj&& Obj)
{
}

CTileMapObj::~CTileMapObj()
{
}

void CTileMapObj::AddTileType()
{
	mEditTileType = static_cast<ETileType>((int)mEditTileType + 1);

	if (mEditTileType == ETileType::End)
	{
		mEditTileType = ETileType::Normal;
	}
}

void CTileMapObj::AddTileFrame()
{
	mEditorTileFrame = (mEditorTileFrame + 1) % mTileMap->GetTileFrameCount();
}

void CTileMapObj::SetDebugLines(bool _bool)
{
	mTileMap->SetTileOutLineRender(_bool);
}

void CTileMapObj::ToggleEditLayer()
{
	mEditLayer = (mEditLayer == ETileLayer::Ground) ? ETileLayer::Overlay : ETileLayer::Ground;

}

bool CTileMapObj::Init()
{
	CSceneObject::Init();

	mTileMapRender = CreateComponent<CTileMapRenderComponent>();

	mTileMap = CreateComponent<CTileMapComponent>();

	// 아웃라인 그리기 -> 쉐이더랑 메쉬 설정 까지 
	mTileMap->SetTileOutLineRender(true);

	//mTileMapRender->SetBackTexture("TileMapBack", TEXT("Texture/MapBackGround.png"));

	//타일 스프라이트 텍스쳐 설정 
	//mTileMapRender->SetTileTexture("TileMap", TEXT(""));
	mTileMapRender->SetTileTexture("TileMap");

	for (int y = 0; y <16; ++y)        // ← rows - 1
	{
		for (int x = 0; x < 16; ++x)
		{
			mTileMap->AddTileFrame(
				x * 16.f,
				y * 16.f,
				16.f,
				16.f
			);
		}
	}

	SetRootComponent(mTileMapRender);

	mTileMap->CreateTile(100, 100, FVector2D(16.f, 16.f),79);
;

	FVector2D mapSize(100 * 16, 100 * 16);
	FVector2D mapCenterOffset(mapSize.x * 0.5f, mapSize.y * 0.5f);

	// 타일맵 (0,0)이 좌상단 기준이면 이게 안전함
	mTileMapRender->SetWorldPos(-mapCenterOffset);

	return true;
}

void CTileMapObj::Update(float DeltaTime)
{
	CSceneObject::Update(DeltaTime);

	if (mScene->GetUIManager()->IsMouseOverUI())
		return;

	const FVector2D& MousePos = mScene->GetInput()->GetMouseWorldPos();

	bool paint =
		mScene->GetInput()->GetMouseDown(EMouseButtonType::LButton) ||
		mScene->GetInput()->GetMouseHold(EMouseButtonType::LButton);


	if (mEditorMode == EEditorMode::TileType)
	{
		if (paint)
		{
			int idx = mTileMap->GetTileIndex(MousePos);
			if (idx != -1)
			{
				mTileMap->ChangeTileType(mEditTileType, idx);

				// 중요: 현재 hover로 잡아둔 타일을 칠한 거면,
				// hover 복구 타입도 "칠한 타입"으로 바꿔야 나중에 되돌릴 때 안 깨짐
				if (idx == mOnMousePrevIndex)
				{
					mOnMousePrevTileType = mEditTileType;
				}
			}
		}
		//마우스가 올려져 있을때
		else if (!mScene->GetInput()->GetMouseDown(EMouseButtonType::LButton))
		{
			int Index = mTileMap->GetTileIndex(MousePos);

			//이전 인덱스와 지금 인덱스가 다를때만 변경
			if (Index != mOnMousePrevIndex)
			{
				ETileType PrevType = mTileMap->ChangeTileType(ETileType::MouseOver, MousePos);

				if (PrevType != ETileType::None)
				{
					if (mOnMousePrevIndex != -1)
					{
						mTileMap->ChangeTileType(mOnMousePrevTileType, mOnMousePrevIndex);
					}

					mOnMousePrevIndex = mTileMap->GetTileIndex(MousePos);
					mOnMousePrevTileType = PrevType;
				}
				else
				{
					mTileMap->ChangeTileType(mOnMousePrevTileType, mOnMousePrevIndex);
					mOnMousePrevIndex = -1;
					mOnMousePrevTileType = ETileType::None;
				}
			}
		}
	}
	//else if (mEditorMode == EEditorMode::TileImage)
	//{
	//	if (mScene->GetInput()->GetMouseHold(EMouseButtonType::LButton))
	//	{
	//		mTileMap->ChangeTileFrame(mEditorTileFrame, MousePos);
	//	}
	//}
	else if (mEditorMode == EEditorMode::TileImage)
	{
		// 좌클릭: 현재 레이어에 프레임 칠하기
		if (paint)
		{
			mTileMap->ChangeTileFrame(mEditLayer, mEditorTileFrame, MousePos);
		}

		bool erase =
			mScene->GetInput()->GetMouseDown(EMouseButtonType::RButton) ||
			mScene->GetInput()->GetMouseHold(EMouseButtonType::RButton);

		if (erase)
		{
			mTileMap->ChangeTileFrame(ETileLayer::Overlay, -1, MousePos);
		}
	}

}

void CTileMapObj::Save(const TCHAR* FullPath)
{
	mTileMap->Save(FullPath);
}

void CTileMapObj::Load(const TCHAR* FullPath)
{
	mTileMap->Load(FullPath);
}

void CTileMapObj::Load(const char* FileName)
{
	mTileMap->Load(FileName);
}
