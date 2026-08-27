#include "SceneEditor.h"
#include "../Object/EditorPlayer.h"
#include "../Object/TileMapObj.h"
#include "Input.h"
#include "../GameManager.h"
#include "../Scene/SceneUIManager.h"
#include "../UI/UserWidget/TilePalettePanel.h"
#include "../Render/RenderManager.h"

CSceneEditor::CSceneEditor()
{
}

CSceneEditor::~CSceneEditor()
{
}

bool CSceneEditor::Init()
{
	CScene::Init();

	CRenderManager::GetInst()->SetBloomIntensity(0.f);

	CEditorPlayer* EditorPlayer = CreateObj<CEditorPlayer>("EditorPlayer");
	mTileMapObj = CreateObj<CTileMapObj>("TileMap");

	auto* palette = GetUIManager()->CreateWidget<TilePalettePanel>("TilePalette");
	GetUIManager()->AddToViewport(palette);
	//palette->SetPos(20.f, 20.f);
	palette->SetTileSize({ 16.f, 16.f });
	//palette->SetGrid(16, 16);
	palette->SetAtlasKey("TileMap");

	// ✅ 씬은 중계만 한다
	palette->SetOnSelectFrame([this](int frame)
		{
			if (mTileMapObj)
				mTileMapObj->SetEditorTileFrame(frame);
		});

	//입력
	mInput->AddBindKey("EditorMode", '1');
	mInput->AddBindFunction("EditorMode", EInputType::Down, this, &CSceneEditor::EditorMode);

	mInput->AddBindKey("TileType", '2');
	mInput->AddBindFunction("TileType", EInputType::Down, this, &CSceneEditor::TileTypeKey);

	mInput->AddBindKey("TileFrame", '3');
	mInput->AddBindFunction("TileFrame", EInputType::Down, this, &CSceneEditor::TileFrameKey);

	mInput->AddBindKey("ToggleLayer", '4');
	mInput->AddBindFunction("ToggleLayer", EInputType::Down, this, &CSceneEditor::ToggleLayerKey);



	mInput->AddBindKey("Save", 'S');
	mInput->ChangeKeyCtrl("Save", true);
	mInput->AddBindFunction("Save", EInputType::Down, this, &CSceneEditor::SaveKey);

	mInput->AddBindKey("Load", 'O');
	mInput->ChangeKeyCtrl("Load", true);
	mInput->AddBindFunction("Load", EInputType::Down, this, &CSceneEditor::LoadKey);


	return true;
}

void CSceneEditor::Update(float DeltaTime)
{
	CScene::Update(DeltaTime);
}

void CSceneEditor::EditorMode(float DeltaTime)
{
	mEditorMode = static_cast<EEditorMode>((int)mEditorMode + 1);

	if (mEditorMode == EEditorMode::End)
	{
		mEditorMode = EEditorMode::TileType;
	}

	mTileMapObj->SetEditorMod(mEditorMode);

}

void CSceneEditor::TileTypeKey(float DeltaTime)
{
	mTileMapObj->AddTileType();
}

void CSceneEditor::TileFrameKey(float DeltaTime)
{
	mTileMapObj->AddTileFrame();
}

void CSceneEditor::SaveKey(float DeltaTime)
{
	OPENFILENAME ofn = {};

	//경로 만들기
	TCHAR FullPath[MAX_PATH] = {};
	TCHAR Filter[] = TEXT("*.tlm\0");

	TCHAR DefaultPath[MAX_PATH] = {};

	lstrcpy(DefaultPath, gRootPath);
	lstrcat(DefaultPath, TEXT("Asset\\Data\\"));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = CGameManager::GetInst()->GetWindowHandle();
	ofn.lpstrFilter = Filter;
	ofn.lpstrDefExt = TEXT("tlm");
	ofn.lpstrInitialDir = DefaultPath;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFile = FullPath;

	if (GetSaveFileName(&ofn) != 0)
	{
		//저장을 시작한다.
		mTileMapObj->Save(FullPath);
	}
}

void CSceneEditor::LoadKey(float DeltaTime)
{
	OPENFILENAME ofn = {};

	//경로 만들기
	TCHAR FullPath[MAX_PATH] = {};
	TCHAR Filter[] = TEXT("*.tlm\0");

	TCHAR DefaultPath[MAX_PATH] = {};

	lstrcpy(DefaultPath, gRootPath);
	lstrcat(DefaultPath, TEXT("Asset\\Data\\"));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = CGameManager::GetInst()->GetWindowHandle();
	ofn.lpstrFilter = Filter;
	ofn.lpstrDefExt = TEXT("tlm");
	ofn.lpstrInitialDir = DefaultPath;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFile = FullPath;

	if (GetOpenFileName(&ofn) != 0)
	{
		//불러오기 시작한다. 
		mTileMapObj->Load(FullPath);
	}
}

void CSceneEditor::ToggleLayerKey(float DeltaTime)
{
	if (mTileMapObj)
	{
		mTileMapObj->ToggleEditLayer();
	}
}
