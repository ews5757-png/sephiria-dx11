#pragma once
#include "UserWidget.h"

#include <functional>

class CImage;

class TilePalettePanel : public CUserWidget
{
public:
    virtual bool Init() override;
    virtual void Update(float DeltaTime) override;

public:
    void SetTileSize(const FVector2D& sz) { mTileSize = sz; }
    void SetGrid(int cols, int rows) { mCols = cols; mRows = rows; }
    void SetAtlasKey(const std::string& key);

    void SetOnSelectFrame(const std::function<void(int)>& f) { mOnSelectFrame = f; }

private:
    void BuildGrid();          // 전체 격자 생성
    void UpdateSelectionRect();// 선택 셀 테두리 갱신

    bool IsMouseOnAtlas(const FVector2D& MousePos) const;
    int  CalcFrame(const FVector2D& MousePos) const;

private:
    CImage* mAtlas = nullptr;      // 16x16 그리드/클릭 기준판 (256x256 고정)
    CImage* mAtlasImg = nullptr;   // 실제 텍스처 표시(좌하단 정렬용)
    int mTexW = 0, mTexH = 0;

    // 그리드 라인들
    std::vector<CImage*> mGridLines;

    // 선택 셀 테두리(4개 라인)
    CImage* mSelL = nullptr;
    CImage* mSelR = nullptr;
    CImage* mSelT = nullptr;
    CImage* mSelB = nullptr;

    std::string mLineTexKey = "white"; // 1x1 흰 텍스처 키
    float mLineThick = 2.f;

    int mCols = 0, mRows = 0;
    FVector2D mTileSize = { 16.f,16.f };

    int mSelectedFrame = -1;
    bool mPrevLDown = false;


    float mScale = 1.5f;


    std::function<void(int)> mOnSelectFrame;
};
