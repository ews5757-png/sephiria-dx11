#include "SpriteComponent.h"

#include "../Share/Timer.h"
#include "../Shader/Shader.h"
#include "../Shader/ShaderManager.h"
#include "../Asset/AssetManager.h"
#include "../Asset/Mesh/Mesh.h"
#include "../Asset/Mesh/MeshManager.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneAssetManager.h"
#include "../Asset/Texture/TextureManager.h"
#include "../Asset/Texture/Texture.h"
#include "../Shader/SpriteCBuffer.h"
#include "../Shader/TransformCBuffer.h"
#include "../Component/CameraComponent.h"
#include "../Scene/CameraManager.h"

#include "../Animation/Animation2D.h"
#include "../Asset/Animation/Animation2DData.h"

CSpriteComponent::CSpriteComponent()
{
	mRenderType = EComponentRender::Render;
	mRenderLayerName = "Object";
}

CSpriteComponent::CSpriteComponent(const CSpriteComponent& Com)
	: CSceneComponent(Com)
{
}

CSpriteComponent::CSpriteComponent(CSpriteComponent&& Com)
	: CSceneComponent(Com)
{
}

CSpriteComponent::~CSpriteComponent()
{
	SAFE_DELETE(mAnimation);

	SAFE_DELETE(mSpriteCBuffer);
}

FVector2D CSpriteComponent::GetTextureSize()
{
	return mTexture->GettextureSize();
}

void CSpriteComponent::SetShader(const std::string& Name)
{
	mShader = CShaderManager::GetInst()->FindShader(Name);
}

void CSpriteComponent::SetShader(class CShader* Shader)
{
	mShader = Shader;
}

void CSpriteComponent::SetTexture(const std::string& Name, int TextureIndex)
{
	if (mScene)
	{
		//Scene->SceneAssetManager 통해서
		mTexture = mScene->GetAssetManager()->FindTexture(Name);
	}
	else
	{
		//AssetManager 에서 TextureManager통해서 가져올수도 있다.
		mTexture = CAssetManager::GetInst()->GetTextureManager()->FindTexture(Name);
	}

	mTextureIndex = TextureIndex;

}

void CSpriteComponent::SetTexture(const std::string& Name, const TCHAR* FileName, int TextureIndex)
{
	if (mScene)
	{
		//Scene->SceneAssetManager 통해서
		if (!mScene->GetAssetManager()->LoadTexture(Name, FileName))
		{
			return;
		}
		mTexture = mScene->GetAssetManager()->FindTexture(Name);
	}
	else
	{
		if (!CAssetManager::GetInst()->GetTextureManager()->LoadTexture(Name, FileName))
		{
			return;
		}

		mTexture = CAssetManager::GetInst()->GetTextureManager()->FindTexture(Name);
	}

	mTextureIndex = TextureIndex;
}

void CSpriteComponent::SetTexture(class CTexture* Texture, int TextureIndex)
{
	mTexture = Texture;
	mTextureIndex = TextureIndex;
}

void CSpriteComponent::SetTextureIndex(int Index)
{
	mTextureIndex = Index;
}

void CSpriteComponent::SetOverrideColor(int b)
{
	mOverrideColor = b;
}

void CSpriteComponent::SetTint(float r, float g, float b)
{
	mTint.x = r;
	mTint.y = g;
	mTint.z = b;
}

void CSpriteComponent::SetTint(float r, float g, float b, float a)
{
	mTint.x = r;
	mTint.y = g;
	mTint.z = b;
	mTint.w = a;
}
void CSpriteComponent::SetTint(FVector4D v)
{
	mTint.x = v.x;
	mTint.y = v.y;
	mTint.z = v.z;
	mTint.w = v.w;
}

void CSpriteComponent::SetTint(FVector3D v, float w)
{
	mTint.x = v.x;
	mTint.y = v.y;
	mTint.z = v.z;
	mTint.w = w;
}

void CSpriteComponent::SetOpacity(float Opacity)
{
	mTint.w = Opacity;
}

void CSpriteComponent::SetFlip(bool Flip)
{
	mIsFlip = Flip;
}

const CAnimation2DData* CSpriteComponent::GetCurrentAnimationData()
{
	if (mAnimation)
	{
		return mAnimation->mCurrentSequence->GetAnimationAsset();
	}

	return nullptr;
}


bool CSpriteComponent::Init()
{
	CSceneComponent::Init();

	mSpriteCBuffer = new CSpriteCBuffer;
	mSpriteCBuffer->Init();

	SetShader("SpriteShader");

	if (mScene)
	{
		mMesh = mScene->GetAssetManager()->FindMesh("SpriteRect");
	}
	else
	{
		mMesh = CAssetManager::GetInst()->GetMeshManager()->FindMesh("SpriteRect");
	}


	return true;
}

