#include "RenderManager.h"

#include "RenderState.h"
#include "RenderStateManager.h"
#include "../Component/SceneComponent.h"
#include "../Device.h"
#include "../Scene/SceneManager.h"
#include "../Asset/Mesh/Mesh.h"
#include "../Asset/AssetManager.h"
#include "../Asset/Mesh/MeshManager.h"
#include "../Shader/ShaderManager.h"
#include "../Shader/Shader.h"
#include "../Share/Log.h"

void CRenderManager::DumpCurRTV(const char* tag) const
{
	auto* ctx = CDevice::GetInst()->GetContext();
	ID3D11RenderTargetView* cur = nullptr;
	ID3D11DepthStencilView* dsv = nullptr;
	ctx->OMGetRenderTargets(1, &cur, &dsv);

	if (cur == mSceneRT.RTV)
		CLog::PrintLog(std::string(tag) + " : CUR = SceneRT");
	else if (cur == CDevice::GetInst()->GetBackBufferRTV())
		CLog::PrintLog(std::string(tag) + " : CUR = BackBuffer");
	else if (cur == nullptr)
		CLog::PrintLog(std::string(tag) + " : CUR = NULL");
	else
		CLog::PrintLog(std::string(tag) + " : CUR = Other");

	SAFE_RELEASE(cur);
	SAFE_RELEASE(dsv);
}

static bool CreateRT(FRenderTarget& outRT, int w, int h, DXGI_FORMAT fmt)
{
    auto* dev = CDevice::GetInst()->GetDevice();

    D3D11_TEXTURE2D_DESC td = {};
    td.Width = w;
    td.Height = h;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = fmt;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    if (FAILED(dev->CreateTexture2D(&td, nullptr, &outRT.Tex))) return false;
    if (FAILED(dev->CreateRenderTargetView(outRT.Tex, nullptr, &outRT.RTV))) return false;
    if (FAILED(dev->CreateShaderResourceView(outRT.Tex, nullptr, &outRT.SRV))) return false;

    outRT.Width = w;
    outRT.Height = h;
    return true;
}

CRenderManager::CRenderManager()
{

}

CRenderManager::~CRenderManager()
{
	auto iter = mLayerList.begin();
	auto iterEnd = mLayerList.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}
	SAFE_RELEASE(mSampler);
	SAFE_DELETE(mStateManager);
	SAFE_RELEASE(mBlendWriteAll);
	SAFE_RELEASE(mPostSampler);


}

void CRenderManager::BeginSceneRT()
{
    auto* dev = CDevice::GetInst();
    auto* ctx = dev->GetContext();

    // ✅ 여기 핵심: DSV를 nullptr
	ctx->OMSetRenderTargets(1, &mSceneRT.RTV, mSceneDepthDSV);

    float clear[4] = { 0,0,0,1 };
    ctx->ClearRenderTargetView(mSceneRT.RTV, clear);

    D3D11_VIEWPORT vp{};
    vp.TopLeftX = 0.f;
    vp.TopLeftY = 0.f;
    vp.Width  = (float)mSceneRT.Width;
    vp.Height = (float)mSceneRT.Height;
    vp.MinDepth = 0.f;
    vp.MaxDepth = 1.f;
    ctx->RSSetViewports(1, &vp);
}

void CRenderManager::EndToBackBuffer()
{
	// BackBuffer로 출력 타겟 전환(기존 함수 사용 가능)
	CDevice::GetInst()->SetTarget();
}

void CRenderManager::RenderCopySceneToBackBuffer()
{
	
	auto* ctx = CDevice::GetInst()->GetContext();

	CDevice::GetInst()->SetTarget();

	FResolution rs = CDevice::GetInst()->GetResolution();
	D3D11_VIEWPORT vp{};
	vp.TopLeftX = 0; vp.TopLeftY = 0;
	vp.Width = (float)rs.Width;
	vp.Height = (float)rs.Height;
	vp.MinDepth = 0.f; vp.MaxDepth = 1.f;
	ctx->RSSetViewports(1, &vp);

	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	ctx->PSSetShaderResources(0, 1, nullSRV);

	ctx->PSSetShaderResources(0, 1, &mSceneRT.SRV);
	ctx->PSSetSamplers(0, 1, &mSampler);

	mCopyShader->SetShader();

	// ✅ topology / vb/ib는 Mesh가 하긴 하지만, 확실히 해도 됨
	mFullScreenMesh->Render();

	ctx->PSSetShaderResources(0, 1, nullSRV);
}

