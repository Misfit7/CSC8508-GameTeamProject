/******************************************************************************
Class:OBJMesh
Implements:Mesh, MD5MeshInstance
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description: Implementation of the Wavefront OBJ mesh format. 

This was going to be a full tutorial, and then I implemented it and was
reminded of how horrific OBJ files can be, so just dumped the code in the 
framework, instead. 

You'll very quickly find OBJ meshes that can't be loaded by this loader.

First up - any mesh with anything other than triangles in it won't work. Quads
should be trivial to add in, n-sided polygons should be OK if they are convex
(add a point in the middle and create triangles out of pairs of points).

Some OBJ meshes use negative indices, which won't work.

In both cases, loading the OBJ into Blender or maybe Milkshape, and exporting
them out as OBJs again might create a file more likely to load. 

OBJ files are ok for simple geometry (the tutorial series uses them for 2
slightly different cubes, and an icosphere), but generally don't work very
well for 'big' geometry. 

The simple loader doesn't do anything fancy with indices - have fun trying
to get that working!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
#include "common.h"

#pragma once
#include "Vector3.h"
#include "Vector2.h"
#include "Mesh.h"
#include "ChildMeshInterface.h"
#include "glad/gl.h"
#include <fstream>
#include <string>
#include <sstream>
#include <map>
using std::string;
using std::map;
namespace NCL::Rendering {
#define OBJOBJECT		"o"	//the current line of the obj file defines the start of a new material
#define OBJMTLLIB		"mtllib"
#define OBJUSEMTL		"usemtl"	//the current line of the obj file defines the start of a new material
#define OBJMESH			"g"			//the current line of the obj file defines the start of a new face
#define OBJCOMMENT		"#"			//The current line of the obj file is a comment
#define OBJVERT			"v"			//the current line of the obj file defines a vertex
#define OBJTEX			"vt"		//the current line of the obj file defines texture coordinates
#define OBJNORM			"vn"		//the current line of the obj file defines a normal
#define OBJFACE			"f"			//the current line of the obj file defines a face

#define MTLNEW			"newmtl"
#define MTLDIFFUSE		"Kd"
#define MTLSPEC			"Ks"
#define MTLSPECWT		"Ns"
#define MTLTRANS		"d"
#define MTLTRANSALT		"Tr"
#define MTLILLUM		"illum"
#define MTLDIFFUSEMAP	"map_Kd"
#define MTLBUMPMAP		"map_bump"
#define MTLBUMPMAPALT	"bump"

	/*
	OBJSubMesh structs are used to temporarily keep the data loaded
	in from the OBJ files, before being parsed into a series of
	Meshes
	*/
	struct OBJSubMesh {
		std::vector<int> texIndices;
		std::vector<int> vertIndices;
		std::vector<int> normIndices;

		int indexOffset;
		string mtlType;
		string mtlSrc;
	};

	class OBJMesh : public Mesh, public ChildMeshInterface {
	public:
		OBJMesh(void) {};
		OBJMesh(std::string filename) { LoadOBJMesh(filename); }

		~OBJMesh(void) {};

		bool LoadOBJMesh(std::string filename);
		//virtual void OBJDraw();
		void UploadToGPU(Rendering::RendererBase* renderer = nullptr) {}

		std::vector<OBJSubMesh*> GetTempSM() {
			return tempSubMeshes;
		}
		std::vector<Vector3> GetTempV(){
			return tempVertices;
		}
		std::vector<Vector3> GetTempN() {
			return tempNormals;
		}
		std::vector<Vector2> GetTempTC() {
			return tempTexCoords;
		};

		GLuint GetOBJTexture() const {
			return OBJTexture;
		};

	protected:
		GLuint OBJTexture;
		std::vector<OBJSubMesh*> tempSubMeshes;
		std::vector<Vector3> tempVertices;
		std::vector<Vector3> tempNormals;
		std::vector<Vector2> tempTexCoords;
	};
}