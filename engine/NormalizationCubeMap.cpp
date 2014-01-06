
#include <windows.h>

#include "NormalizationCubeMap.h"
#include "Vector.h"
#include "Image.h"

void GenerateNormalisationCubeMap(Image &posX, Image &negX, Image &posY, Image &negY, Image &posZ, Image &negZ, int size)
{
	float offset = 0.5f;
	float halfSize = size * 0.5f;
	Vec3 tmpVector;
	BYTE *pByte;

	BYTE *pixels = new BYTE[size * size * 3];

	// Positive X
	pByte = pixels;

	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
		{
			tmpVector.x = halfSize;
			tmpVector.y = -(i + offset - halfSize);
			tmpVector.z = -(j + offset - halfSize);

			tmpVector.Normalize();
			tmpVector = tmpVector * 0.5f + Vec3(0.5f, 0.5f, 0.5f);

			pByte[0] = (BYTE)(tmpVector.x * 255);
			pByte[1] = (BYTE)(tmpVector.y * 255);
			pByte[2] = (BYTE)(tmpVector.z * 255);

			pByte += 3;
		}
	}

	posX.Create(size, size, 3, IFORMAT_RGB, pixels);

	// Negative X
	pByte = pixels;

	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
		{
			tmpVector.x = -halfSize;
			tmpVector.y = -(i + offset - halfSize);
			tmpVector.z = j + offset - halfSize;

			tmpVector.Normalize();
            tmpVector = tmpVector * 0.5f + Vec3(0.5f, 0.5f, 0.5f);

			pByte[0] = (BYTE)(tmpVector.x * 255);
			pByte[1] = (BYTE)(tmpVector.y * 255);
			pByte[2] = (BYTE)(tmpVector.z * 255);

			pByte += 3;
		}
	}

	negX.Create(size, size, 3, IFORMAT_RGB, pixels);

	// Positive Y
	pByte = pixels;

	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
		{
			tmpVector.x = j + offset - halfSize;
			tmpVector.y = halfSize;
			tmpVector.z = i + offset - halfSize;

			tmpVector.Normalize();
			tmpVector = tmpVector * 0.5f + Vec3(0.5f, 0.5f, 0.5f);

			pByte[0] = (BYTE)(tmpVector.x * 255);
			pByte[1] = (BYTE)(tmpVector.y * 255);
			pByte[2] = (BYTE)(tmpVector.z * 255);

			pByte += 3;
		}
	}

	posY.Create(size, size, 3, IFORMAT_RGB, pixels);

	// Negative Y
	pByte = pixels;

	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
		{
			tmpVector.x = j + offset - halfSize;
			tmpVector.y = -halfSize;
			tmpVector.z = -(i + offset - halfSize);

			tmpVector.Normalize();
			tmpVector = tmpVector * 0.5f + Vec3(0.5f, 0.5f, 0.5f);

			pByte[0] = (BYTE)(tmpVector.x * 255);
			pByte[1] = (BYTE)(tmpVector.y * 255);
			pByte[2] = (BYTE)(tmpVector.z * 255);

			pByte += 3;
		}
	}

	negY.Create(size, size, 3, IFORMAT_RGB, pixels);

	// Positive Z
	pByte = pixels;

	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
		{
			tmpVector.x = j + offset - halfSize;
			tmpVector.y = -(i + offset - halfSize);
			tmpVector.z = halfSize;

			tmpVector.Normalize();
			tmpVector = tmpVector * 0.5f + Vec3(0.5f, 0.5f, 0.5f);

			pByte[0] = (BYTE)(tmpVector.x * 255);
			pByte[1] = (BYTE)(tmpVector.y * 255);
			pByte[2] = (BYTE)(tmpVector.z * 255);

			pByte += 3;
		}
	}

	posZ.Create(size, size, 3, IFORMAT_RGB, pixels);

	// Negative Z
	pByte = pixels;

	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
		{
			tmpVector.x = -(j + offset - halfSize);
			tmpVector.y = -(i + offset - halfSize);
			tmpVector.z = -halfSize;

			tmpVector.Normalize();
			tmpVector = tmpVector * 0.5f + Vec3(0.5f, 0.5f, 0.5f);

			pByte[0] = (BYTE)(tmpVector.x * 255);
			pByte[1] = (BYTE)(tmpVector.y * 255);
			pByte[2] = (BYTE)(tmpVector.z * 255);

			pByte += 3;
		}
	}

	negZ.Create(size, size, 3, IFORMAT_RGB, pixels);
}