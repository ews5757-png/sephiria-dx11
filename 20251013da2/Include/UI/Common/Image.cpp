#include "Image.h"

#include "../../Asset/AssetManager.h"
#include "../../Asset/Mesh/Mesh.h"
#include "../../Asset/Texture/Texture.h"
#include "../../Asset/Texture/TextureManager.h"
#include "../../Scene/Scene.h"
#include "../../Scene/SceneAssetManager.h"
#include "../../Scene/Input.h"
#include "../../Shader/Shader.h"
#include "../../Shader/TransformCBuffer.h"
#include "../../Shader/UICBuffer.h"
#include "../../Scene/CameraManager.h"
#include "../../Share/Log.h"

CImage::CImage()
{
}

CImage::~CImage()
{
}

int CImage::GetAnimFrameCount() const
{
	{
		if (!mBrush.Frames.empty())
			return (int)mBrush.Frames.size();

		// 프레임-텍스처 방식(프레임 더미 안 채웠을 때도 돌아가게)
		if (mBrush.Texture && mBrush.Texture->GetTextureCount() > 0)
			return mBrush.Texture->GetTextureCount();

		return 0;
	}
}

void CImage::SetTexture(const std::string& Name)
{
	if (mScene)
	{
		mBrush.Texture = mScene->GetAssetManager()->FindTexture(Name);
	}
	else
	{
		mBrush.Texture = CAssetManager::GetInst()->GetTextureManager()->FindTexture(Name);
	}
	if (!mBrush.Texture)
		CLog::PrintLog(std::string("CImage::SetTexture FAIL : ") + Name);
}

void CImage::SetTexture(const std::string& Name, const TCHAR* FileName)
{
	if (mScene)
	{
		if (!mScene->GetAssetManager()->LoadTexture(Name, FileName))
		{
			return;

		}

		mBrush.Texture = mScene->GetAssetManager()->FindTexture(Name);
	}
	else
	{
		if (!CAssetManager::GetInst()->GetTextureManager()->LoadTexture(Name, FileName))
		{
			return;
		}

		mBrush.Texture = CAssetManager::GetInst()->GetTextureManager()->FindTexture(Name);
	}
}

void CImage::SetTexture(class CTexture* Texture)
{
	mBrush.Texture = Texture;
}

void CImage::SetTint(float r, float g, float b, float a)
{
	mBrush.Tint.x = r;
	mBrush.Tint.y = g;
	mBrush.Tint.z = b;
	mBrush.Tint.w = a;
}

void CImage::SetBrushAnimation(bool Animation)
{
	mBrush.AnimationEnable = Animation;

	// ✅ 시작/재시작 시 종료 플래그 초기화
	if (Animation)
		mAnimEndCalled = false;
}

void CImage::AddBurshFrame(const FVector2D& Start, const FVector2D& Size)
{
	FAnimationFrame Frame;
	Frame.Start = Start;
	Frame.Size = Size;

	mBrush.Frames.emplace_back(Frame);

	mBrush.FrameTime = mBrush.PlayTime / mBrush.Frames.size();
}

void CImage::AddBurshFrame(float StartX, float StartY, float SizeX, float SizeY)
{
	FAnimationFrame Frame;
	Frame.Start.x = StartX;
	Frame.Start.y = StartY;
	Frame.Size.x = SizeX;
	Frame.Size.y = SizeY;

	mBrush.Frames.emplace_back(Frame);

	mBrush.FrameTime = mBrush.PlayTime / mBrush.Frames.size();
}

void CImage::SetCurrentFrame(int Frame)
{
	mBrush.Frame = Frame;
}

void CImage::SetAnimationPlayTime(float PlayTime)
{
	mBrush.PlayTime = PlayTime;
	mBrush.FrameTime = 0.f; // ★ 반드시 추가
}

void CImage::SetAnimationPlayRate(float PlayRate)
{
	mBrush.PlayRate = PlayRate;
	mBrush.FrameTime = 0.f; // ★ 반드시 추가
}

bool CImage::Init()
{
	if (!CWidget::Init())
	{
		return false;
	}

	return true;
}

