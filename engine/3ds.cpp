
//#include "main.h"

#include <assert.h>

#include "3ds.h"
#include "Vector.h"
//#include "Console.h"

//extern Console *console;

int g_Buffer[50000] = {0};	// Lixo

Importer3DS::Importer3DS()
{
	m_pFile = NULL;
}

bool Importer3DS::Import3DS(Model *pModel, char *strFileName)
{
	char strMessage[256]	= {0};
	tChunk currentChunk		= {0};

	m_pFile = fopen(strFileName, "rb");

//	console->printfAndRender("Loading Model '%s'", strFileName);

	if(!m_pFile) 
	{
		sprintf(strMessage, "Unable to find the file: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	ReadChunk(&currentChunk);

	// Verificar que se trata de um ficheiro 3DS
	if (currentChunk.ID != M3DMAGIC)
	{
		sprintf(strMessage, "Unable to Load M3DMAGIC chuck from file: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	ProcessNextChunk(pModel, &currentChunk);

	// Copiar os índices dos vértices para a array m_pVertIndex
	for(int i = 0; i < pModel->m_NumMeshes; i++)
	{
		pModel->m_pMesh[i].m_pVertIndex = new int [pModel->m_pMesh[i].m_NumFaces * 3];
		memset(pModel->m_pMesh[i].m_pVertIndex, 0, sizeof(int) * pModel->m_pMesh[i].m_NumFaces * 3);

		for(int j = 0; j < pModel->m_pMesh[i].m_NumFaces; j++)
		{
			pModel->m_pMesh[i].m_pVertIndex[j * 3 + 0] = pModel->m_pMesh[i].m_pFaces[j].vertIndex[0];
			pModel->m_pMesh[i].m_pVertIndex[j * 3 + 1] = pModel->m_pMesh[i].m_pFaces[j].vertIndex[1];
			pModel->m_pMesh[i].m_pVertIndex[j * 3 + 2] = pModel->m_pMesh[i].m_pFaces[j].vertIndex[2];
		}
	}

	pModel->computeTangentSpace();

	//pModel->computeConectivity();

	pModel->computePlanes();

	pModel->computeBoundingBox();

	pModel->loadTextures();

	CleanUp();

	return true;
}

void Importer3DS::CleanUp()
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}




///////////////////////////////// PROCESS NEXT CHUNK\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads the main sections of the .3DS file, then dives deeper with recursion
/////
///////////////////////////////// PROCESS NEXT CHUNK\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Importer3DS::ProcessNextChunk(Model *pModel, tChunk *pPreviousChunk)
{
	Mesh newObject /*= {0}*/;					// This is used to add to our object list
	t_Material newTexture = {0};				// This is used to add to our material list

	tChunk currentChunk = {0};					// The current chunk to Load
	tChunk tempChunk = {0};						// A temp chunk for holding data		

	// Below we check our chunk ID each time we read a new chunk.  Then, if
	// we want to extract the information from that chunk, we do so.
	// If we don't want a chunk, we just read past it.  

	// Continue to read the sub chunks until we have reached the length.
	// After we read ANYTHING we add the bytes read to the chunk and then check
	// check against the length.
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// Read next Chunk
		ReadChunk(&currentChunk);

		// Check the chunk ID
		switch (currentChunk.ID)
		{
		case VERSION:							// This holds the version of the file
			
			// If the file was made in 3D Studio Max, this chunk has an int that 
			// holds the file version.  Since there might be new additions to the 3DS file
			// format in 4.0, we give a warning to that problem.
			// However, if the file wasn't made by 3D Studio Max, we don't 100% what the
			// version length will be so we'll simply ignore the value

			// Read the file version and add the bytes read to our bytesRead variable
			currentChunk.bytesRead += fread(g_Buffer, 1, currentChunk.length - currentChunk.bytesRead, m_pFile);

			// If the file version is over 3, give a warning that there could be a problem
			if ((currentChunk.length - currentChunk.bytesRead == 4) && (g_Buffer[0] > 0x03)) {
				MessageBox(NULL, "This 3DS file is over version 3 so it may Load incorrectly", "Warning", MB_OK);
			}
			break;

		case OBJECTINFO:						// This holds the version of the mesh
			{	
			// This chunk holds the version of the mesh.  It is also the head of the MATERIAL
			// and OBJECT chunks.  From here on we start reading in the material and object info.

			// Read the next chunk
			ReadChunk(&tempChunk);

			// Get the version of the mesh
			tempChunk.bytesRead += fread(g_Buffer, 1, tempChunk.length - tempChunk.bytesRead, m_pFile);

			// Increase the bytesRead by the bytes read from the last chunk
			currentChunk.bytesRead += tempChunk.bytesRead;

			// Go to the next chunk, which is the object has a texture, it should be MATERIAL, then OBJECT.
			ProcessNextChunk(pModel, &currentChunk);
			break;
		}
		case MATERIAL:							// This holds the material information

			// This chunk is the header for the material info chunks

			// Increase the number of materials
			pModel->m_NumMaterials++;

			// Add a empty texture structure to our texture list.
			// If you are unfamiliar with STL's "vector" class, all push_back()
			// does is add a new node onto the list.  I used the vector class
			// so I didn't need to write my own link list functions.  
			pModel->m_pMaterial.push_back(newTexture);

			// Proceed to the material loading function
			ProcessNextMaterialChunk(pModel, &currentChunk);
			break;

		case OBJECT:							// This holds the name of the object being read
				
			// This chunk is the header for the object info chunks.  It also
			// holds the name of the object.

			// Increase the object count
			pModel->m_NumMeshes++;
		
			// Add a new tObject node to our list of objects (like a link list)
			pModel->m_pMesh.push_back(newObject);
			
			// Initialize the object and all it's data members
			memset(&(pModel->m_pMesh[pModel->m_NumMeshes - 1]), 0, sizeof(Mesh));

			// Get the name of the object and store it, then add the read bytes to our byte counter.
			currentChunk.bytesRead += GetString(pModel->m_pMesh[pModel->m_NumMeshes - 1].m_strName);
			
			// Now proceed to read in the rest of the object information
			ProcessNextObjectChunk(pModel, &(pModel->m_pMesh[pModel->m_NumMeshes - 1]), &currentChunk);
			break;

		case EDITKEYFRAME:

			// Because I wanted to make this a SIMPLE tutorial as possible, I did not include
			// the key frame information.  This chunk is the header for all the animation info.
			// In a later tutorial this will be the subject and explained thoroughly.

			//ProcessNextKeyFrameChunk(pModel, currentChunk);

			// Read past this chunk and add the bytes read to the byte counter
			currentChunk.bytesRead += fread(g_Buffer, 1, currentChunk.length - currentChunk.bytesRead, m_pFile);
			break;

		default: 
			
			// If we didn't care about a chunk, then we get here.  We still need
			// to read past the unknown or ignored chunk and add the bytes read to the byte counter.
			currentChunk.bytesRead += fread(g_Buffer, 1, currentChunk.length - currentChunk.bytesRead, m_pFile);
			break;
		}

		// Add the bytes read from the last chunk to the previous chunk passed in.
		pPreviousChunk->bytesRead += currentChunk.bytesRead;
	}
}

///////////////////////////////// PROCESS NEXT OBJECT CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles all the information about the objects in the file
/////
///////////////////////////////// PROCESS NEXT OBJECT CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Importer3DS::ProcessNextObjectChunk(Model *pModel, Mesh *m_pMesh, tChunk *pPreviousChunk)
{
	// The current chunk to work with
	tChunk currentChunk = {0};

	// Continue to read these chunks until we read the end of this sub chunk
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// Read the next chunk
		ReadChunk(&currentChunk);

		// Check which chunk we just read
		switch (currentChunk.ID)
		{
		case OBJECT_MESH:					// This lets us know that we are reading a new object
		
			// We found a new object, so let's read in it's info using recursion
			ProcessNextObjectChunk(pModel, m_pMesh, &currentChunk);
			break;

		case OBJECT_VERTICES:				// This is the objects vertices
			ReadVertices(m_pMesh, &currentChunk);
			break;

		case OBJECT_FACES:					// This is the objects face information
			ReadVertexIndices(m_pMesh, &currentChunk);
			break;

		case OBJECT_MATERIAL:				// This holds the material name that the object has
			
			// This chunk holds the name of the material that the object has assigned to it.
			// This could either be just a color or a texture map.  This chunk also holds
			// the faces that the texture is assigned to (In the case that there is multiple
			// textures assigned to one object, or it just has a texture on a part of the object.
			// Since most of my game objects just have the texture around the whole object, and 
			// they aren't multitextured, I just want the material name.

			// We now will read the name of the material assigned to this object
			ReadObjectMaterial(pModel, m_pMesh, &currentChunk);			
			break;

		case OBJECT_UV:						// This holds the UV texture coordinates for the object

			// This chunk holds all of the UV coordinates for our object.  Let's read them in.
			ReadUVCoordinates(m_pMesh, &currentChunk);
			break;

		case SMOOTH_GROUP:
			ReadSmoothGroup(m_pMesh, &currentChunk);

		default:  

			// Read past the ignored or unknown chunks
			currentChunk.bytesRead += fread(g_Buffer, 1, currentChunk.length - currentChunk.bytesRead, m_pFile);
			break;
		}

		// Add the bytes read from the last chunk to the previous chunk passed in.
		pPreviousChunk->bytesRead += currentChunk.bytesRead;
	}
}

void Importer3DS::ProcessBumpMapChunk(Model *pModel, tChunk *pPreviousChunk)
{
	tChunk currentChunk = {0};
	tChunk tmpChunk		= {0};

	while(pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		ReadChunk(&currentChunk);

		switch (currentChunk.ID)
		{
		case 0xA252:	// bumpiness ?????
			short bumpiness;
			currentChunk.bytesRead += fread(&bumpiness, 1, sizeof(short), m_pFile);
			pModel->m_pMaterial[pModel->m_NumMaterials - 1].bumpiness = (float)bumpiness / 2.0f;
			break;
		case MAT_MAPNAME:	// Filename da textura
			currentChunk.bytesRead += fread(pModel->m_pMaterial[pModel->m_NumMaterials - 1].strBumpTexFile, 1, currentChunk.length - currentChunk.bytesRead, m_pFile);
			break;
		
		default:			// Passar as chunks ignoradas
			currentChunk.bytesRead += fread(g_Buffer, 1, currentChunk.length - currentChunk.bytesRead, m_pFile);
			break;
		}

		pPreviousChunk->bytesRead += currentChunk.bytesRead;
	}
}

//====================================================================================================
// MATERIAL (0xAxxx)
//====================================================================================================

void Importer3DS::ProcessNextMaterialChunk(Model *pModel, tChunk *pPreviousChunk)
{
	tChunk currentChunk = {0};
	tChunk tmpChunk		= {0};

	while(pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		ReadChunk(&currentChunk);

		switch (currentChunk.ID)
		{
		case MAT_NAME:		// Ler a string com o nome
			currentChunk.bytesRead += fread(pModel->m_pMaterial[pModel->m_NumMaterials - 1].strName, 1, currentChunk.length - currentChunk.bytesRead, m_pFile);
			break;

		case MAT_AMBIENT:	// Cor ambiente
			ReadColorChunk((float*)(pModel->m_pMaterial[pModel->m_NumMaterials - 1].ambient), &currentChunk);
			break;

		case MAT_DIFFUSE:	// Cor difusa
			ReadColorChunk((float*)(pModel->m_pMaterial[pModel->m_NumMaterials - 1].difuse), &currentChunk);
			break;

		case MAT_SPECULAR:	// Cor especular
			ReadColorChunk((float*)(pModel->m_pMaterial[pModel->m_NumMaterials - 1].specular), &currentChunk);
			break;

		case MAT_SHININESS:	// Percentagem de brilho - Glossiness (0 - 100)
			short shininess;
			ReadChunk(&tmpChunk);
            currentChunk.bytesRead += tmpChunk.bytesRead;
			currentChunk.bytesRead += fread(&shininess, 1, tmpChunk.length - tmpChunk.bytesRead, m_pFile);
			pModel->m_pMaterial[pModel->m_NumMaterials - 1].shininess = (float)shininess;
			break;

		case MAT_TEXMAP:	// Cabeçalho para informações da textura
			ProcessNextMaterialChunk(pModel, &currentChunk);
			break;

		case MAT_BUMPMAP:	// Filename da textura
			ProcessBumpMapChunk(pModel, &currentChunk);	
			break;

		case MAT_MAPNAME:	// Filename da textura
			currentChunk.bytesRead += fread(pModel->m_pMaterial[pModel->m_NumMaterials - 1].strDifuseTexFile, 1, currentChunk.length - currentChunk.bytesRead, m_pFile);
			break;
		
		default:			// Passar as chunks ignoradas
			currentChunk.bytesRead += fread(g_Buffer, 1, currentChunk.length - currentChunk.bytesRead, m_pFile);
			break;
		}

		pPreviousChunk->bytesRead += currentChunk.bytesRead;
	}
}

void Importer3DS::ReadChunk(tChunk *pChunk)
{
	// Chunk ID - 2 bytes
	pChunk->bytesRead = fread(&pChunk->ID, 1, 2, m_pFile);

	// Lenght - 4 bytes
	pChunk->bytesRead += fread(&pChunk->length, 1, 4, m_pFile);
}

int Importer3DS::GetString(char *pBuffer)
{
	int index = 0;

	fread(pBuffer, 1, 1, m_pFile);

	while (*(pBuffer + index++) != 0)
		fread(pBuffer + index, 1, 1, m_pFile);

	return strlen(pBuffer) + 1;
}

void Importer3DS::ReadColorChunk(float *pColor, tChunk *pChunk)
{
	tChunk tempChunk = {0};

	ReadChunk(&tempChunk);

	BYTE color[3];

	tempChunk.bytesRead += fread(color, 1, tempChunk.length - tempChunk.bytesRead, m_pFile);
	
	// Converter byte para float
	pColor[0] = (float)color[0] / 255.0f;
	pColor[1] = (float)color[1] / 255.0f;
	pColor[2] = (float)color[2] / 255.0f;

	pChunk->bytesRead += tempChunk.bytesRead;
}

void Importer3DS::ReadVertexIndices(Mesh *m_pMesh, tChunk *pPreviousChunk)
{
	unsigned short index = 0;

	pPreviousChunk->bytesRead += fread(&m_pMesh->m_NumFaces, 1, 2, m_pFile);

	m_pMesh->m_pFaces = new t_Face [m_pMesh->m_NumFaces];
	memset(m_pMesh->m_pFaces, 0, sizeof(t_Face) * m_pMesh->m_NumFaces);

	// The fourth value is a visibility flag for 3D Studio Max, we don't care about this.
	for(int i = 0; i < m_pMesh->m_NumFaces; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			pPreviousChunk->bytesRead += fread(&index, 1, sizeof(index), m_pFile);

			if(j < 3)
			{
				m_pMesh->m_pFaces[i].vertIndex[j] = index;
			}
		}
	}
}

