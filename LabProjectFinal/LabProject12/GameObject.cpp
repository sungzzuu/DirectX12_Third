#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"
#include "Player.h"

CGameObject::CGameObject(int nMeshes)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh * [m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++) m_ppMeshes[i] = NULL;
	}
}
CGameObject::~CGameObject()
{
	if (m_ppMeshes) {
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void CGameObject::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CGameObject::SetScale(XMFLOAT3 xmf3scale)
{
	m_f3Scale = xmf3scale;
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(XMMatrixScaling(m_f3Scale.x, m_f3Scale.y, m_f3Scale.z), XMLoadFloat4x4(&m_xmf4x4World)));

}

bool CGameObject::Animate(float fTimeElapsed)
{
	UpdateBoundingBox();

	return OBJ_NONE;
}

void CGameObject::OnPrepareRender()
{
}
void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}
void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();
	UpdateShaderVariables(pd3dCommandList);
	if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);
	//���� ��ü�� �����ϴ� ��� �޽��� �������Ѵ�. 
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}

void CGameObject::UpdateBoundingBox()
{
	if (m_ppMeshes[0])
	{
		m_ppMeshes[0]->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	}
}

void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	//��ü�� ���� ��ȯ ����� ��Ʈ ���(32-��Ʈ ��)�� ���Ͽ� ���̴� ����(��� ����)�� �����Ѵ�.
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 4, &m_xmf4HitColor, 16);

}

void CGameObject::ReleaseShaderVariables()
{
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}
void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}
XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}
//���� ��ü�� ���� z-�� ���͸� ��ȯ�Ѵ�. 
XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32,
		m_xmf4x4World._33)));
}
//���� ��ü�� ���� y-�� ���͸� ��ȯ�Ѵ�.
XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22,
		m_xmf4x4World._23)));
}
//���� ��ü�� ���� x-�� ���͸� ��ȯ�Ѵ�. 
XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12,
		m_xmf4x4World._13)));
}
XMFLOAT3 CGameObject::GetDir()
{
	return(Vector3::Normalize(m_xmf3MoveDir));
}
//���� ��ü�� ���� x-�� �������� �̵��Ѵ�. 
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}
//���� ��ü�� ���� y-�� �������� �̵��Ѵ�. 
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance); CGameObject::SetPosition(xmf3Position);
}
//���� ��ü�� ���� z-�� �������� �̵��Ѵ�. 
void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}
void CGameObject::MoveByDir(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Dir = GetDir();

	xmf3Position = Vector3::Add(xmf3Position, xmf3Dir, fDistance);
	CGameObject::SetPosition(xmf3Position);
}
//���� ��ü�� �־��� ������ ȸ���Ѵ�. 
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::SetLook(XMFLOAT3 vLook)
{
	m_xmf4x4World._31 = vLook.x;
	m_xmf4x4World._32 = vLook.y;
	m_xmf4x4World._33 = vLook.z;

}

void CGameObject::SetHit(bool bHit, XMFLOAT4 xmColor)
{
	m_bHit = bHit; 
	m_xmf4HitColor = xmColor;
	m_fHitTime = 0.f;
}

void CGameObject::SetMesh(int nIndex, CMesh* pMesh)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
}
void CGameObject::ReleaseUploadBuffers()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->ReleaseUploadBuffers();
		}
	}
}

CRotatingFlagObject::CRotatingFlagObject(int nMeshes)
	: CGameObject(nMeshes)
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 15.0f;

}

CRotatingFlagObject::~CRotatingFlagObject()
{
}

bool CRotatingFlagObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
	if (m_eState == DOWN)
	{
		XMFLOAT3 pos = GetPosition();

		// ũ�⸸ŭ �������ٸ�
		if (m_fOriginY - m_xmOOBB.Extents.y * 2.f+1.f > GetPosition().y)
		{
			m_eState = FINISH;
			SetPosition(pos.x, m_fOriginY - m_xmOOBB.Extents.y * 2.f+1.f, pos.z);
		}
		else
			SetPosition(pos.x, pos.y - 15.f * fTimeElapsed, pos.z);
	}
	UpdateBoundingBox();

	return OBJ_NONE;

}

void CRotatingFlagObject::SetState(STATE _eState)
{
	m_eState = _eState;

	if (_eState == DOWN)
	{
		m_fOriginY = GetPosition().y;
		m_fRotationSpeed *= 3.f;
	}

}

CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, 
	int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color)
	:CGameObject(0)
{
	//������ ����� ���� ���� ����, ������ ũ���̴�.
	m_nWidth = nWidth;
	m_nLength = nLength;
	/*���� ��ü�� ���� �޽����� �迭�� ���� ���̴�. nBlockWidth, nBlockLength�� ���� �޽� �ϳ��� ����, ���� ũ
	���̴�. cxQuadsPerBlock, czQuadsPerBlock�� ���� �޽��� ���� ����� ���� ���� �簢���� �����̴�.*/
	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;
	//xmf3Scale�� ������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ����. 
	m_xmf3Scale = xmf3Scale;
	
	//������ ����� ���� ���� �����Ѵ�. 
	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);
	
	//�������� ���� ����, ���� �������� ���� �޽��� �� ���� �ִ� ���� ��Ÿ����. 
	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;
	//���� ��ü�� ǥ���ϱ� ���� ���� �޽��� �����̴�. 
	m_nMeshes = cxBlocks * czBlocks;
	
	//���� ��ü�� ǥ���ϱ� ���� ���� �޽��� ���� ������ �迭�� �����Ѵ�. 
	m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)m_ppMeshes[i] = NULL;
	CHeightMapGridMesh* pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			//������ �Ϻκ��� ��Ÿ���� ���� �޽��� ���� ��ġ(��ǥ)�̴�. 
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			//������ �Ϻκ��� ��Ÿ���� ���� �޽��� �����Ͽ� ���� �޽��� �����Ѵ�. 
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart, 
			zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
		}
	}
	//������ �������ϱ� ���� ���̴��� �����Ѵ�. 
	CTerrainShader *pShader = new CTerrainShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	SetShader(pShader);

}

CHeightMapTerrain::~CHeightMapTerrain()
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

CMyTeamObject::CMyTeamObject(void* pContext, void* pPlayer, XMFLOAT3 xmf3Offset, float fMeshHeightHalf, int nMeshes)
	:CTerrainObject(pContext,fMeshHeightHalf,nMeshes)
{
	
	// �÷��̾� ��ġ���� offset��ŭ ������ ��ġ�� �߰�. ������ y�� �޾ƿ���
	m_pPlayer = (CPlayer*)pPlayer;
	XMFLOAT3 pos = { m_pPlayer->GetPosition().x + xmf3Offset.x, 0, m_pPlayer->GetPosition().z + xmf3Offset.z};
	pos.y = m_pTerrain->GetHeight(pos.x, pos.z);
	SetPosition(pos);

	// ������ �����ϱ�
	m_xmf3Offset = xmf3Offset;
	

}

CMyTeamObject::~CMyTeamObject()
{
}
bool CMyTeamObject::Animate(float fTimeElapsed)
{
	// �÷��̾�� �������� �����ϸ� �Ѿư���
	XMFLOAT3 playerPos = m_pPlayer->GetPosition();
	XMFLOAT3 newPos = { playerPos.x + m_xmf3Offset.x, playerPos.y, playerPos.z + m_xmf3Offset.z };
	SetPosition(newPos.x, m_pTerrain->GetHeight(newPos.x, newPos.z) + m_fMeshHeightHalf, newPos.z);
	OnObjectUpdateCallback(fTimeElapsed);
	UpdateBoundingBox();
	return OBJ_NONE;

}

CEnemyFlyShip::CEnemyFlyShip(int nMeshes)
{
	m_bSpawn = false;
}

CEnemyFlyShip::~CEnemyFlyShip()
{
}

bool CEnemyFlyShip::Animate(float fTimeElapsed)
{
	switch (m_eState)
	{
	case CEnemyFlyShip::BEGIN:
		MoveByDir(fTimeElapsed * m_fSpeed);
		if (GetPosition().z < m_pTargetPos.z)
		{
			m_eState = CEnemyFlyShip::SPAWN;
		}
		break;
	case CEnemyFlyShip::SPAWN:
		// ����
		m_fSpawnTime += fTimeElapsed;
		if (m_fSpawnTime > 3.f)
		{
			m_fSpawnTime = 0.f;
			m_fSpeed *= 2.f;
			m_eState = CEnemyFlyShip::END;
		}
		break;
	case CEnemyFlyShip::END:
		m_xmf3MoveDir.y = 0.3f;
		MoveByDir(fTimeElapsed * m_fSpeed);
		break;
	default:
		break;
	}
	if (GetPosition().z < 0.f )
		return OBJ_DEAD;

	return OBJ_NONE;

}

