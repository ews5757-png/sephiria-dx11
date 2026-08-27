#include "TileMapComponent.h"

#include "TileMapRenderComponent.h"
#include "../Device.h"
#include "../Asset/AssetManager.h"
#include "../Asset/Mesh/MeshManager.h"
#include "../Object/SceneObject.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneAssetManager.h"
#include "../Shader/ShaderManager.h"
#include "../Scene/CameraManager.h"
#include "../Shader/TransformCBuffer.h"
#include "../Shader/ColliderCBuffer.h"
#include "../Asset/Mesh/Mesh.h"
#include "../Asset/Texture/Texture.h"
#include "../Shader/TileMapCBuffer.h"
#include "../Component/CameraComponent.h"

#include "../Share/Log.h"


CTileMapComponent::CTileMapComponent()
{
}

CTileMapComponent::CTileMapComponent(const CTileMapComponent& Com)
	: CComponent(Com)
{
}

CTileMapComponent::CTileMapComponent(CTileMapComponent&& Com)
	: CComponent(Com)
{
}

CTileMapComponent::~CTileMapComponent()
{
	size_t Size = mTileList.size();

	for (size_t i = 0; i < Size; ++i)
	{
		SAFE_DELETE(mTileList[i]);
	}

	SAFE_DELETE(mLineTransformCBuffer);
	SAFE_DELETE(mCBuffer);
	SAFE_DELETE(mTileMapCBuffer);

}

void CTileMapComponent::CreateTile(int CountX, int CountY, const FVector2D& TileSize, int TileTextureFrame)
{
	mCountX = CountX;
	mCountY = CountY;
	mTileSize = TileSize;

	//타일맵 전체 크기 
	mMapSize.x = mCountX * mTileSize.x;
	mMapSize.y = mCountY * mTileSize.y;

	CTileMapRenderComponent* Renderer = mOwnerObject->FindSceneComponent<CTileMapRenderComponent>();

	if (Renderer)
	{
		FVector2D Scale;
		Scale.x = mCountX * mTileSize.x;
		Scale.y = mCountY * mTileSize.y;
		Renderer->SetWorldScale(Scale);
	}


	//타일맵 생성
	mTileList.resize(mCountX * mCountY);

	for (int i = 0; i < mCountY; ++i)
	{
		for (int j = 0; j < mCountX; ++j)
		{
			CTile* Tile = new CTile;

			//x축
			Tile->mPos.x = j * mTileSize.x;
			//y축
			Tile->mPos.y = i * mTileSize.y;

			Tile->mSize = mTileSize;


			//타일의 중아
			Tile->mCenter = Tile->mPos + mTileSize * 0.5f;

			Tile->SetLayerFrame(ETileLayer::Ground, TileTextureFrame);
			Tile->SetLayerFrame(ETileLayer::Overlay, -1); // 기본 비움

			mTileList[i * mCountX + j] = Tile;
		}
	}

}

void CTileMapComponent::ChangeTileFrame(ETileLayer Layer, int Frame, const FVector3D& Pos)
{
	int Index = GetTileIndex(Pos);
	if (Index == -1) return;
	mTileList[Index]->SetLayerFrame(Layer, Frame);
}

void CTileMapComponent::ChangeTileFrame(ETileLayer Layer, int Frame, const FVector2D& Pos)
{
	int Index = GetTileIndex(Pos);
	if (Index == -1) return;
	mTileList[Index]->SetLayerFrame(Layer, Frame);
}

void CTileMapComponent::ChangeTileFrame(ETileLayer Layer, int Frame, float x, float y)
{
	int Index = GetTileIndex(x, y);
	if (Index == -1) return;
	mTileList[Index]->SetLayerFrame(Layer, Frame);
}

void CTileMapComponent::ChangeTileFrame(ETileLayer Layer, int Frame, int Index)
{
	if (Index < 0 || Index >= (int)mTileList.size()) return;
	mTileList[Index]->SetLayerFrame(Layer, Frame);
}

