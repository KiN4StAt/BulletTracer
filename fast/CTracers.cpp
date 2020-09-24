#include "CTracers.h"
#include "dllmain.h"
#include "MinHook.h"
#include <stdio.h>

// ��������� �� ��������
// ������� - ����� ������� ����� �������, � ����� ���������� �������� ����� �����, � ����������� ���������� ������������ �������
static CTracers::DoBulletImpact fpDoImpact = 0;

CTracers::CTracers() {
	// ������������� ��� �� ������� CWeapon::DoBulletImpact
	MH_CreateHook((void*)0x73B550, &pTracers->DoBulletImpactHooked, reinterpret_cast<LPVOID*>(&fpDoImpact));
	// �������� ���
	MH_EnableHook((void*)0x73B550);
}

CTracers::~CTracers() {
	MH_DisableHook((void*)0x726AF0);
}

// �������� �������� ���������� ������� �� �������
// ����������, �� ������ �� ������
bool CTracers::GetScreenCoords(float& x, float& y, float& z, CVector* vecWorld)
{
	D3DXVECTOR3 vecScreen;

	D3DXMATRIX    m((float*)(0xB6FA2C));
	DWORD* dwLenX = (DWORD*)(0xC17044);
	DWORD* dwLenY = (DWORD*)(0xC17048);

	vecScreen.x = (vecWorld->z * m._31) + (vecWorld->y * m._21) + (vecWorld->x * m._11) + m._41;
	vecScreen.y = (vecWorld->z * m._32) + (vecWorld->y * m._22) + (vecWorld->x * m._12) + m._42;
	vecScreen.z = (vecWorld->z * m._33) + (vecWorld->y * m._23) + (vecWorld->x * m._13) + m._43;

	double    fRecip = (double)1.0 / vecScreen.z;
	vecScreen.x *= (float)(fRecip * (*dwLenX));
	vecScreen.y *= (float)(fRecip * (*dwLenY));

	x = vecScreen.x;
	y = vecScreen.y;
	z = vecScreen.z;
	return vecScreen.z >= 1.0f;
}

// ��������� float[3/4] (ImGui) � D3DCOLOR (d3d9hook)
DWORD CTracers::floatToHex(float* color, bool colType) {
	if (colType)
		return D3DCOLOR_ARGB(static_cast<int>(color[3] * 255.0f), static_cast<int>(color[0] * 255.0f), static_cast<int>(color[1] * 255.0f), static_cast<int>(color[2] * 255.0f));
	return D3DCOLOR_XRGB(static_cast<int>(color[0] * 255.0f), static_cast<int>(color[1] * 255.0f), static_cast<int>(color[2] * 255.0f));
}
// ������ ��������
void CTracers::Render() {
	
	for (int i = 0; i < pConfig->iTracersCount; i++) {
		float originX, originY, originZ, targetX, targetY, targetZ;
		bool resultOrigin = this->GetScreenCoords(originX, originY, originZ, &this->tracers[i].fOrigin);
		bool resultTarget = this->GetScreenCoords(targetX, targetY, targetZ, &this->tracers[i].fTarget);

		if (tracers[i].eType != 0xff && resultOrigin && resultTarget) {

			if (GetTickCount64() - tracers[i].uLongTicks < pConfig->iTracerTime * 1000) {

				pD3DHook->pRender->Line(originX, originY, targetX, targetY, 
					this->floatToHex(pConfig->fColors[this->tracers[i].eType + 7 * this->tracers[i].bOwner]));
			}
		}
	}
}
// ������� ������ � ��������� � �������� ������ ������ �����
void CTracers::AddTracer(CVector origin, CVector target, unsigned char eType, unsigned char bOwner) {
	// �������� ������
	for (int i = 99; i > 0; i--) {
		tracers[i] = tracers[i - 1];
	}
	// ������ �� ������� ���������
	if (eType != ENTITY_TYPE_NOTHING) {
		tracers[0].eType = eType;
		tracers[0].fOrigin = origin;
		tracers[0].fTarget = target;
		tracers[0].uLongTicks = GetTickCount64();
		tracers[0].bOwner = bOwner;
	}
}

// ���������� ���� ������� ����, ����� ������ ������ ����� __fastcall � ���������� �������� void* EDX - �������� CTracers.h
void __fastcall CTracers::DoBulletImpactHooked(void* weapon, void* EDX, CEntity* owner, CEntity* victim, CVector* startPoint, CVector* endPoint, CColPoint* colPoint, int arg5) {
	// ���������, ������ �� ���� ����-��, ����� ��� �������
	if (victim != nullptr) {
		// ��������� ��� �������� ���� �����, ��� ������ ���
		unsigned char own = (owner == *(CEntity**)0xB6F5F0) ? CTracers::ownerType::OWNER_LOCALPLAYER: CTracers::ownerType::OWNER_NOTLOCALPLAYER;
		// ��������� ������
		pTracers->AddTracer(*startPoint, colPoint->m_vecPoint, victim->m_nType, own);
	}
	// �������� ������������ ������� ����������, ����� �� ��������
	return fpDoImpact(weapon, EDX, owner, victim, startPoint, endPoint, colPoint, arg5);
}