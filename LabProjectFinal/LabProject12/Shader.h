#pragma once
#include "GameObject.h"
#include "Player.h"
#include "Camera.h"

//���� ��ü�� ������ ���̴����� �Ѱ��ֱ� ���� ����ü(��� ����)�̴�. 
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
};

class CShader
{
public:
	CShader();
	virtual ~CShader();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature
		* pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList,
		XMFLOAT4X4* pxmf4x4World);


	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

protected:
	//���������� ���� ��ü���� ����Ʈ(�迭)�̴�. 
	ID3D12PipelineState **m_ppd3dPipelineStates = NULL;
	int m_nPipelineStates = 0;
};

class CPlayerShader : public CShader
{
public:
	CPlayerShader();
	virtual ~CPlayerShader();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature
		* pd3dGraphicsRootSignature);
};

class CObjectsShader : public CShader
{
public:
	CObjectsShader();
	virtual ~CObjectsShader();
	virtual void InitBuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, void* pContext);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature
		* pd3dGraphicsRootSignature);
	virtual void ReleaseUploadBuffers();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

public:
	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	CPlayer* GetPlayer() { return m_pPlayer; }
	void SetTerrain(CHeightMapTerrain* pTerrain) { m_pTerrain = pTerrain; }
	// �Ʊ� ���� �Լ�
	void BuildMyTeam();
	// ���� ���� �Լ�
	void BuildEnemyFlyShip(XMFLOAT3 spotPos);
	void BuildEnemy(XMFLOAT3 spotPos);
	// �浹üũ �Լ�
	void Collision_Check();

	// �ܺο��� ��ü �߰��ϴ� �Լ�
	void AddObject(OBJ::OBJID _eID, CGameObject* pObject);

	// �÷��̾� �Ѿ� �߰��ϴ� �Լ�
	void AddPlayerBullet();
	
protected:
	list<CGameObject*>						m_listObjects[OBJ::END_OBJID];
	CPlayer*								m_pPlayer;
	CHeightMapTerrain						*m_pTerrain;
	int										m_iMyTeamBuildNum=0;
	XMFLOAT3								m_xmf3EnemyBasePos;
	XMFLOAT3								m_xmf3MyBasePos;

protected:
	// �޽��� �̸� ����� ���´�.
	CCubeMeshDiffused*						m_pCubeMyTeamMesh;			// �Ʊ� �޽�
	CCubeMeshDiffused*						m_pCubeMySpotMesh;			// �Ʊ� ���� �޽�
	CCubeMeshDiffused*						m_pCubeEnemySpotMesh;		// ���� ���� �޽�
	CModelMeshDiffused*						m_pEnemyFlyerShipMesh;		// ���� ����� �޽�
	CModelMeshDiffused*						m_pBaseMesh;				// ���� �޽�
	CCubeMeshDiffused*						m_pCubeEnemyMesh;			// ���� �޽�
	CCubeMeshDiffused*						m_pCubeEnemyBulletMesh;		// ���� �Ѿ� �޽�
	CCubeMeshDiffused*						m_pCubePlayerBulletMesh;	// �÷��̾� �Ѿ� �޽�
	CCubeMeshDiffused*						m_pCubeWaterMesh;			// �÷��̾� �Ѿ� �޽�

};

class CTerrainShader : public CShader
{
public:
	CTerrainShader();
	virtual ~CTerrainShader();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature
		* pd3dGraphicsRootSignature);
};