void CTileMapComponent::RenderTileLayer(ETileLayer Layer)
{

	for (int i = mViewStartY; i <= mViewEndY; ++i)
	{
		for (int j = mViewStartX; j <= mViewEndX; ++j)
		{
			int Index = i * mCountX + j;

			int TileFrame = mTileList[Index]->GetLayerFrame(Layer);

			if (TileFrame == -1)
				continue;

			FMatrix matScale, matTranslate, matWorld;

			matScale.Scaling(mTileList[Index]->GetSize());

			FVector2D Pos = mTileList[Index]->GetPos();
			Pos.x += mOwnerObject->GetWorldPosition().x;
			Pos.y += mOwnerObject->GetWorldPosition().y;

			// ★ 여기서 내려준다 (렌더 전용)
			if (TileFrame >= 0)
			{
				Pos += mFrameRenderOffset[TileFrame];
			}

			matTranslate.Translation(Pos);
			matWorld = matScale * matTranslate;

			FMatrix matView, matProj;
			matView = mScene->GetCameraManager()->GetViewMatrix();
			matProj = mScene->GetCameraManager()->GetProjMatrix();

			mLineTransformCBuffer->SetWorldMatrix(matWorld);
			mLineTransformCBuffer->SetViewMatrix(matView);
			mLineTransformCBuffer->SetProjMatrix(matProj);
			mLineTransformCBuffer->UpdateBuffer();

			FVector2D LTUV, RBUV;
			LTUV = mTileFrameList[TileFrame].Start / mTileTextureSize;
			RBUV = LTUV + mTileFrameList[TileFrame].Size / mTileTextureSize;

			mTileMapCBuffer->SetUV(LTUV, RBUV);
			mTileMapCBuffer->UpdateBuffer();

			mTileShader->SetShader();
			mTileMesh->Render();
		}
	}
}

void CTileMapComponent::SetTileOutLineRender(bool Render)
{
	mTileOutlineRender = Render;
	mDebugLayerView = Render;

	if (mTileOutlineRender)
	{
		mOutlineShader = CShaderManager::GetInst()->FindShader("FrameMeshShader");

		if (mScene)
		{
			mOutlineMesh = mScene->GetAssetManager()->FindMesh("FrameLBRect");
		}
		else
		{
			mOutlineMesh = CAssetManager::GetInst()->GetMeshManager()->FindMesh("FrameLBRect");
		}
	}
	else
	{
		mOutlineMesh = nullptr;
		mOutlineShader = nullptr;
	}

}

void CTileMapComponent::RenderTileOutLine()
{

	for (int i = mViewStartY; i <= mViewEndY; ++i)
	{
		for (int j = mViewStartX; j <= mViewEndX; ++j)
		{
			int Index = i * mCountX + j;

			//Tranform 상수버퍼 데이터 넣어주기 
			//크기, 위치
			FMatrix matScale, matTranslate, matWorld;
			matScale.Scaling(mTileList[Index]->GetSize());

			FVector2D Pos = mTileList[Index]->GetPos();

			Pos.x += mOwnerObject->GetWorldPosition().x;
			Pos.y += mOwnerObject->GetWorldPosition().y;

			matTranslate.Translation(Pos);

			//크자이공부 !!!!!
			matWorld = matScale * matTranslate;

			FMatrix matView, matProj;
			matView = mScene->GetCameraManager()->GetViewMatrix();
			matProj = mScene->GetCameraManager()->GetProjMatrix();

			mLineTransformCBuffer->SetWorldMatrix(matWorld);
			mLineTransformCBuffer->SetViewMatrix(matView);
			mLineTransformCBuffer->SetProjMatrix(matProj);

			mLineTransformCBuffer->UpdateBuffer();


			// 상수버퍼들 업데이트
			//Collider 상수버퍼 데이터 넣어주기
			//충돌 중 : 빨강

			switch (mTileList[Index]->GetTileType())
			{
			case ETileType::Normal:
				mCBuffer->SetColor(0.f, 1.f, 0.f, 1.f);
				break;
			case ETileType::UnableToMove:
				mCBuffer->SetColor(1.f, 0.f, 0.f, 1.f);
				break;
			case ETileType::MouseOver:
				mCBuffer->SetColor(1.f, 1.f, 0.f, 1.f);
				break;
			}

			mCBuffer->UpdateBuffer();

			//쉐이더 셋팅 
			mOutlineShader->SetShader();

			//그려라 
			mOutlineMesh->Render();

		}
	}

}