void CRenderManager::AddRenderList(class CSceneComponent* Component)
{
	//mRenderList.emplace_back(Component);

	FRenderLayer* Layer = FindLayer(Component->GetRenderLayerName());

	if (!Layer)
	{
		return;
	}

	//등록
	Layer->RenderList.emplace_back(Component);
}

void CRenderManager::RemoveRenderList(CSceneComponent* Component)
{
	// 1. 해당 컴포넌트가 속한 레이어를 찾습니다.
	FRenderLayer* Layer = FindLayer(Component->GetRenderLayerName());
	if (!Layer) return;

	// 2. 해당 레이어의 리스트에서 컴포넌트를 찾아 삭제합니다.
	auto iter = Layer->RenderList.begin();
	auto iterEnd = Layer->RenderList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter).Get() == Component)
		{
			Layer->RenderList.erase(iter);
			return; // 찾아서 지웠으면 종료
		}
	}
}

void CRenderManager::ClearRenderList()
{
	auto iter = mLayerList.begin();
	auto iterEnd = mLayerList.end();

	for (; iter != iterEnd; ++iter)
	{
		iter->second->RenderList.clear();
	}
}
bool CRenderManager::CreateRenderLayer(const std::string& Name, int ZOrder)
{
	FRenderLayer* Layer = FindLayer(Name);

	if (Layer)
	{
		return true;
	}

	Layer = new FRenderLayer;

	Layer->ZOrder = ZOrder;

	mLayerList.insert(std::make_pair(ZOrder, Layer));
	mLayerNameList[Name] = ZOrder;

	return true;
}

FRenderLayer* CRenderManager::FindLayer(const std::string& Name)
{
	auto iter = mLayerNameList.find(Name);

	if (iter == mLayerNameList.end())
	{
		return nullptr;
	}

	int ZOrder = iter->second;

	auto iter1 = mLayerList.find(ZOrder);

	return iter1->second;
}


