#include "Widget.h"

#include "../Asset/AssetManager.h"
#include "../Shader/ShaderManager.h"
#include "../Shader/TransformCBuffer.h"
#include "../Shader/UICBuffer.h"
#include "../Shader/Shader.h"
#include "../Asset/Mesh/Mesh.h"
#include "../Asset/Mesh/MeshManager.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneAssetManager.h"

#include "../Share/Log.h"

FMatrix CWidget::mUIProj;


CWidget::CWidget()
{
}

CWidget::~CWidget()
{
	SAFE_DELETE(mTransformCBuffer);
	SAFE_DELETE(mUICBuffer);
}



void CWidget::SetShader(const std::string& Name)
{
	mShader = CShaderManager::GetInst()->FindShader(Name);
}

void CWidget::SetShader(class CShader* Shader)
{
	mShader = Shader;
}

bool CWidget::Init()
{
	SetShader("UIShader");

	if (mScene)
	{
		mMesh = mScene->GetAssetManager()->FindMesh("SpriteRect");
	}
	else
	{
		mMesh = CAssetManager::GetInst()->GetMeshManager()->FindMesh("SpriteRect");
	}

	mUICBuffer = new CUICBuffer;

	mUICBuffer->Init();

	mTransformCBuffer = new CTransformCBuffer;

	mTransformCBuffer->Init();

	return true;
}

void CWidget::ForceUnhover()
{
	if (mMouseOn)
	{
		mMouseOn = false;
		MouseUnHovered();
	}
}

FVector2D CWidget::GetAnchorPos() const
{

	if (!mParent)
		return mPos;

	FVector2D parentPivotWorld = mParent->mRenderPos + (mParent->mSize * mParent->mPivot);

	return parentPivotWorld + mPos;

}
void CWidget::Render()
{
	FVector2D anchor = GetAnchorPos();
	if (mParent)
	{
		FVector2D margin = (mParent->GetPivot() - 0.5f) * mParent->GetSize();
		mRenderPos = anchor - (mSize * mPivot) - margin;
	}
	else
	{
		mRenderPos = anchor - (mSize * mPivot);
	}

}

void CWidget::Render(const FVector3D& Pos)
{
	FVector2D anchor = GetAnchorPos();
	anchor.x += Pos.x;
	anchor.y += Pos.y;

	mRenderPos = anchor - (mSize * mPivot);      // 항상 좌상단
	mUICBuffer->SetFlip(mIsFlip);
}

void CWidget::Update(float DeltaTime)
{
}

bool CWidget::CollisionMouse(CWidget** outHit, const FVector2D& MousePos)
{
    if (outHit) *outHit = nullptr;


    FVector2D Min = mRenderPos;
    FVector2D Max = Min + mSize;

    if (mRotation == 0.f || mRotation == 360.f)
    {
        if (MousePos.x < Min.x || MousePos.x > Max.x ||
            MousePos.y < Min.y || MousePos.y > Max.y)
        {
            if (mMouseOn)
            {
                mMouseOn = false;
                MouseUnHovered();
            }
            return false;
        }

        if (outHit) *outHit = this;

        mMouseOn = true;
        MouseHovered();
        return true;
    }

    FVector2D Axis[2];
    FVector2D HalfSize = mSize * 0.5f;
    FVector2D Center = mRenderPos + (mSize * 0.5f);

    FMatrix matRot;
    matRot.RotationZ(mRotation);

    FVector3D OriginAxis[2];
    OriginAxis[0] = FVector3D::Axis[EAxis::X];
    OriginAxis[1] = FVector3D::Axis[EAxis::Y];

    OriginAxis[0] = OriginAxis[0].TransformNormal(matRot);
    OriginAxis[1] = OriginAxis[1].TransformNormal(matRot);

    Axis[0].x = OriginAxis[0].x;
    Axis[0].y = OriginAxis[0].y;
    Axis[1].x = OriginAxis[1].x;
    Axis[1].y = OriginAxis[1].y;

    Axis[0].Normalize();
    Axis[1].Normalize();

    FVector2D CenterLine = Center - MousePos;

    float Dist = abs(CenterLine.Dot(Axis[0]));
    if (Dist > HalfSize.x)
    {
        if (mMouseOn)
        {
            mMouseOn = false;
            MouseUnHovered();
        }
        return false;
    }

    Dist = abs(CenterLine.Dot(Axis[1]));
    if (Dist > HalfSize.y)
    {
        if (mMouseOn)
        {
            mMouseOn = false;
            MouseUnHovered();
        }
        return false;
    }

    if (outHit) *outHit = this;
    mMouseOn = true;
    MouseHovered();
    return true;
}

void CWidget::EndFrame()
{

}

void CWidget::MouseHovered()
{
}

void CWidget::MouseUnHovered()
{
}