void CTileMapComponent::RenderDebugLayerOverlay()
{
	if (!mDebugFillShader || !mDebugFillMesh)
		return;

	for (int i = mViewStartY; i <= mViewEndY; ++i)
		for (int j = mViewStartX; j <= mViewEndX; ++j)
		{
			int idx = i * mCountX + j;

			int overlayFrame = mTileList[idx]->GetLayerFrame(ETileLayer::Overlay);
			int groundFrame = mTileList[idx]->GetLayerFrame(ETileLayer::Ground);

			if (overlayFrame >= 0)
				mCBuffer->SetColor(0.7f, 0.f, 1.f, 0.35f);
			else if (groundFrame >= 0)
				mCBuffer->SetColor(0.f, 0.6f, 1.f, 0.20f);
			else
				continue;

			// Transform (타일 크기/위치)
			FMatrix matScale, matTranslate, matWorld;
			matScale.Scaling(mTileList[idx]->GetSize());

			FVector2D Pos = mTileList[idx]->GetPos();
			Pos.x += mOwnerObject->GetWorldPosition().x;
			Pos.y += mOwnerObject->GetWorldPosition().y;

			matTranslate.Translation(Pos);
			matWorld = matScale * matTranslate;

			FMatrix matView = mScene->GetCameraManager()->GetViewMatrix();
			FMatrix matProj = mScene->GetCameraManager()->GetProjMatrix();

			mLineTransformCBuffer->SetWorldMatrix(matWorld);
			mLineTransformCBuffer->SetViewMatrix(matView);
			mLineTransformCBuffer->SetProjMatrix(matProj);
			mLineTransformCBuffer->UpdateBuffer();

			mCBuffer->UpdateBuffer();

			// Draw fill overlay
			mDebugFillShader->SetShader();
			mDebugFillMesh->Render();
		}
}


void CTileMapComponent::RenderTile()
{
	//for (int i = mViewStartY; i <= mViewEndY; ++i)
	//{
	//	for (int j = mViewStartX; j <= mViewEndX; ++j)
	//	{
	//		int Index = i * mCountX + j;

	//		int TileFrame = mTileList[Index]->GetTextureFrame();

	//		if (TileFrame == -1)
	//		{
	//			continue;
	//		}

	//		FMatrix matScale, matTranslate, matWorld;

	//		matScale.Scaling(mTileList[Index]->GetSize());

	//		FVector2D Pos = mTileList[Index]->GetPos();

	//		Pos.x += mOwnerObject->GetWorldPosition().x;
	//		Pos.y += mOwnerObject->GetWorldPosition().y;

	//		matTranslate.Translation(Pos);

	//		matWorld = matScale * matTranslate;

	//		FMatrix matView, matProj;
	//		matView = mScene->GetCameraManager()->GetViewMatrix();
	//		matProj = mScene->GetCameraManager()->GetProjMatrix();

	//		mLineTransformCBuffer->SetWorldMatrix(matWorld);
	//		mLineTransformCBuffer->SetViewMatrix(matView);
	//		mLineTransformCBuffer->SetProjMatrix(matProj);

	//		mLineTransformCBuffer->UpdateBuffer();

	//		//타일용 상수버퍼에 데이터를 넣어준다.
	//		FVector2D LTUV, RBUV;

	//		// UV 비율 구하기 
	//		LTUV = mTileFrameList[TileFrame].Start / mTileTextureSize;
	//		RBUV = LTUV + mTileFrameList[TileFrame].Size / mTileTextureSize;

	//		mTileMapCBuffer->SetUV(LTUV, RBUV);
	//		mTileMapCBuffer->UpdateBuffer();

	//		//쉐이더 셋팅 
	//		mTileShader->SetShader();

	//		//그려라 
	//		mTileMesh->Render();

	//	}
	//}
	RenderTileLayer(ETileLayer::Ground);
	RenderTileLayer(ETileLayer::Overlay);

	//if (mTileOutlineRender)
	//	RenderTileOutLine();

	//if (mDebugLayerView)
	//	RenderDebugLayerOverlay();   // <- 새로 추가(덧그리기)

}

