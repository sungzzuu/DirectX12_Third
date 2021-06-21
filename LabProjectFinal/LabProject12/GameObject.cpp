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

void CGameObject::Animate(float fTimeElapsed)
{
	UpdateBoundingBox();

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
	//게임 객체가 포함하는 모든 메쉬를 렌더링한다. 
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
		cout << m_xmOOBB.Center.x << endl;
	}
}

void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	//객체의 월드 변환 행렬을 루트 상수(32-비트 값)를 통하여 셰이더 변수(상수 버퍼)로 복사한다.
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
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
//게임 객체의 로컬 z-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32,
		m_xmf4x4World._33)));
}
//게임 객체의 로컬 y-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22,
		m_xmf4x4World._23)));
}
//게임 객체의 로컬 x-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12,
		m_xmf4x4World._13)));
}
XMFLOAT3 CGameObject::GetDir()
{
	return(Vector3::Normalize(m_xmf3MoveDir));
}
//게임 객체를 로컬 x-축 방향으로 이동한다. 
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}
//게임 객체를 로컬 y-축 방향으로 이동한다. 
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance); CGameObject::SetPosition(xmf3Position);
}
//게임 객체를 로컬 z-축 방향으로 이동한다. 
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
//게임 객체를 주어진 각도로 회전한다. 
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

void CRotatingFlagObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
	if (m_eState == DOWN)
	{
		XMFLOAT3 pos = GetPosition();

		// 크기만큼 내려갔다면
		if (m_fOriginY - m_xmOOBB.Extents.y * 2.f+1.f > GetPosition().y)
		{
			m_eState = FINISH;
			SetPosition(pos.x, m_fOriginY - m_xmOOBB.Extents.y * 2.f+1.f, pos.z);
		}
		else
			SetPosition(pos.x, pos.y - 15.f * fTimeElapsed, pos.z);
	}
	UpdateBoundingBox();
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
	//지형에 사용할 높이 맵의 가로, 세로의 크기이다.
	m_nWidth = nWidth;
	m_nLength = nLength;
	/*지형 객체는 격자 메쉬들의 배열로 만들 것이다. nBlockWidth, nBlockLength는 격자 메쉬 하나의 가로, 세로 크
	기이다. cxQuadsPerBlock, czQuadsPerBlock은 격자 메쉬의 가로 방향과 세로 방향 사각형의 개수이다.*/
	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;
	//xmf3Scale는 지형을 실제로 몇 배 확대할 것인가를 나타낸다. 
	m_xmf3Scale = xmf3Scale;
	
	//지형에 사용할 높이 맵을 생성한다. 
	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);
	
	//지형에서 가로 방향, 세로 방향으로 격자 메쉬가 몇 개가 있는 가를 나타낸다. 
	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;
	//지형 전체를 표현하기 위한 격자 메쉬의 개수이다. 
	m_nMeshes = cxBlocks * czBlocks;
	
	//지형 전체를 표현하기 위한 격자 메쉬에 대한 포인터 배열을 생성한다. 
	m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)m_ppMeshes[i] = NULL;
	CHeightMapGridMesh* pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			//지형의 일부분을 나타내는 격자 메쉬의 시작 위치(좌표)이다. 
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			//지형의 일부분을 나타내는 격자 메쉬를 생성하여 지형 메쉬에 저장한다. 
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart, 
			zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
		}
	}
	//지형을 렌더링하기 위한 셰이더를 생성한다. 
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
	
	// 플레이어 위치에서 offset만큼 떨어진 위치에 추가. 지형의 y값 받아오기
	m_pPlayer = (CPlayer*)pPlayer;
	XMFLOAT3 pos = { m_pPlayer->GetPosition().x + xmf3Offset.x, 0, m_pPlayer->GetPosition().z + xmf3Offset.z};
	pos.y = m_pTerrain->GetHeight(pos.x, pos.z);
	SetPosition(pos);

	// 오프셋 저장하기
	m_xmf3Offset = xmf3Offset;
	

}

CMyTeamObject::~CMyTeamObject()
{
}
void CMyTeamObject::Animate(float fTimeElapsed)
{
	// 플레이어와 오프셋을 유지하며 쫓아가기
	XMFLOAT3 playerPos = m_pPlayer->GetPosition();
	XMFLOAT3 newPos = { playerPos.x + m_xmf3Offset.x, playerPos.y, playerPos.z + m_xmf3Offset.z };
	SetPosition(newPos.x, m_pTerrain->GetHeight(newPos.x, newPos.z) + m_fMeshHeightHalf, newPos.z);
	OnObjectUpdateCallback(fTimeElapsed);
	UpdateBoundingBox();

}

CEnemyFlyShip::CEnemyFlyShip(int nMeshes)
{
	m_bSpawn = false;
}

CEnemyFlyShip::~CEnemyFlyShip()
{
}

void CEnemyFlyShip::Animate(float fTimeElapsed)
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
		// 생성
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
}

CEnemy::~CEnemy()
{
}

void CEnemy::Animate(float fTimeElapsed)
{
	if (m_eState == CEnemy::BEGIN)
	{
		// 돌면서 크기 커지면서 아래로 내려옴
		// 터레인의 높이보다 작아지면 터레인 태움 && 상태 NORMAL로 변경

	}
	else
	{

	}
}

CTerrainObject::CTerrainObject(void* pContext, float fMeshHeightHalf, int nMeshes)
{
	// 오브젝트의 위치가 변경될 때 지형의 정보에 따라 오브젝트의 위치를 변경할 수 있도록 설정한다. 
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
