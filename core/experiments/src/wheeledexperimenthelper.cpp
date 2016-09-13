/********************************************************************************
 *  SALSA Experiments Library                                                   *
 *  Copyright (C) 2007-2012                                                     *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
 *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
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

#include "wheeledexperimenthelper.h"
#include "logger.h"
#include "phybox.h"
#include "phycylinder.h"
#include "robots.h"
#include "arena.h"
#include "intervals.h"

namespace salsa {

// This anonymous namespace contains helper functions used in this file
namespace {
	/**
	 * \brief Computes the position of a vertex of a wall in the global
	 *        frame of reference
	 *
	 * This assumes the local Z axis of the wall goes from the ground up.
	 * This function is used by the constructor of Wall2DWrapper
	 * \param wall the wall object
	 * \param vertexID the id of the
	 * \return the coordinates of the vertex in the global frame of
	 *         reference
	 * \internal
	 */
	wVector computeWallVertex(PhyBox* wall, unsigned int vertexID)
	{
		const wVector centerOnPlane = wall->matrix().w_pos - wall->matrix().z_ax.scale(wall->sideZ() / 2.0);
		const wVector halfSide1Vector = wall->matrix().x_ax.scale(wall->sideX() / 2.0);
		const wVector halfSide2Vector = wall->matrix().y_ax.scale(wall->sideY() / 2.0);

		wVector vertex;
		switch(vertexID % 4) {
			case 0:
				vertex = centerOnPlane + halfSide1Vector + halfSide2Vector;
				break;
			case 1:
				vertex = centerOnPlane + halfSide1Vector - halfSide2Vector;
				break;
			case 2:
				vertex = centerOnPlane - halfSide1Vector + halfSide2Vector;
				break;
			case 3:
				vertex = centerOnPlane - halfSide1Vector - halfSide2Vector;
				break;
			default:
				break;
		}

		return vertex;
	}


	/**
	 * \brief Returns the angle of the vector with the X axis of the given
	 *        matrix
	 *
	 * The angle is in the range [-pi, pi]
	 * \param mtr the matrix
	 * \param v the vector
	 * \return the angle between v and the X axis of the given matrix
	 */
	double getAngleWithXAxis(const wMatrix& mtr, const wVector& v)
	{
		SALSA_DEBUG_TEST_INVALID(mtr.x_ax.x) SALSA_DEBUG_TEST_INVALID(mtr.x_ax.y) SALSA_DEBUG_TEST_INVALID(mtr.x_ax.z)
		SALSA_DEBUG_TEST_INVALID(v.x) SALSA_DEBUG_TEST_INVALID(v.y) SALSA_DEBUG_TEST_INVALID(v.z)

		// Normalizing v
		const wVector vdir = v.scale(1.0 / v.norm()); SALSA_DEBUG_TEST_INVALID(vdir.x) SALSA_DEBUG_TEST_INVALID(vdir.y) SALSA_DEBUG_TEST_INVALID(vdir.z)

		// To get the angle (unsigned), computing the acos of the dot product of the two vectors. We have to
		// constrain the cross product between -1 and 1 because sometimes it can have values outside the range
		const double crossProduct = min(1.0, max(-1.0, mtr.x_ax % vdir)); SALSA_DEBUG_TEST_INVALID(crossProduct)
		const double unsignedAngle = acos(crossProduct); SALSA_DEBUG_TEST_INVALID(unsignedAngle)

		// Now choosing the right sign. To do this we first compute the cross product of the x axis and
		// the vector direction, then we see if it has the same direction of Z or not
		const double s = mtr.z_ax % (mtr.x_ax * vdir); SALSA_DEBUG_TEST_INVALID(s)

		return (s < 0.0) ? -unsignedAngle : unsignedAngle;
	}

	/**
	 * \brief Computes the portion of a linear the view field occupied by a
	 *        circle
	 *
	 * This function returns the angular range occupied by a cicle on a
	 * linear camera described by the given matrix. The Z axis of the matrix
	 * is the upvector, the camera lies on the XY plane and points towars
	 * the X axis (i.e. angles start from the X axis). The circle must lie
	 * on the ZY plane of its local frame of reference and the X axis should
	 * point upward and be coincident with the plane Z axis. Moreover the
	 * angle for colors must be considered from the local Y axis and
	 * counterclockwise when seen from the tip of the local X axis (just
	 * like the colors in cylinders). If the object is not visible  by the
	 * camera, this function returns a negative distance. The min angle can
	 * be greater than the max angle if the object is behind the camera. The
	 * matrix representing the camera is translated so that it is on the
	 * same plane as the cirlce before doing computations.
	 * \param cameraMtr the matrix describing the camera (see function
	 *                  description)
	 * \param objMtr the matrix of the circle
	 * \param radius the radius of the circle
	 * \param segmentsColorBegin the iterator the the beginning of the list
	 *                           of colors of segments of the circle.
	 *                           Segments must be sorted by ascending order
	 *                           and agles must be between -PI_GRECO and
	 *                           PI_GRECO (these conditions are guaranteed
	 *                           if the iterator is obtained through
	 *                           PhyCylinder::segmentsColor())
	 * \param segmentsColorEnd the iterator to the end of the list of colors
	 *                         of segments of the circle
         * \param rangesAndColors the array that will contain the camera
	 *                        activation
	 * \param distance this is set to the distance of the object from the
	 *                 camera. It is negative if the object is not visible
	 *                 by the camera
	 * \param maxDistance this is the maximum distance the camera can see.
	 *                    If the object is farther than this distance the
	 *                    function can safely return a negative distance
	 *                    (i.e. the object is not seen) and stop any other
	 *                    computation
	 */
	template <class SegmentColorIt>
	void computeLinearViewFieldOccupiedRangeForCircle(const wMatrix& cameraMtr, const wMatrix& objMtr, double radius, const SegmentColorIt segmentsColorBegin, const SegmentColorIt segmentsColorEnd, QVector<PhyObject2DWrapper::AngularRangeAndColor>& rangesAndColors, double& distance, double maxDistance)
	{
		// Useful constant
		const real epsilon = 0.0001f;

		// We have to translate the camera to lie on the same plane of the cylinder base. We translate it along
		// its local upvector (Z axis) until it reaches the  plane containing the base of the cylinder. Of course
		// this only works if the camera Z axis is not paraller to the plane with the base of the cylinder. In
		// that case all computations would be invalid, so we don't do anything
		wMatrix mtr = cameraMtr;
		if (fabs(mtr.z_ax % objMtr.x_ax) < epsilon) {
			distance = -1.0;
			return;
		}
		mtr.w_pos = mtr.w_pos + mtr.z_ax.scale((objMtr.w_pos.z - mtr.w_pos.z) / mtr.z_ax.z);  SALSA_DEBUG_TEST_INVALID(mtr.w_pos.x) SALSA_DEBUG_TEST_INVALID(mtr.w_pos.y) SALSA_DEBUG_TEST_INVALID(mtr.w_pos.z) SALSA_DEBUG_TEST_INVALID(mtr.z_ax.x) SALSA_DEBUG_TEST_INVALID(mtr.z_ax.y) SALSA_DEBUG_TEST_INVALID(mtr.z_ax.z) SALSA_DEBUG_TEST_INVALID(objMtr.w_pos.x) SALSA_DEBUG_TEST_INVALID(objMtr.w_pos.y) SALSA_DEBUG_TEST_INVALID(objMtr.w_pos.z)

		// First of all we have to calculate the distance between the center of the camera and the center of the
		// cylinder. Here we also check that the camera is not inside the cylinder. The vector from the camera
		// to the center of the cylinder is computed in the object frame of reference because we need it in this
		// frame of reference later
		const wVector centerDir = objMtr.unrotateVector(mtr.w_pos - objMtr.w_pos);
		const double centerDistance = centerDir.norm();
		distance = centerDistance - radius;
		if ((distance < 0.0) || (distance > maxDistance)) {
			distance = -1.0;
			return;
		}

		// We also need to compute the angles of the two tangent points to compute the ranges for
		// various colors. The angles are relative to the cylinder center. The first thing we need
		// is the angle between the vector from the camera to center of the cylinder and the radius
		// perpendicular to the tangent
		const double halfSectorAngle = acos(radius / centerDistance); SALSA_DEBUG_TEST_INVALID(halfSectorAngle)
		// Now computing the starting and ending angle in the cylinder frame of reference. We need
		// the angle of the centerDir vector, then we only have to subtract halfSectorAngle to get
		// starting angle. The end angle is easy to obtain, then
		const real startAngleInCylinder = normalizeRad(atan2(centerDir.z, centerDir.y) - halfSectorAngle);
		const real endAngleInCylinder = normalizeRad(startAngleInCylinder + 2.0 * halfSectorAngle);

		// Clearing the vector that will contain segments and colors. It will first contain ranges in the
		// cylinder frame of reference, and then the angles will be converted to linear camera ranges
		rangesAndColors.clear();
		if ((segmentsColorBegin + 1) == segmentsColorEnd) {
			// There is only one color, we can simply add one element to the rangesAndColors vector
			rangesAndColors.append(PhyObject2DWrapper::AngularRangeAndColor(startAngleInCylinder, endAngleInCylinder, segmentsColorBegin->color));
		} else {
			// The interval modelling the visible arc, with which all other arcs are intersected. If
			// the arc crosses -pi/pi, the interval would go from -inf to end and from start to inf,
			// so we restrict it to -pi, pi
			const Intervals visibleArc = Intervals(SimpleInterval(startAngleInCylinder, endAngleInCylinder)) & SimpleInterval(-PI_GRECO, PI_GRECO);

			// This could perhaps be made a bit more efficient by checking if the we already analyzed
			// all segments in the range, but it would be necessary only if we had objects with a lot
			// of segments
			for (SegmentColorIt it = segmentsColorBegin; it != segmentsColorEnd; ++it) {
				// Computing the intersection between the current segment and the visible range and adding it. We don't
				// need to intersect the cur segment with [-pi, pi] as we did for visibleArc because the intersection
				// with the latter will remove all values below -pi or above pi
				const Intervals curVisibleSegmentArc = visibleArc & it->intervals;
				if (!curVisibleSegmentArc.isEmpty()) {
					for (Intervals::const_iterator intrvIt = curVisibleSegmentArc.begin(); intrvIt != curVisibleSegmentArc.end(); ++intrvIt) {
						rangesAndColors.append(PhyObject2DWrapper::AngularRangeAndColor(intrvIt->start, intrvIt->end, it->color));
					}
				}
			}
		}

		// Now converting angles to linear camera ranges
		for (int i = 0; i < rangesAndColors.size(); i++) {
			// Getting the points corresponding to the angles in the current range in the frame of reference
			// of the cylinder
			const wVector startPointCylinder(0.0, radius * cos(rangesAndColors[i].minAngle), radius * sin(rangesAndColors[i].minAngle)); SALSA_DEBUG_TEST_INVALID(startPointCylinder.x) SALSA_DEBUG_TEST_INVALID(startPointCylinder.y) SALSA_DEBUG_TEST_INVALID(startPointCylinder.z)
			const wVector endPointCylinder(0.0, radius * cos(rangesAndColors[i].maxAngle), radius * sin(rangesAndColors[i].maxAngle)); SALSA_DEBUG_TEST_INVALID(endPointCylinder.x) SALSA_DEBUG_TEST_INVALID(endPointCylinder.y) SALSA_DEBUG_TEST_INVALID(endPointCylinder.z)

			// Now computing the points in the global frame of reference
			const wVector startPoint = objMtr.transformVector(startPointCylinder); SALSA_DEBUG_TEST_INVALID(startPoint.x) SALSA_DEBUG_TEST_INVALID(startPoint.y) SALSA_DEBUG_TEST_INVALID(startPoint.z)
			const wVector endPoint = objMtr.transformVector(endPointCylinder); SALSA_DEBUG_TEST_INVALID(endPoint.x) SALSA_DEBUG_TEST_INVALID(endPoint.y) SALSA_DEBUG_TEST_INVALID(endPoint.z)

			// Now computing the angles in the linear camera. As we don't know which is the start angle and which
			// the end angle, we rely on the fact that for cylinders the camera cannot see a portion greater than
			// PI_GRECO. We also check that the vectors to start and end point are not zero; if they are the angle is
			// computed with respect to the center of the other cylinder
			const wVector firstAngleVector = startPoint - mtr.w_pos;
			const double firstAngleCamera = (fabs(firstAngleVector.norm()) < epsilon) ? getAngleWithXAxis(mtr, objMtr.w_pos - mtr.w_pos) : getAngleWithXAxis(mtr, firstAngleVector); SALSA_DEBUG_TEST_INVALID(firstAngleCamera)
			const wVector secondAngleVector = endPoint - mtr.w_pos;
			const double secondAngleCamera = (fabs(secondAngleVector.norm()) < epsilon) ? getAngleWithXAxis(mtr, objMtr.w_pos - mtr.w_pos) : getAngleWithXAxis(mtr, secondAngleVector); SALSA_DEBUG_TEST_INVALID(secondAngleCamera)
			if (firstAngleCamera > secondAngleCamera) {
				// Checking if they are on different sides of the -PI_GRECO/PI_GRECO boundary or on the same side. Here we exploit
				// the fact that the camera cannot see more than PI_GRECO of the cylinder
				if ((firstAngleCamera > (PI_GRECO / 2.0)) && (secondAngleCamera < (-PI_GRECO / 2.0))) {
					rangesAndColors[i].minAngle = firstAngleCamera;
					rangesAndColors[i].maxAngle = secondAngleCamera;
				} else {
					rangesAndColors[i].minAngle = secondAngleCamera;
					rangesAndColors[i].maxAngle = firstAngleCamera;
				}
			} else {
				// Checking if they are on different sides of the -PI_GRECO/PI_GRECO boundary or on the same side. Here we exploit
				// the fact that the camera cannot see more than PI_GRECO of the cylinder
				if ((firstAngleCamera < (-PI_GRECO / 2.0)) && (secondAngleCamera > (PI_GRECO / 2.0))) {
					rangesAndColors[i].minAngle = secondAngleCamera;
					rangesAndColors[i].maxAngle = firstAngleCamera;
				} else {
					rangesAndColors[i].minAngle = firstAngleCamera;
					rangesAndColors[i].maxAngle = secondAngleCamera;
				}
			}
		}
	}
