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

#include "sensors.h"
#include "configurationhelper.h"
#include "motorcontrollers.h"
#include "logger.h"
#include "graphicalmarkers.h"
#include "arena.h"
#include <QStringList>
#include <QList>
#include <QVector>
#include <QtAlgorithms>
#include <limits>
#include <cmath>
#include <QLinkedList>
#include <QFile>
#include <QTextStream>

namespace salsa {

// FakeSensor::FakeSensor(ConfigurationParameters& params, QString prefix) :
// 	Sensor(params, prefix),
// 	m_additionalInputs(ConfigurationHelper::getUnsignedInt(params, prefix + "additionalInputs", 1)),
// 	m_neuronsIteratorResource(actualResourceNameForMultirobot(ConfigurationHelper::getString(params, prefix + "neuronsIterator", "neuronsIterator"))),
// 	m_additionalInputsResource(actualResourceNameForMultirobot(ConfigurationHelper::getString(params, prefix + "additionalInputsResource", "additionalInputs"))),
// 	m_neuronsIterator(NULL)
// {
// 	usableResources(QStringList() << m_neuronsIteratorResource << m_additionalInputsResource);
//
// 	for (unsigned int i = 0; i < m_additionalInputs.size(); i++) {
// 		m_additionalInputs[i] = 0.0;
// 	}
// }
//
// FakeSensor::~FakeSensor()
// {
// 	// Removing resources
// 	try {
// 		deleteResource(m_additionalInputsResource);
// 	} catch (...) {
// 		// Doing nothing, this is here just to prevent throwing an exception from the destructor
// 	}
// }
//
// void FakeSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	Sensor::save( params, prefix );
// 	params.startObjectParameters(prefix, "FakeSensor", this);
// 	params.createParameter(prefix, "additionalInputs", QString::number(m_additionalInputs.size()));
// 	params.createParameter(prefix, "neuronsIterator", m_neuronsIteratorResource);
// 	params.createParameter(prefix, "additionalInputsResource", m_additionalInputsResource);
// }
//
// void FakeSensor::describe(QString type)
// {
// 	Sensor::describe(type);
//
// 	Descriptor d = addTypeDescription(type, "Adds input neurons that can be used for custom operations", "With this sensor you can specify how many additional inputs are needed in the controller. This also declares a resource that can be used to access the additional inputs");
// 	d.describeInt("additionalInputs").def(1).limits(1,100).props(IsMandatory).help("The number of additional inputs that will be added to the controller (default 1)");
// 	d.describeString("neuronsIterator").def("neuronsIterator").help("The name of the resource associated with the neural network iterator (default is \"neuronsIterator\")");
// 	d.describeString("additionalInputsResource").def("additionalInputs").help("The name of the resource associated with the vector of additional inputs (default is \"additionalInputs\")");
// }
//
// void FakeSensor::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	ResourcesLocker locker(this);
//
// 	// Copying the output inside the vector of additional outputs
// 	m_neuronsIterator->setCurrentBlock(name());
// 	for (unsigned int i = 0; i < m_additionalInputs.size(); i++, m_neuronsIterator->nextNeuron()) {
// 		m_neuronsIterator->setInput(m_additionalInputs[i]);
// 	}
// }
//
// int FakeSensor::size()
// {
// 	return m_additionalInputs.size();
// }
//
// void FakeSensor::shareResourcesWith(ResourcesUser* other)
// {
// 	// Calling parent function
// 	Sensor::shareResourcesWith(other);
//
// 	// Now declaring our resource
// 	declareResource(m_additionalInputsResource, &m_additionalInputs);
// }
//
// void FakeSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	if (changeType == Deleted) {
// 		resetNeededResourcesCheck();
// 		return;
// 	}
//
// 	if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
// 		for(int i = 0; i < size(); i++, m_neuronsIterator->nextNeuron()) {
// 			m_neuronsIterator->setGraphicProperties("Fk" + QString::number(i), 0.0, 1.0, Qt::red);
// 		}
// 	} else if (resourceName != m_additionalInputsResource) {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// //ObjectPositionSensor : begin implementation
// // it returns the absolute coordinate of an object into the world
// ObjectPositionSensor::ObjectPositionSensor(ConfigurationParameters &params, QString prefix) :
// 	Sensor(params, prefix) {
// 	neuronsIteratorResource = ConfigurationHelper::getString(params, prefix + "neuronsIterator", "neuronsIterator");
// 	objectName = ConfigurationHelper::getString( params, prefix+"object", "object" );
// 	QVector<double> vec1 = ConfigurationHelper::getVector( params, prefix+"bbMin" );
// 	QVector<double> vec2 = ConfigurationHelper::getVector( params, prefix+"bbMax" );
// 	if ( vec1.size() == 3 && vec2.size() == 3 ) {
// 		linearize = true;
// 		bbMin = wVector( vec1[0], vec1[1], vec1[2] );
// 		bbMax = wVector( vec2[0], vec2[1], vec2[2] );
// 	} else {
// 		linearize = false;
// 		if ( ! (vec1.isEmpty() && vec2.isEmpty()) ) {
// 			Logger::warning( QString("ObjectPositionSensor %1 - bbMin and/or bbMax parameters are not well specified; they will be ignored").arg(name()) );
// 		}
// 	}
//
// 	// Declaring the resources that are needed here
// 	usableResources( QStringList() << objectName << neuronsIteratorResource );
// }
//
// ObjectPositionSensor::~ObjectPositionSensor() {
// 	// nothing to do
// }
//
// void ObjectPositionSensor::describe( QString type ) {
// 	Sensor::describe( type );
// 	Descriptor d = addTypeDescription( type, "Sensor for reading the three absolute coordinate (position into the worlf frame) of an object" );
// 	d.describeString("neuronsIterator").def("neuronsIterator").help("the name of the resource associated with the neural network iterator (default is \"neuronsIterator\")");
// 	d.describeString( "object" ).def( "object" ).props( IsMandatory ).help( "The name of the resource associated with the object to track with this sensor" );
// 	d.describeReal( "bbMin" ).props( IsList ).help( "The minimum 3D point used for linearize the object position into [0,1]" );
// 	d.describeReal( "bbMax" ).props( IsList ).help( "The maximum 3D point used for linearize the object position into [0,1]" );
// }
//
// void ObjectPositionSensor::update() {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	// Acquiring the lock to get resources
// 	ResourcesLocker locker( this );
//
// 	WObject* object = getResource<WObject>( objectName );
// 	wVector pos = object->matrix().w_pos;
// 	NeuronsIterator* evonetIt = getResource<NeuronsIterator>( neuronsIteratorResource );
// 	evonetIt->setCurrentBlock( name() );
// 	for( int i=0; i<3; i++, evonetIt->nextNeuron() ) {
// 		if ( linearize ) {
// 			// linearize into [0,1]
// 			evonetIt->setInput( linearMap( pos[i], bbMin[i], bbMax[i], 0, 1 ) );
// 		} else {
// 			evonetIt->setInput( pos[i] );
// 		}
// 	}
// }
//
// int ObjectPositionSensor::size() {
// 	return 3;
// }
//
// void ObjectPositionSensor::resourceChanged(QString resourceName, ResourceChangeType changeType) {
// 	if (changeType == Deleted) {
// 		resetNeededResourcesCheck();
// 		return;
// 	}
//
// 	if (resourceName == objectName) {
// 		// Nothing to do here, we get the object with getResource() in update()
// 	} else if (resourceName == neuronsIteratorResource) {
// 		NeuronsIterator* evonetIt = getResource<NeuronsIterator>();
// 		evonetIt->setCurrentBlock( name() );
// 		for( int i=0; i<3; i++, evonetIt->nextNeuron() ) {
// 			evonetIt->setGraphicProperties( QString("obj")+QString::number(i), -10.0, 10.0, Qt::red );
// 		}
// 	} else {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// void ObjectPositionSensor::save(ConfigurationParameters &params, QString prefix)
// {
// 	Sensor::save( params, prefix );
// 	params.startObjectParameters( prefix, "ObjectPositionSensor", this );
// 	params.createParameter(prefix, "neuronsIterator", neuronsIteratorResource);
// 	params.createParameter( prefix, "object", objectName );
// 	if ( linearize ) {
// 		params.createParameter( prefix, "bbMin", QString("%1 %2 %3").arg(bbMin[0]).arg(bbMin[1]).arg(bbMin[2]) );
// 		params.createParameter( prefix, "bbMax", QString("%1 %2 %3").arg(bbMax[0]).arg(bbMax[1]).arg(bbMax[2]) );
// 	}
// }
// //ObjectPositionSensor : end implementation
//
// namespace LinearCameraOld {
// 	namespace __LinearCamera_internal {
// 		#ifndef GLMultMatrix
// 		#define GLMultMatrix glMultMatrixf
// 		// for double glMultMatrixd
// 		#endif
//
// 		/**
// 		 * \brief The side of the cube representing the linear camera
// 		 */
// 		const float linearCameraCubeSide = 0.02f;
//
// 		/**
// 		 * \brief The length of the lines representing borders of the receptors
// 		 *        of the linear camera
// 		 */
// 		const float linearCameraReceptorsLength = 0.1f;
//
// 		/**
// 		 * \brief The graphical representation of the linear camera
// 		 */
// 		class LinearCameraGraphic : public GraphicalWObject
// 		{
// 		public:
// 			/**
// 			 * \brief Constructor
// 			 *
// 			 * This also sets the object to be our owner
// 			 * \param object the object to which we are attached.
// 			 * \param transformation the transformation matrix relative the
// 			 *                       one of the object to which we are
// 			 *                       attached
// 			 * \param minAngle the minimum angle of the camera
// 			 * \param maxAngle the maximum angle of the camera
// 			 * \param numReceptors the number of receptors
// 			 * \param name the name of this object
// 			 */
// 			LinearCameraGraphic(WObject *object, const wMatrix& transformation, double minAngle, double maxAngle, unsigned int numReceptors, QString name = "unamed") :
// 				GraphicalWObject(object->world(), name),
// 				m_object(object),
// 				m_minAngle(minAngle),
// 				m_maxAngle(maxAngle),
// 				m_numReceptors(numReceptors),
// 				m_receptorRange((m_maxAngle - m_minAngle) / double(m_numReceptors)),
// 				m_receptors(m_numReceptors, Qt::black)
// 			{
// 				// Attaching to object (which also becomes our owner)
// 				attachToObject(m_object, true, transformation);
//
// 				// We also use our own color and texture
// 				setUseColorTextureOfOwner(false);
// 				setTexture("");
// 				setColor(Qt::white);
// 			}
//
// 			/**
// 			 * \brief Destructor
// 			 */
// 			~LinearCameraGraphic()
// 			{
// 			}
//
// 			/**
// 			 * \brief Sets the colors perceived by the camera
// 			 *
// 			 * This function is thread-safe
// 			 * \param receptors the colors perceived by receptors
// 			 */
// 			void setPerceivedColors(const QVector<QColor>& receptors)
// 			{
// 				m_receptorsMutex.lock();
// 					m_receptors = receptors;
// 				m_receptorsMutex.unlock();
// 			}
//
// 		protected:
// 			/**
// 			 * \brief Performs the actual drawing
// 			 *
// 			 * \param renderer the RenderWObject object associated with this one.
// 			 *                 Use it e.g. to access the container
// 			 * \param gw the OpenGL context
// 			 */
// 			virtual void render(RenderWObject* renderer, QGLContext* gw)
// 			{
// 				// Bringing the frame of reference at the center of the camera
// 				glPushMatrix();
// 				renderer->container()->setupColorTexture(gw, renderer);
// 				GLMultMatrix(&tm[0][0]);
//
// 				// First of all drawing the camera as a small white box. The face in the
// 				// direction of view (X axis) is painted half green: the green part is the
// 				// one in the direction of the upvector (Z axis)
// 				glBegin(GL_QUADS);
// 				const float hside = linearCameraCubeSide / 2.0;
//
// 				// front (top part)
// 				glColor3f(0.0, 1.0, 0.0);
// 				glNormal3f(1.0, 0.0, 0.0);
// 				glVertex3f( hside, -hside,  hside);
// 				glVertex3f( hside, -hside,    0.0);
// 				glVertex3f( hside,  hside,    0.0);
// 				glVertex3f( hside,  hside,  hside);
//
// 				// front (bottom part)
// 				glColor3f(1.0, 1.0, 1.0);
// 				glNormal3f(1.0, 0.0, 0.0);
// 				glVertex3f( hside, -hside,    0.0);
// 				glVertex3f( hside, -hside, -hside);
// 				glVertex3f( hside,  hside, -hside);
// 				glVertex3f( hside,  hside,    0.0);
//
// 				// back
// 				glNormal3f(-1.0, 0.0, 0.0);
// 				glVertex3f(-hside, -hside, -hside);
// 				glVertex3f(-hside, -hside,  hside);
// 				glVertex3f(-hside,  hside,  hside);
// 				glVertex3f(-hside,  hside, -hside);
//
// 				// top
// 				glNormal3f(0.0, 1.0, 0.0);
// 				glVertex3f(-hside,  hside,  hside);
// 				glVertex3f( hside,  hside,  hside);
// 				glVertex3f( hside,  hside, -hside);
// 				glVertex3f(-hside,  hside, -hside);
//
// 				// bottom
// 				glNormal3f(0.0, -1.0, 0.0);
// 				glVertex3f(-hside, -hside, -hside);
// 				glVertex3f( hside, -hside, -hside);
// 				glVertex3f( hside, -hside,  hside);
// 				glVertex3f(-hside, -hside,  hside);
//
// 				// right
// 				glNormal3f(0.0, 0.0, 1.0);
// 				glVertex3f( hside, -hside,  hside);
// 				glVertex3f(-hside, -hside,  hside);
// 				glVertex3f(-hside,  hside,  hside);
// 				glVertex3f( hside,  hside,  hside);
//
// 				// left
// 				glNormal3f(0.0, 0.0, -1.0);
// 				glVertex3f( hside, -hside, -hside);
// 				glVertex3f(-hside, -hside, -hside);
// 				glVertex3f(-hside,  hside, -hside);
// 				glVertex3f( hside,  hside, -hside);
//
// 				glEnd();
//
// 				// Now we draw white lines to separare the various sectors of the camera
// 				// Disabling lighting here (we want pure lines no matter from where we look at them)
// 				glPushAttrib(GL_LIGHTING_BIT);
// 				glDisable(GL_LIGHTING);
// 				glLineWidth(2.5);
// 				glColor3f(1.0, 1.0, 1.0);
//
// 				// Drawing the lines
// 				glBegin(GL_LINES);
// 				for (unsigned int i = 0; i <= m_numReceptors; i++) {
// 					const double curAngle = m_minAngle + double(i) * m_receptorRange;
//
// 					const wVector lineEnd = wVector(cos(curAngle), sin(curAngle), 0.0).scale(linearCameraReceptorsLength);
//
// 					glVertex3f(0.0, 0.0, 0.0);
// 					glVertex3f(lineEnd.x, lineEnd.y, lineEnd.z);
// 				}
// 				glEnd();
//
// 				// Now drawing the state of receptors. Here we also have to lock the semaphore for
// 				// the m_receptors vector
// 				m_receptorsMutex.lock();
//
// 				// Drawing the status
// 				glBegin(GL_QUADS);
// 				glNormal3f(0.0, 1.0, 0.0);
// 				const double colorPatchAngle = m_receptorRange / 3.0;
// 				const double colorPatchMinLength = linearCameraReceptorsLength / 3.0;
// 				const double colorPatchMaxLength = 2.0 * linearCameraReceptorsLength / 3.0;
// 				for (unsigned int i = 0; i < m_numReceptors; i++) {
// 					const double curAngle = m_minAngle + double(i) * m_receptorRange;
//
// 					for (unsigned int c = 0; c < 3; c++) {
// 						const double startAngle = curAngle + double(c) * colorPatchAngle;
// 						const double endAngle = curAngle + double(c + 1) * colorPatchAngle;
//
// 						// Computing the four vertexes
// 						const wVector v1 = wVector(cos(startAngle), sin(startAngle), 0.0).scale(colorPatchMinLength);
// 						const wVector v2 = wVector(cos(startAngle), sin(startAngle), 0.0).scale(colorPatchMaxLength);
// 						const wVector v3 = wVector(cos(endAngle), sin(endAngle), 0.0).scale(colorPatchMaxLength);
// 						const wVector v4 = wVector(cos(endAngle), sin(endAngle), 0.0).scale(colorPatchMinLength);
//
// 						// Setting the color
// 						switch (c) {
// 							case 0:
// 								glColor3f(m_receptors[i].redF(), 0.0, 0.0);
// 								break;
// 							case 1:
// 								glColor3f(0.0, m_receptors[i].greenF(), 0.0);
// 								break;
// 							case 2:
// 								glColor3f(0.0, 0.0, m_receptors[i].blueF());
// 								break;
// 							default:
// 								break;
// 						}
//
// 						// Drawing the patch
// 						glVertex3f(v1.x, v1.y, v1.z);
// 						glVertex3f(v2.x, v2.y, v2.z);
// 						glVertex3f(v3.x, v3.y, v3.z);
// 						glVertex3f(v4.x, v4.y, v4.z);
// 					}
// 				}
// 				glEnd();
// 				m_receptorsMutex.unlock();
//
// 				// Restoring lighting status
// 				glPopAttrib();
//
// 				glPopMatrix();
// 			}
//
// 			/**
// 			 * \brief The object to which we are attached
// 			 */
// 			WObject* const m_object;
//
// 			/**
// 			 * \brief The minimum angle of the camera
// 			 */
// 			const double m_minAngle;
//
// 			/**
// 			 * \brief The maximum angle of the camera
// 			 */
// 			const double m_maxAngle;
//
// 			/**
// 			 * \brief The number of receptors
// 			 */
// 			const unsigned int m_numReceptors;
//
// 			/**
// 			 * \brief The range of each receptor
// 			 *
// 			 * This is computed in the constructor.
// 			 */
// 			const double m_receptorRange;
//
// 			/**
// 			 * \brief The vector with perceived colors
// 			 */
// 			QVector<QColor> m_receptors;
//
// 			/**
// 			 * \brief The mutex protecting the m_receptors variable
// 			 *
// 			 * The variable could be accessed by multiple threads
// 			 * concurrently, so we protect it with a mutex
// 			 */
// 			QMutex m_receptorsMutex;
// 		};
// 	}
//
// 	using namespace __LinearCamera_internal;
//
// 	LinearCamera::LinearCamera(WObject* obj, wMatrix mtr, double aperture, unsigned int numReceptors, double maxDistance, QColor backgroundColor) :
// 		ConcurrentResourcesUser(),
// 		m_receptors(numReceptors),
// 		m_object(obj),
// 		m_transformation(mtr),
// 		m_aperture((aperture > (2.0 * PI_GRECO)) ? (2.0 * PI_GRECO) : ((aperture < 0.0) ? 0.0 :  aperture)),
// 		m_numReceptors(numReceptors),
// 		m_maxDistance(maxDistance),
// 		m_backgroundColor(backgroundColor),
// 		m_apertureMin(-m_aperture / 2.0),
// 		m_apertureMax(m_aperture / 2.0),
// 		m_receptorRange(m_aperture / double(m_numReceptors)),
// 		m_arena(NULL),
// 		m_drawCamera(false),
// 		m_ignoreWalls(false),
// 		m_graphicalCamera(NULL)
//
// 	{
// 		// Stating which resources we use here
// 		addUsableResource("arena");
// 	}
//
// 	LinearCamera::~LinearCamera()
// 	{
// 		// Nothing to do here
// 	}
//
// 	namespace {
// 		// This namespace contains some structures used in the LinearCamera::update() function
//
// 		// The structure containing a color and the range of the camera field hit by this color.
// 		// It also contains the distance from the camera for ordering.
// 		struct ColorRangeAndDistance
// 		{
// 			ColorRangeAndDistance() :
// 				color(),
// 				minAngle(0.0),
// 				maxAngle(0.0),
// 				distance(0.0)
// 			{
// 			}
//
// 			ColorRangeAndDistance(QColor c, double min, double max, double d) :
// 				color(c),
// 				minAngle(min),
// 				maxAngle(max),
// 				distance(d)
// 			{
// 			}
//
// 			// This is to order objects of this type
// 			bool operator<(const ColorRangeAndDistance& other) const
// 			{
// 				return (distance < other.distance);
// 			}
//
// 			QColor color;
// 			double minAngle;
// 			double maxAngle;
// 			double distance;
// 		};
//
// 		// An helper structure memorizing information about colors in a single receptor. minAngle and maxAngle
// 		// are used to store the current portion of the receptor for which we already know the color, while
// 		// colorsAndFractions is the list of colors and the portion of the receptor occupied by that color
// 		struct ColorsInReceptor
// 		{
// 			Intervals curInterval;
//
// 			struct ColorAndFraction {
// 				ColorAndFraction() :
// 					color(),
// 					fraction(0.0)
// 				{
// 				}
//
// 				ColorAndFraction(QColor c, double f) :
// 					color(c),
// 					fraction(f)
// 				{
// 				}
//
// 				QColor color;
// 				double fraction;
// 			};
// 			QList<ColorAndFraction> colorsAndFractions;
// 		};
// 	}
//
// 	void LinearCamera::update()
// 	{
// 		// Getting the list of objects from the arena (if we have the pointer to the arena)
// 		if (m_arena == NULL) {
// 			m_receptors.fill(m_backgroundColor);
//
// 			return;
// 		}
// 		const QVector<PhyObject2DWrapper*>& objectsList = m_arena->getObjects();
//
// 		// If no object is present, we can fill the receptors list with background colors and return
// 		if (objectsList.size() == 0) {
// 			m_receptors.fill(m_backgroundColor);
//
// 			return;
// 		}
//
// 		// Updating the matrix with the current camera position
// 		wMatrix currentMtr = m_transformation * m_object->matrix();
//
// 		// First of all we need to compute which color hits each receptor
//
// 		// Now filling the list with colors, ranges and distances. If an object is perceived at the
// 		// extremities of the aperture, it is split in two different ColorRangeAndDistance objects
// 		QList<ColorRangeAndDistance> colorsRangesAndDistances;
//
// 		// For the moment we use the distance to order objects (see ColorRangeAndDistance::operator<), however
// 		// this is not correct (occlusion doesn't work well) and so should be changed
// 		for (int i = 0; i < objectsList.size(); i++) {
// 			// Checking if we have to ignore a wall
// 			if (m_ignoreWalls && (objectsList[i]->type() == PhyObject2DWrapper::Wall)) {
// 				continue;
// 			} else if (m_object == objectsList[i]->wObject()) {
// 				// Skipping checks with self
// 				continue;
// 			}
//
// 			QVector<PhyObject2DWrapper::AngularRangeAndColor> rangesAndColors;
// 			double distance;
// 			objectsList[i]->computeLinearViewFieldOccupiedRange(currentMtr, rangesAndColors, distance, m_maxDistance);
//
// 			// computeLinearViewFieldOccupiedRange returns a negative distance if the object is outside the view field
// 			if ((distance < 0.0) || (distance > m_maxDistance)) {
// 				continue;
// 			}
//
// 			for (int j = 0; j < rangesAndColors.size(); j++) {
// 				// To safely compare with the aperture, we have to convert angles between -PI_GRECO and PI_GRECO
// 				const double minAngle = normalizeRad(rangesAndColors[j].minAngle);
// 				const double maxAngle = normalizeRad(rangesAndColors[j].maxAngle);
// 				const QColor color = rangesAndColors[j].color;
//
// 				// If the minAngle is greater than the maxAngle, splitting in two, so that we do not have to
// 				// make special cases in the subsequent part of the function. Here we also check if the object
// 				// is completely outside the view field or not (in the first case we don't add it to the list)
// 				// We just check if the object is at least partially visible, we don't set the limits to be
// 				// within the view field
// 				if (minAngle > maxAngle) {
// 					if ((minAngle > m_apertureMin) && (minAngle < m_apertureMax)) {
// 						colorsRangesAndDistances.append(ColorRangeAndDistance(color, minAngle, m_apertureMax, distance));
// 					}
// 					if ((maxAngle > m_apertureMin) && (maxAngle < m_apertureMax)) {
// 						colorsRangesAndDistances.append(ColorRangeAndDistance(color, m_apertureMin, maxAngle, distance));
// 					}
// 				} else {
// 					if (((minAngle > m_apertureMin) && (minAngle < m_apertureMax)) || ((maxAngle > m_apertureMin) && (maxAngle < m_apertureMax))) {
// 						colorsRangesAndDistances.append(ColorRangeAndDistance(color, max(minAngle, m_apertureMin), min(maxAngle, m_apertureMax), distance));
// 					}
// 				}
// 			}
// 		}
//
// 		// Ordering colors by distance from the camera
// 		qSort(colorsRangesAndDistances);
//
// 		// Now we can add the background color at the end of the list. It covers all receptors to be sure to fill
// 		// the whole field with a valid color
// 		colorsRangesAndDistances.append(ColorRangeAndDistance(m_backgroundColor, m_apertureMin, m_apertureMax, std::numeric_limits<double>::infinity()));
//
// 		// The next step is to calculate the percentage of each color in the colorsRangesAndDistances list
// 		// in each receptor
// 		QVector<ColorsInReceptor> colorsInReceptors(m_numReceptors);
// 		for (QList<ColorRangeAndDistance>::const_iterator it = colorsRangesAndDistances.begin(); it != colorsRangesAndDistances.end(); ++it) {
// 			// Computing the index of receptors which are interested by this color
// 			const int minIndex = max(0.0, floor((it->minAngle - m_apertureMin) / m_receptorRange));
// 			const int maxIndex = min(double(m_numReceptors - 1), floor((it->maxAngle - m_apertureMin) / m_receptorRange));
//
// 			// Now cycling over the computed receptors in the colorsInReceptors list to fill it
// 			for (int i = minIndex; i <= maxIndex; i++) {
// 				const double receptorMin = m_apertureMin + m_receptorRange * double(i);
// 				const double receptorMax = m_apertureMin + m_receptorRange * double(i + 1);
// 				const double initLength = receptorMax - receptorMin;
// 				if (colorsInReceptors[i].colorsAndFractions.size() == 0) {
// 					// This is the first color in the receptor, we have to initialize the interval
// 					colorsInReceptors[i].curInterval.unite(SimpleInterval(receptorMin, receptorMax));
// 				}
//
// 				const double curLength = colorsInReceptors[i].curInterval.length();
// 				colorsInReceptors[i].curInterval.subtract(SimpleInterval(it->minAngle, it->maxAngle));
// 				const double newLength = colorsInReceptors[i].curInterval.length();
// 				const double fraction = min(1.0, (curLength - newLength) / initLength);
// 				colorsInReceptors[i].colorsAndFractions.append(ColorsInReceptor::ColorAndFraction(it->color, fraction));
// 			}
// 		}
//
// 		// The final step is to compute the resulting color for each receptor. See class description for a comment
// 		// on this procedure
// 		for (unsigned int i = 0; i < m_numReceptors; i++) {
// 			double red = 0.0;
// 			double green = 0.0;
// 			double blue = 0.0;
// 			for (QList<ColorsInReceptor::ColorAndFraction>::const_iterator it = colorsInReceptors[i].colorsAndFractions.begin(); it != colorsInReceptors[i].colorsAndFractions.end(); ++it) {
// 				red += it->color.redF() * it->fraction;
// 				green += it->color.greenF() * it->fraction;
// 				blue += it->color.blueF() * it->fraction;
// 			}
// 			m_receptors[i] = QColor::fromRgbF(min(1.0f, max(0.0f, red)), min(1.0f, max(0.0f, green)), min(1.0f, max(0.0f, blue)));
// 		}
//
// 		// Updating graphics if we have to
// 		if (m_drawCamera) {
// 			m_graphicalCamera->setPerceivedColors(m_receptors);
// 		}
// 	}
//
// 	void LinearCamera::drawCamera(bool d)
// 	{
// 		if (m_drawCamera == d) {
// 			return;
// 		}
//
// 		m_drawCamera = d;
// 		if (m_drawCamera) {
// 			m_graphicalCamera = new LinearCameraGraphic(m_object, m_transformation, m_apertureMin, m_apertureMax, m_numReceptors, "linearCamera");
// 		} else {
// 			delete m_graphicalCamera;
// 		}
// 	}
//
// 	void LinearCamera::resourceChanged(QString resourceName, ResourceChangeType changeType)
// 	{
// 		if (resourceName == "arena") {
// 			switch (changeType) {
// 				case Created:
// 				case Modified:
// 					m_arena = getResource<Arena>();
// 					break;
// 				case Deleted:
// 					m_arena = NULL;
// 					break;
// 			}
// 		} else {
// 			Logger::info("Unknown resource " + resourceName + " (in LinearCamera)");
// 		}
// 	}
// }
//
// namespace LinearCameraNew {
// 	namespace __LinearCamera_internal {
// 		#ifndef GLMultMatrix
// 		#define GLMultMatrix glMultMatrixf
// 		// for double glMultMatrixd
// 		#endif
//
// 		/**
// 		 * \brief The side of the cube representing the linear camera
// 		 */
// 		const float linearCameraCubeSide = 0.02f;
//
// 		/**
// 		 * \brief The length of the lines representing borders of the receptors
// 		 *        of the linear camera
// 		 */
// 		const float linearCameraReceptorsLength = 0.1f;
//
// 		/**
// 		 * \brief The graphical representation of the linear camera
// 		 */
// 		class LinearCameraGraphic : public GraphicalWObject
// 		{
// 		public:
// 			/**
// 			 * \brief Constructor
// 			 *
// 			 * This also sets the object to be our owner
// 			 * \param object the object to which we are attached.
// 			 * \param transformation the transformation matrix relative the
// 			 *                       one of the object to which we are
// 			 *                       attached
// 			 * \param minAngle the minimum angle of the camera
// 			 * \param maxAngle the maximum angle of the camera
// 			 * \param numReceptors the number of receptors
// 			 * \param name the name of this object
// 			 */
// 			LinearCameraGraphic(WObject *object, const wMatrix& transformation, QVector<SimpleInterval> receptorsRanges, QString name = "unamed") :
// 				GraphicalWObject(object->world(), name),
// 				m_object(object),
// 				m_receptorsRanges(receptorsRanges),
// 				m_receptors(m_receptorsRanges.size(), Qt::black),
// 				m_receptorsMutex()
// 			{
// 				// Attaching to object (which also becomes our owner)
// 				attachToObject(m_object, true, transformation);
//
// 				// We also use our own color and texture
// 				setUseColorTextureOfOwner(false);
// 				setTexture("");
// 				setColor(Qt::white);
// 			}
//
// 			/**
// 			 * \brief Destructor
// 			 */
// 			~LinearCameraGraphic()
// 			{
// 			}
//
// 			/**
// 			 * \brief Sets the colors perceived by the camera
// 			 *
// 			 * This function is thread-safe
// 			 * \param receptors the colors perceived by receptors
// 			 */
// 			void setPerceivedColors(const QVector<QColor>& receptors)
// 			{
// 				m_receptorsMutex.lock();
// 					m_receptors = receptors;
// 				m_receptorsMutex.unlock();
// 			}
//
// 		protected:
// 			/**
// 			 * \brief Performs the actual drawing
// 			 *
// 			 * \param renderer the RenderWObject object associated with this one.
// 			 *                 Use it e.g. to access the container
// 			 * \param gw the OpenGL context
// 			 */
// 			virtual void render(RenderWObject* renderer, QGLContext* gw)
// 			{
// 				// Bringing the frame of reference at the center of the camera
// 				glPushMatrix();
// 				renderer->container()->setupColorTexture(gw, renderer);
// 				GLMultMatrix(&tm[0][0]);
//
// 				// First of all drawing the camera as a small white box. The face in the
// 				// direction of view (X axis) is painted half green: the green part is the
// 				// one in the direction of the upvector (Z axis)
// 				glBegin(GL_QUADS);
// 				const float hside = linearCameraCubeSide / 2.0;
//
// 				// front (top part)
// 				glColor3f(0.0, 1.0, 0.0);
// 				glNormal3f(1.0, 0.0, 0.0);
// 				glVertex3f( hside, -hside,  hside);
// 				glVertex3f( hside, -hside,    0.0);
// 				glVertex3f( hside,  hside,    0.0);
// 				glVertex3f( hside,  hside,  hside);
//
// 				// front (bottom part)
// 				glColor3f(1.0, 1.0, 1.0);
// 				glNormal3f(1.0, 0.0, 0.0);
// 				glVertex3f( hside, -hside,    0.0);
// 				glVertex3f( hside, -hside, -hside);
// 				glVertex3f( hside,  hside, -hside);
// 				glVertex3f( hside,  hside,    0.0);
//
// 				// back
// 				glNormal3f(-1.0, 0.0, 0.0);
// 				glVertex3f(-hside, -hside, -hside);
// 				glVertex3f(-hside, -hside,  hside);
// 				glVertex3f(-hside,  hside,  hside);
// 				glVertex3f(-hside,  hside, -hside);
//
// 				// top
// 				glNormal3f(0.0, 1.0, 0.0);
// 				glVertex3f(-hside,  hside,  hside);
// 				glVertex3f( hside,  hside,  hside);
// 				glVertex3f( hside,  hside, -hside);
// 				glVertex3f(-hside,  hside, -hside);
//
// 				// bottom
// 				glNormal3f(0.0, -1.0, 0.0);
// 				glVertex3f(-hside, -hside, -hside);
// 				glVertex3f( hside, -hside, -hside);
// 				glVertex3f( hside, -hside,  hside);
// 				glVertex3f(-hside, -hside,  hside);
//
// 				// right
// 				glNormal3f(0.0, 0.0, 1.0);
// 				glVertex3f( hside, -hside,  hside);
// 				glVertex3f(-hside, -hside,  hside);
// 				glVertex3f(-hside,  hside,  hside);
// 				glVertex3f( hside,  hside,  hside);
//
// 				// left
// 				glNormal3f(0.0, 0.0, -1.0);
// 				glVertex3f( hside, -hside, -hside);
// 				glVertex3f(-hside, -hside, -hside);
// 				glVertex3f(-hside,  hside, -hside);
// 				glVertex3f( hside,  hside, -hside);
//
// 				glEnd();
//
// 				// Now we draw white lines to separare the various sectors of the camera
// 				// Disabling lighting here (we want pure lines no matter from where we look at them)
// 				glPushAttrib(GL_LIGHTING_BIT);
// 				glDisable(GL_LIGHTING);
// 				glLineWidth(2.5);
// 				glColor3f(1.0, 1.0, 1.0);
//
// 				// Drawing the lines
// 				glBegin(GL_LINES);
// 				for (int i = 0; i < m_receptorsRanges.size(); i++) {
// 					const wVector line1End = wVector(cos(m_receptorsRanges[i].start), sin(m_receptorsRanges[i].start), 0.0).scale(linearCameraReceptorsLength);
// 					const wVector line2End = wVector(cos(m_receptorsRanges[i].end), sin(m_receptorsRanges[i].end), 0.0).scale(linearCameraReceptorsLength);
//
// 					glVertex3f(0.0, 0.0, 0.0);
// 					glVertex3f(line1End.x, line1End.y, line1End.z);
// 					glVertex3f(0.0, 0.0, 0.0);
// 					glVertex3f(line2End.x, line2End.y, line2End.z);
// 				}
// 				glEnd();
//
// 				// Now drawing the state of receptors. Here we also have to lock the semaphore for
// 				// the m_receptors vector
// 				m_receptorsMutex.lock();
//
// 				// Drawing the status
// 				glBegin(GL_QUADS);
// 				glNormal3f(0.0, 1.0, 0.0);
// 				const double colorPatchMinLength = linearCameraReceptorsLength / 3.0;
// 				const double colorPatchMaxLength = 2.0 * linearCameraReceptorsLength / 3.0;
// 				for (int i = 0; i < m_receptorsRanges.size(); i++) {
// 					const double colorPatchAngle = m_receptorsRanges[i].length() / 3.0;
// 					const double curAngle = m_receptorsRanges[i].start;
//
// 					for (unsigned int c = 0; c < 3; c++) {
// 						const double startAngle = curAngle + double(c) * colorPatchAngle;
// 						const double endAngle = curAngle + double(c + 1) * colorPatchAngle;
//
// 						// Computing the four vertexes
// 						const wVector v1 = wVector(cos(startAngle), sin(startAngle), 0.0).scale(colorPatchMinLength);
// 						const wVector v2 = wVector(cos(startAngle), sin(startAngle), 0.0).scale(colorPatchMaxLength);
// 						const wVector v3 = wVector(cos(endAngle), sin(endAngle), 0.0).scale(colorPatchMaxLength);
// 						const wVector v4 = wVector(cos(endAngle), sin(endAngle), 0.0).scale(colorPatchMinLength);
//
// 						// Setting the color
// 						switch (c) {
// 							case 0:
// 								glColor3f(m_receptors[i].redF(), 0.0, 0.0);
// 								break;
// 							case 1:
// 								glColor3f(0.0, m_receptors[i].greenF(), 0.0);
// 								break;
// 							case 2:
// 								glColor3f(0.0, 0.0, m_receptors[i].blueF());
// 								break;
// 							default:
// 								break;
// 						}
//
// 						// Drawing the patch
// 						glVertex3f(v1.x, v1.y, v1.z);
// 						glVertex3f(v2.x, v2.y, v2.z);
// 						glVertex3f(v3.x, v3.y, v3.z);
// 						glVertex3f(v4.x, v4.y, v4.z);
// 					}
// 				}
// 				glEnd();
// 				m_receptorsMutex.unlock();
//
// 				// Restoring lighting status
// 				glPopAttrib();
//
// 				glPopMatrix();
// 			}
//
// 			/**
// 			 * \brief The object to which we are attached
// 			 */
// 			WObject* const m_object;
//
// 			/**
// 			 * \brief The list of receptors
// 			 */
// 			const QVector<SimpleInterval> m_receptorsRanges;
//
// 			/**
// 			 * \brief The vector with perceived colors
// 			 */
// 			QVector<QColor> m_receptors;
//
// 			/**
// 			 * \brief The mutex protecting the m_receptors variable
// 			 *
// 			 * The variable could be accessed by multiple threads
// 			 * concurrently, so we protect it with a mutex
// 			 */
// 			QMutex m_receptorsMutex;
// 		};
// 	}
//
// 	using namespace __LinearCamera_internal;
//
// 	namespace {
// 		// This namespace contains utility functions used in the constructor
//
// 		// Generates a list of receptors ranges from aperture and number of receptors
// 		QVector<SimpleInterval> receptorsFromApertureAndNumReceptors(double aperture, unsigned int numReceptors)
// 		{
// 			QVector<SimpleInterval> r;
// 			// Clamping aperture in the interval [0, 2pi]
// 			aperture = ((aperture > (2.0 * PI_GRECO)) ? (2.0 * PI_GRECO) : ((aperture < 0.0) ? 0.0 :  aperture));
//
// 			const double apertureMin = -aperture / 2.0;
// 			const double receptorRange = aperture / double(numReceptors);
//
// 			for (unsigned int i = 0; i < numReceptors; i++) {
// 				r.append(SimpleInterval(apertureMin + i * receptorRange, apertureMin + (i + 1) * receptorRange));
// 			}
//
// 			return r;
// 		}
// 	}
//
// 	LinearCamera::LinearCamera(WObject* obj, wMatrix mtr, double aperture, unsigned int numReceptors, double maxDistance, QColor backgroundColor) :
// 		ConcurrentResourcesUser(),
// 		m_receptors(numReceptors),
// 		m_object(obj),
// 		m_transformation(mtr),
// 		m_receptorsRanges(receptorsFromApertureAndNumReceptors(aperture, numReceptors)),
// 		m_maxDistance(maxDistance),
// 		m_backgroundColor(backgroundColor),
// 		m_arena(NULL),
// 		m_drawCamera(false),
// 		m_ignoreWalls(false),
// 		m_graphicalCamera(NULL)
//
// 	{
// 		// Stating which resources we use here
// 		addUsableResource("arena");
// 	}
//
// 	LinearCamera::LinearCamera(WObject* obj, wMatrix mtr, QVector<SimpleInterval> receptorsRanges, double maxDistance, QColor backgroundColor) :
// 		ConcurrentResourcesUser(),
// 		m_receptors(receptorsRanges.size()),
// 		m_object(obj),
// 		m_transformation(mtr),
// 		m_receptorsRanges(receptorsRanges),
// 		m_maxDistance(maxDistance),
// 		m_backgroundColor(backgroundColor),
// 		m_arena(NULL),
// 		m_drawCamera(false),
// 		m_ignoreWalls(false),
// 		m_graphicalCamera(NULL)
//
// 	{
// 		// Stating which resources we use here
// 		addUsableResource("arena");
// 	}
//
// 	LinearCamera::~LinearCamera()
// 	{
// 		// Nothing to do here
// 	}
//
// 	namespace {
// 		// This namespace contains some structures used in the LinearCamera::update() function
//
// 		// The structure containing a color and the range of the camera field hit by this color.
// 		// It also contains the distance from the camera for ordering.
// 		struct ColorRangeAndDistance
// 		{
// 			ColorRangeAndDistance() :
// 				color(),
// 				minAngle(0.0),
// 				maxAngle(0.0),
// 				distance(0.0)
// 			{
// 			}
//
// 			ColorRangeAndDistance(QColor c, double min, double max, double d) :
// 				color(c),
// 				minAngle(min),
// 				maxAngle(max),
// 				distance(d)
// 			{
// 			}
//
// 			// This is to order objects of this type
// 			bool operator<(const ColorRangeAndDistance& other) const
// 			{
// 				return (distance < other.distance);
// 			}
//
// 			QColor color;
// 			double minAngle;
// 			double maxAngle;
// 			double distance;
// 		};
//
// 		// An helper structure memorizing information about colors in a single receptor. curInterval is
// 		// the interval which is not covered by any color, while colorsAndFractions is the list of
// 		// colors and the portion of the receptor occupied by that color
// 		struct ColorsInReceptor
// 		{
// 			Intervals curInterval;
// 			real initialLength;
//
// 			struct ColorAndFraction {
// 				ColorAndFraction() :
// 					color(),
// 					fraction(0.0)
// 				{
// 				}
//
// 				ColorAndFraction(QColor c, double f) :
// 					color(c),
// 					fraction(f)
// 				{
// 				}
//
// 				QColor color;
// 				double fraction;
// 			};
// 			QList<ColorAndFraction> colorsAndFractions;
// 		};
// 	}
//
// 	void LinearCamera::update()
// 	{
// 		// Getting the list of objects from the arena (if we have the pointer to the arena)
// 		if (m_arena == NULL) {
// 			m_receptors.fill(m_backgroundColor);
//
// 			return;
// 		}
// 		const QVector<PhyObject2DWrapper*>& objectsList = m_arena->getObjects();
//
// 		// If no object is present, we can fill the receptors list with background colors and return
// 		if (objectsList.size() == 0) {
// 			m_receptors.fill(m_backgroundColor);
//
// 			return;
// 		}
//
// 		// Updating the matrix with the current camera position
// 		wMatrix currentMtr = m_transformation * m_object->matrix();
//
// 		// First of all we need to compute which color hits each receptor
//
// 		// Now filling the list with colors, ranges and distances. If an object is perceived at the
// 		// extremities of the aperture, it is split in two different ColorRangeAndDistance objects
// 		QList<ColorRangeAndDistance> colorsRangesAndDistances;
//
// 		// For the moment we use the distance to order objects (see ColorRangeAndDistance::operator<), however
// 		// this is not correct (occlusion doesn't work well) and so should be changed
// 		for (int i = 0; i < objectsList.size(); i++) {
// 			// Checking if we have to ignore a wall
// 			if (m_ignoreWalls && (objectsList[i]->type() == PhyObject2DWrapper::Wall)) {
// 				continue;
// 			} else if (m_object == objectsList[i]->wObject()) {
// 				// Skipping checks with self
// 				continue;
// 			}
//
// 			QVector<PhyObject2DWrapper::AngularRangeAndColor> rangesAndColors;
// 			double distance;
// 			objectsList[i]->computeLinearViewFieldOccupiedRange(currentMtr, rangesAndColors, distance, m_maxDistance);
//
// 			// computeLinearViewFieldOccupiedRange returns a negative distance if the object is outside the view field
// 			if ((distance < 0.0) || (distance > m_maxDistance)) {
// 				continue;
// 			}
//
// 			for (QVector<PhyObject2DWrapper::AngularRangeAndColor>::const_iterator it = rangesAndColors.constBegin(); it != rangesAndColors.end(); ++it) {
// 				// To safely compare with the aperture, we have to convert angles between -PI_GRECO and PI_GRECO
// 				SALSA_DEBUG_TEST_INVALID(it->minAngle) SALSA_DEBUG_TEST_INVALID(it->maxAngle)
// 				const double minAngle = normalizeRad(it->minAngle);
// 				const double maxAngle = normalizeRad(it->maxAngle);
// 				const QColor color = it->color;
//
// 				// If the minAngle is greater than the maxAngle, splitting in two, so that we do not have to
// 				// make special cases in the subsequent part of the function.
// 				if (minAngle > maxAngle) {
// 					colorsRangesAndDistances.append(ColorRangeAndDistance(color, minAngle, PI_GRECO, distance));
// 					colorsRangesAndDistances.append(ColorRangeAndDistance(color, -PI_GRECO, maxAngle, distance));
// 				} else {
// 					colorsRangesAndDistances.append(ColorRangeAndDistance(color, minAngle, maxAngle, distance));
// 				}
// 			}
// 		}
//
// 		// Ordering colors by distance from the camera
// 		qSort(colorsRangesAndDistances);
//
// 		// Now we can add the background color at the end of the list. It covers all receptors to be sure to fill
// 		// the whole field with a valid color
// 		colorsRangesAndDistances.append(ColorRangeAndDistance(m_backgroundColor, -PI_GRECO, PI_GRECO, std::numeric_limits<double>::infinity()));
//
// 		// The next step is to calculate the percentage of each color in the colorsRangesAndDistances list
// 		// in each receptor. Before doing it we initialize the colorsInReceptors list so that the current
// 		// interval is equal to the receptor range
// 		QVector<ColorsInReceptor> colorsInReceptors(getNumReceptors());
// 		QVector<ColorsInReceptor>::iterator colorIt = colorsInReceptors.begin();
// 		QVector<SimpleInterval>::const_iterator recpIt = m_receptorsRanges.constBegin();
// 		for (; colorIt != colorsInReceptors.end(); ++colorIt, ++recpIt) {
// 			// Normalizing the receptor range between -PI_GRECO and PI_GRECO
// 			const double receptorMinAngle = normalizeRad(recpIt->start);
// 			const double receptorMaxAngle = normalizeRad(recpIt->end);
//
// 			// Checking if the receptor crosses -PI_GRECO. If so we split the interval in two
// 			if (receptorMinAngle > receptorMaxAngle) {
// 				// The receptor crosses -PI_GRECO
// 				colorIt->curInterval.unite(SimpleInterval(receptorMinAngle, PI_GRECO)).unite(SimpleInterval(-PI_GRECO, receptorMaxAngle));
// 			} else {
// 				colorIt->curInterval.unite(SimpleInterval(receptorMinAngle, receptorMaxAngle));
// 			}
// 			colorIt->initialLength = colorIt->curInterval.length();
// 		}
// 		for (QList<ColorRangeAndDistance>::const_iterator colRangeIt = colorsRangesAndDistances.begin(); colRangeIt != colorsRangesAndDistances.end(); ++colRangeIt) {
// 			for (colorIt = colorsInReceptors.begin(); colorIt != colorsInReceptors.end(); ++colorIt) {
// 				const real curLength = colorIt->curInterval.length();
// 				colorIt->curInterval -= SimpleInterval(colRangeIt->minAngle, colRangeIt->maxAngle);
// 				const real newLength = colorIt->curInterval.length();
// #if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
// 	#warning PROVARE A VEDERE QUANTE VOLTE curLength È DIVERSO DA newLength ANCHE SE DOVREBBE ESSERE UGUALE (PER ERRORI NUMERICI)
// #endif
// 				if (curLength != newLength) {
// 					const real fraction = min(1.0, (curLength - newLength) / colorIt->initialLength);
// 					colorIt->colorsAndFractions.append(ColorsInReceptor::ColorAndFraction(colRangeIt->color, fraction));
// 				}
// 			}
// 		}
//
// 		// The final step is to compute the resulting color for each receptor. See class description for a comment
// 		// on this procedure
// 		QVector<ColorsInReceptor>::const_iterator colorIt2 = colorsInReceptors.constBegin();
// 		QVector<QColor>::iterator recpActIt = m_receptors.begin();
// 		for (; colorIt2 != colorsInReceptors.end(); ++colorIt2, ++recpActIt) {
// 			double red = 0.0;
// 			double green = 0.0;
// 			double blue = 0.0;
// 			for (QList<ColorsInReceptor::ColorAndFraction>::const_iterator it = colorIt2->colorsAndFractions.begin(); it != colorIt2->colorsAndFractions.end(); ++it) {
// 				red += it->color.redF() * it->fraction;
// 				green += it->color.greenF() * it->fraction;
// 				blue += it->color.blueF() * it->fraction;
// 			}
// 			*recpActIt = QColor::fromRgbF(min(1.0f, max(0.0f, red)), min(1.0f, max(0.0f, green)), min(1.0f, max(0.0f, blue)));
// 		}
//
// 		// Updating graphics if we have to
// 		if (m_drawCamera) {
// 			m_graphicalCamera->setPerceivedColors(m_receptors);
// 		}
// 	}
//
// 	void LinearCamera::drawCamera(bool d)
// 	{
// 		if (m_drawCamera == d) {
// 			return;
// 		}
//
// 		m_drawCamera = d;
// 		if (m_drawCamera) {
// 			m_graphicalCamera = new LinearCameraGraphic(m_object, m_transformation, m_receptorsRanges, "linearCamera");
// 		} else {
// 			delete m_graphicalCamera;
// 		}
// 	}
//
// 	void LinearCamera::resourceChanged(QString resourceName, ResourceChangeType changeType)
// 	{
// 		if (resourceName == "arena") {
// 			switch (changeType) {
// 				case Created:
// 				case Modified:
// 					m_arena = getResource<Arena>();
// 					break;
// 				case Deleted:
// 					m_arena = NULL;
// 					break;
// 			}
// 		} else {
// 			Logger::info("Unknown resource " + resourceName + " (in LinearCamera)");
// 		}
// 	}
// }

SampledIRDataLoader::SampledIRDataLoader(QString filename) :
	m_filename(filename),
	m_numIR(0),
	m_numSamplingAngles(0),
	m_numDistances(0),
	m_initialDistance(0.0f),
	m_distanceInterval(0.0f),
	m_finalDistance(0.0f),
	m_activations(),
	m_nullActivations()
{
	// The maximum length of a line. This value is greater than needed, we use it just
	// to avoid problems with maalformed files
	const int maxLineLength = 1024;

	// Opening the input file
	QFile file(m_filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		throw SampleFileLoadingException(m_filename.toLatin1().data(), "Cannot open file for reading");
	}

	// Now opening a text stream on the file to read it
	QTextStream in(&file);

	// Reading the first line, the one with configuration parameters and splitting it
	QStringList confs = in.readLine(maxLineLength).split(" ", QString::SkipEmptyParts);
	if (confs.size() != 5) {
		throw SampleFileLoadingException(m_filename.toLatin1().data(), ("Wrong format for the first line, expected 5 elements, got " + QString::number(confs.size())).toLatin1().data());
	}

	// Now converting the elements of the configuration line
	bool ok;
	m_numIR = confs[0].toUInt(&ok);
	if (!ok) {
		throw SampleFileLoadingException(m_filename.toLatin1().data(), ("Error reading the first element of the first row: expected an unsigned integer, got \"" + confs[0] + "\"").toLatin1().data());
	}
	m_numSamplingAngles = confs[1].toUInt(&ok);
	if (!ok) {
		throw SampleFileLoadingException(m_filename.toLatin1().data(), ("Error reading the second element of the first row: expected an unsigned integer, got \"" + confs[1] + "\"").toLatin1().data());
	}
	m_numDistances = confs[2].toUInt(&ok);
	if (!ok) {
		throw SampleFileLoadingException(m_filename.toLatin1().data(), ("Error reading the third element of the first row: expected an unsigned integer, got \"" + confs[2] + "\"").toLatin1().data());
	}
	m_initialDistance = confs[3].toFloat(&ok) / 1000.0;
	if (!ok) {
		throw SampleFileLoadingException(m_filename.toLatin1().data(), ("Error reading the fourth element of the first row: expected a real number, got \"" + confs[3] + "\"").toLatin1().data());
	}
	m_distanceInterval = confs[4].toFloat(&ok) / 1000.0;
	if (!ok) {
		throw SampleFileLoadingException(m_filename.toLatin1().data(), ("Error reading the fifth element of the first row: expected a real number, got \"" + confs[4] + "\"").toLatin1().data());
	}
	m_finalDistance = m_initialDistance + (m_numDistances - 1) * m_distanceInterval;

	// Resizing the vector of activations
	m_activations.resize(m_numIR * m_numSamplingAngles * m_numDistances);
	m_nullActivations.fill(0, m_numIR);

	// Now reading the blocks. I use the id after "TURN" for a safety check, the original evorobot code used that
	// in a "creative" way...
	int i = 0; // The index over the m_activations array
	for (unsigned int dist = 0; dist < m_numDistances; dist++) {
		QString turnLine = in.readLine(maxLineLength);
		QStringList turnLineSplitted = turnLine.split(" ", QString::SkipEmptyParts);

		// The line we just read should have been split in two. The first element should
		// be equal to "TURN", the second one to the current dist
		if ((turnLineSplitted.size() != 2) || (turnLineSplitted[0] != "TURN") || (turnLineSplitted[1].toUInt() != dist)) {
			throw SampleFileLoadingException(m_filename.toLatin1().data(), ("Invalid TURN line: \"" + turnLine + "\"").toLatin1().data());
		}

		// Now reading the block for the current distance
		for (unsigned int ang = 0; ang < m_numSamplingAngles; ang++) {
			QString activationsLine = in.readLine(maxLineLength);
			QStringList activationsLineSplitted = activationsLine.split(" ", QString::SkipEmptyParts);

			// activationsLineSplitted should have m_numIR elements, all integers between 0 and 1023
			if (activationsLineSplitted.size() != int(m_numIR)) {
				throw SampleFileLoadingException(m_filename.toLatin1().data(), ("Invalid activations line (wrong number of elements, expected " + QString::number(m_numIR) + ", got " + QString::number(activationsLineSplitted.size()) + "): \"" + activationsLine + "\"").toLatin1().data());
			}
			// Reading activations
			for (unsigned int id = 0; id < m_numIR; id++) {
				bool ok;
				const unsigned int act = activationsLineSplitted[id].toUInt(&ok);
				if ((!ok) || (act > 1023)) {
					throw SampleFileLoadingException(m_filename.toLatin1().data(), ("Invalid activations line (invalid activation value): \"" + activationsLineSplitted[id] + "\"").toLatin1().data());
				}
				m_activations[i++] = act;
			}
		}
	}

	// The final row in the file should be "END"
	QString finalLine = in.readLine(maxLineLength);
	if (finalLine != "END") {
		throw SampleFileLoadingException(m_filename.toLatin1().data(), ("The last line in the file should be \"END\", actual value: \"" + finalLine + "\"").toLatin1().data());
	}
}

SampledIRDataLoader::~SampledIRDataLoader()
{
	// Nothing to do here
}

unsigned int SampledIRDataLoader::getActivation(unsigned int i, real dist, real ang) const
{
	// Using the other version of the getActivation function
	QVector<unsigned int>::const_iterator it = getActivation(dist, ang);

	return *(it + i);
}

QVector<unsigned int>::const_iterator SampledIRDataLoader::getActivation(real dist, real ang) const
{
	const real distIndex = (dist - m_initialDistance) / m_distanceInterval;
	const unsigned int d = (distIndex < 0.0) ? 0 : (unsigned int) distIndex;

	// If we are over the maximum distance, returning all zeros
	if (d >= m_numDistances) {
		return m_nullActivations.begin();
	}

	// We first have to restrict the angle between 0.0 and 2*PI, then we can compute the index.
	const real normAng = normalizeRad02pi(ang);
	const real angIndex = (normAng / (2.0 * PI_GRECO)) * real(m_numSamplingAngles);
	unsigned int a = (angIndex < 0.0) ? 0 : (unsigned int) angIndex;
	if (a >= m_numSamplingAngles) {
		a = m_numSamplingAngles - 1;
	}

	return m_activations.begin() + getLinearIndex(0, a, d);
}

unsigned int SampledIRDataLoader::getLinearIndex(unsigned int id, unsigned int ang, unsigned int dist) const
{
	// Inverting ang, positive angles in the file are clockwise angles
	ang = m_numSamplingAngles - ang - 1;
	return (dist * m_numSamplingAngles + ang) * m_numIR + id;
}

// QColor getColorAtArenaGroundPosition(Arena* arena, wVector pos)
// {
// 	// Bringing the point on the plane
// 	pos.z = 0.0;
//
// 	// Taking the arena plane color by default
// 	QColor color = arena->getPlane()->color();
//
// 	// Now cycling through the objects in the arena
// 	const QVector<PhyObject2DWrapper*>& objectsList = arena->getObjects();
// 	foreach(PhyObject2DWrapper* obj, objectsList)
// 	{
// 		switch (obj->type())
// 		{
// 			case PhyObject2DWrapper::RectangularTargetArea:
// 				{
// 					Box2DWrapper* rectangularTargetArea = dynamic_cast<Box2DWrapper*>(obj);
//
// 					// Box properties
// 					const wVector center = rectangularTargetArea->centerOnPlane();
// 					const real halfWidth = rectangularTargetArea->width() / 2.0f;
// 					const real halfDepth = rectangularTargetArea->depth() / 2.0f;
// 					if ((pos.x >= center.x - halfWidth) && (pos.x <= center.x + halfWidth) &&
// 					    (pos.y >= center.y - halfDepth) && (pos.y <= center.y + halfDepth)) {
// 						color = rectangularTargetArea->color();
// 					}
// 				}
// 				break;
// 			case PhyObject2DWrapper::CircularTargetArea:
// 				{
// 					Cylinder2DWrapper* circularTargetArea = dynamic_cast<Cylinder2DWrapper*>(obj);
//
// 					wVector areaCenterOnPlane = circularTargetArea->position();
// 					areaCenterOnPlane.z = 0.0;
// 					if ((pos - areaCenterOnPlane).norm() <= circularTargetArea->radius()) {
// 						color = circularTargetArea->color();
// 					}
// 				}
// 				break;
// 			default:
// 				break;
// 		}
// 	}
//
// 	return color;
// }

} // end namespace salsa
