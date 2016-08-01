/********************************************************************************
 *  SALSA                                                                       *
 *  Copyright (C) 2007-2012                                                     *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
 *                                                                              *
 *  This program is free software; you can redistribute it and/or modify        *
 *  it under the terms of the GNU General Public License as published by        *
 *  the Free Software Foundation; either version 2 of the License, or           *
 *  (at your option) any later version.                                         *
 *                                                                              *
 *  This program is distributed in the hope that it will be useful,             *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 *  GNU General Public License for more details.                                *
 *                                                                              *
 *  You should have received a copy of the GNU General Public License           *
 *  along with this program; if not, write to the Free Software                 *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA  *
 ********************************************************************************/

// Thanks to Brett Porter and Mete Ciragan for help with this MS3D model loading code
// Thanks to Ronny A. Reierstad and Vadim Tikhanoff
// www.morrowland.com
// apron@morrowland.com

#include "wmesh.h"

//----------------------------------
//--- MS3D STRUCTURES --------------
// PACK_STRUCT : byte-align structures
#ifdef _MSC_VER
	#pragma pack( push, packing )
	#pragma pack( 1 )
	#define PACK_STRUCT
	#ifndef PATH_MAX
		#define PATH_MAX _MAX_PATH
	#endif
#elif defined( __GNUC__ )
	#define PACK_STRUCT __attribute__((packed))
	#include <limits.h>
#else
	#error you must byte-align these structures with the appropriate compiler directives
#endif

namespace salsa {

typedef unsigned char byte;
typedef unsigned short word;
// File Header
struct MS3DHeader {
	char m_ID[10];
	int m_version;
} PACK_STRUCT;
// Vertex info
struct MS3DVertex {
	byte m_flags;
	float m_vertex[3];
	char m_boneID;
	byte m_refCount;
} PACK_STRUCT;
// Triangle info
struct MS3DTriangle {
	word m_flags;
	word m_vertexIndices[3];
	float m_vertexNormals[3][3];
	float m_s[3], m_t[3];
	byte m_smoothingGroup;
	byte m_groupIndex;
} PACK_STRUCT;
// Material info
struct MS3DMaterial {
	static unsigned int Texture[15];
	char m_name[32];
	float m_ambient[4];
	float m_diffuse[4];
	float m_specular[4];
	float m_emissive[4];
	float m_shininess; // 0.0f - 128.0f
	float m_transparency; // 0.0f - 1.0f
	byte m_mode; // 0, 1, 2 (unused now)
	char m_texture[128];
	char m_alphamap[128];
} PACK_STRUCT;
// back to Default alignment
#ifdef _MSC_VER
	#pragma pack( pop, packing )
#endif
#undef PACK_STRUCT
//--- MS3D STRUCTURES ENDS ---------
//----------------------------------

} // end namespace salsa

#include <cstring>
#include <QString>
#include <QFile>
#include <QImage>
#include "salsaglutils.h"