// 	Old implementation of the function above, kept for a while for reference
// 	void computeLinearViewFieldOccupiedRangeForCircle(const wMatrix& cameraMtr, const wMatrix& objMtr, double radius, const QList<PhyCylinder::SegmentColor>& segmentsColor, QVector<PhyObject2DWrapper::AngularRangeAndColor>& rangesAndColors, double& distance, double maxDistance)
// 	{
// 		// We have to translate the camera to lie on the same plane of the cylinder base. We translate it along
// 		// its local upvector (Z axis) until it reaches the  plane containing the base of the cylinder. Of course
// 		// this only works if the camera Z axis is not paraller to the plane with the base of the cylinder. In
// 		// that case all computations would be invalid, so we don't do anything
// 		wMatrix mtr = cameraMtr;
// 		if (fabs(mtr.z_ax % objMtr.x_ax) < 0.0001) {
// 			distance = -1.0;
// 			return;
// 		}
// 		mtr.w_pos = mtr.w_pos + mtr.z_ax.scale((objMtr.w_pos.z - mtr.w_pos.z) / mtr.z_ax.z);
//
// 		// First of all we have to calculate the distance between the center of the camera and the center of the
// 		// cylinder. Here we also check that the camera is not inside the cylinder. The vector from the camera
// 		// to the center of the cylinder is computed in the object frame of reference because we need it in this
// 		// frame of reference later
// 		const wVector centerDir = objMtr.unrotateVector(mtr.w_pos - objMtr.w_pos);
// 		const double centerDistance = centerDir.norm();
// 		distance = centerDistance - radius;
// 		if ((distance < 0.0) || (distance > maxDistance)) {
// 			distance = -1.0;
// 			return;
// 		}
//
// 		// We also need to compute the angles of the two tangent points to compute the ranges for
// 		// various colors. The angles are relative to the cylinder center. The first thing we need
// 		// is the angle between the vector from the camera to center of the cylinder and the radius
// 		// perpendicular to the tangent
// 		const double halfSectorAngle = acos(radius / centerDistance);
// 		// Now computing the starting and ending angle in the cylinder frame of reference. We need
// 		// the angle of the centerDir vector, then we only have to subtract halfSectorAngle to get
// 		// starting angle. The end angle is easy to obtain, then
// 		const real startAngleInCylinder = normalizeRad(atan2(centerDir.z, centerDir.y) - halfSectorAngle);
// 		const real endAngleInCylinder = normalizeRad(startAngleInCylinder + 2.0 * halfSectorAngle);
//
// 		// Clearing the vector that will contain segments and colors. It will first contain ranges in the
// 		// cylinder frame of reference, and then the angles will be converted to linear camera ranges
// 		rangesAndColors.clear();
// 		if (segmentsColor.size() == 1) {
// 			// There is only one color, we can simply add one element to the rangesAndColors vector
// 			rangesAndColors.append(PhyObject2DWrapper::AngularRangeAndColor(startAngleInCylinder, endAngleInCylinder, segmentsColor[0].color));
// 		} else {
// 			// The interval modelling the visible arc, with which all other arcs are intersected. If
// 			// the arc crosses -pi/pi, the interval would go from -inf to end and from start to inf,
// 			// so we restrict it to -pi, pi
// 			const Intervals visibleArc = Intervals(SimpleInterval(startAngleInCylinder, endAngleInCylinder)) & SimpleInterval(-PI_GRECO, PI_GRECO);
//
// 			// This could perhaps be made a bit more efficient by checking if the we already analyzed
// 			// all segments in the range, but it would be necessary only if we had objects with a lot
// 			// of segments
// 			for (int i = 0; i < segmentsColor.size(); i++) {
// 				// Computing the intersection between the current segment and the visible range and adding it. We don't
// 				// need to intersect the cur segment with [-pi, pi] as we did for visibleArc because the intersection
// 				// with the latter will remove all values below -pi or above pi
// 				const Intervals curVisibleSegmentArc = visibleArc & segmentsColor[i].intervals;
// 				if (!curVisibleSegmentArc.isEmpty()) {
// 					for (Intervals::const_iterator it = curVisibleSegmentArc.begin(); it != curVisibleSegmentArc.end(); ++it) {
// 						rangesAndColors.append(PhyObject2DWrapper::AngularRangeAndColor(it->start, it->end, segmentsColor[i].color));
// 					}
// 				}
// 			}
// 		}
//
// 		// Now converting angles to linear camera ranges
// 		for (int i = 0; i < rangesAndColors.size(); i++) {
// 			// Getting the points corresponding to the angles in the current range in the frame of reference
// 			// of the cylinder
// 			const wVector startPointCylinder(0.0, radius * cos(rangesAndColors[i].minAngle), radius * sin(rangesAndColors[i].minAngle));
// 			const wVector endPointCylinder(0.0, radius * cos(rangesAndColors[i].maxAngle), radius * sin(rangesAndColors[i].maxAngle));
//
// 			// Now computing the points in the global frame of reference
// 			const wVector startPoint = objMtr.transformVector(startPointCylinder);
// 			const wVector endPoint = objMtr.transformVector(endPointCylinder);
//
// 			// Now computing the angles in the linear camera. As we don't know which is the start angle and which the end angle, we
// 			// rely on the fact that for cylinders the camera cannot see a portion greater than PI_GRECO
// 			const double firstAngleCamera = getAngleWithXAxis(mtr, startPoint - mtr.w_pos);
// 			const double secondAngleCamera = getAngleWithXAxis(mtr, endPoint - mtr.w_pos);
// 			if (firstAngleCamera > secondAngleCamera) {
// 				// Checking if they are on different sides of the -PI_GRECO/PI_GRECO boundary or on the same side. Here we exploit
// 				// the fact that the camera cannot see more than PI_GRECO of the cylinder
// 				if ((firstAngleCamera > (PI_GRECO / 2.0)) && (secondAngleCamera < (-PI_GRECO / 2.0))) {
// 					rangesAndColors[i].minAngle = firstAngleCamera;
// 					rangesAndColors[i].maxAngle = secondAngleCamera;
// 				} else {
// 					rangesAndColors[i].minAngle = secondAngleCamera;
// 					rangesAndColors[i].maxAngle = firstAngleCamera;
// 				}
// 			} else {
// 				// Checking if they are on different sides of the -PI_GRECO/PI_GRECO boundary or on the same side. Here we exploit
// 				// the fact that the camera cannot see more than PI_GRECO of the cylinder
// 				if ((firstAngleCamera < (-PI_GRECO / 2.0)) && (secondAngleCamera > (PI_GRECO / 2.0))) {
// 					rangesAndColors[i].minAngle = secondAngleCamera;
// 					rangesAndColors[i].maxAngle = firstAngleCamera;
// 				} else {
// 					rangesAndColors[i].minAngle = firstAngleCamera;
// 					rangesAndColors[i].maxAngle = secondAngleCamera;
// 				}
// 			}
// 		}
// 	}

	/**
	 * \brief Computes the distance and orientation of a cylindrical object
	 *        respect to the given robot
	 *
	 * This function computes the distance of a cylindrical object from the
	 * given robot. The robot is always modelled as a vertical cylindrical
	 * object. The distance is computed between the nearest points of the
	 * two objects on the plane (i.e. both the cylinder and the robot are
	 * treated as circles)
	 * \param robotPosition the position of the robot
	 * \param robotOrientation the orientation of the robot
	 * \param robotRadius the radius of the robot
	 * \param radius the radius of the cylinder
	 * \param position the position of the cylinder (z is discarded)
	 * \param distance the computed distance. It is negativa if the distance
	 *                 couldn't be computed
	 * \param angle the computed orientation
	 */
	void computeDistanceAndOrientationFromRobotToCylinder(wVector robotPosition, double robotOrientation, double robotRadius, double radius, wVector position, double& distance, double& angle)
	{
		// Setting to 0.0 the z coordinate of positions
		robotPosition.z = 0.0;
		position.z = 0.0;

		// Computing the distance. We have to remove both the robot radius and the object radius
		distance = (position - robotPosition).norm() - robotRadius - radius;

		// Now computing the angle between the robot and the object
		angle = atan2(position.y - robotPosition.y, position.x - robotPosition.x) - robotOrientation;
	}
}