void CTileMapComponent::SetTileTexture(const std::string& Name)
{
	CTileMapRenderComponent* Renderer = mOwnerObject->FindSceneComponent<CTileMapRenderComponent>();

	if (Renderer)
	{
		Renderer->SetTileTexture(Name);
	}
}

void CTileMapComponent::SetTileTexture(const std::string& Name, const TCHAR* FileName)
{
	CTileMapRenderComponent* Renderer = mOwnerObject->FindSceneComponent<CTileMapRenderComponent>();

	if (Renderer)
	{
		Renderer->SetTileTexture(Name, FileName);
	}
}

void CTileMapComponent::SetTileTexture(class CTexture* Texture)
{
	CTileMapRenderComponent* Renderer = mOwnerObject->FindSceneComponent<CTileMapRenderComponent>();

	if (Renderer)
	{
		Renderer->SetTileTexture(Texture);
	}
}

void CTileMapComponent::AddTileFrame(const FVector2D& Start, const FVector2D& Size)
{
	FAnimationFrame Frame;
	Frame.Start = Start;
	Frame.Size = Size;

	mTileFrameList.emplace_back(Frame);
}

void CTileMapComponent::AddTileFrame(float StartX, float StartY, float SizeX, float SizeY)
{
	FAnimationFrame Frame;
	Frame.Start.x = StartX;
	Frame.Start.y = StartY;
	Frame.Size.x = SizeX;
	Frame.Size.y = SizeY;

	mTileFrameList.emplace_back(Frame);
}


// 타일 인덱스 찾기 
int CTileMapComponent::GetTileIndexX(const FVector3D& Pos) const
{
	//Pos 월드좌표를 타일맵 좌표로 변환이 필요하다.
	float Convert = Pos.x - mOwnerObject->GetWorldPosition().x;

	float Value = Convert / mTileSize.x;

	if (Value < 0.f)
	{
		return -1;
	}

	int Index = static_cast<int>(Value);

	return (Index < 0 || Index >= mCountX) ? -1 : Index;
}

int CTileMapComponent::GetTileIndexX(const FVector2D& Pos) const
{
	//Pos 월드좌표를 타일맵 좌표로 변환이 필요하다.
	float Convert = Pos.x - mOwnerObject->GetWorldPosition().x;

	float Value = Convert / mTileSize.x;

	if (Value < 0.f)
	{
		return -1;
	}

	int Index = static_cast<int>(Value);

	return (Index < 0 || Index >= mCountX) ? -1 : Index;
}

int CTileMapComponent::GetTileIndexX(float x) const
{
	//Pos 월드좌표를 타일맵 좌표로 변환이 필요하다.
	float Convert = x - mOwnerObject->GetWorldPosition().x;

	float Value = Convert / mTileSize.x;

	if (Value < 0.f)
	{
		return -1;
	}

	int Index = static_cast<int>(Value);

	return (Index < 0 || Index >= mCountX) ? -1 : Index;
}

int CTileMapComponent::GetTileIndexY(const FVector3D& Pos) const
{
	//Pos 월드좌표를 타일맵 좌표로 변환이 필요하다.
	float Convert = Pos.y - mOwnerObject->GetWorldPosition().y;

	float Value = Convert / mTileSize.y;

	if (Value < 0.f)
	{
		return -1;
	}

	int Index = static_cast<int>(Value);

	return (Index < 0 || Index >= mCountY) ? -1 : Index;
}

int CTileMapComponent::GetTileIndexY(const FVector2D& Pos) const
{
	//Pos 월드좌표를 타일맵 좌표로 변환이 필요하다.
	float Convert = Pos.y - mOwnerObject->GetWorldPosition().y;

	float Value = Convert / mTileSize.y;

	if (Value < 0.f)
	{
		return -1;
	}

	int Index = static_cast<int>(Value);

	return (Index < 0 || Index >= mCountY) ? -1 : Index;
}

