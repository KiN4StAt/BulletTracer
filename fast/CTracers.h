#pragma once

#include "game_sa.h"


class CTracers {
private:
	
	// ��������� � ��������� ����������� ��� ���������, � ������:
	struct Tracer {
		CVector fOrigin = { 0.0f, 0.0f, 0.0f }; // ������ ����� ����
		CVector fTarget = { 0.0f, 0.0f, 0.0f }; // ���� ����� ����
		unsigned char eType = 0xff; // � ����� ��� Entity �� ������ (ENTITY_TYPE)
		unsigned char bOwner = 1; // ��� ������ ����
		unsigned long long uLongTicks = 0; // ���������� �����, �� ������� ��������� ����
	};
	// ������ �������� ��������� �������
	Tracer tracers[100];

	// ��������� ��������� ������� ����� �� ������
	bool GetScreenCoords(float& x, float& y, float& z, CVector* vecWorld);
	// ��������� Float color � D3DCOLOR
	inline DWORD floatToHex(float* color, bool colType = true);

public:
	enum ownerType {
		OWNER_LOCALPLAYER = 0,
		OWNER_NOTLOCALPLAYER = 1
	};

	CTracers();

	~CTracers();

	void Render();
	void AddTracer(CVector origin, CVector target, unsigned char eType, unsigned char bOwner);

	// ��� ��� DoBulletImpact - ������� ������, ��� ����� ��� ������ __thiscall (��� ������ __fastcall, __stdcall (WINAPI), __cdecl (������� �������), __declspec (���� ������� � �����������)
	// (�� ����� ���� ��� ���������� ���������� � ������)
	// __thiscall �������� ������ ���������� ��������� �� ������ ������ (typename T* this) (� ������ ��� � ���� �� ���������)
	// �� ����� __thiscall ������� ������ ��������, ������� �� ����� �������� ������������ � ���� ����� __fastcall
	// __fastcall �������� ��������� ����� ��������, ��� ������� ������� �������� ����� ����. �� ���� ������������ ��� ������ __thiscall, �� � ��� ���������� ����������� ������ ��������
	// ������� � ��� ������ ���������� ����� void* EDX, � �� ����� CEntity* owner
	static void __fastcall DoBulletImpactHooked(void* weapon, void* EDX, CEntity* owner, CEntity* victim, CVector* startPoint, CVector* endPoint, CColPoint* colPoint, int arg5);
	// ��������� �������� �������, ����� ����� ����� ���� ������� ����� ��������
	typedef void(__fastcall* DoBulletImpact)(void*, void*, CEntity*, CEntity*, CVector*, CVector*, CColPoint*, int);
};