PhyObject2DWrapper::PhyObject2DWrapper(Arena *arena) :
	m_arena(arena),
	m_previousMatrix()
{
	// Nothing to do here
}

PhyObject2DWrapper::~PhyObject2DWrapper()
{
	// Nothing to do here
}

WObject* PhyObject2DWrapper::wObject()
{
	return phyObject();
}

const WObject* PhyObject2DWrapper::wObject() const
{
	return phyObject();
}

void PhyObject2DWrapper::setStatic(bool s)
{
	if (phyObject() != nullptr) {
		phyObject()->setStatic(s);
	}
}

bool PhyObject2DWrapper::getStatic() const
{
	if (phyObject() != nullptr) {
		return phyObject()->getStatic();
	} else {
		return true;
	}
}

void PhyObject2DWrapper::setKinematic(bool b, bool c)
{
	if (phyObject() != nullptr) {
		phyObject()->setKinematic(b, c);
	}
}

bool PhyObject2DWrapper::getKinematic() const
{
	if (phyObject() != nullptr) {
		return phyObject()->getKinematic();
	} else {
		return true;
	}
}

void PhyObject2DWrapper::setPosition(wVector pos)
{
	setPosition(pos.x, pos.y);
}

wVector PhyObject2DWrapper::position() const
{
	return wObject()->matrix().w_pos;
}