namespace salsa {

WMesh::WMesh(World* world, SharedDataWrapper<Shared> shared, const wMatrix& disp, QString name, const wMatrix& tm)
	: OwnerFollower(world, shared, disp, name, tm)
	, m_shared(shared)
{
	// Nothing to do here, we initialize the shared data structure when a file is loaded
}

WMesh::~WMesh()
{
	// Nothing to do here
}

bool WMesh::loadMS3DModel(QString filename)
{
	QFile inputFile(filename);
	if (!inputFile.open(QIODevice::ReadOnly)) {
		return false;
	}

	QByteArray bBuffer = inputFile.readAll();
	const char *pPtr = bBuffer.data();
	MS3DHeader *pHeader = (MS3DHeader*) pPtr;
	pPtr += sizeof(MS3DHeader);

	if (strncmp(pHeader->m_ID, "MS3D000000", 10) != 0) {
		 // "Not a valid Milkshape3D model file."
		return false;
	}
	if (pHeader->m_version < 3) {
		 // "Unhandled file version. Only Milkshape3D Version 1.3 and 1.4 is supported."
		return false;
	}

	Shared* const d = m_shared.getModifiableShared();

	const int nVertices = *(word*) pPtr;
	d->vertices.resize(nVertices);
	pPtr += sizeof(word);
	for (int i = 0; i < nVertices; ++i) {
		MS3DVertex *pVertex = (MS3DVertex*)pPtr;
		d->vertices[i].boneID = pVertex->m_boneID;
		memcpy(d->vertices[i].location, pVertex->m_vertex, sizeof(float) * 3);
		pPtr += sizeof(MS3DVertex);
	}

	const int nTriangles = *(word*) pPtr;
	d->triangles.resize(nTriangles);
	pPtr += sizeof(word);
	for (int i = 0; i < nTriangles; ++i) {
		MS3DTriangle *pTriangle = (MS3DTriangle*) pPtr;
		int vertexIndices[3] = {pTriangle->m_vertexIndices[0], pTriangle->m_vertexIndices[1], pTriangle->m_vertexIndices[2]};
		float t[3] = {1.0f - pTriangle->m_t[0], 1.0f - pTriangle->m_t[1], 1.0f - pTriangle->m_t[2]};

		memcpy(d->triangles[i].vertexNormals, pTriangle->m_vertexNormals, sizeof(float) * 3 * 3 );
		memcpy(d->triangles[i].s, pTriangle->m_s, sizeof(float) * 3);
		memcpy(d->triangles[i].t, t, sizeof(float) * 3);
		memcpy(d->triangles[i].vertexIndices, vertexIndices, sizeof(int) * 3);
		pPtr += sizeof(MS3DTriangle);
	}

	const int nGroups = *(word*) pPtr;
	d->meshes.resize(nGroups);
	pPtr += sizeof(word);
	for (int i = 0; i < nGroups; ++i) {
		pPtr += sizeof(byte); // flags
		pPtr += 32; // name
		const word nTriangles = *(word*) pPtr;
		pPtr += sizeof(word);
		QVector<int> triangleIndices(nTriangles);
		for (int j = 0; j < nTriangles; ++j) {
			triangleIndices[j] = *(word*) pPtr;
			pPtr += sizeof(word);
		}
		const char materialIndex = *(char*) pPtr;
		pPtr += sizeof(char);
		d->meshes[i].materialIndex = materialIndex;
		d->meshes[i].triangleIndices = triangleIndices;
	}

	const int nMaterials = *(word*) pPtr;
	d->materials.resize(nMaterials);
	pPtr += sizeof(word);
	for (int i = 0; i < nMaterials; ++i) {
		MS3DMaterial *pMaterial = (MS3DMaterial*) pPtr;
		memcpy(d->materials[i].ambient, pMaterial->m_ambient, sizeof(float) * 4);
		memcpy(d->materials[i].diffuse, pMaterial->m_diffuse, sizeof(float) * 4 );
		memcpy(d->materials[i].specular, pMaterial->m_specular, sizeof(float) * 4);
		memcpy(d->materials[i].emissive, pMaterial->m_emissive, sizeof(float) * 4);
		d->materials[i].shininess = pMaterial->m_shininess;
		d->materials[i].pTextureFilename = QString(pMaterial->m_texture);
		pPtr += sizeof(MS3DMaterial);
	}

	return true;
}

RenderWMesh::RenderWMesh(const WMesh* entity)
	: RenderOwnerFollower(entity)
{
}

RenderWMesh::~RenderWMesh()
{
	// Nothing to do here
}

void RenderWMesh::render(const WMeshShared* sharedData, GLContextAndData* contextAndData)
{
	// Computing the final matrix and pushing it
	const wMatrix tm = finalMatrix(sharedData);
	glPushMatrix();
	GLMultMatrix(&tm[0][0]);

	foreach (const WMeshShared::Mesh& mesh, sharedData->meshes) {
		const int materialIndex = mesh.materialIndex;
		if (materialIndex >= 0) {
			const WMeshShared::Material& material = sharedData->materials[materialIndex];
			glMaterialfv(GL_FRONT, GL_AMBIENT,   material.ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE,   material.diffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR,  material.specular);
			glMaterialfv(GL_FRONT, GL_EMISSION,  material.emissive);
			glMaterialf( GL_FRONT, GL_SHININESS, material.shininess);
			setupColorTexture(sharedData, contextAndData);
		}
		glBegin(GL_TRIANGLES);
		foreach (int triangleIndex, mesh.triangleIndices) {
			const WMeshShared::Triangle& triangle = sharedData->triangles[triangleIndex];
			for (int k = 0; k < 3; ++k) {
				const int index = triangle.vertexIndices[k];
				glNormal3fv(triangle.vertexNormals[k]);
				glTexCoord2f(triangle.s[k], triangle.t[k]);
				glVertex3fv(sharedData->vertices[index].location);
			}
		}
		glEnd();
	}

	glPopMatrix();
}

} // end namespace salsa