bool CSpriteComponent::Init(const char* FileName)
{
	CSceneComponent::Init(FileName);

	mSpriteCBuffer = new CSpriteCBuffer;
	mSpriteCBuffer->Init();

	SetShader("SpriteShader");

	//SpriteRect
	// Sprite이미지는 Mesh 고정해서 사용할것이다. 
	if (mScene)
	{
		mMesh = mScene->GetAssetManager()->FindMesh("SpriteRect");
	}
	else
	{
		mMesh = CAssetManager::GetInst()->GetMeshManager()->FindMesh("SpriteRect");
	}

	return true;
}

void CSpriteComponent::PreUpdate(float DeltaTime)
{
	CSceneComponent::PreUpdate(DeltaTime);
}

void CSpriteComponent::Update(float DeltaTime)
{
	CSceneComponent::Update(DeltaTime);

	if (mAnimation)
		mAnimation->Update(DeltaTime);

	if (mUseFrameNativeSize)
	{
		FVector2D native = GetCurrentFrameNativeSize();
		if (native.x > 0.f && native.y > 0.f)
		{
			native.x *= mFrameNativeScale;
			native.y *= mFrameNativeScale;

			// 기준 메시 크기에 맞춰 스케일로 환산
			FVector3D scale = GetRelativeScale();
			scale.x = native.x / mBaseMeshSize.x;
			scale.y = native.y / mBaseMeshSize.y;
			SetRelativeScale(scale);
		}
	}

}

void CSpriteComponent::PostUpdate(float DeltaTime)
{
	CSceneComponent::PostUpdate(DeltaTime);
}

void CSpriteComponent::Collision(float DeltaTime)
{
	CSceneComponent::Collision(DeltaTime);
}

void CSpriteComponent::PreRender()
{
	CSceneComponent::PreRender();
}

void CSpriteComponent::Render()
{
	CSceneComponent::Render();

	//애니메이션 상수버퍼 셋팅
	if (mAnimation)
	{
		//애니메이션 셋팅
		mAnimation->SetShader();
	}
	else
	{
		CAnimation2D::DisableAnimation();
	}

	//반전여부 
	CAnimation2D::SetAnimFlip(mIsFlip);

	//스프라이트 상수버퍼 셋팅 해주기
	mSpriteCBuffer->SetTint(mTint);
	mSpriteCBuffer->SetUseTime(mUseTime);
	mSpriteCBuffer->SetOverridecolor(mOverrideColor);


	if (mUseTime)
	{
		static float TotalTime = 0.f;
		TotalTime += CTimer::GetDeltaTime() * 0.2f;
		mSpriteCBuffer->SetTime(TotalTime);
	}


	mSpriteCBuffer->UpdateBuffer();

	//Trnasform;
	mTransformCBuffer->SetWorldMatrix(mmatWorld);
	FMatrix matView, matProj;
	matView = mScene->GetCameraManager()->GetViewMatrix();
	matProj = mScene->GetCameraManager()->GetProjMatrix();

	mTransformCBuffer->SetViewMatrix(matView);
	mTransformCBuffer->SetProjMatrix(matProj);
	mTransformCBuffer->SetPivot(mPivot);
	//FMatrix matProj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(90.f), 1280.f / 720.f, 0.5f, 1000.f);
	//mTransformCBuffer->SetProjMatrix(matProj);

	mTransformCBuffer->UpdateBuffer();

	mShader->SetShader();

	if (mTexture)
	{
		mTexture->SetShader(0, EShaderBufferType::Pixel, mTextureIndex);
	}

	mMesh->Render();
}

void CSpriteComponent::PostRender()
{
	CSceneComponent::PostRender();
}

CSpriteComponent* CSpriteComponent::Clone()
{
	return new CSpriteComponent(*this);
}

FVector2D CSpriteComponent::GetCurrentFrameNativeSize() const
{
	// 1) 애니메이션(시트) 기반: 현재 프레임 rect/size가 어딘가에 있어야 함
	if (mAnimation && mAnimation->mCurrentSequence)
	{
		CAnimation2DData* data = mAnimation->mCurrentSequence->GetAnimationAsset();
		if (data)
		{
			return data->GetTexture()->GettextureSize();
		}
	}

	// 2) 텍스처 여러 장 방식: mTextureIndex가 프레임이라고 가정하는 경우
	if (mTexture && mTexture->GetTextureCount() > 0)
	{
		int idx = mTextureIndex;
		if (idx < 0) idx = 0;
		if (idx >= mTexture->GetTextureCount()) idx = mTexture->GetTextureCount() - 1;

		const FTextureInfo* info = mTexture->GetTexture(idx);
		if (info)
			return FVector2D((float)info->Width, (float)info->Height);
	}

	// fallback
	return FVector2D(0.f, 0.f);
}