void PhyObject2DWrapper::setTexture(QString textureName)
{
	wObject()->setTexture(textureName);
}

QString PhyObject2DWrapper::texture() const
{
	return wObject()->texture();
}

void PhyObject2DWrapper::setColor(QColor color)
{
	wObject()->setColor(color);
}

QColor PhyObject2DWrapper::color() const
{
	return wObject()->color();
}

void PhyObject2DWrapper::setUseColorTextureOfOwner(bool b)
{
	wObject()->setUseColorTextureOfOwner(b);
}

bool PhyObject2DWrapper::useColorTextureOfOwner() const
{
	return wObject()->useColorTextureOfOwner();
}

Box2DWrapper::Box2DWrapper(Arena* arena, PhyBox* box, Type type) :
	PhyObject2DWrapper(arena),
	m_box(box),
	m_vertexes(QVector<wVector>() << computeWallVertex(m_box, 0) << computeWallVertex(m_box, 1) << computeWallVertex(m_box, 2) << computeWallVertex(m_box, 3)),
	m_centerOnPlane(m_box->matrix().w_pos - m_box->matrix().z_ax.scale(m_box->sideZ() / 2.0)),
	m_type(((type != Plane) && (type != Wall) && (type != RectangularTargetArea)) ? Box : type)
{
	if (m_type == RectangularTargetArea) {
		m_box->setKinematic(true, false);
	} else if (m_type != Box) {
		m_box->setStatic(true);
	}
}

