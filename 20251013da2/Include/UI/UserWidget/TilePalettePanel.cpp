#include "TilePalettePanel.h"
#include "../../Scene/Scene.h"
#include "../../Scene/Input.h"
#include "../Common/Image.h"
#include "../../Asset/Texture/Texture.h"
#include "../../Device.h"


static void SetupLine(CImage* img, const std::string& texKey, float x, float y, float w, float h, int z)
{
    img->SetPivot(0.f, 0.f);
    img->SetPos(x, y);
    img->SetSize(w, h);
    img->SetZOrder(z);
    img->SetTexture(texKey);
    img->SetTint(1.f, 1.f, 1.f, 0.7f); // 살짝 투명
}

bool TilePalettePanel::Init()
{
    if (!CUserWidget::Init())
        return false;

    SetPivot(0.f, 0.f);
    SetZOrder(200);

    // 클릭 기준판(항상 256x256, 위치 고정)
    mAtlas = CreateChildWidget<CImage>("AtlasFrame");
    mAtlas->SetPivot(0.f, 0.f);
    mAtlas->SetPos(0.f, 0.f);
    mAtlas->SetSize(16.f * 16.f, 16.f * 16.f); // 256x256
    // mAtlas는 텍스처를 안 넣어도 됨(투명 판 역할)
    // 필요하면 1x1 투명 텍스처를 깔아도 됨

    // 실제 텍스처 표시용(얘만 아래로 내릴 것)
    mAtlasImg = CreateChildWidget<CImage>("AtlasImage");
    mAtlasImg->SetPivot(0.f, 0.f);
    mAtlasImg->SetPos(0.f, 0.f); // SetAtlasKey에서 최종 배치
    mAtlasImg->SetZOrder(205);

    // 선택 테두리 4개 ...
    mSelL = CreateChildWidget<CImage>("SelL");
    mSelR = CreateChildWidget<CImage>("SelR");
    mSelT = CreateChildWidget<CImage>("SelT");
    mSelB = CreateChildWidget<CImage>("SelB");

    mSelL->SetTexture(mLineTexKey);
    mSelR->SetTexture(mLineTexKey);
    mSelT->SetTexture(mLineTexKey);
    mSelB->SetTexture(mLineTexKey);

    mSelL->SetTint(1.f, 1.f, 0.f, 1.f);
    mSelR->SetTint(1.f, 1.f, 0.f, 1.f);
    mSelT->SetTint(1.f, 1.f, 0.f, 1.f);
    mSelB->SetTint(1.f, 1.f, 0.f, 1.f);



    const float panelW = 16.f * 16.f * mScale;
    const float panelH = 16.f * 16.f * mScale;

    SetSize(panelW, panelH);
    mAtlas->SetSize(panelW, panelH);

    // 타일 크기도 같이 키워야 함
    mTileSize = FVector2D(16.f * mScale, 16.f * mScale);



    FResolution rs = CDevice::GetInst()->GetResolution();

    //SetPivot(1.f, 1.f);         // 패널의 좌상단 기준


    //SetPos(-1*(rs.Width / 2), -1* (rs.Height/2));   // 화면 좌하단에 붙이기
    return true;
}

void TilePalettePanel::SetAtlasKey(const std::string& TextureKey)
{

    if (!mAtlas || !mAtlasImg) return;

    mCols = 16;
    mRows = 16;

    const float panelW = 16.f * 16.f * mScale;
    const float panelH = 16.f * 16.f * mScale;

    SetSize(panelW, panelH);
    mAtlas->SetSize(panelW, panelH);

    // 타일 크기도 같이 키워야 함(가장 중요)
    mTileSize = FVector2D(16.f * mScale, 16.f * mScale);

    // 텍스처 설정
    mAtlasImg->SetTexture(TextureKey);

    // 텍스처 실제 픽셀 크기
    mTexW = 0; mTexH = 0;
    if (mAtlasImg->mBrush.Texture)
    {
        const FTextureInfo* info = mAtlasImg->mBrush.Texture->GetTexture(0);
        if (info) { mTexW = info->Width; mTexH = info->Height; }
    }

    // ★ 표시도 스케일 적용해서 커지게 (안 하면 그림은 작고 그리드만 큼)
    mAtlasImg->SetSize((float)mTexW * mScale, (float)mTexH * mScale);

    // 좌하단 붙이기(스케일 기준)
    mAtlasImg->SetPos(0.f, panelH - (float)mTexH * mScale);

    BuildGrid();
}