void Importer3DS::ReadUVCoordinates(Mesh *m_pMesh, tChunk *pPreviousChunk)
{
	pPreviousChunk->bytesRead += fread(&m_pMesh->m_NumTexUV, 1, 2, m_pFile);

	m_pMesh->m_pTexVerts = new Vec2 [m_pMesh->m_NumTexUV];

	pPreviousChunk->bytesRead += fread(m_pMesh->m_pTexVerts, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_pFile);
}

void Importer3DS::ReadSmoothGroup(Mesh *m_pMesh, tChunk *pPreviousChunk)
{
	/*
	nfaces * 4bytes

	If read as long int, the nth bit indicate if the face belongs or not
	to the nth smoothing group.
	*/

	for(int i = 0; i < m_pMesh->m_NumFaces; i++)
	{
		pPreviousChunk->bytesRead += fread(&m_pMesh->m_pFaces[i].smoothGroup, 1, 4, m_pFile);
		//long a = m_pMesh->m_pFaces[i].smoothingGroup;
	}
}

void Importer3DS::ReadVertices(Mesh *m_pMesh, tChunk *pPreviousChunk)
{
	pPreviousChunk->bytesRead += fread(&(m_pMesh->m_NumVerts), 1, 2, m_pFile);

	m_pMesh->m_pVerts = new Vec3 [m_pMesh->m_NumVerts];
	memset(m_pMesh->m_pVerts, 0, sizeof(Vec3) * m_pMesh->m_NumVerts);

	pPreviousChunk->bytesRead += fread(m_pMesh->m_pVerts, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_pFile);

	// Now we should have all of the vertices read in.  Because 3D Studio Max
	// Models with the Z-Axis pointing up (strange and ugly I know!), we need
	// to flip the y values with the z values in our vertices.  That way it
	// will be normal, with Y pointing up.  If you prefer to work with Z pointing
	// up, then just delete this next loop.  Also, because we swap the Y and Z
	// we need to negate the Z to make it come out correctly.

	// Go through all of the vertices that we just read and swap the Y and Z values
	for(int i = 0; i < m_pMesh->m_NumVerts; i++)
	{
		// Store off the Y value
		float fTempY = m_pMesh->m_pVerts[i].y;

		// Set the Y value to the Z value
		m_pMesh->m_pVerts[i].y = m_pMesh->m_pVerts[i].z;

		// Set the Z value to the Y value, 
		// but negative Z because 3D Studio max does the opposite.
		m_pMesh->m_pVerts[i].z = -fTempY;
	}
}

//====================================================================================================
// Associar o material à mesh
//====================================================================================================

void Importer3DS::ReadObjectMaterial(Model *pModel, Mesh *m_pMesh, tChunk *pPreviousChunk)
{
	char strMaterial[255] = {0};

	pPreviousChunk->bytesRead += GetString(strMaterial);

	// Procurar um material correspondente
	for(int i = 0; i < pModel->m_NumMaterials; i++)
	{
		if(strcmp(strMaterial, pModel->m_pMaterial[i].strName) == 0)
		{
			m_pMesh->m_MaterialID = i;

			if(strlen(pModel->m_pMaterial[i].strDifuseTexFile) > 0)
				m_pMesh->m_bHasTexture = true;

			break;
		}
		else
		{
			m_pMesh->m_MaterialID = -1;
		}
	}

	// Shared vertices não interessam
	pPreviousChunk->bytesRead += fread(&g_Buffer, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_pFile);
}