#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class DXEngineMathHelper
{
public:
	static float Length(XMFLOAT4 vec1, XMFLOAT4 v2);
	static float Length(XMFLOAT4 vec);
	static XMFLOAT4 Normalize(XMFLOAT4 vec);
	static XMFLOAT4 IdentityFloat4();
	static XMFLOAT4X4 IdentityFloat4X4();
};

XMFLOAT4 operator-(const XMFLOAT4& rh, const XMFLOAT4& lh);
XMFLOAT4 operator+(const XMFLOAT4& rh, const XMFLOAT4& lh);
XMFLOAT4 operator*(const XMFLOAT4& rh, float lh);
XMFLOAT4 operator*(float rh, const XMFLOAT4&lh);
XMFLOAT4 operator/(const XMFLOAT4& rh, float lh);
XMFLOAT4 operator/(float rh, const XMFLOAT4&lh);