Box2DWrapper::~Box2DWrapper()
{
	// Nothing to do here
}

PhyBox* Box2DWrapper::phyObject()
{
	return m_box;
}

const PhyBox* Box2DWrapper::phyObject() const
{
	return m_box;
}

Box2DWrapper::Type Box2DWrapper::type() const
{
	return m_type;
}

void Box2DWrapper::setStatic(bool s)
{
	// Only Boxes can be made non-static
	if (m_type != Box) {
		return;
	}

	phyObject()->setStatic(s);
}

void Box2DWrapper::setKinematic(bool b, bool c)
{
	if (m_type != RectangularTargetArea) {
		PhyObject2DWrapper::setKinematic(b, c);
	}
}

void Box2DWrapper::setPosition(real x, real y)
{
	// Planes and Walls cannot be moved
	if ((m_type == Plane) || (m_type == Wall)) {
		return;
	}

	wVector pos = phyObject()->matrix().w_pos;

	pos.x = x;
	pos.y = y;

	phyObject()->setPosition(pos);

	// We also have to recompute the vertexes and center
	for (unsigned int i = 0; i < 4; i++) {
		m_vertexes[i] = computeWallVertex(m_box, i);
	}
	m_centerOnPlane = m_box->matrix().w_pos - m_box->matrix().z_ax.scale(m_box->sideZ() / 2.0);
}

