/********************************************************************************
 *  FARSA                                                                       *
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

// #include "phyheightfield.h"
// #include "private/phyobjectprivate.h"
// #include "private/worldprivate.h"
//
// namespace farsa {
//
// PhyHeightField::PhyHeightField( real side_x, real side_y, World* w, QString name, const wMatrix& tm )
// 	: PhyObject( w, name, tm, false ) {
// 	sidex = side_x;
// 	sidey = side_y;
// 	w->pushObject( this );
// 	createPrivateObject();
// 	changedMatrix();
// }
//
// PhyHeightField::~PhyHeightField() {
// #ifdef WORLDSIM_USE_NEWTON
// 	/* nothing to do */
// #endif
// }
//
// void PhyHeightField::createPrivateObject() {
// #ifdef WORLDSIM_USE_NEWTON
// 	// creating for now a structure for creating a simple box with no elevations at all
// 	real cellSize = 0.25f; //0.01f;
// 	real elevationScaling = 0.001f;
// 	int xCells = ceil(sidex/cellSize);
// 	int yCells = ceil(sidey/cellSize);
// 	unsigned short* elevations = new uint16_t[xCells*yCells];
// 	char* attributes = new char[xCells*yCells];
// 	qDebug() << "HEIGHTFIELD" << xCells << yCells;
// 	for( int i=0; i<(xCells*yCells); i++ ) {
// 		elevations[i] = 0;
// 		attributes[i] = 1;
// 	}
// 	NewtonCollision* c = NewtonCreateHeightFieldCollision(worldpriv->world, xCells, yCells, 0, elevations, attributes, cellSize, elevationScaling, 0);
// 	wMatrix initialTransformationMatrix = wMatrix::identity(); // The transformation matrix is set in other places
// 	priv->body = NewtonCreateBody( worldpriv->world, c, &initialTransformationMatrix[0][0] );
// 	priv->collision = c;
//
// 	//NewtonBodySetAutoSleep( priv->body, 0 );
// 	//setMass( 1 );
// 	NewtonBodySetUserData( priv->body, this );
// 	//NewtonBodySetLinearDamping( priv->body, 0.0 );
// 	//wVector zero = wVector(0,0,0,0);
// 	//NewtonBodySetAngularDamping( priv->body, &zero[0] );
// 	//NewtonBodySetAutoSleep( priv->body, 0 );
// 	//NewtonBodySetFreezeState( priv->body, 0 );
// 	// Sets the signal-wrappers callback
// 	NewtonBodySetTransformCallback( priv->body, (PhyObjectPrivate::setTransformHandler) );
// 	//NewtonBodySetForceAndTorqueCallback( priv->body, (PhyObjectPrivate::applyForceAndTorqueHandler) );
//
// 	// storing information about the mesh of collision
// 	mesh = NewtonMeshCreateFromCollision(c);
// 	NewtonMeshTriangulate( mesh );
// #endif
// }
//
// void PhyHeightField::changedMatrix() {
// #ifdef WORLDSIM_USE_NEWTON
// 	//qDebug() << "SYNC POSITION" << tm[3][0] << tm[3][1] << tm[3][2];
// 	wMatrix mtr = tm.rotateAround( wVector(1,0,0), wVector(0,0,0), toRad(-90) );
// 	mtr.w_pos[0] -= sidex/2.0;
// 	mtr.w_pos[1] -= sidey/2.0;
// 	NewtonBodySetMatrix( priv->body, &mtr[0][0] );
// #endif
// }
//
// // class RenderPhyHeightField : public RenderWObject {
// // public:
// // 	RenderPhyHeightField( WObject* wobj, RenderWObjectContainer* container ) : RenderWObject( wobj, container ) {
// // 		hf = (PhyHeightField*)wobj;
// // 		dx = hf->sideX();
// // 		dy = hf->sideY();
// // 		dz = 0.01f;
// // 	};
// //     virtual ~RenderPhyHeightField() { };
// //     virtual void render( QGLContext* gw ) {
// // 		glPushMatrix();
// // 		container()->setupColorTexture( gw, this );
// // 		wMatrix m = hf->matrix().rotateAround( wVector(1,0,0), wVector(0,0,0), toRad(-90) );
// // 		m.w_pos[0] -= dx/2.0;
// // 		m.w_pos[1] -= dy/2.0;
// // 		GLMultMatrix(&m[0][0]);
// //
// // 		NewtonMesh* mesh = hf->mesh;
// // 		int stride = NewtonMeshGetVertexStrideInByte(mesh) / sizeof (double);
// // 		const double* const vertexList = NewtonMeshGetVertexArray(mesh);
// //
// // 		glBegin(GL_TRIANGLES);
// // 		for (void* face = NewtonMeshGetFirstFace(mesh); face; face = NewtonMeshGetNextFace(mesh, face)) {
// // 			if (!NewtonMeshIsFaceOpen (mesh, face)) {
// // 				int indices[1024];
// // 				int vertexCount = NewtonMeshGetFaceIndexCount (mesh, face);
// // 				Q_ASSERT (vertexCount < sizeof (indices)/sizeof (indices[0]));
// // 				NewtonMeshGetFaceIndices (mesh, face, indices);
// //
// // 				//dFloat64 normal[4];
// // 				//NewtonMeshCalculateFaceNormal (mesh, face, normal);
// // 				//glNormal3f(normal[0], normal[1], normal[2]);
// //
// // 				int i0 =  indices[0];
// // 				int i1 =  indices[1];
// // 				wVector p0 ( dFloat(vertexList[i0 * stride + 0]),
// // 							 dFloat(vertexList[i0 * stride + 1]),
// // 							 dFloat(vertexList[i0 * stride + 2]),
// // 							 0.0f );
// // 				wVector p1 ( dFloat(vertexList[i1 * stride + 0]),
// // 							 dFloat(vertexList[i1 * stride + 1]),
// // 							 dFloat(vertexList[i1 * stride + 2]),
// // 							 0.0f );
// // 				for (int i = 2; i < vertexCount; i++) {
// // 					int i2 = indices[i];
// // 					wVector p2 ( dFloat(vertexList[i2 * stride + 0]),
// // 								 dFloat(vertexList[i2 * stride + 1]),
// // 								 dFloat(vertexList[i2 * stride + 2]),
// // 								 0.0f );
// // 					glVertex3f(p0[0], p0[1], p0[2]);
// // 					glVertex3f(p1[0], p1[1], p1[2]);
// // 					glVertex3f(p2[0], p2[1], p2[2]);
// // 					p1 = p2;
// // 				}
// // 			}
// // 		}
// //
// // 		glEnd();
// //
// // 		glPopMatrix();
// // 	};
// // 	virtual void renderAABB( RenderWorld* gw ) {
// // 		wVector minpoint, maxpoint;
// // 		calculateAABB( minpoint, maxpoint, obj->matrix() );
// // 		gw->drawWireBox( minpoint, maxpoint );
// // 	};
// // 	virtual void calculateAABB( wVector& minPoint, wVector& maxPoint, const wMatrix tm ) {
// // 		real tdx = fabs(tm.x_ax[0]*dx) + fabs(tm.y_ax[0]*dy) + fabs(tm.z_ax[0]*dz);
// // 		real tdy = fabs(tm.x_ax[1]*dx) + fabs(tm.y_ax[1]*dy) + fabs(tm.z_ax[1]*dz);
// // 		real tdz = fabs(tm.x_ax[2]*dx) + fabs(tm.y_ax[2]*dy) + fabs(tm.z_ax[2]*dz);
// //
// // 		wVector hds( tdx/2.0, tdy/2.0, tdz/2.0 );
// // 		minPoint = tm.w_pos - hds;
// // 		maxPoint = tm.w_pos + hds;
// // 	};
// //
// // 	virtual void calculateOBB( wVector& dimension, wVector& minPoint, wVector& maxPoint ) {
// // 		dimension[0] = dx;
// // 		dimension[1] = dy;
// // 		dimension[2] = dz;
// // 		wVector hds( dx/2.0, dy/2.0, dz/2.0 );
// // 		minPoint = -hds;
// // 		maxPoint = +hds;
// // 	};
// //
// // protected:
// // 	PhyHeightField* hf;
// // 	real dx, dy, dz;
// // };
//
// } // end namespace farsa