int CTileMapComponent::GetTileIndexY(float y) const
{
	//Pos 월드좌표를 타일맵 좌표로 변환이 필요하다.
	float Convert = y - mOwnerObject->GetWorldPosition().y;

	float Value = Convert / mTileSize.y;

	if (Value < 0.f)
	{
		return -1;
	}

	int Index = static_cast<int>(Value);

	return (Index < 0 || Index >= mCountY) ? -1 : Index;
}

int CTileMapComponent::GetTileIndex(const FVector3D& Pos) const
{
	int IndexX = GetTileIndexX(Pos.x);

	if (IndexX == -1)
	{
		return -1;
	}

	int IndexY = GetTileIndexY(Pos.y);

	if (IndexY == -1)
	{
		return -1;
	}

	//최종 타일 인덱스가 나온다. 
	return IndexY * mCountX + IndexX;
}

int CTileMapComponent::GetTileIndex(const FVector2D& Pos) const
{
	int IndexX = GetTileIndexX(Pos.x);

	if (IndexX == -1)
	{
		return -1;
	}

	int IndexY = GetTileIndexY(Pos.y);

	if (IndexY == -1)
	{
		return -1;
	}

	//최종 타일 인덱스가 나온다. 
	return IndexY * mCountX + IndexX;
}

int CTileMapComponent::GetTileIndex(float x, float y) const
{
	int IndexX = GetTileIndexX(x);

	if (IndexX == -1)
	{
		return -1;
	}

	int IndexY = GetTileIndexY(y);

	if (IndexY == -1)
	{
		return -1;
	}

	//최종 타일 인덱스가 나온다. 
	return IndexY * mCountX + IndexX;
}


//타일 타입 변경하기 
ETileType CTileMapComponent::ChangeTileType(ETileType Type, const FVector3D& Pos)
{
	int Index = GetTileIndex(Pos);

	if (Index == -1)
	{
		return ETileType::None;
	}

	ETileType PrevType = mTileList[Index]->GetTileType();

	mTileList[Index]->SetTileType(Type);

	return PrevType;
}

ETileType CTileMapComponent::ChangeTileType(ETileType Type, const FVector2D& Pos)
{
	int Index = GetTileIndex(Pos);

	if (Index == -1)
	{
		return ETileType::None;
	}

	ETileType PrevType = mTileList[Index]->GetTileType();

	mTileList[Index]->SetTileType(Type);

	return PrevType;
}

ETileType CTileMapComponent::ChangeTileType(ETileType Type, float x, float y)
{
	int Index = GetTileIndex(x, y);

	if (Index == -1)
	{
		return ETileType::None;
	}

	ETileType PrevType = mTileList[Index]->GetTileType();

	mTileList[Index]->SetTileType(Type);

	return PrevType;
}

ETileType CTileMapComponent::ChangeTileType(ETileType Type, int Index)
{
	ETileType PrevType = mTileList[Index]->GetTileType();

	mTileList[Index]->SetTileType(Type);

	return PrevType;
}

//void CTileMapComponent::ChangeTileFrame(int Frame, const FVector3D& Pos)
//{
//	int Index = GetTileIndex(Pos);
//
//	if (Index == -1)
//	{
//		return;
//	}
//	mTileList[Index]->SetTextureFrame(Frame);
//
//	return;
//}
//
//void CTileMapComponent::ChangeTileFrame(int Frame, const FVector2D& Pos)
//{
//	int Index = GetTileIndex(Pos);
//
//	if (Index == -1)
//	{
//		return;
//	}
//	mTileList[Index]->SetTextureFrame(Frame);
//
//	return;
//}
//
//void CTileMapComponent::ChangeTileFrame(int Frame, float x, float y)
//{
//	int Index = GetTileIndex(x, y);
//
//	if (Index == -1)
//	{
//		return;
//	}
//	mTileList[Index]->SetTextureFrame(Frame);
//
//	return;
//}
//
//void CTileMapComponent::ChangeTileFrame(int Frame, int Index)
//{
//	mTileList[Index]->SetTextureFrame(Frame);
//
//	return;
//}

