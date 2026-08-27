#pragma once
#include "../Common/Button.h"
class CSlotDragButton :
    public CButton
{
public:

    int mIndex;

    bool Init()override;

    void ButtonClick() override; 

    virtual void Update(float DeltaTime) override;

    void DraggingStart() override;

    void DraggingEnd() override;

    bool CanDrop(CWidget* DragWidget) const override
    {
        return true;
    }
    void OnDrop(CWidget* DragWidget) override;


};

