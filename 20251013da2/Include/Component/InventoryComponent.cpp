#include "InventoryComponent.h"
#include "../Asset/AssetManager.h"
#include "../Asset/Item/ItemManager.h"
#include "../Scene/SceneMain.h"

static void RotateCoord(ERotation rot, int size, int px, int py, int& outX, int& outY)
{
    int rx = px;
    int ry = py;

    switch (rot)
    {
    case ERotation::R90:
        rx = size - 1 - py;
        ry = px;
        break;
    case ERotation::R180:
        rx = size - 1 - px;
        ry = size - 1 - py;
        break;
    case ERotation::R270:
        rx = py;
        ry = size - 1 - px;
        break;
    case ERotation::R0:
    default:
        break;
    }

    outX = rx;
    outY = ry;
}


CInventoryComponent::CInventoryComponent()
{
}

CInventoryComponent::~CInventoryComponent()
{
}

bool CInventoryComponent::Init()
{
    mSlots.resize(24);
    mSlotBuff.resize(24);
	return true;
}

bool CInventoryComponent::AddItem(int itemID)
{
    for (int i = 0; i < (int)mSlots.size(); ++i)
    {
        if (!mSlots[i].Item.Defins.IsValid())
            return SetItem(i, itemID, ERotation::R0);
    }
    return false; // 가득 참
}

bool CInventoryComponent::SetItem(int index, int itemID, ERotation rot)
{
    if (index < 0 || index >= (int)mSlots.size())
        return false;

    const FItemDef* def = CAssetManager::GetInst()->GetItemManager()->GetDef(itemID);
    if (!def)
        return false;

    // 배치 가능 검사(현재는 범위만 검사)
    int slotX = index % mWidth;
    int slotY = index / mWidth;

    mSlots[index].Item.Defins = *def;
    mSlots[index].Item.Rot = rot;

    RebuildSlotBuff();
    return true;
}

bool CInventoryComponent::ClearItem(int index)
{
    if (index < 0 || index >= (int)mSlots.size())
        return false;

    mSlots[index].Item.Defins = FItemDef();   // ID=0 => empty
    mSlots[index].Item.Rot = ERotation::R0;

    RebuildSlotBuff();
    return true;
}

bool CInventoryComponent::RotateItem(int index)
{
    if (index < 0 || index >= (int)mSlots.size())
        return false;

    FItemSlot& slot = mSlots[index];
    if (!slot.Item.Defins.IsValid())
        return false;

    int slotX = index % mWidth;
    int slotY = index / mWidth;

    ERotation newRot = NextRot(slot.Item.Rot);

    slot.Item.Rot = newRot;

    RebuildSlotBuff();

    return true;
}

bool CInventoryComponent::MoveItem(int from, int to)
{

    if (from < 0 || from >= (int)mSlots.size()) return false;
    if (to < 0 || to >= (int)mSlots.size()) return false;
    if (from == to) return false;

    // 이동 대상이 비어있든 아니든(스왑) 단순 처리
    std::swap(mSlots[from], mSlots[to]);

    RebuildSlotBuff();
    return true;
}

void CInventoryComponent::RebuildSlotBuff()
{
    mSlotBuff.assign(mWidth * mHeight, 0);

    for (int i = 0; i < (int)mSlots.size(); ++i)
    {
        if (!mSlots[i].Item.Defins.IsValid())
            continue;

        int slotX = i % mWidth;
        int slotY = i / mWidth;

        ApplyPatternToBuff(slotX, slotY, mSlots[i].Item.Defins, mSlots[i].Item.Rot);
    }


    for (int i = 0; i < (int)mSlots.size(); ++i)
    {
        mSlots[i].Level = mSlotBuff[i];
    }

    UpdateUI();
}
void CInventoryComponent::ApplyPatternToBuff(int slotX, int slotY, const FItemDef& def, ERotation rot)
{
    const int PatternSize = 11;
    const int Center = 5;

    for (int py = 0; py < PatternSize; ++py)
    {
        for (int px = 0; px < PatternSize; ++px)
        {
            int value = def.Pattern[py * PatternSize + px];
            if (value == 0)
                continue;

            int rx, ry;
            RotateCoord(rot, PatternSize, px, py, rx, ry);

            int ix = slotX + (rx - Center);
            int iy = slotY + (ry - Center);

            // 범위 밖은 무시(이미 CanApplyPattern으로 막고 들어오면 여기엔 잘 안 걸림)
            if (ix < 0 || iy < 0 || ix >= mWidth || iy >= mHeight)
                continue;

            int idx = iy * mWidth + ix;
            mSlotBuff[idx] += value;
        }
    }
}

void CInventoryComponent::UpdateUI()
{
    CSceneMain* temp = dynamic_cast<CSceneMain*>(mScene);
    if (temp)
    {
        temp->UpdateInventoty();
    }
}

