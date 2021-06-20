#pragma once
#include "Shader.h"
class CGameObject;
class CCamera;
class CPlayer;

class CScene
{
public:
	CScene();
	~CScene();
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);

	void InitBuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();

	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void ReleaseUploadBuffers();

	//그래픽 루트 시그너쳐를 생성한다. 
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

	CHeightMapTerrain* GetTerrain() { return(m_pTerrain); }
	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	void SetShaderPlayer(int index) { m_pShaders[index].SetPlayer(m_pPlayer); }

	// 오브젝트 생성. 일단 아군만
	
protected:

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	//루트 시그너쳐를 나타내는 인터페이스 포인터이다. 

	CHeightMapTerrain* m_pTerrain = NULL;
	CPlayer* m_pPlayer;

protected:
	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다. 
	CObjectsShader *m_pShaders = NULL;
	int m_nShaders = 0;

};


