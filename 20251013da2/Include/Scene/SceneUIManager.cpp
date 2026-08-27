#include "SceneUIManager.h"
#include "../Share/Log.h"
#include "../Device.h"
#include "../Scene/Scene.h"
#include "../Scene/Input.h"

static bool IsDescendantOf(CWidget* w, CWidget* ancestor)
{
	if (!w || !ancestor) return false;

	CWidget* p = w;
	while (p)
	{
		if (p == ancestor) return true;
		p = p->GetParent(); // GetParent 없으면 p = p->mParent;
	}
	return false;
}

CSceneUIManager::CSceneUIManager()
{
}

CSceneUIManager::~CSceneUIManager()
{
}

void CSceneUIManager::AddToViewport(CWidget* Widget)
{
	mWidgetList.emplace_back(Widget);
}

bool CSceneUIManager::Init()
{

	FResolution RS = CDevice::GetInst()->GetResolution();
	CWidget::CreateUIProjection(RS.Width, RS.Height, 1000.f);

	return true;
}

void CSceneUIManager::Update(float DeltaTime)
{
	auto iter = mWidgetList.begin();
	auto iterEnd = mWidgetList.end();

	for (; iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			iter = mWidgetList.erase(iter);
			iterEnd = mWidgetList.end();
			continue;;
		}
		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}
		(*iter)->Update(DeltaTime);
		++iter;
	}

	if (mMouseHoveredWidget)
	{
	}

}

bool CSceneUIManager::CollisionMouse(float DeltaTime, const FVector2D& MousePos)
{
	if (mOverlayWidgetList.size() >= 2)
		std::sort(mOverlayWidgetList.begin(), mOverlayWidgetList.end(), CSceneUIManager::SortCollision);

	for (auto iter = mOverlayWidgetList.begin(); iter != mOverlayWidgetList.end(); )
	{
		if (!(*iter)->IsActive()) { iter = mOverlayWidgetList.erase(iter); continue; }
		if (!(*iter)->IsEnable()) { ++iter; continue; }

		CWidget* hovered = nullptr;
		if ((*iter)->CollisionMouse(&hovered, MousePos))
		{
			if (mMouseHoveredWidget != hovered)
			{
				if (mMouseHoveredWidget) mMouseHoveredWidget->ForceUnhover();
				mMouseHoveredWidget = hovered;
			}
			return true;
		}
		++iter;
	}

	if (mWidgetList.size() >= 2)
		std::sort(mWidgetList.begin(), mWidgetList.end(), CSceneUIManager::SortCollision);
	
	for (auto iter = mWidgetList.begin(); iter != mWidgetList.end(); )
	{
		if (!(*iter)->IsActive())
		{
			iter = mWidgetList.erase(iter);
			continue;
		}
		if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		CWidget* hovered = nullptr;

		if ((*iter)->CollisionMouse(&hovered, MousePos))
		{
			if (mMouseHoveredWidget != hovered)
			{
				if (mMouseHoveredWidget)
					mMouseHoveredWidget->ForceUnhover();   // 상태까지 끔

				mMouseHoveredWidget = hovered;
			}
			return true;
		}

		++iter;
	}

	if (mMouseHoveredWidget)
	{
		mMouseHoveredWidget->ForceUnhover();   // 충돌 없을 때도 상태 해제
		mMouseHoveredWidget = nullptr;
	}

	return false;
}