void CTileMapComponent::ChangeTileFrame(int Frame, const FVector3D& Pos)
{
	ChangeTileFrame(ETileLayer::Ground, Frame, Pos);
}
void CTileMapComponent::ChangeTileFrame(int Frame, const FVector2D& Pos)
{
	ChangeTileFrame(ETileLayer::Ground, Frame, Pos);
}
void CTileMapComponent::ChangeTileFrame(int Frame, float x, float y)
{
	ChangeTileFrame(ETileLayer::Ground, Frame, x, y);
}
void CTileMapComponent::ChangeTileFrame(int Frame, int Index)
{
	ChangeTileFrame(ETileLayer::Ground, Frame, Index);
}

bool CTileMapComponent::Init()
{
	CComponent::Init();

	mFrameRenderOffset.fill(FVector2D(0.f, 0.f));

	for (int i = 49; i < 98; i++)
	{
		mFrameRenderOffset[i] = FVector2D(0.f, -8.f); // “아래”가 +인지 -인지는 1번 테스트로 확정
	}



	CTileMapRenderComponent* Renderer = mOwnerObject->FindSceneComponent<CTileMapRenderComponent>();

	if (Renderer)
	{
		Renderer->SetTileMapComponent(this);
	}

	mLineTransformCBuffer = new CTransformCBuffer;
	mLineTransformCBuffer->Init();

	mCBuffer = new CColliderCBuffer;
	mCBuffer->Init();

	mTileMapCBuffer = new CTileMapCBuffer;
	mTileMapCBuffer->Init();

	mTileShader = CShaderManager::GetInst()->FindShader("TileShader");

	if (mScene)
	{
		mTileMesh = mScene->GetAssetManager()->FindMesh("SpriteRect");
	}
	else
	{
		mTileMesh = CAssetManager::GetInst()->GetMeshManager()->FindMesh("SpriteRect");
	}

	mDebugFillShader = CShaderManager::GetInst()->FindShader("FrameMeshShader"); // 단색이면 그대로 사용 가능
	if (mScene)
		mDebugFillMesh = mScene->GetAssetManager()->FindMesh("SpriteRect");
	else
		mDebugFillMesh = CAssetManager::GetInst()->GetMeshManager()->FindMesh("SpriteRect");

	return true;
}

bool CTileMapComponent::Init(const char* FileName)
{
	CComponent::Init(FileName);

	CTileMapRenderComponent* Renderer = mOwnerObject->FindSceneComponent<CTileMapRenderComponent>();

	if (Renderer)
	{
		Renderer->SetTileMapComponent(this);
	}

	mLineTransformCBuffer = new CTransformCBuffer;
	mLineTransformCBuffer->Init();

	mCBuffer = new CColliderCBuffer;
	mCBuffer->Init();

	mTileMapCBuffer = new CTileMapCBuffer;
	mTileMapCBuffer->Init();

	mTileShader = CShaderManager::GetInst()->FindShader("TileShader");

	if (mScene)
	{
		mTileMesh = mScene->GetAssetManager()->FindMesh("SpriteRect");
	}
	else
	{
		mTileMesh = CAssetManager::GetInst()->GetMeshManager()->FindMesh("SpriteRect");
	}


	return true;
}

void CTileMapComponent::PreUpdate(float DeltaTime)
{
	CComponent::PreUpdate(DeltaTime);
}

void CTileMapComponent::Update(float DeltaTime)
{
	CComponent::Update(DeltaTime);


	//화면에 Min Max 를 구해보자!!
	FResolution RS = CDevice::GetInst()->GetResolution();
	float zoom = 1.f;
	//카메라의 위치는 화면중앙의 월드위치 
	FVector3D Center = mScene->GetCameraManager()->GetCameraWorldPos();

	auto* cam = mScene->GetCameraManager()->GetViewTarget();
	if (cam) zoom = cam->GetZoom(); // GetZoom() 추가했지?

	float viewW = RS.Width / zoom;
	float viewH = RS.Height / zoom;


	//타일맵 기준에서의 카메라 위치 
	Center -= mOwnerObject->GetWorldPosition();

	//예시
	// 내 위치						150 100
	// 책상 위치					100 100
	// 책상 기준에서의 내위치		50	0

	//뷰포트 좌표에서의 시작 타일 좌표
	// 화면 기준 타일맵 인덱스가 나올것이다. 
	mViewStartX = (int)((Center.x - viewW * 0.5f) / mTileSize.x);
	mViewStartY = (int)((Center.y - viewH * 0.5f) / mTileSize.y);

	mViewEndX = (int)((Center.x + viewW * 0.5f) / mTileSize.x);
	mViewEndY = (int)((Center.y + viewH * 0.5f) / mTileSize.y);

	//예외처리 1
// Start
	if (mViewStartX < 0) mViewStartX = 0;
	else if (mViewStartX >= mCountX) mViewStartX = mCountX - 1;

	if (mViewStartY < 0) mViewStartY = 0;
	else if (mViewStartY >= mCountY) mViewStartY = mCountY - 1;

	// End
	if (mViewEndX < 0) mViewEndX = 0;
	else if (mViewEndX >= mCountX) mViewEndX = mCountX - 1;

	if (mViewEndY < 0) mViewEndY = 0;
	else if (mViewEndY >= mCountY) mViewEndY = mCountY - 1;

	// 추가 안전장치: start <= end 보장
	if (mViewStartX > mViewEndX) std::swap(mViewStartX, mViewEndX);
	if (mViewStartY > mViewEndY) std::swap(mViewStartY, mViewEndY);

}