void TilePalettePanel::BuildGrid()
{
    float W = mCols * mTileSize.x; // 256
    float H = mRows * mTileSize.y; // 256

    // 필요하면 기존 라인들 비활성/삭제 처리 (생략)

    auto MakeLineH = [&](float y)
        {
            CImage* l = CreateChildWidget<CImage>("BorderH");
            l->SetTexture(mLineTexKey);
            l->SetPivot(0.f, 0.f);
            l->SetPos(0.f, y);
            l->SetSize(W, 1.f);
            l->SetZOrder(211);
            mGridLines.emplace_back(l);
        };

    auto MakeLineV = [&](float x)
        {
            CImage* l = CreateChildWidget<CImage>("BorderV");
            l->SetTexture(mLineTexKey);
            l->SetPivot(0.f, 0.f);
            l->SetPos(x, 0.f);
            l->SetSize(1.f, H);
            l->SetZOrder(211);
            mGridLines.emplace_back(l);
        };

    // ★ 16칸 그리드(0~256까지 17줄)
    for (int r = 0; r <= mRows; ++r)
        MakeLineH(r * mTileSize.y);

    for (int c = 0; c <= mCols; ++c)
        MakeLineV(c * mTileSize.x);
}

bool TilePalettePanel::IsMouseOnAtlas(const FVector2D& MousePos) const
{
    FVector2D min = mAtlas->GetRenderPos();
    FVector2D max = min + mAtlas->GetSize();
    return !(MousePos.x < min.x || MousePos.x > max.x ||
        MousePos.y < min.y || MousePos.y > max.y);
}

int TilePalettePanel::CalcFrame(const FVector2D& MousePos) const
{
    FVector2D local = MousePos - mAtlas->GetRenderPos();

    int tx = (int)(local.x / mTileSize.x);
    int tyTop = (int)(local.y / mTileSize.y);

    if (tx < 0 || tx >= 16 || tyTop < 0 || tyTop >= 16)
        return -1;

    // 좌하단 0번
    int tyBottom = 15 - tyTop;
    return tyBottom * 16 + tx;
}

void TilePalettePanel::UpdateSelectionRect()
{
    if (mSelectedFrame < 0 || mCols <= 0 || mRows <= 0)
    {
        mSelL->SetSize(0.f, 0.f);
        mSelR->SetSize(0.f, 0.f);
        mSelT->SetSize(0.f, 0.f);
        mSelB->SetSize(0.f, 0.f);
        return;
    }

    int tx = mSelectedFrame % mCols;

    // mSelectedFrame은 "좌하단 기준 tyBottom"
    int tyBottom = mSelectedFrame / mCols;

    // 화면 표시용은 "좌상단 기준 tyTop"로 다시 변환
    int tyTop = (mRows - 1) - tyBottom;

    float x = tx * mTileSize.x;
    float y = tyTop * mTileSize.y;
    float w = mTileSize.x;
    float h = mTileSize.y;

    SetupLine(mSelT, mLineTexKey, x, y, w, mLineThick, 220);
    SetupLine(mSelB, mLineTexKey, x, y + h - mLineThick, w, mLineThick, 220);
    SetupLine(mSelL, mLineTexKey, x, y, mLineThick, h, 220);
    SetupLine(mSelR, mLineTexKey, x + w - mLineThick, y, mLineThick, h, 220);

    mSelL->SetTint(1.f, 1.f, 0.f, 1.f);
    mSelR->SetTint(1.f, 1.f, 0.f, 1.f);
    mSelT->SetTint(1.f, 1.f, 0.f, 1.f);
    mSelB->SetTint(1.f, 1.f, 0.f, 1.f);
}

void TilePalettePanel::Update(float DeltaTime)
{
    CUserWidget::Update(DeltaTime);

    if (!mScene || !mAtlas) return;

    auto* input = mScene->GetInput();
    FVector2D mouse = input->GetMousePos();

    bool lDown = input->GetMouseDown(EMouseButtonType::LButton) ||
        input->GetMouseHold(EMouseButtonType::LButton);

    bool click = lDown && !mPrevLDown;
    mPrevLDown = lDown;

    if (!click) return;
    if (!IsMouseOnAtlas(mouse)) return;

    int frame = CalcFrame(mouse);
    if (frame < 0) return;

    mSelectedFrame = frame;
    UpdateSelectionRect();

    if (mOnSelectFrame)
        mOnSelectFrame(frame);
}
