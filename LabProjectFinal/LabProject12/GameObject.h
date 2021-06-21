#pragma once
#include "Mesh.h"
class CShader;
class CCamera;
class CPlayer;

class CGameObject
{
public:
	CGameObject(int nMeshes=1);
	virtual ~CGameObject();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
protected:
	XMFLOAT4X4		m_xmf4x4World;
	CShader*		m_pShader = NULL;
	//���� ��ü�� ���� ���� �޽��� �����ϴ� ��� ���� ��ü�� ������ �޽��鿡 ���� �����Ϳ� �� �����̴�.
	CMesh** m_ppMeshes = NULL;
	int m_nMeshes = 0;
	// ��ü ������ ����
	XMFLOAT3					m_f3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

public:
	BoundingOrientedBox			m_xmOOBB;

public:
	void ReleaseUploadBuffers(); 
	virtual void SetMesh(int nIndex, CMesh* pMesh);
	virtual void SetShader(CShader* pShader);
	virtual void SetScale(XMFLOAT3 xmf3scale);

	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void UpdateBoundingBox();
public:
	//��� ���۸� �����Ѵ�. 
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList 
	*pd3dCommandList);
	//��� ������ ������ �����Ѵ�. 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	//���� ��ü�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x-��, y-��, z-��) ���͸� ��ȯ�Ѵ�. 
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	//���� ��ü�� ��ġ�� �����Ѵ�. 
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	//���� ��ü�� ���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	//���� ��ü�� ȸ��(x-��, y-��, z-��)�Ѵ�. 
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	void SetLook(XMFLOAT3 vLook);
};

class CRotatingFlagObject : public CGameObject
{
public:
	enum STATE{NORMAL, DOWN, FINISH, END};
public:
	CRotatingFlagObject(int nMeshes = 1);
	virtual ~CRotatingFlagObject();
private:
	XMFLOAT3	m_xmf3RotationAxis;
	float		m_fRotationSpeed;
	STATE		m_eState;
	float		m_fOriginY;
public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) {m_xmf3RotationAxis = xmf3RotationAxis;}
	virtual void Animate(float fTimeElapsed);
	STATE GetState() { return m_eState; }
	void SetState(STATE _eState);
};



class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int
		nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4
		xmf4Color);
	virtual ~CHeightMapTerrain();
private:
	//������ ���� ������ ����� �̹����̴�. 
	CHeightMapImage *m_pHeightMapImage;
	
	//���� ���� ���ο� ���� ũ���̴�. 
	int m_nWidth;
	int m_nLength;
	//������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ���� ������ �����̴�. 
	XMFLOAT3 m_xmf3Scale;

public:
	//������ ���̸� ����ϴ� �Լ��̴�(���� ��ǥ��). ���� ���� ���̿� �������� y�� ���� ���̴�. 
	float GetHeight(float x, float z) { return(m_pHeightMapImage->GetHeight(x / m_xmf3Scale.x,
		z / m_xmf3Scale.z)* m_xmf3Scale.y);}

	//������ ���� ���͸� ����ϴ� �Լ��̴�(���� ��ǥ��). ���� ���� ���� ���͸� ����Ѵ�. 
	XMFLOAT3 GetNormal(float x, float z) { 
		return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x),
		int(z / m_xmf3Scale.z))); }

	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	//������ ũ��(����/����)�� ��ȯ�Ѵ�. ���� ���� ũ�⿡ �������� ���� ���̴�. 
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }

};


class CTerrainObject : public CGameObject
{
public:
	CTerrainObject(void* pContext, void* pPlayer, XMFLOAT3 xmf3Offset, float fMeshHeightHalf, int nMeshes = 1);
	virtual ~CTerrainObject();

	//�÷��̾��� ��ġ�� �ٲ� ������ ȣ��Ǵ� �Լ��� �� �Լ����� ����ϴ� ������ �����ϴ� �Լ��̴�.
	virtual void OnObjectUpdateCallback(float fTimeElapsed);
	void SetTerrain(CHeightMapTerrain* pTerrain) { m_pTerrain = pTerrain; }
	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	void SetMeshHeightHalf(float fheight) { m_fMeshHeightHalf = fheight; }
	virtual void Animate(float fTimeElapsed);

private:
	CHeightMapTerrain*				m_pTerrain;
	CPlayer*						m_pPlayer;
	XMFLOAT3						m_xmf3Offset;
	float							m_fMeshHeightHalf;

};