void CTileMapComponent::PostUpdate(float DeltaTime)
{
	CComponent::PostUpdate(DeltaTime);
}

void CTileMapComponent::PostRender()
{
	CComponent::PostRender();
}

CTileMapComponent* CTileMapComponent::Clone()
{
	return new CTileMapComponent(*this);
}

void CTileMapComponent::EndFrame()
{
	CComponent::EndFrame();
}

void CTileMapComponent::Save(const TCHAR* FullPath)
{
	//유니코드를 멀티바이트로 변경하는 작업
	char ConvertPath[MAX_PATH] = {};

	//TCHar -> char
	int Count = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);

	WideCharToMultiByte(CP_ACP, 0, FullPath, -1, ConvertPath, Count, nullptr, nullptr);

	// Bin\\ 폴더까지의 경로와 이후 경로를 분리한다.
	char FileName[MAX_PATH] = {};

	int RootPathCount = (int)strlen(gRootPathMultibyte);
	RootPathCount += (int)strlen("Asset\\Data\\");

	strcpy_s(FileName, &ConvertPath[RootPathCount]);

	Save(FileName);
}

void CTileMapComponent::Save(const char* FileName)
{
	char FullPath[MAX_PATH] = {};

	strcpy_s(FullPath, gRootPathMultibyte);
	strcat_s(FullPath, "Asset\\Data\\");
	strcat_s(FullPath, FileName);

	FILE* File = nullptr;

	fopen_s(&File, FullPath, "wb");

	if (!File)
	{
		return;
	}

	fwrite(&mTileSize, sizeof(FVector2D), 1, File);
	fwrite(&mCountX, sizeof(int), 1, File);
	fwrite(&mCountY, sizeof(int), 1, File);

	//이미지 프레임 정보
	//std::vector<FAnimationFrame> mTileFrameList;
	int FrameCount = static_cast<int>(mTileFrameList.size());

	fwrite(&FrameCount, sizeof(int), 1, File);
	fwrite(&mTileFrameList[0], sizeof(FAnimationFrame), FrameCount, File);

	//mTileTextureSize
	fwrite(&mTileTextureSize, sizeof(FVector2D), 1, File);


	//각각의 타일을 저장한다. 
	//std::vector<CTile*> mTileList;

	int TileCount = static_cast<int>(mTileList.size());

	for (int i = 0; i < TileCount; ++i)
	{
		mTileList[i]->Save(File);
	}

	CTileMapRenderComponent* Renderer = mOwnerObject->FindSceneComponent<CTileMapRenderComponent>();

	if (Renderer)
	{
		bool TextureEnable = false;

		CTexture* Texture = Renderer->GetBackTexture();

		if (Texture)
		{
			TextureEnable = true;

			fwrite(&TextureEnable, sizeof(bool), 1, File);

			int NameCount = (int)Texture->GetName().length();
			fwrite(&NameCount, sizeof(int), 1, File);

			fwrite(Texture->GetName().c_str(), sizeof(char), NameCount, File);

			fwrite(Texture->GetTexture(0)->FileName, sizeof(TCHAR), MAX_PATH, File);
		}
		else
		{
			fwrite(&TextureEnable, sizeof(bool), 1, File);
		}

		TextureEnable = false;
		Texture = Renderer->GetTileTexture();
		if (Texture)
		{
			TextureEnable = true;

			fwrite(&TextureEnable, sizeof(bool), 1, File);

			int NameCount = (int)Texture->GetName().length();
			fwrite(&NameCount, sizeof(int), 1, File);

			fwrite(Texture->GetName().c_str(), sizeof(char), NameCount, File);

			fwrite(Texture->GetTexture(0)->FileName, sizeof(TCHAR), MAX_PATH, File);
		}
		else
		{
			fwrite(&TextureEnable, sizeof(bool), 1, File);
		}
	}
	fclose(File);
}

