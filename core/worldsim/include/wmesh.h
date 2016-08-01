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

#ifndef WMESH_H
#define WMESH_H

#include "worldsimconfig.h"
#include "world.h"
#include "ownerfollower.h"
#include <QVector>

namespace salsa {

class RenderWMesh;

/**
 * \brief The shared data for the WMesh
 */
class SALSA_WSIM_TEMPLATE WMeshShared : public OwnerFollowerShared
{
public:
	/**
	 * \brief The structure for a single mesh
	 */
	struct Mesh {
		int materialIndex;
		int numTriangles;
		QVector<int> triangleIndices;
	};

	/**
	 * \brief The structure with material properties
	 */
	struct Material {
		float ambient[4], diffuse[4], specular[4], emissive[4];
		float shininess;
		QString pTextureFilename;
	};

	/**
	 * \brief The structure with information about a triangle of the mesh
	 */
	struct Triangle {
		float vertexNormals[3][3];
		float s[3], t[3];
		int vertexIndices[3];
	};

	/**
	 * \brief The structure with information about a single vertex
	 */
	struct Vertex {
		char boneID;
		float location[3];
	};

public:
	/**
	 * \brief Constructor
	 *
	 * This initializes all fields to the default value
	 */
	WMeshShared()
		: OwnerFollowerShared()
		, meshes()
		, materials()
		, triangles()
		, vertices()
	{
	}

	/**
	 * \brief The vector with all the meshes
	 */
	QVector<Mesh> meshes;

	/**
	 * \brief The vector with all the materials
	 */
	QVector<Material> materials;

	/**
	 * \brief The vector with all the triangles
	 */
	QVector<Triangle> triangles;

	/**
	 * \brief The vector with all the vertices
	 */
	QVector<Vertex> vertices;
};

/**
 * \brief A purely graphical 3D mesh
 *
 * WMesh represents a 3D shape using a triangle mesh. It is just for graphical
 * purpose, but it is a subclass of OwnerFollower and it is contained into a
 * World to share it amongs WCameras and to attach it to other object (like
 * iCub's cover)
 */
class SALSA_WSIM_API WMesh : public OwnerFollower
{
public:
	/**
	 * \brief The class with shared data
	 */
	typedef WMeshShared Shared;

	/**
	 * \brief The class for rendering
	 */
	typedef RenderWMesh Renderer;

protected:
	/**
	 * \brief Constructor
	 *
	 * \param world the world where object lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               object has been deleted)
	 * \param disp the displacement matrix
	 * \param name the name of the object
	 * \param tm rotation and position in global coordinate frame
	 */
	WMesh(World* world, SharedDataWrapper<Shared> shared, const wMatrix& disp = wMatrix::identity(), QString name = "unamed", const wMatrix& tm = wMatrix::identity());


	/**
	 * \brief Destructor
	 */
	virtual ~WMesh();

	/**
	 * \brief Loads the mesh from a MS3D file (MilkShape-3D)
	 *
	 * \param filename the file from which the mesh is loaded
	 * \return false in case of errors, true otherwise
	 */
	bool loadMS3DModel(QString filename);

private:
	/**
	 * \brief The pointer to the object with shared data
	 */
	SharedDataWrapper<Shared> m_shared;

	/**
	 * \brief World is friend to be able to create and destroy this object
	 */
	friend class World;
};

/**
 * \brief The class rendering the WMesh
 *
 * We only implement the render function, we don't provide the rendering of the
 * AABB
 */
class SALSA_WSIM_API RenderWMesh : public RenderOwnerFollower
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param entity the entity we render. YOU MUST NOT KEEP A REFERENCE TO
	 *               IT!!! This is only passed in case you need to do custom
	 *               initializations, the render() function will be passed
	 *               updated data at each call externally
	 */
	RenderWMesh(const WMesh* entity);

	/**
	 * \brief Destructor
	 *
	 * Here just to declare it as virtual
	 */
	virtual ~RenderWMesh();

	/**
	 * \brief The function performing the rendering
	 *
	 * \param sharedData the object with data from WObject to use for
	 *                   rendering
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	void render(const WMeshShared* sharedData, GLContextAndData* contextAndData);
};

} // end namespace salsa

#endif