void CSceneUIManager::Render()
{
	// 1) 일반 UI 렌더
	if (mWidgetList.size() >= 2)
		std::sort(mWidgetList.begin(), mWidgetList.end(), CSceneUIManager::SortRender);

	for (auto iter = mWidgetList.begin(); iter != mWidgetList.end(); )
	{
		if (!(*iter)->IsActive())
		{
			iter = mWidgetList.erase(iter);
			continue;
		}
		if (!(*iter)->IsEnable() || (*iter)->IsInOverlay())
		{
			++iter;
			continue;
		}

		(*iter)->Render();
		++iter;
	}

	// 2) 오버레이 UI 렌더 (부모 무시, 무조건 최상단)
	if (mOverlayWidgetList.size() >= 2)
		std::sort(mOverlayWidgetList.begin(), mOverlayWidgetList.end(), CSceneUIManager::SortRender);

	for (auto iter = mOverlayWidgetList.begin(); iter != mOverlayWidgetList.end(); )
	{
		if (!(*iter)->IsActive())
		{
			iter = mOverlayWidgetList.erase(iter);
			continue;
		}
		if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Render();
		++iter;
	}

	// 3) EndFrame도 둘 다 호출
	for (auto& w : mWidgetList)
		w->EndFrame();

	for (auto& w : mOverlayWidgetList)
		w->EndFrame();
}

void CSceneUIManager::BeginDrag(CWidget* Slot)
{
	mDraggingWidget = Slot;
}

void CSceneUIManager::EndDrag(CWidget* DragWidget)
{
	FVector2D mouse = mScene->GetInput()->GetMousePos();


	CWidget* target = HitTest(mouse, DragWidget, true);





	while (target && !target->CanDrop(DragWidget))
	{
		target = target->GetParent();   // 핵심
	}

	if (target == DragWidget)
		target = nullptr;

	if (target)
		target->OnDrop(DragWidget);

	mDraggingWidget = nullptr;

}

CWidget* CSceneUIManager::HitTest(const FVector2D& MousePos, CWidget* Ignore, bool IgnoreChildren)
{
	if (mWidgetList.size() >= 2)
		std::sort(mWidgetList.begin(), mWidgetList.end(), CSceneUIManager::SortCollision);

	auto testList = [&](std::vector<CSharedPtr<CWidget>>& list) -> CWidget*
		{
			CWidget* best = nullptr;

			for (auto iter = list.begin(); iter != list.end(); )
			{
				if (!(*iter)->IsActive())
				{
					iter = list.erase(iter);
					continue;
				}
				if (!(*iter)->IsEnable())
				{
					++iter;
					continue;
				}

				CWidget* w = iter->Get();

				if (Ignore)
				{
					if (w == Ignore) { ++iter; continue; }
					if (IgnoreChildren && IsDescendantOf(w, Ignore)) { ++iter; continue; }
				}

				CWidget* hovered = nullptr;
				if (w->CollisionMouse(&hovered, MousePos))
				{
					CWidget* hit = hovered ? hovered : w;

					if (Ignore && hit == Ignore)
					{
						++iter;
						continue;
					}
					if (Ignore && IgnoreChildren && IsDescendantOf(hit, Ignore))
					{
						++iter;
						continue;
					}

					if (!best || hit->GetZOrder() >= best->GetZOrder())
						best = hit;
				}

				++iter;
			}

			return best;
		};


	if (CWidget* hit = testList(mWidgetList))
		return hit;

	return nullptr;
}





// 오름차순 : 충돌 처리는 앞에서부터 뒤로 검사하여 가장 앞에 있는 UI가 충돌됬느지 검사하기 위해서 
bool CSceneUIManager::SortCollision(const CSharedPtr<CWidget>& Src, const CSharedPtr<CWidget>& Dest)
{
	return Src->GetZOrder() > Dest->GetZOrder();
}

bool CSceneUIManager::SortRender(const CSharedPtr<CWidget>& Src, const CSharedPtr<CWidget>& Dest)
{
	// 내림차순
	return Src->GetZOrder() < Dest->GetZOrder();
}

void CSceneUIManager::AddOverlayWidget(CWidget* Widget)
{
	Widget->mInOverlay = true;
	mOverlayWidgetList.emplace_back(Widget);
}

void CSceneUIManager::ClearOverlayWidgets()
{
	for (auto& w : mOverlayWidgetList)
		w->mInOverlay = false;
	mOverlayWidgetList.clear();
}