void CRenderManager::RenderBloom()
{
	auto* dev = CDevice::GetInst();
	auto* ctx = dev->GetContext();

	ID3D11ShaderResourceView* nullSRV[8] = { nullptr };

	auto ApplyPostCommon = [&]()
		{
			ctx->RSSetScissorRects(0, nullptr);
			ctx->OMSetBlendState(nullptr, nullptr, 0xffffffff); // blend off
		};

	// Debug: 원본 확인
	if (mBloomDebug == 4)
	{
		DebugBlitSRV(mSceneRT.SRV);
		return;
	}

	// ============================================================
	// 1) Bright Extract : SceneRT -> BrightRT
	// ============================================================
	{
		ctx->PSSetSamplers(0, 1, &mPostSampler);
		ctx->PSSetShaderResources(0, 8, nullSRV);

		ctx->OMSetRenderTargets(1, &mBrightRT.RTV, nullptr);

		SetViewport(mBrightRT.Width, mBrightRT.Height);
		ApplyPostCommon();

		if (mDepthDisable) mDepthDisable->SetState();
		if (mRasterizer)   mRasterizer->SetState();

		ctx->PSSetShaderResources(0, 1, &mSceneRT.SRV); // t0 = Scene

		mBrightShader->SetShader();
		UpdatePostCB_Threshold(0.0002f);

		mFullScreenMesh->Render();

		if (mBloomDebug == 1)
		{
			DebugBlitSRV(mBrightRT.SRV);
			return;
		}
	}

	// ============================================================
	// 2) Blur H : BrightRT -> BlurA
	// ============================================================
	{
		ctx->PSSetSamplers(0, 1, &mPostSampler);
		ctx->PSSetShaderResources(0, 8, nullSRV);

		ctx->OMSetRenderTargets(1, &mBlurA.RTV, nullptr);

		SetViewport(mBlurA.Width, mBlurA.Height);
		ApplyPostCommon();

		if (mDepthDisable) mDepthDisable->SetState();
		if (mRasterizer)   mRasterizer->SetState();

		ctx->PSSetShaderResources(0, 1, &mBrightRT.SRV); // t0 = Bright

		mBlurShader->SetShader();
		UpdatePostCB_Blur(mBlurA.Width, mBlurA.Height, 1.0f, 0.0f);

		mFullScreenMesh->Render();

		if (mBloomDebug == 2)
		{
			DebugBlitSRV(mBlurA.SRV);
			return;
		}
	}

	// ============================================================
	// 3) Blur V : BlurA -> BlurB
	// ============================================================
	{
		ctx->PSSetSamplers(0, 1, &mPostSampler);
		ctx->PSSetShaderResources(0, 8, nullSRV);

		ctx->OMSetRenderTargets(1, &mBlurB.RTV, nullptr);

		SetViewport(mBlurB.Width, mBlurB.Height);
		ApplyPostCommon();

		if (mDepthDisable) mDepthDisable->SetState();
		if (mRasterizer)   mRasterizer->SetState();

		ctx->PSSetShaderResources(0, 1, &mBlurA.SRV); // t0 = BlurA

		mBlurShader->SetShader();
		UpdatePostCB_Blur(mBlurB.Width, mBlurB.Height, 0.0f, 1.0f);

		mFullScreenMesh->Render();

		if (mBloomDebug == 3)
		{
			DebugBlitSRV(mBlurB.SRV);
			return;
		}
	}

	// ============================================================
	// 4) Composite : SceneRT + BlurB -> BackBuffer
	//    ★ 여기서 SetTarget "한 번만"
	// ============================================================
	{
		ctx->PSSetSamplers(0, 1, &mPostSampler);
		ctx->PSSetShaderResources(0, 8, nullSRV);


		// 백버퍼 바인딩: 둘 중 하나만 사용
		// 1) 가드 매크로를 쓰면 이거만:
		ID3D11RenderTargetView* nullRTV[1] = { nullptr };
		ctx->OMSetRenderTargets(1, nullRTV, nullptr);
		SET_TARGET_GUARD();



		// 2) 매크로 안 쓰면:
		// dev->SetTarget();

		// Viewport = 화면 해상도
		FResolution rs = dev->GetResolution();
		D3D11_VIEWPORT vp{};
		vp.TopLeftX = 0.f;
		vp.TopLeftY = 0.f;
		vp.Width = (float)rs.Width;
		vp.Height = (float)rs.Height;
		vp.MinDepth = 0.f;
		vp.MaxDepth = 1.f;
		ctx->RSSetViewports(1, &vp);

		ApplyPostCommon();

		if (mDepthDisable) mDepthDisable->SetState();
		if (mRasterizer)   mRasterizer->SetState();

		// t0 = Scene, t1 = Bloom
		ctx->PSSetShaderResources(0, 1, &mSceneRT.SRV);
		ctx->PSSetShaderResources(1, 1, &mBlurB.SRV);

		mCompositeShader->SetShader();
		UpdatePostCB_Intensity(mBloomIntensity);

		mFullScreenMesh->Render();
	}

	// ============================================================
	// 5) 정리: SRV 언바인드
	// ============================================================
	ctx->PSSetShaderResources(0, 8, nullSRV);
}


void CRenderManager::SetViewport(int w, int h)
{
	D3D11_VIEWPORT vp = {};
	vp.Width = (float)w;
	vp.Height = (float)h;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	CDevice::GetInst()->GetContext()->RSSetViewports(1, &vp);
}

void CRenderManager::UpdatePostCB_Tone(float haze, float desat, float contrast, FVector3D v3)
{
	if (!mPostCB) return;
	mPostInfo.Haze = haze;
	mPostInfo.Desat = desat;
	mPostInfo.Contrast = contrast;
	mPostInfo.HazeColor.x = v3.x;
	mPostInfo.HazeColor.y = v3.y;
	mPostInfo.HazeColor.z = v3.z;
	mPostCB->Update(&mPostInfo);
}