void Box2DWrapper::computeLinearViewFieldOccupiedRange(const wMatrix& cameraMtr, QVector<AngularRangeAndColor>& rangesAndColors, double& distance, double maxDistance) const
{
	// Here for the moment we only have one color, so we can compute one range

	// If this is a Plane or a RectangularTargetArea, we simply return a negative distance (they are not visible with
	// a linear camera)
	if ((m_type == Plane) || (m_type == RectangularTargetArea)) {
		distance = -1.0;
		return;
	}

	// We have to translate the camera to lie on the same plane of the vertex. We translate it along
	// its local upvector (Z axis) until it reaches the  plane containing the base of the wall. Of course
	// this only works if the camera Z axis is not paraller to the plane with the base of the wall. In
	// that case all computations would be invalid, so we don't do anything
	wMatrix mtr = cameraMtr;
	if (fabs(mtr.z_ax % m_box->matrix().z_ax) < 0.0001) {
		distance = -1.0;
		return;
	}
	mtr.w_pos = mtr.w_pos + mtr.z_ax.scale((m_centerOnPlane.z - mtr.w_pos.z) / mtr.z_ax.z);

	// First of all computing the angle for every vertex
	QVector<double> angles(4);

	for (int i = 0; i < 4; i++) {
		// Computing the vector giving the direction to the vertex
		const wVector vdir = m_vertexes[i] - mtr.w_pos;

		// Now computing the angle
		angles[i] = getAngleWithXAxis(mtr, vdir);
	}

	// Now finding the min and max angle (their indexes). We have to take into account the fact that the
	// angle with the minimum value could be the upper limit and viceversa because the object could be
	// behind the camera. However we know that, as the camera is outside the wall, the maximum possible
	// angular sector of the view filed occupied by the wall is 180°. This means that also the angular
	// distance of one vertex with the center of the wall must be less than 180°. So, if we compute this
	// distance and get a value greater than 180°, we have to take (360° - computed_angular_distance)
	// and invert min with max.
	const wVector centerDir = m_centerOnPlane - mtr.w_pos;
	const double centerAngle = getAngleWithXAxis(mtr, centerDir);
	int minAngleID = 0;
	int maxAngleID = 0;

	// These two are the angular distances of the current min and max angles from the center. Their initial
	// value is the lowest possible
	double minDelta = 0.0;
	double maxDelta = 0.0;

	for (int i = 0; i < 4; i++) {
		const double curDelta = fabs(angles[i] - centerAngle);

		// Checking if the vertex and the center are behind the camera
		if (curDelta > PI_GRECO) {
			const double actualDelta = (2.0 * PI_GRECO) - curDelta;
			if (angles[i] > centerAngle) {
				// This is a candidate minimum angle
				if (actualDelta > minDelta) {
					minAngleID = i;
					minDelta = actualDelta;
				}
			} else {
				// This is a candidate maximum angle
				if (actualDelta > maxDelta) {
					maxAngleID = i;
					maxDelta = actualDelta;
				}
			}
		} else {
			if (angles[i] < centerAngle) {
				// This is a candidate minimum angle
				if (curDelta > minDelta) {
					minAngleID = i;
					minDelta = curDelta;
				}
			} else {
				// This is a candidate maximum angle
				if (curDelta > maxDelta) {
					maxAngleID = i;
					maxDelta = curDelta;
				}
			}
		}
	}

	// Filling the minAngle and maxAngle parameters
	rangesAndColors.clear();
	rangesAndColors.append(AngularRangeAndColor(angles[minAngleID], angles[maxAngleID], color()));

#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning QUESTO MODO DI CALCOLARE LA DISTANZA È SBAGLIATO (E NON NE CAPISCO IL SENSO), MA È QUELLO USATO IN EVOROBOT, QUINDI PER IL MOMENTO LO USO (ANCHE PERCHÉ USARE LA DISTANZA PER L OCCLUSIONE NON VA BENE COMUNQUE)
#endif
	// Now computing distance. This way of calculating the distance is plainly wrong (and I can't
	// see why it is written this way), but it is the method used by Evorobot, so for the moment
	// using it (moreover using distance for occlusion is not correct)
	distance = ((mtr.w_pos - m_vertexes[minAngleID]).norm() + (mtr.w_pos - m_vertexes[maxAngleID]).norm()) / 2.0;

	// Checking that the distance is less than the maximum one
	if (distance > maxDistance) {
		distance = -1.0;
	}
}

bool Box2DWrapper::computeDistanceAndOrientationFromRobot(const WheeledRobot2DWrapper& robot, double& distance, double& angle) const
{
#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning I CALCOLI PER DISTANZA E ORIENTAMENTO IN EVOROBOT SONO STRANI, QUI HO CERCATO DI FARE QUALCOSA CHE MI SEMBRI SENSATO...
#endif
	// Only doing computations for walls and boxes
	if ((m_type != Wall) && (m_type != Box)) {
		return false;
	}

	// Taking the robot position and setting z to lie on the same plane of the vertexes
	const wVector robotPosition(robot.position().x, robot.position().y, m_vertexes[0].z);

	// Now computing the robot position in the box frame of reference
	const wVector relRobotPosition = m_box->matrix().untransformVector(robotPosition);

	// Now we can find the point in the rectangle that is nearest to the robot position. As we work in the box
	// frame of reference, the vertex are easy to compute. They are (discarding z):
	// 	(+m_box->sideX() / 2.0, +m_box->sideY() / 2.0)
	// 	(+m_box->sideX() / 2.0, -m_box->sideY() / 2.0)
	// 	(-m_box->sideX() / 2.0, +m_box->sideY() / 2.0)
	// 	(-m_box->sideX() / 2.0, -m_box->sideY() / 2.0)
	// Finding the nearest point is just a matter of separately computing x and y.
	real nearestX;
	if (relRobotPosition.x < -m_box->sideX() / 2.0) {
		nearestX = -m_box->sideX() / 2.0;
	} else if (relRobotPosition.x > +m_box->sideX() / 2.0) {
		nearestX = +m_box->sideX() / 2.0;
	} else {
		nearestX = relRobotPosition.x;
	}
	real nearestY;
	if (relRobotPosition.y < -m_box->sideY() / 2.0) {
		nearestY = -m_box->sideY() / 2.0;
	} else if (relRobotPosition.y > +m_box->sideY() / 2.0) {
		nearestY = +m_box->sideY() / 2.0;
	} else {
		nearestY = relRobotPosition.y;
	}

	// Although distance is independent of the frame of reference, we convert the nearest point to the global frame
	// of reference because we only have the robot orientation in that frame
	const wVector nearestPoint = m_box->matrix().transformVector(wVector(nearestX, nearestY, relRobotPosition.z));

	// Now we can easily compute the distance and orientation. For the distance we have to remove the robot radius
	distance = (nearestPoint - robotPosition).norm() - robot.getRadius();
	const real robotOrientation = (dynamic_cast<const RobotOnPlane*>(robot.wObject()))->orientation(m_arena->getPlane());
	angle = atan2(nearestPoint.y - robotPosition.y, nearestPoint.x - robotPosition.x) - robotOrientation;

	return true;
}