bool CEnemyFlyShip::SpawnCheck()
{
	if (m_eState == CEnemyFlyShip::SPAWN && !m_bSpawn)
	{
		m_bSpawn = true;
		return true;
	}

	return false;
}

CEnemy::CEnemy(void* pContext, float fMeshHeightHalf, int nMeshes)
	:CTerrainObject(pContext, fMeshHeightHalf, nMeshes)
{
	m_eState = CEnemy::BEGIN;
	m_fRotationSpeed = 2000.f;
	m_fDownSpeed = 10.f;
	m_fScale = 0.1f;
	m_fBulletCreateTime = 3.f;

}

CEnemy::~CEnemy()
{
}

bool CEnemy::Animate(float fTimeElapsed)
{
	if (m_fScale < 1.f)
	{
		m_fScale *= 1.01f;
		SetScale(XMFLOAT3(1.01f, 1.01f, 1.01f));
	}


	if (m_eState == CEnemy::BEGIN)
	{
		// ���鼭 �Ʒ��� ������
		Rotate(0.f, fTimeElapsed * m_fRotationSpeed, 0.f);
		MoveUp(-m_fDownSpeed * fTimeElapsed);

		// �ͷ����� ���̺��� �۾����� �ͷ��� �¿� && ���� NORMAL�� ����
		XMFLOAT3 pos = GetPosition();
		if (m_pTerrain->GetHeight(pos.x, pos.z) + m_fMeshHeightHalf> pos.y)
		{
			m_eState = NORMAL;
			SetPosition(pos.x, m_pTerrain->GetHeight(pos.x, pos.z) + m_fMeshHeightHalf, pos.z);
		}
	}
	else
	{
		m_fBulletCreateTime += fTimeElapsed;
		if (m_fBulletCreateTime > 5.f)
		{
			// ����̸� �Ѿ� ����!!!
			CBullet* pBullet = new CBullet(1);
			pBullet->SetPosition(GetPosition());
			XMFLOAT3 PlayerPos = m_pObjectsShader->GetPlayer()->GetPosition();
			XMFLOAT3 vDir = Vector3::Normalize(Vector3::Subtract(PlayerPos, GetPosition()));
			pBullet->SetDir(vDir);
			m_pObjectsShader->AddObject(OBJ::MONSTER_BULLET, pBullet);
			// ������ �ð����� !!
			m_fBulletCreateTime = float(rand() % 5) * 0.5f;
		}
	
	}
	if (m_bHit)
		m_fHitTime += fTimeElapsed;
	// �浹 ���� ������Ʈ
	if (m_fHitTime > 0.2f)
	{
		m_fHitTime = 0.f;
		m_bHit = false;
		m_xmf4HitColor = m_xmf4HitColor = { 0.f,0.f,0.f,0.f };
	}

	UpdateBoundingBox();
	return OBJ_NONE;

}

CTerrainObject::CTerrainObject(void* pContext, float fMeshHeightHalf, int nMeshes)
{
	// ������Ʈ�� ��ġ�� ����� �� ������ ������ ���� ������Ʈ�� ��ġ�� ������ �� �ֵ��� �����Ѵ�. 
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	SetTerrain(pTerrain);
	m_fMeshHeightHalf = fMeshHeightHalf;
}

CTerrainObject::~CTerrainObject()
{
}

void CTerrainObject::OnObjectUpdateCallback(float fTimeElapsed)
{
	XMFLOAT3 xmf3PlayerPosition = GetPosition();
	float fHeight = m_pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z) + 6.0f;
}

CBullet::CBullet(int nMeshes)
{
	m_fSpeed = 80.f;
	m_fCreateTime = 0.f;

}

CBullet::~CBullet()
{
}

bool CBullet::Animate(float fTimeElapsed)
{
	MoveByDir(m_fSpeed * fTimeElapsed);
	Rotate(200.f * fTimeElapsed, 300.f * fTimeElapsed, 200.f * fTimeElapsed);
	UpdateBoundingBox();

	// �÷��̾�� �־����� ����
	m_fCreateTime += fTimeElapsed;
	if(m_fCreateTime > 10.f)
		return OBJ_DEAD;
	return OBJ_NONE;
}
