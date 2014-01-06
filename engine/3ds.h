#ifndef _3DS_H
#define _3DS_H

#include "Model.h"

//>------ Primary Chunk, at the beginning of each file
#define M3DMAGIC       0x4D4D

//>------ Main Chunks
#define OBJECTINFO    0x3D3D				// This gives the version of the mesh and is found right before the material and object information
#define VERSION       0x0002				// This gives the version of the .3ds file
#define EDITKEYFRAME  0xB000				// This is the header for all of the key frame info

//>------ sub defines of OBJECTINFO
#define MATERIAL	  0xAFFF				// This stored the texture info
#define OBJECT		  0x4000				// This stores the faces, vertices, etc...

//>------ sub defines of MATERIAL
#define MAT_NAME       0xA000				// This holds the material name
//#define MATDIFFUSE    0xA020				// This holds the color of the object/material
#define MAT_AMBIENT		0xA010		//followed by color chunk  
#define MAT_DIFFUSE		0xA020		//followed by color chunk  
#define MAT_SPECULAR	0xA030		//followed by color chunk  
#define MAT_SHININESS	0xA040		//followed by percentage chunk  

#define MAT_TEXMAP      0xA200				// This is a header for a new material

#define MAT_BUMPMAP		0xA230   

#define MAT_MAPNAME    0xA300				// This holds the file name of the texture

#define OBJECT_MESH   0x4100				// This lets us know that we are reading a new object

//>------ sub defines of OBJECT_MESH
#define OBJECT_VERTICES     0x4110			// The objects vertices
#define OBJECT_FACES		0x4120			// The objects faces
#define OBJECT_MATERIAL		0x4130			// This is found if the object has a material, either texture map or color
#define OBJECT_UV			0x4140			// The UV texture coordinates
#define SMOOTH_GROUP		0x4150

// Here is our structure for our 3DS indicies (since .3DS stores 4 unsigned shorts)
struct tIndices {							

	unsigned short a, b, c, bVisible;		// This will hold point1, 2, and 3 index's into the vertex array plus a visible flag
};

// This holds the chunk info
struct tChunk
{
	unsigned short int ID;					// The chunk's ID		
	unsigned int length;					// The length of the chunk
	unsigned int bytesRead;					// The amount of bytes read within that chunk
};

// This class handles all of the loading code
class Importer3DS
{
public:
	Importer3DS();								// This inits the data members

	// This is the function that you call to Load the 3DS
	bool Import3DS(Model *pModel, char *strFileName);

private:
	// This reads in a string and saves it in the char array passed in
	int GetString(char *);

	// This reads the next chunk
	void ReadChunk(tChunk *);

	// This reads the next large chunk
	void ProcessNextChunk(Model *pModel, tChunk *);

	// This reads the object chunks
	void ProcessNextObjectChunk(Model *pModel, Mesh *m_pMesh, tChunk *);

	void ProcessBumpMapChunk(Model *pModel, tChunk *);

	// This reads the material chunks
	void ProcessNextMaterialChunk(Model *pModel, tChunk *);

	// This reads the RGB value for the object's color
	void ReadColorChunk(float *pColor, tChunk *pChunk);

	// This reads the objects vertices
	void ReadVertices(Mesh *m_pMesh, tChunk *);

	// This reads the objects face information
	void ReadVertexIndices(Mesh *m_pMesh, tChunk *);

	// This reads the texture coodinates of the object
	void ReadUVCoordinates(Mesh *m_pMesh, tChunk *);

	void ReadSmoothGroup(Mesh *m_pMesh, tChunk *);

	// This reads in the material name assigned to the object and sets the m_MaterialID
	void ReadObjectMaterial(Model *pModel, Mesh *m_pMesh, tChunk *pPreviousChunk);
	
	// This computes the vertex normals for the object (used for lighting)
	void ComputeNormals(Model *pModel);

	// This frees memory and closes the file
	void CleanUp();
	
	// The file pointer
	FILE *m_pFile;
};


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
// 
// This file is created in the hopes that you can just plug it into your code
// easily.  You will probably want to query more chunks though for animation, etc..
//
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
//