Cylinder2DWrapper::Cylinder2DWrapper(Arena* arena, PhyCylinder* cylinder, Type type) :
	PhyObject2DWrapper(arena),
	m_cylinder(cylinder),
	m_type(((type != SmallCylinder) && (type != BigCylinder) && (type != CircularTargetArea)) ? Cylinder : type)
{
	if (m_type == CircularTargetArea) {
		m_cylinder->setKinematic(true, false);
	}
}

Cylinder2DWrapper::~Cylinder2DWrapper()
{
	// Nothing to do here
}

PhyCylinder* Cylinder2DWrapper::phyObject()
{
	return m_cylinder;
}

const PhyCylinder* Cylinder2DWrapper::phyObject() const
{
	return m_cylinder;
}

void Cylinder2DWrapper::setStatic(bool s)
{
	// CircularTargetArea cannot be made non-static
	if (m_type == CircularTargetArea) {
		return;
	}

	phyObject()->setStatic(s);
}

void Cylinder2DWrapper::setKinematic(bool b, bool c)
{
	// CircularTargetArea cannot be made non-kinematic
	if (m_type == CircularTargetArea) {
		return;
	}

	phyObject()->setKinematic(b, c);
}

Cylinder2DWrapper::Type Cylinder2DWrapper::type() const
{
	return m_type;
}

void Cylinder2DWrapper::setPosition(real x, real y)
{
	wVector pos = phyObject()->matrix().w_pos;

	pos.x = x;
	pos.y = y;

	phyObject()->setPosition(pos);
}

void Cylinder2DWrapper::computeLinearViewFieldOccupiedRange(const wMatrix& cameraMtr, QVector<AngularRangeAndColor>& rangesAndColors, double& distance, double maxDistance) const
{
	// If this is a CircularTargetArea, we simply return a negative distance (it is not visible with
	// a linear camera)
	if (m_type == CircularTargetArea) {
		distance = -1.0;
		return;
	}

	// Getting the vector with cylinder colors
	computeLinearViewFieldOccupiedRangeForCircle(cameraMtr, m_cylinder->matrix(), m_cylinder->radius(), m_cylinder->segmentsColor().constBegin(), m_cylinder->segmentsColor().constEnd(), rangesAndColors, distance, maxDistance);
}

bool Cylinder2DWrapper::computeDistanceAndOrientationFromRobot(const WheeledRobot2DWrapper& robot, double& distance, double& angle) const
{
	// If this is a CircularTargetArea, we simply return a negative distance
	if (m_type == CircularTargetArea) {
		return false;
	}

	const double robotOrientation = (dynamic_cast<const RobotOnPlane*>(robot.wObject()))->orientation(m_arena->getPlane());
	computeDistanceAndOrientationFromRobotToCylinder(robot.position(), robotOrientation, robot.getRadius(), m_cylinder->radius(), m_cylinder->matrix().w_pos, distance, angle);

	return true;
}


Sphere2DWrapper::Sphere2DWrapper(Arena* arena, PhySphere* sphere, Type /*type*/) :
	PhyObject2DWrapper(arena),
	m_sphere(sphere),
	m_type(LightBulb) // For the moment type can only be LightBulb
{
	m_sphere->setKinematic(true);
}

Sphere2DWrapper::~Sphere2DWrapper()
{
	// Nothing to do here
}

PhySphere* Sphere2DWrapper::phyObject()
{
	return m_sphere;
}

const PhySphere* Sphere2DWrapper::phyObject() const
{
	return m_sphere;
}

void Sphere2DWrapper::setStatic(bool /*s*/)
{
	// The sphere is always static, this does nothing
}

void Sphere2DWrapper::setKinematic(bool /*b*/, bool /*c*/)
{
	// The sphere is always kinematic, this does nothing
}

Sphere2DWrapper::Type Sphere2DWrapper::type() const
{
	return m_type;
}

void Sphere2DWrapper::setPosition(real x, real y)
{
	wVector pos = phyObject()->matrix().w_pos;

	pos.x = x;
	pos.y = y;

	phyObject()->setPosition(pos);
}

void Sphere2DWrapper::computeLinearViewFieldOccupiedRange(const wMatrix& /*cameraMtr*/, QVector<AngularRangeAndColor>& /*rangesAndColors*/, double& distance, double /*maxDistance*/) const
{
	// Light bulbs are not seen by the linear camera
	distance = -1.0;
	return;
}

bool Sphere2DWrapper::computeDistanceAndOrientationFromRobot(const WheeledRobot2DWrapper& robot, double& distance, double& angle) const
{
	wVector robotPosition = robot.position();
	robotPosition.z = 0.0;
	wVector position = m_sphere->matrix().w_pos;
	position.z = 0.0;

	distance = (position - robotPosition).norm() - robot.getRadius() - m_sphere->radius();

	const double robotOrientation = (dynamic_cast<const RobotOnPlane*>(robot.wObject()))->orientation(m_arena->getPlane());
	angle = atan2(position.y - robotPosition.y, position.x - robotPosition.x) - robotOrientation;

	return true;
}