void CImage::Update(float DeltaTime)
{
	CWidget::Update(DeltaTime);

	// --- 프레임 카운트 결정 (프레임-텍스처 > 시트) ---
	const int texCount = (mBrush.Texture ? mBrush.Texture->GetTextureCount() : 0);
	const int frameCount = (texCount > 1) ? texCount : (int)mBrush.Frames.size();

	if (mUseFrameNativeSize)
	{
		// 프레임-텍스처 방식(CTexture에 여러 장)
		if (mBrush.Texture && mBrush.Texture->GetTextureCount() > 0)
		{
			int idx = mBrush.Frame;
			const int max = mBrush.Texture->GetTextureCount();

			if (idx < 0) idx = 0;
			if (idx >= max) idx = max - 1;

			const FTextureInfo* info = mBrush.Texture->GetTexture(idx);
			if (info)
			{
				mSize.x = (float)info->Width * mFrameNativeScale;
				mSize.y = (float)info->Height * mFrameNativeScale;
			}
		}
		// 시트 방식(Frames에 Size 들어있음)
		else if (!mBrush.Frames.empty())
		{
			int idx = mBrush.Frame;
			if (idx < 0) idx = 0;
			if (idx >= (int)mBrush.Frames.size())
				idx = (int)mBrush.Frames.size() - 1;

			mSize.x = mBrush.Frames[idx].Size.x * mFrameNativeScale;
			mSize.y = mBrush.Frames[idx].Size.y * mFrameNativeScale;
		}
	}


	if (frameCount <= 0)
		return;

	// --- FrameTime 안전 보정 (AddBurshFrame 안 해도 빠르게 도는 문제 방지) ---
	if (mBrush.FrameTime <= 0.f)
	{
		float playTime = mBrush.PlayTime;
		if (playTime <= 0.f) playTime = 1.f;
		mBrush.FrameTime = playTime / (float)frameCount;
	}

	// --- 애니메이션 진행 ---
	if (mBrush.AnimationEnable)
	{
		mBrush.Time += DeltaTime * mBrush.PlayRate;

		// while로 큰 델타 타임에도 안정적으로 처리
		while (mBrush.Time >= mBrush.FrameTime)
		{
			mBrush.Time -= mBrush.FrameTime;

			// 마지막 프레임 처리
			if (mBrush.Frame >= frameCount - 1)
			{
				if (mAnimLoop)
				{
					mBrush.Frame = 0;
				}
				else
				{
					mBrush.Frame = frameCount - 1;
					mBrush.AnimationEnable = false;

					if (!mAnimEndCalled)
					{
						mAnimEndCalled = true;
						if (mAnimEndFunc) mAnimEndFunc();
					}
					break; // 여기서 끝(사이즈 갱신은 아래에서 그대로 됨)
				}
			}
			else
			{
				++mBrush.Frame;
			}
		}
	}

	// --- ✅ 프레임 원본 크기 적용(스케일 포함) ---

}

void CImage::Render()
{
	CWidget::Render();

	FMatrix matScale, matRot, matTranslate, matWorld;

	matScale.Scaling(mSize);
	matRot.RotationZ(mRotation);
	matTranslate.Translation(mRenderPos);

	matWorld = matScale * matRot * matTranslate;

	mTransformCBuffer->SetWorldMatrix(matWorld);
	mTransformCBuffer->SetProjMatrix(mUIProj);
	mTransformCBuffer->SetPivot(mPivot);
	mTransformCBuffer->UpdateBuffer();

	// UI 버퍼(색/텍스쳐/애니메이션) 셋팅
	mUICBuffer->SetTint(mBrush.Tint);

	// =========================
	// 텍스처 바인딩(프레임 텍스처 지원)
	// =========================
	int texIndex = 0;

	if (mBrush.Texture)
	{
		const int texCount = mBrush.Texture->GetTextureCount();
		if (texCount > 0)
			texIndex = mBrush.Frame % texCount;

		mUICBuffer->SetTextureEnable(true);
		mBrush.Texture->SetShader(0, EShaderBufferType::Pixel, texIndex);
	}
	else
	{
		mUICBuffer->SetTextureEnable(false);
	}

	// =========================
	// UV 설정(시트 / 프레임 텍스처 분기)
	// =========================
	if (mBrush.AnimationEnable)
	{
		mUICBuffer->SetAnimationEnable(true);

		// 프레임 텍스처 방식: CTexture 안에 여러 장이 들어있거나, Frames가 비어있으면 전체 UV 고정
		if (mBrush.Texture && (mBrush.Texture->GetTextureCount() > 1 || mBrush.Frames.empty()))
		{
			mUICBuffer->SetUV(0.f, 0.f, 1.f, 1.f);
		}
		else
		{
			// 기존 시트 방식
			if (!mBrush.Texture || mBrush.Frames.empty())
			{
				mUICBuffer->SetUV(0.f, 0.f, 1.f, 1.f);
			}
			else
			{
				int Frame = mBrush.Frame;
				if (Frame < 0) Frame = 0;
				if (Frame >= (int)mBrush.Frames.size()) Frame = 0;

				FAnimationFrame FrameInfo = mBrush.Frames[Frame];

				float LTX = FrameInfo.Start.x / mBrush.Texture->GetTexture(0)->Width;
				float LTY = FrameInfo.Start.y / mBrush.Texture->GetTexture(0)->Height;
				float RBX = LTX + FrameInfo.Size.x / mBrush.Texture->GetTexture(0)->Width;
				float RBY = LTY + FrameInfo.Size.y / mBrush.Texture->GetTexture(0)->Height;

				mUICBuffer->SetUV(LTX, LTY, RBX, RBY);
			}
		}
	}
	else
	{
		mUICBuffer->SetAnimationEnable(false);
	}

	mUICBuffer->UpdateBuffer();

	// 드로우
	mShader->SetShader();
	mMesh->Render();
}


