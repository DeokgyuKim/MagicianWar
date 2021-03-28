//***************************************************************************************
// MathHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper math class.
//***************************************************************************************

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <cstdint>

class MathHelper
{
public:
	// Returns random float in [0, 1).
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	// Returns random float in [a, b).
	static float RandF(float a, float b)
	{
		return a + RandF()*(b-a);
	}

    static int Rand(int a, int b)
    {
        return a + rand() % ((b - a) + 1);
    }

	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}
	 
	template<typename T>
	static T Lerp(const T& a, const T& b, float t)
	{
		return a + (b-a)*t;
	}

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x); 
	}

	// Returns the polar angle of the point (x,y) in [0, 2*PI).
	static float AngleFromXY(float x, float y);

	static DirectX::XMVECTOR SphericalToCartesian(float radius, float theta, float phi)
	{
		return DirectX::XMVectorSet(
			radius*sinf(phi)*cosf(theta),
			radius*cosf(phi),
			radius*sinf(phi)*sinf(theta),
			1.0f);
	}

    static DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX M)
	{
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
        DirectX::XMMATRIX A = M;
        A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

        DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
        return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
	}

    static DirectX::XMFLOAT4X4 Identity4x4()
    {
        static DirectX::XMFLOAT4X4 I(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        return I;
    }

    static DirectX::XMVECTOR RandUnitVec3();
    static DirectX::XMVECTOR RandHemisphereUnitVec3(DirectX::XMVECTOR n);

	static const float Infinity;
	static const float Pi;

	static DirectX::XMFLOAT4X4 MultiplyFloat_X_Matrix(const float scalar, DirectX::XMFLOAT4X4 xmmtx4x4Matrix)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		
		xmmtx4x4Result._11 = xmmtx4x4Matrix._11 * scalar;
		xmmtx4x4Result._12 = xmmtx4x4Matrix._12 * scalar;
		xmmtx4x4Result._13 = xmmtx4x4Matrix._13 * scalar;
		xmmtx4x4Result._14 = xmmtx4x4Matrix._14 * scalar;
		
		xmmtx4x4Result._21 = xmmtx4x4Matrix._21 * scalar;
		xmmtx4x4Result._22 = xmmtx4x4Matrix._22 * scalar;
		xmmtx4x4Result._23 = xmmtx4x4Matrix._23 * scalar;
		xmmtx4x4Result._24 = xmmtx4x4Matrix._24 * scalar;
		
		xmmtx4x4Result._31 = xmmtx4x4Matrix._31 * scalar;
		xmmtx4x4Result._32 = xmmtx4x4Matrix._32 * scalar;
		xmmtx4x4Result._33 = xmmtx4x4Matrix._33 * scalar;
		xmmtx4x4Result._34 = xmmtx4x4Matrix._34 * scalar;

		xmmtx4x4Result._41 = xmmtx4x4Matrix._41 * scalar;
		xmmtx4x4Result._42 = xmmtx4x4Matrix._42 * scalar;
		xmmtx4x4Result._43 = xmmtx4x4Matrix._43 * scalar;
		xmmtx4x4Result._44 = xmmtx4x4Matrix._44 * scalar;

		return(xmmtx4x4Result);
	}

	static DirectX::XMFLOAT4X4 AddMatrix(DirectX::XMFLOAT4X4 xmmtx4x4Matrix1, DirectX::XMFLOAT4X4 xmmtx4x4Matrix2)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		xmmtx4x4Result._11 = xmmtx4x4Matrix1._11 + xmmtx4x4Matrix2._11;
		xmmtx4x4Result._12 = xmmtx4x4Matrix1._12 + xmmtx4x4Matrix2._12;
		xmmtx4x4Result._13 = xmmtx4x4Matrix1._13 + xmmtx4x4Matrix2._13;
		xmmtx4x4Result._14 = xmmtx4x4Matrix1._14 + xmmtx4x4Matrix2._14;
												  
		xmmtx4x4Result._21 = xmmtx4x4Matrix1._21 + xmmtx4x4Matrix2._21;
		xmmtx4x4Result._22 = xmmtx4x4Matrix1._22 + xmmtx4x4Matrix2._22;
		xmmtx4x4Result._23 = xmmtx4x4Matrix1._23 + xmmtx4x4Matrix2._23;
		xmmtx4x4Result._24 = xmmtx4x4Matrix1._24 + xmmtx4x4Matrix2._24;
												  
		xmmtx4x4Result._31 = xmmtx4x4Matrix1._31 + xmmtx4x4Matrix2._31;
		xmmtx4x4Result._32 = xmmtx4x4Matrix1._32 + xmmtx4x4Matrix2._32;
		xmmtx4x4Result._33 = xmmtx4x4Matrix1._33 + xmmtx4x4Matrix2._33;
		xmmtx4x4Result._34 = xmmtx4x4Matrix1._34 + xmmtx4x4Matrix2._34;
												  
		xmmtx4x4Result._41 = xmmtx4x4Matrix1._41 + xmmtx4x4Matrix2._41;
		xmmtx4x4Result._42 = xmmtx4x4Matrix1._42 + xmmtx4x4Matrix2._42;
		xmmtx4x4Result._43 = xmmtx4x4Matrix1._43 + xmmtx4x4Matrix2._43;
		xmmtx4x4Result._44 = xmmtx4x4Matrix1._44 + xmmtx4x4Matrix2._44;
		return(xmmtx4x4Result);
	}
};