WheeledRobot2DWrapper::WheeledRobot2DWrapper(Arena* arena, RobotOnPlane* robot, double height, double radius) :
	PhyObject2DWrapper(arena),
	m_robot(robot),
	m_height(height),
	m_radius(radius)
{
}

WheeledRobot2DWrapper::~WheeledRobot2DWrapper()
{
}

RobotOnPlane* WheeledRobot2DWrapper::robotOnPlane()
{
	return m_robot;
}

const RobotOnPlane* WheeledRobot2DWrapper::robotOnPlane() const
{
	return m_robot;
}

WObject* WheeledRobot2DWrapper::wObject()
{
	return dynamic_cast<WObject*>(m_robot);
}

const WObject* WheeledRobot2DWrapper::wObject() const
{
	return dynamic_cast<const WObject*>(m_robot);
}

PhyObject* WheeledRobot2DWrapper::phyObject()
{
	return nullptr;
}

const PhyObject* WheeledRobot2DWrapper::phyObject() const
{
	return nullptr;
}

WheeledRobot2DWrapper::Type WheeledRobot2DWrapper::type() const
{
	return WheeledRobot;
}

void WheeledRobot2DWrapper::setPosition(real x, real y)
{
	wVector pos = wObject()->matrix().w_pos;

	pos.x = x;
	pos.y = y;

	wObject()->setPosition(pos);
}

void WheeledRobot2DWrapper::computeLinearViewFieldOccupiedRange(const wMatrix& cameraMtr, QVector<AngularRangeAndColor>& rangesAndColors, double& distance, double maxDistance) const
{
	const wMatrix mtr = wObject()->matrix().rotateAround(wObject()->matrix().y_ax, wObject()->matrix().w_pos, -PI_GRECO / 2.0);

	// Getting the vector with cylinder colors. For the moment only the MarXbot can have multiple colors
#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning QUESTA ROBA È BRUTTA, IL CODICE PER LE SIMULAZIONI CON I WHEELED SI STA INGARBUGLIANDO...
#endif
	PhyMarXbot* marxbot = dynamic_cast<PhyMarXbot*>(m_robot);
	if (marxbot == nullptr) {
		PhyEpuck* epuck = dynamic_cast<PhyEpuck*>(m_robot);

		if (epuck == nullptr) {
			// This cast should never fail!
			WObject* r = dynamic_cast<WObject*>(m_robot);
			PhyCylinderSegmentColor s(SimpleInterval(-PI_GRECO, PI_GRECO), r->color());
			computeLinearViewFieldOccupiedRangeForCircle(cameraMtr, mtr, m_radius, &s, (&s + 1), rangesAndColors, distance, maxDistance);
		} else {
			computeLinearViewFieldOccupiedRangeForCircle(cameraMtr, mtr, m_radius, epuck->segmentsColor().constBegin(), epuck->segmentsColor().constEnd(), rangesAndColors, distance, maxDistance);
		}
	} else {
		computeLinearViewFieldOccupiedRangeForCircle(cameraMtr, mtr, m_radius, marxbot->segmentsColor().constBegin(), marxbot->segmentsColor().constEnd(), rangesAndColors, distance, maxDistance);
	}
}

bool WheeledRobot2DWrapper::computeDistanceAndOrientationFromRobot(const WheeledRobot2DWrapper& robot, double& distance, double& angle) const
{
	if (this == &robot) {
		return false;
	}

	const double robotOrientation = (dynamic_cast<const RobotOnPlane*>(robot.wObject()))->orientation(m_arena->getPlane());
	computeDistanceAndOrientationFromRobotToCylinder(robot.position(), robotOrientation, robot.getRadius(), m_radius, position(), distance, angle);

	return true;
}

wVector positionOnPlane(const Box2DWrapper* plane, real x, real y)
{
	wVector pos = plane->position();

	pos.x = x;
	pos.y = y;
	pos.z += plane->phyObject()->sideZ() / 2.0f;

	return pos;
}

void orientationOnPlane(const Box2DWrapper* plane, real angle, wMatrix& mtr)
{
	wMatrix rotatedMtr = plane->phyObject()->matrix();

	// Now rotating the matrix around the Z axis
	rotatedMtr = rotatedMtr.rotateAround(rotatedMtr.z_ax, rotatedMtr.w_pos, angle);

	// Setting the position of the rotated matrix to be the same as the original one
	rotatedMtr.w_pos = mtr.w_pos;

	// Now overwriting the matrix
	mtr = rotatedMtr;
}

real angleBetweenXAxes(const wMatrix& mtr1, const wMatrix& mtr2)
{
	// Taking the two x axes. We can take the x axis of mtr1 as is, while we need to project the X axis
	// of mtr2 onto the XY plane of mtr1. To do so we simply project the x axis of mtr2 on the z axis of
	// mtr1 and subtract this vector from the original x axis of mtr2
	const wVector& x1 = mtr1.x_ax;
	const wVector x2 = mtr2.x_ax - mtr1.z_ax.scale(mtr2.x_ax % mtr1.z_ax);

	// Now normalizing both axes
	const wVector normX1 = x1.scale(1.0 / x1.norm());
	const wVector normX2 = x2.scale(1.0 / x2.norm());

	// To get the angle (unsigned), computing the acos of the dot product of the two vectors
	const double unsignedAngle = acos(normX1 % normX2);

	// Now choosing the right sign. To do this we first compute the cross product of the two x axes and
	// then we see if it has the same direction of the z axis of the first matrix or not
	const double s = mtr1.z_ax % (normX1 * normX2);
	return (s < 0.0) ? -unsignedAngle : unsignedAngle;
}

} // end namespace salsa
