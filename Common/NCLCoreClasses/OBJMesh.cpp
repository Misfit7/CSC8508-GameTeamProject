#include "OBJMesh.h"
#include "Assets.h"
/*
OBJ files look generally something like this:

v xpos ypos zpos
..more vertices

vt xtex ytex
...more texcoords

vn xnorm ynorm znorm
...more normals

f vert index / tex index / norm index  vert index / tex index / norm index  vert index / tex index / norm index
...more faces

(i.e there's a set of float/float/float for each vertex of a face)

OBJ files can also be split up into a number of submeshes, making loading them
in even more annoying. 
*/
using namespace NCL::Rendering;

bool OBJMesh::LoadOBJMesh(std::string filename)	{
	std::ifstream f(Assets::MESHDIR + filename.c_str(),std::ios::in);

	if(!f) { return false; }

	//Stores the loaded in vertex attributes
	std::vector<Vector2>inputTexCoords;
	std::vector<Vector3>inputVertices, inputNormals;

	//SubMeshes temporarily get kept in here
	std::vector<OBJSubMesh*> inputSubMeshes;

	OBJSubMesh* currentMesh = new OBJSubMesh();
	inputSubMeshes.push_back(currentMesh);	//It's safe to assume our OBJ will have a mesh in it ;)

	string currentMtlLib;
	string currentMtlType;

	while(!f.eof()) {
		std::string currentLine;
		f >> currentLine;

		if(currentLine == OBJCOMMENT) { 
			continue; 
		}
		else if(currentLine == OBJMTLLIB) { 
			f >> currentMtlLib; 
		}
		else if(currentLine == OBJUSEMTL) {
			currentMesh = new OBJSubMesh();
			inputSubMeshes.push_back(currentMesh);

			currentMesh->mtlSrc = currentMtlLib;

			f >> currentMtlType;

			currentMesh->mtlType = currentMtlType;
		}
		else if(currentLine == OBJMESH || currentLine == OBJOBJECT) {	//This line is a submesh!
		//	currentMesh = new OBJSubMesh();
		//	inputSubMeshes.push_back(currentMesh);
		//	
		//	//f >> currentMtlType;
		//	currentMesh->mtlSrc = currentMtlLib;
		//	currentMesh->mtlType = currentMtlType;
		}
		else if(currentLine == OBJVERT) {	//This line is a vertex
			Vector3 vertex;
			f >> vertex.x; f >> vertex.y; f >> vertex.z;
			inputVertices.push_back(vertex);
		}
		else if(currentLine == OBJNORM) {	//This line is a Normal!
			Vector3 normal;
			f >> normal.x; f >> normal.y; f >> normal.z;
			inputNormals.push_back(normal);
		}
		else if(currentLine == OBJTEX) {	//This line is a texture coordinate!
			Vector2 texCoord;
			f >> texCoord.x; f >> texCoord.y;
			/*
			TODO! Some OBJ files might have 3D tex coords...
			*/
			inputTexCoords.push_back(texCoord);
		}
		else if(currentLine == OBJFACE) {	//This is an object face!
			if(!currentMesh) {
				inputSubMeshes.push_back(new OBJSubMesh());
				currentMesh = inputSubMeshes[inputSubMeshes.size()-1];
			}

			std::string			faceData;		//Keep the entire line in this!
			std::vector<int>	faceIndices;	//Keep the extracted indices in here!
		
			getline(f,faceData);		//Use a string helper function to read in the entire face line

			/*
			It's possible an OBJ might have normals defined, but not tex coords!
			Such files should then have a face which looks like this:

				f <vertex index>//<normal index>
				instead of <vertex index>/<tex coord>/<normal index>

				you can be some OBJ files will have "/ /" instead of "//" though... :(
			*/
			bool	skipTex = false;
			if(faceData.find("//") != std::string::npos) {	
				skipTex = true;
			}

			/*
			Count the number of slashes, but also convert the slashes to spaces
			so that string streaming of ints doesn't fail on the slash

				"f  0/0/0" becomes "f 0 0 0" etc
			*/
			for(size_t i = 0; i < faceData.length(); ++i) {
				if(faceData[i] == '/') {
					faceData[i] = ' ';
				}
			}

			/*
			Now string stream the indices from the string into a temporary
			vector.
			*/
			int tempIndex;
			std::stringstream	ss(faceData);
			while(ss >> tempIndex) {
				faceIndices.push_back(tempIndex);
			}

			//If the face indices vector is a multiple of 3, we're looking at triangles
			//with some combination of vertices, normals, texCoords
			if(faceIndices.size()%3 == 0) {		//This face is a triangle (probably)!
				if(faceIndices.size() == 3) {	//This face has only vertex information;
					currentMesh->vertIndices.push_back(faceIndices.at(0));
					currentMesh->vertIndices.push_back(faceIndices.at(1));
					currentMesh->vertIndices.push_back(faceIndices.at(2));
				}
				else if(faceIndices.size() == 9) {	//This face has vertex, normal and tex information!
					for(int i = 0; i < 9; i += 3) {
						currentMesh->vertIndices.push_back(faceIndices.at(i));
						currentMesh->texIndices.push_back(faceIndices.at(i+1));
						currentMesh->normIndices.push_back(faceIndices.at(i+2));
					}
				}
				else if(faceIndices.size() == 6) {	//This face has vertex, and one other index...
					for(int i = 0; i < 6; i += 2) {
						currentMesh->vertIndices.push_back(faceIndices.at(i));
						if(!skipTex) {		// a double slash means it's skipping tex info...
							currentMesh->texIndices.push_back(faceIndices.at(i+1));
						}
						else{
							currentMesh->normIndices.push_back(faceIndices.at(i+1));
						}
					}
				}
				else if (faceIndices.size() == 12) {	//This face has vertex, normal and tex information!
					
					for (int i = 0; i < 12; i += 3) {
						currentMesh->vertIndices.push_back(faceIndices.at(i));
						currentMesh->texIndices.push_back(faceIndices.at(i + 1));
						currentMesh->normIndices.push_back(faceIndices.at(i + 2));
					}
				}
			}
			else {	
				//Uh oh! Face isn't a triangle. Have fun adding stuff to this ;)
				bool a = true;
			}
		}
		else{
			std::cout << "OBJMesh::LoadOBJMesh Unknown file data:" << currentLine << std::endl;
		}
	}

	f.close();

	tempSubMeshes  = inputSubMeshes;
	tempVertices   = inputVertices;
	tempNormals    = inputNormals;
	tempTexCoords  = inputTexCoords;

	return true;
}