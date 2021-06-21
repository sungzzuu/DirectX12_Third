#pragma once
#include "Mesh.h"
class CShader;
class CCamera;
class CPlayer;
class CObjectsShader;

class CGameObject
{
public:
	CGameObject(int nMeshes=1);
	virtual ~CGameObject();
private:
	int							m_nReferences = 0;
public:
	void						AddRef() { m_nReferences++; }
	void						Release() { if (--m_nReferences <= 0) delete this; }
protected:
	XMFLOAT4X4					m_xmf4x4World;
	CShader*					m_pShader = NULL;
	//���� ��ü�� ���� ���� �޽��� �����ϴ� ��� ���� ��ü�� ������ �޽��鿡 ���� �����Ϳ� �� �����̴�.
	CMesh**						m_ppMeshes = NULL;
	int							m_nMeshes = 0;
	// ��ü ������ ����
	XMFLOAT3					m_f3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT3					m_xmf3MoveDir = XMFLOAT3(0.0f, 0.0f, 1.0f);

	CObjectsShader*				m_pObjectsShader;

public:
	BoundingOrientedBox			m_xmOOBB;

public:
	void ReleaseUploadBuffers(); 
	virtual void SetMesh(int nIndex, CMesh* pMesh);
	virtual void SetShader(CShader* pShader);
	virtual void SetScale(XMFLOAT3 xmf3scale);
	virtual XMFLOAT3 GetScale() {return XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._22, m_xmf4x4World._33);}
	virtual bool Animate(float fTimeElapsed);
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
	XMFLOAT3 GetDir();
	//���� ��ü�� ��ġ�� �����Ѵ�. 
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	//���� ��ü�� ���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	// Dir �������� ���� ��ü �̵�
	void MoveByDir(float fDistance = 1.0f);
	//���� ��ü�� ȸ��(x-��, y-��, z-��)�Ѵ�. 
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void SetDir(XMFLOAT3 vDir) { m_xmf3MoveDir = vDir; }
	void SetLook(XMFLOAT3 vLook);

	// ���̴� ����
	void SetObjectsShader(CObjectsShader* pShader) { m_pObjectsShader = pShader; }
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
	bool		m_bMyTeam;
public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) {m_xmf3RotationAxis = xmf3RotationAxis;}
	virtual bool Animate(float fTimeElapsed);
	STATE GetState() { return m_eState; }
	void SetState(STATE _eState);
	void SetMyTeam(bool bMyTeam) { m_bMyTeam = bMyTeam; }
	bool GetMyTeam() { return m_bMyTeam; }
};

// ���ƿ��� �����
// Ÿ�� �����ֱ�
// DIR �����ֱ�
class CEnemyFlyShip : public CGameObject
{
public:
	enum STATE
	{
		BEGIN, SPAWN, END
	};

public:
	CEnemyFlyShip(int nMeshes = 1);
	virtual ~CEnemyFlyShip();

private:
	STATE			m_eState;
	XMFLOAT3		m_pTargetPos;
	float			m_fSpeed;
	float			m_fSpawnTime = 0.f;
	bool			m_bSpawn = false;

public:
	STATE			GetState() { return m_eState; }
	void			SetState(STATE _eState) { m_eState = _eState; }
	void			SetTargetPos(XMFLOAT3 pos) { m_pTargetPos = pos; }
	void			SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	virtual bool	Animate(float fTimeElapsed);
	bool			SpawnCheck();

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
	CTerrainObject(void* pContext, float fMeshHeightHalf, int nMeshes = 1);
	virtual ~CTerrainObject();

	//�÷��̾��� ��ġ�� �ٲ� ������ ȣ��Ǵ� �Լ��� �� �Լ����� ����ϴ� ������ �����ϴ� �Լ��̴�.
	virtual void OnObjectUpdateCallback(float fTimeElapsed);
	void SetTerrain(CHeightMapTerrain* pTerrain) { m_pTerrain = pTerrain; }
	void SetMeshHeightHalf(float fheight) { m_fMeshHeightHalf = fheight; }
	virtual bool Animate(float fTimeElapsed) {
		return OBJ_NONE;
	};

protected:
	CHeightMapTerrain*				m_pTerrain;
	float							m_fMeshHeightHalf;

};
// �� ������Ʈ
class CEnemy : public CTerrainObject
{
public:
	enum STATE { BEGIN, NORMAL };
	CEnemy(void* pContext, float fMeshHeightHalf, int nMeshes = 1);
	virtual ~CEnemy();

public:
	virtual bool	Animate(float fTimeElapsed);
private:
	STATE		m_eState;
	float		m_fRotationSpeed;
	float		m_fDownSpeed;
	float		m_fScale;
	float		m_fBulletCreateTime;
};

// �Ʊ� ������Ʈ
class CMyTeamObject : public CTerrainObject
{
public:
	CMyTeamObject(void* pContext, void* pPlayer, XMFLOAT3 xmf3Offset, float fMeshHeightHalf, int nMeshes = 1);
	virtual ~CMyTeamObject();
	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	virtual bool Animate(float fTimeElapsed);

private:
	CPlayer*						m_pPlayer;
	XMFLOAT3						m_xmf3Offset;
};

// Dir�� �̵��ϴ� �Ѿ�
class CBullet : public CGameObject
{
public:
	CBullet(int nMeshes=1);
	virtual ~CBullet();
	virtual bool Animate(float fTimeElapsed);

private:
	float			m_fSpeed;
	float			m_fCreateTime = 0.f;

};