void CImage::Render(const FVector3D& Pos)
{
	CWidget::Render(Pos);

	FVector2D RenderPos = mRenderPos;

	FMatrix matScale, matRot, matTranslate, matWorld;

	matScale.Scaling(mSize);
	matRot.RotationZ(mRotation);
	matTranslate.Translation(RenderPos);

	matWorld = matScale * matRot * matTranslate;

	FMatrix matView = mScene->GetCameraManager()->GetViewMatrix();
	FMatrix matProj = mScene->GetCameraManager()->GetProjMatrix();

	mTransformCBuffer->SetWorldMatrix(matWorld);
	mTransformCBuffer->SetViewMatrix(matView);
	mTransformCBuffer->SetProjMatrix(matProj);
	mTransformCBuffer->SetPivot(mPivot);
	mTransformCBuffer->UpdateBuffer();

	mUICBuffer->SetTint(mBrush.Tint);

	// =========================
	// 텍스처 바인딩(프레임 텍스처 지원)
	// =========================
	int texIndex = 0;

	if (mBrush.Texture)
	{
		const int texCount = mBrush.Texture->GetTextureCount();
		if (texCount > 0)
			texIndex = mBrush.Frame % texCount;

		mUICBuffer->SetTextureEnable(true);
		mBrush.Texture->SetShader(0, EShaderBufferType::Pixel, texIndex);
	}
	else
	{
		mUICBuffer->SetTextureEnable(false);
	}

	// =========================
	// UV 설정(시트 / 프레임 텍스처 분기)
	// =========================
	if (mBrush.AnimationEnable)
	{
		mUICBuffer->SetAnimationEnable(true);

		if (mBrush.Texture && (mBrush.Texture->GetTextureCount() > 1 || mBrush.Frames.empty()))
		{
			mUICBuffer->SetUV(0.f, 0.f, 1.f, 1.f);
		}
		else
		{
			if (!mBrush.Texture || mBrush.Frames.empty())
			{
				mUICBuffer->SetUV(0.f, 0.f, 1.f, 1.f);
			}
			else
			{
				int Frame = mBrush.Frame;
				if (Frame < 0) Frame = 0;
				if (Frame >= (int)mBrush.Frames.size()) Frame = 0;

				FAnimationFrame FrameInfo = mBrush.Frames[Frame];

				float LTX = FrameInfo.Start.x / mBrush.Texture->GetTexture(0)->Width;
				float LTY = FrameInfo.Start.y / mBrush.Texture->GetTexture(0)->Height;
				float RBX = LTX + FrameInfo.Size.x / mBrush.Texture->GetTexture(0)->Width;
				float RBY = LTY + FrameInfo.Size.y / mBrush.Texture->GetTexture(0)->Height;

				mUICBuffer->SetUV(LTX, LTY, RBX, RBY);
			}
		}
	}
	else
	{
		mUICBuffer->SetAnimationEnable(false);
	}

	mUICBuffer->UpdateBuffer();

	mShader->SetShader();
	mMesh->Render();
}