bool CRenderManager::Init()
{
	//사용할 레이어 등록
	CreateRenderLayer("BackGround", INT_MIN);
	CreateRenderLayer("ObjectBehind", -10);
	CreateRenderLayer("Object", 0);
	CreateRenderLayer("ObjectFront", 10);

	mStateManager = new CRenderStateManager;

	if (!mStateManager->Init())
	{
		return false;
	}

	D3D11_SAMPLER_DESC Desc = {};

	Desc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
	//Desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	/*
	WRAP	: UV 0 ~ 1을 벗어나면 다시 0~ 1 사이로 반복한다.
	MIRROR	: UV 0 ~ 1을 벗어나면 거울에 비친것처럼 반전되어 반복한다.
	CLAMP	:  UV 0 ~ 1을 벗어나면 가장자리 픽셀 색상으로 고정된다.
	*/

	//Desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//Desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//Desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	Desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	Desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	Desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	//LOD 사용시 필요하다.
	//우리는 사용안할것이기에 필요없다. 
	Desc.MipLODBias = 0.f;
	Desc.MaxAnisotropy = 1;
	// 샘플링 비교 함수
	Desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	Desc.MinLOD = -FLT_MAX;
	Desc.MaxLOD = FLT_MAX;

	if (FAILED(CDevice::GetInst()->GetDevice()->CreateSamplerState(&Desc, &mSampler)))
	{
		return false;
	}

	mAlphaBlend = mStateManager->FindState("AlphaBlend");

	mRasterizer = mStateManager->FindState("Rasterizer");

	mDepthDisable = mStateManager->FindState("DepthDisable");

	FResolution rs = CDevice::GetInst()->GetResolution();
	const int w = (int)rs.Width;
	const int h = (int)rs.Height;

	// SceneRT: 원본(전체 해상도)
	if (!CreateRT(mSceneRT, w, h, DXGI_FORMAT_R8G8B8A8_UNORM))
		return false;

	// Bloom용은 반해상도(빠르고 퍼짐 좋음)
	if (!CreateRT(mBrightRT, w / 2, h / 2, DXGI_FORMAT_R8G8B8A8_UNORM))
		return false;

	if (!CreateRT(mBlurA, w / 2, h / 2, DXGI_FORMAT_R8G8B8A8_UNORM))
		return false;

	if (!CreateRT(mBlurB, w / 2, h / 2, DXGI_FORMAT_R8G8B8A8_UNORM))
		return false;


	mFullScreenMesh = CAssetManager::GetInst()->GetMeshManager()->FindMesh("CenterTexRect");

	mCopyShader = CShaderManager::GetInst()->FindShader("CopyShader");
	mBlurShader = CShaderManager::GetInst()->FindShader("BlurShader");
	mCompositeShader = CShaderManager::GetInst()->FindShader("CompositeShader");


	mBrightShader = CShaderManager::GetInst()->FindShader("BrightExtractShader");

	mPostCB = CShaderManager::GetInst()->FindCBuffer("Post");
	if (!mPostCB) return false;

	D3D11_BLEND_DESC bd{};
	bd.AlphaToCoverageEnable = FALSE;
	bd.IndependentBlendEnable = FALSE;

	bd.RenderTarget[0].BlendEnable = FALSE;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (FAILED(CDevice::GetInst()->GetDevice()->CreateBlendState(&bd, &mBlendWriteAll)))
		return false;
	
	D3D11_SAMPLER_DESC sd = {};
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	if (FAILED(CDevice::GetInst()->GetDevice()->CreateSamplerState(&sd, &mPostSampler)))
		return false;

	CreateSceneDepth(mSceneRT.Width, mSceneRT.Height);

	mPostCB->Update(&mPostInfo);
	UpdatePostCB_Tone(0, 0, 1);
	return true;
}

void CRenderManager::Render()
{
	//기본 샘플러 셋팅 
	CDevice::GetInst()->GetContext()->PSSetSamplers(0, 1, &mSampler);

	//알파블랜드 
	mAlphaBlend->SetState();
	//레스터라이저 셋팅 
	mRasterizer->SetState();

	mDepthDisable->SetState();

	auto iter = mLayerList.begin();
	auto iterEnd = mLayerList.end();

	for (; iter != iterEnd; ++iter)
	{
		FRenderLayer* Layer = iter->second;

		//정렬해주기
		if (Layer->RenderList.size() > 1)
		{
			Layer->RenderList.sort([](
				const CSharedPtr<CSceneComponent>& a,
				const CSharedPtr<CSceneComponent>& b)
				{
					// 1순위: RenderOrder
					if (a->GetRenderOrder() != b->GetRenderOrder())
						return a->GetRenderOrder() < b->GetRenderOrder();

					// 2순위: Y 정렬
					return CRenderManager::SortY(a, b);
				});
		}


		//그리기 
		auto iter1 = Layer->RenderList.begin();
		auto iterEnd1 = Layer->RenderList.end();

		for (; iter1 != iterEnd1;)
		{
			//Active
			if (!(*iter1)->IsActive())
			{
				iter1 = Layer->RenderList.erase(iter1);
				iterEnd1 = Layer->RenderList.end();
				continue;
			}
			else if (!(*iter1)->IsEnable())
			{
				++iter1;
				continue;
			}

			// 그려줘 
			(*iter1)->PreRender();

			(*iter1)->Render();

			(*iter1)->PostRender();

			++iter1;
		}
	}

	// UI는 여기서 그려줄 것이다!
	CSceneManager::GetInst()->RenderUI();

	mDepthDisable->ResetState();



	//알파 블랜드 종료 
	mAlphaBlend->ResetState();
	//레스터라이저 종료
	mRasterizer->ResetState();

}