void CTileMapComponent::Load(const TCHAR* FullPath)
{
	//유니코드를 멀티바이트로 변경하는 작업
	char ConvertPath[MAX_PATH] = {};

	//TCHar -> char
	int Count = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);

	WideCharToMultiByte(CP_ACP, 0, FullPath, -1, ConvertPath, Count, nullptr, nullptr);

	// Bin\\ 폴더까지의 경로와 이후 경로를 분리한다.
	char FileName[MAX_PATH] = {};

	int RootPathCount = (int)strlen(gRootPathMultibyte);
	RootPathCount += (int)strlen("Asset\\Data\\");

	strcpy_s(FileName, &ConvertPath[RootPathCount]);

	Load(FileName);
}

void CTileMapComponent::Load(const char* FileName)
{
	char FullPath[MAX_PATH] = {};

	strcpy_s(FullPath, gRootPathMultibyte);
	strcat_s(FullPath, "Asset\\Data\\");
	strcat_s(FullPath, FileName);

	FILE* File = nullptr;

	fopen_s(&File, FullPath, "rb");

	if (!File)
	{
		return;
	}

	fread(&mTileSize, sizeof(FVector2D), 1, File);
	fread(&mCountX, sizeof(int), 1, File);
	fread(&mCountY, sizeof(int), 1, File);


	int FrameCount = 0;

	fread(&FrameCount, sizeof(int), 1, File);

	mTileFrameList.clear();
	mTileFrameList.resize((size_t)FrameCount);

	fread(&mTileFrameList[0], sizeof(FAnimationFrame), FrameCount, File);

	//mTileTextureSize
	fread(&mTileTextureSize, sizeof(FVector2D), 1, File);


	//타일
	//기존의 타일을 삭제한다.
	size_t Size = mTileList.size();

	for (size_t i = 0; i < Size; ++i)
	{
		SAFE_DELETE(mTileList[i]);
	}

	mTileList.clear();

	int TileCount = mCountX * mCountY;

	mTileList.resize(TileCount);

	for (int i = 0; i < TileCount; ++i)
	{
		CTile* Tile = new CTile;

		Tile->Load(File);

		mTileList[i] = Tile;
	}

	CTileMapRenderComponent* Renderer = mOwnerObject->FindSceneComponent<CTileMapRenderComponent>();

	if (Renderer)
	{
		bool TextureEnable = false;

		fread(&TextureEnable, sizeof(bool), 1, File);

		if (TextureEnable)
		{
			char TexName[256] = {};
			int NameCount = 0;

			fread(&NameCount, sizeof(int), 1, File);
			fread(TexName, sizeof(char), NameCount, File);

			TCHAR FileName[MAX_PATH] = {};

			fread(FileName, sizeof(TCHAR), MAX_PATH, File);

			Renderer->SetBackTexture(TexName, FileName);
		}

		TextureEnable = false;
		fread(&TextureEnable, sizeof(bool), 1, File);

		if (TextureEnable)
		{
			char TexName[256] = {};
			int NameCount = 0;

			fread(&NameCount, sizeof(int), 1, File);
			fread(TexName, sizeof(char), NameCount, File);

			TCHAR FileName[MAX_PATH] = {};

			fread(FileName, sizeof(TCHAR), MAX_PATH, File);

			Renderer->SetTileTexture(TexName, FileName);
		}
	}
	fclose(File);
}