bool CRenderManager::SortY(const CSharedPtr<class CSceneComponent>& Src, const CSharedPtr<class CSceneComponent>& Dest)
{
	float SrcY = Src->GetWorldPosition().y - Src->GetPivot().y * Src->GetWorldScale().y;

	float DestY = Dest->GetWorldPosition().y - Dest->GetPivot().y * Dest->GetWorldScale().y;

	return SrcY > DestY;
}

bool CRenderManager::SortOrder(const CSharedPtr<CSceneComponent>& a, const CSharedPtr<CSceneComponent>& b)
{
	return a->GetRenderOrder() < b->GetRenderOrder();
}

bool CRenderManager::SortAlpha(const CSharedPtr<class CSceneComponent>& Src,
	const CSharedPtr<class CSceneComponent>& Dest)
{
	return false;
}

void CRenderManager::UpdatePostCB_Threshold(float threshold)
{
	if (!mPostCB) return;
	mPostInfo.Threshold = threshold;
	mPostCB->Update(&mPostInfo);
}

void CRenderManager::UpdatePostCB_Blur(int w, int h, float dx, float dy)
{
	if (!mPostCB) return;
	mPostInfo.TexelSize = FVector2D(1.f / w, 1.f / h);
	mPostInfo.Direction = FVector2D(dx, dy);
	mPostCB->Update(&mPostInfo);
}

void CRenderManager::UpdatePostCB_Intensity(float intensity)
{
	if (!mPostCB) return;
	mPostInfo.Intensity = intensity;
	mPostCB->Update(&mPostInfo);
}

bool CRenderManager::CreateSceneDepth(int w, int h)
{
	auto* dev = CDevice::GetInst()->GetDevice();

	D3D11_TEXTURE2D_DESC dd{};
	dd.Width = w;
	dd.Height = h;
	dd.MipLevels = 1;
	dd.ArraySize = 1;
	dd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dd.SampleDesc.Count = 1;      // ✅ SceneRT와 동일
	dd.SampleDesc.Quality = 0;
	dd.Usage = D3D11_USAGE_DEFAULT;
	dd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	if (FAILED(dev->CreateTexture2D(&dd, nullptr, &mSceneDepthTex))) return false;
	if (FAILED(dev->CreateDepthStencilView(mSceneDepthTex, nullptr, &mSceneDepthDSV))) return false;
	return true;
}

void CRenderManager::DebugBlitSRV(ID3D11ShaderResourceView* srv)
{
	auto* ctx = CDevice::GetInst()->GetContext();
	CDevice::GetInst()->SetTarget();

	FResolution rs = CDevice::GetInst()->GetResolution();
	D3D11_VIEWPORT vp{};
	vp.TopLeftX = 0; vp.TopLeftY = 0;
	vp.Width = (float)rs.Width;
	vp.Height = (float)rs.Height;
	vp.MinDepth = 0.f; vp.MaxDepth = 1.f;
	ctx->RSSetViewports(1, &vp);

	// 상태(중요): DepthOff + CullNone + ScissorOff
	if (mDepthDisable) mDepthDisable->SetState();
	if (mRasterizer)   mRasterizer->SetState();
	ctx->RSSetScissorRects(0, nullptr);

	// SRV
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	ctx->PSSetShaderResources(0, 1, nullSRV);
	ctx->PSSetShaderResources(0, 1, &srv);
	ctx->PSSetSamplers(0, 1, &mPostSampler);   // post sampler로 통일 권장

	mCopyShader->SetShader();
	mFullScreenMesh->Render();

	ctx->PSSetShaderResources(0, 1, nullSRV);
}


