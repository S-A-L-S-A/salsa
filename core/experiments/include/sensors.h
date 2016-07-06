/********************************************************************************
 *  FARSA Experimentes Library                                                  *
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

#ifndef SENSORS_H
#define SENSORS_H

#include "experimentsconfig.h"
#include "controllerinputoutput.h"
#include "world.h"
#include "robots.h"
#include "wheeledexperimenthelper.h"
#include "baseexception.h"
#include "intervals.h"
#include <QVector>

namespace farsa {

// /**
//  * \brief A class to add input neurons that can be used for custom operations
//  *
//  * This class allows to add a given number of inputs to the controller and then
//  * provides a resource to access the value of the new neurons
//  *
//  * In addition to all parameters defined by the parent class (Sensor), this
//  * class also defines the following parameters:
//  * - additionalInputs: the number of inputs that will be added to the controller
//  *                     (default 1)
//  * - neuronsIterator: the name of the resource associated with the neural
//  *                    network iterator (default is "neuronsIterator")
//  * - additionalInputsResource: the name of the resource that can be used to
//  *                             access the additional inputs (default is
//  *                             "additionalInputs")
//  *
//  * The resources required by this Sensor are:
//  * - name defined by the neuronsIterator parameter: the object to iterate over
//  *                                                  inputs of the controller
//  *
//  * This sensor also defines the following resources:
//  * - name defined by the additionalInputsResource parameter: the name of the
//  *                   resource that can be used to access the additional inputs.
//  *                   This can be accessed as a farsa::ResourceVector\<real\>
//  */
// class FARSA_EXPERIMENTS_API FakeSensor : public Sensor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Motor
// 	 */
// 	FakeSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	~FakeSensor();
//
// 	/**
// 	 * \brief Saves the parameters of the FakeSensor into the provided
// 	 *        ConfigurationParameters object
// 	 *
// 	 * \param params the ConfigurationParameters where save the parameters
// 	 * \param prefix the path prefix for the parameters to save
// 	 */
// 	virtual void save(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Describes all the parameter needed to configure this class
// 	 *
// 	 * \param type a string representation for the name of this type
// 	 */
// 	static void describe(QString type);
//
// 	/**
// 	 * \brief Updates the state of the Sensor every time step
// 	 */
// 	virtual void update();
//
// 	/**
// 	 * \brief Returns the number of inputs
// 	 *
// 	 * This returns the value set for the paramenter additionalInputs
// 	 */
// 	virtual int size();
//
// 	/**
// 	 * \brief The function to share resources
// 	 *
// 	 * The calling instance will lose the possibility to access the
// 	 * resources it had before this call
// 	 * \param other the instance with which resources will be shared. If
// 	 *              NULL we lose the association with other objects and
// 	 *              start with an empty resource set
// 	 * \note This is NOT thread safe (both this and the other instance
// 	 *       should not be being accessed by other threads)
// 	 */
// 	virtual void shareResourcesWith(ResourcesUser* other);
//
// protected:
// 	/**
// 	 * \brief The function called when a resource used here is changed
// 	 *
// 	 * \param resourceName the name of the resource that has changed.
// 	 * \param chageType the type of change the resource has gone through
// 	 *                  (whether it was created, modified or deleted)
// 	 */
// 	virtual void resourceChanged(QString resourceName, ResourceChangeType changeType);
//
// 	/**
// 	 * \brief The vector with additional inputs
// 	 */
// 	ResourceVector<real> m_additionalInputs;
//
// 	/**
// 	 * \brief The name of th resource associated with the neural network
// 	 *        iterator
// 	 */
// 	const QString m_neuronsIteratorResource;
//
// 	/**
// 	 * \brief The name of the resource associated with the vector of
// 	 *        additional inputs
// 	 */
// 	const QString m_additionalInputsResource;
//
// 	/**
// 	 * \brief The object to iterate over neurons of the neural network
// 	 */
// 	NeuronsIterator* m_neuronsIterator;
// };
//
// /**
//  * \brief It gets the three absolute coordinate (world frame) of an object
//  */
// class FARSA_EXPERIMENTS_API ObjectPositionSensor : public Sensor {
// public:
// 	/*! Constructor and Configure
// 	 *  \param params is the ConfigurationParameters containing the parameters
// 	 *  \param prefix is the path prefix to the paramters for this Sensor
// 	 */
// 	ObjectPositionSensor( ConfigurationParameters& params, QString prefix );
// 	/*! Destructor */
// 	~ObjectPositionSensor();
// 	/*! Save the parameters of the ObjectPositionSensor into the ConfigurationParameters
// 	 *  \param params is the ConfigurationParameters where save the parameters
// 	 *  \param prefix is the path prefix for the parameters to save
// 	 */
// 	void save( ConfigurationParameters& params, QString prefix );
// 	/*! Describe all the parameter for configuring the iCubArmJointsSensor */
// 	static void describe( QString type );
// 	/*! Update the state of the Sensor every time step */
// 	void update();
// 	/*! Return the number of neurons on which the Sensor will set the input: 3 */
// 	int size();
// protected:
// 	/*! The function called when a resource used here is changed
// 	 * \param resourceName the name of the resource that has changed.
// 	 * \param chageType the type of change the resource has gone through
// 	 *                  (whether it was created, modified or deleted)
// 	 */
// 	void resourceChanged(QString resourceName, ResourceChangeType changeType);
// 	/*! The name of th resource associated with the neural network iterator */
// 	QString neuronsIteratorResource;
// 	/*! the object resource name */
// 	QString objectName;
// 	/*! if true will use bbMin and bbMax to linearize the position into [0,1] */
// 	bool linearize;
// 	/*! minimum 3D point for linearization */
// 	wVector bbMin;
// 	/*! maximum 3D point for linearization */
// 	wVector bbMax;
// };
//
// /**
//  * \brief The namespace with the old implementation of the linear camera
//  */
// namespace LinearCameraOld {
// 	namespace __LinearCamera_internal {
// 		class LinearCameraGraphic;
// 	}
//
// 	/**
// 	 * \brief An helper class for linear camera sensors of various robots
// 	 *
// 	 * This class has all that is needed to implement a linear camera sensor. It is
// 	 * fully parametrized, so to implement an actual sensor one just needs to
// 	 * instantiate it with the correct set of parameters. The camera is attached to
// 	 * an object and its transformation matrix is relative to the object to which it
// 	 * is attached. The up vector is the local Z axis, the camera points towards X
// 	 * and is on the XY plane. The camera only takes into account the objects in the
// 	 * "2dobjects" resource (a list of PhyObject2DWrapper objects). The aperture and
// 	 * the number of receptors can be specified at construction time. Moreover the
// 	 * sensor takes into account visual occlusion. To work, this class need to
// 	 * access the arena object, so a resource named "arena" which points to the
// 	 * instance of the arena should be present.
// 	 *
// 	 * \note The current implementation of the camera doesn't work well with
// 	 *       occluding objects. In fact the occlusion is only computed on the basis
// 	 *       of the distance between the object center and the camera. If however
// 	 *       you have e.g. a cylinder in front of big wall, the cylinder can be more
// 	 *       distant than the wall and so being considered behind the wall. For this
// 	 *       reason it is possible to ignore all walls using the ignoreWalls()
// 	 *       function (this is usefult if e.g. you have only perimetral walls of the
// 	 *       same color as the background)
// 	 * \note When more than one object of different colors hits a single receptor,
// 	 *       here we take the average of the colors (weighted by the percentage of
// 	 *       the receptor occupied by each object). This is not physically correct,
// 	 *       so please be careful when using big receptors with a lot of objects of
// 	 *       different colors.
// 	 */
// 	class FARSA_EXPERIMENTS_API LinearCamera : public ConcurrentResourcesUser
// 	{
// 	public:
// 		/**
// 		 * \brief Constructor
// 		 *
// 		 * \param obj the object to which the sensor is attached. This cannot be
// 		 *            NULL
// 		 * \param mtr the transformation matrix relative to the object to which
// 		 *            the sensor is attached. The up vector is the local Z axis,
// 		 *            the camera points towards X and is on the XY plane.
// 		 * \param aperture the aperture of the sensor in radiants
// 		 * \param numReceptors the number of receptors
// 		 * \param maxDistance the distance above which objects are not seen
// 		 *                    anymore
// 		 * \param backgroundColor the background color (used when no object is
// 		 *                        perceived by a given receptor)
// 		 */
// 		LinearCamera(WObject* obj, wMatrix mtr, double aperture, unsigned int numReceptors, double maxDistance, QColor backgroundColor);
//
// 		/**
// 		 * \brief Destructor
// 		 */
// 		virtual ~LinearCamera();
//
// 		/**
// 		 * \brief Updates the sensor reading
// 		 */
// 		void update();
//
// 		/**
// 		 * \brief Returns the color perceived by the i-th receptor
// 		 *
// 		 * \param i the index of the photoreceptor to return
// 		 * \return the color perceived by the receptor
// 		 */
// 		const QColor& colorForReceptor(int i) const
// 		{
// 			return m_receptors[i];
// 		}
//
// 		/**
// 		 * \brief Returns the aperture of the sensor in radiants
// 		 *
// 		 * \return the aperture of the sensor in radiants
// 		 */
// 		double getAperture() const
// 		{
// 			return m_aperture;
// 		}
//
// 		/**
// 		 * \brief Returns the number of receptors
// 		 *
// 		 * \return the number of receptors
// 		 */
// 		unsigned int getNumReceptors() const
// 		{
// 			return m_numReceptors;
// 		}
//
// 		/**
// 		 * \brief Returns the background color
// 		 *
// 		 * \return the background color (i.e. the color used when no object is
// 		 *         perceived by a given receptor)
// 		 */
// 		QColor getBackgroundColor() const
// 		{
// 			return m_backgroundColor;
// 		}
//
// 		/**
// 		 * \brief Sets the background color
// 		 *
// 		 * \param color the background color (i.e. the color used when no object
// 		 *              is perceived by a given receptor)
// 		 */
// 		void setBackgroundColor(QColor color)
// 		{
// 			m_backgroundColor = color;
// 		}
//
// 		/**
// 		 * \brief Sets whether to draw the linear camera or not
// 		 *
// 		 * \param d if true the camera is drawn, if false it is not
// 		 */
// 		void drawCamera(bool d);
//
// 		/**
// 		 * \brief Sets whether walls are ignored or not
// 		 *
// 		 * See the note in the class description
// 		 * \param ignore whether to ignore walls or not
// 		 */
// 		void ignoreWalls(bool ignore)
// 		{
// 			m_ignoreWalls = ignore;
// 		}
//
// 		/**
// 		 * \brief Whether walls are ignored or not
// 		 *
// 		 * See the note in the class description
// 		 * \return true if walls are ignored, false otherwise
// 		 */
// 		bool wallsIgnored() const
// 		{
// 			return m_ignoreWalls;
// 		}
//
// 	private:
// 		/**
// 		 * \brief The function called when a resource used here is changed
// 		 *
// 		 * \param resourceName the name of the resource that has changed.
// 		 * \param chageType the type of change the resource has gone through
// 		 *                  (whether it was created, modified or deleted)
// 		 */
// 		virtual void resourceChanged(QString resourceName, ResourceChangeType changeType);
//
// 		/**
// 		 * \brief The vector with perceived colors
// 		 */
// 		QVector<QColor> m_receptors;
//
// 		/**
// 		 * \brief The object to which the sensor is attached. This cannot be
// 		 *        NULL
// 		 */
// 		WObject* const m_object;
//
// 		/**
// 		 * \brief The transformation matrix relative to the object to which the
// 		 *        sensor is attached.
// 		 *
// 		 * The up vector is the local Z axis, the camera points towards X and is
// 		 * on the XY plane
// 		 */
// 		const wMatrix m_transformation;
//
// 		/**
// 		 * \brief The aperture of the sensor in degrees
// 		 */
// 		const double m_aperture;
//
// 		/**
// 		 * \brief The number of receptors
// 		 */
// 		const unsigned int m_numReceptors;
//
// 		/**
// 		 * \brief The distance above which objects are not seen anymore
// 		 */
// 		const double m_maxDistance;
//
// 		/**
// 		 * \brief The background color
// 		 *
// 		 * This color is used when no object is perceived by a given receptor
// 		 */
// 		QColor m_backgroundColor;
//
// 		/**
// 		 * \brief The minimum angle of the aperture
// 		 *
// 		 * This is computed in the constructor.
// 		 */
// 		const double m_apertureMin;
//
// 		/**
// 		 * \brief The maximum angle of the aperture
// 		 *
// 		 * This is computed in the constructor.
// 		 */
// 		const double m_apertureMax;
//
// 		/**
// 		 * \brief The range of each receptor
// 		 *
// 		 * This is computed in the constructor.
// 		 */
// 		const double m_receptorRange;
//
// 		/**
// 		 * \brief The arena with objects to take into account
// 		 */
// 		Arena* m_arena;
//
// 		/**
// 		 * \brief Whether to draw the linear camera or not
// 		 */
// 		bool m_drawCamera;
//
// 		/**
// 		 * \brief Whether walls are ignored or not
// 		 *
// 		 * See the note in the class description
// 		 */
// 		bool m_ignoreWalls;
//
// 		/**
// 		 * \brief The graphical representation of the camera, if present
// 		 *
// 		 * This is NULL if m_drawCamera is false
// 		 */
// 		__LinearCamera_internal::LinearCameraGraphic* m_graphicalCamera;
// 	};
//
// }
//
// /**
//  * \brief The namespace with the old implementation of the linear camera
//  */
// namespace LinearCameraNew {
// 	namespace __LinearCamera_internal {
// 		class LinearCameraGraphic;
// 	}
//
// 	/**
// 	 * \brief An helper class for linear camera sensors of various robots
// 	 *
// 	 * This class has all that is needed to implement a linear camera sensor. It is
// 	 * fully parametrized, so to implement an actual sensor one just needs to
// 	 * instantiate it with the correct set of parameters. The camera is attached to
// 	 * an object and its transformation matrix is relative to the object to which it
// 	 * is attached. The up vector is the local Z axis, the camera points towards X
// 	 * and is on the XY plane. The camera only takes into account the objects in the
// 	 * "2dobjects" resource (a list of PhyObject2DWrapper objects). The number of
// 	 * receptors, their position and their aperture can be specified in two ways.
// 	 * The easiest one is to set the aperture and the number of receptors: all
// 	 * receptors will have the same range and will be placed symmetrically with
// 	 * respect to the X axis. The more general way is to supply a list of
// 	 * SimpleIntervals, where each interval represents a single receptor. This way
// 	 * you can have receptors of different sizes, overlapping receptors, blind spots
// 	 * and receptors with a custom order (the order of receptors is the same as in
// 	 * the list of intervals). Moreover the sensor takes into account visual
// 	 * occlusion. To work, this class need to access the arena object, so a resource
// 	 * named "arena" which points to the instance of the arena should be present.
// 	 *
// 	 * \note The current implementation of the camera doesn't work well with
// 	 *       occluding objects. In fact the occlusion is only computed on the basis
// 	 *       of the distance between the object center and the camera. If however
// 	 *       you have e.g. a cylinder in front of big wall, the cylinder can be more
// 	 *       distant than the wall and so being considered behind the wall. For this
// 	 *       reason it is possible to ignore all walls using the ignoreWalls()
// 	 *       function (this is usefult if e.g. you have only perimetral walls of the
// 	 *       same color as the background)
// 	 * \note When more than one object of different colors hits a single receptor,
// 	 *       here we take the average of the colors (weighted by the percentage of
// 	 *       the receptor occupied by each object). This is not physically correct,
// 	 *       so please be careful when using big receptors with a lot of objects of
// 	 *       different colors.
// 	 */
// 	class FARSA_EXPERIMENTS_API LinearCamera : public ConcurrentResourcesUser
// 	{
// 	public:
// 		/**
// 		 * \brief Constructor
// 		 *
// 		 * Use this constructor to have receptors that have the same aperture
// 		 * and are symmetrical with respect to the X axis
// 		 * \param obj the object to which the sensor is attached. This cannot be
// 		 *            NULL
// 		 * \param mtr the transformation matrix relative to the object to which
// 		 *            the sensor is attached. The up vector is the local Z axis,
// 		 *            the camera points towards X and is on the XY plane.
// 		 * \param aperture the aperture of the sensor in radiants
// 		 * \param numReceptors the number of receptors
// 		 * \param maxDistance the distance above which objects are not seen
// 		 *                    anymore
// 		 * \param backgroundColor the background color (used when no object is
// 		 *                        perceived by a given receptor)
// 		 */
// 		LinearCamera(WObject* obj, wMatrix mtr, double aperture, unsigned int numReceptors, double maxDistance, QColor backgroundColor);
//
// 		/**
// 		 * \brief Constructor
// 		 *
// 		 * Use this contructor to be able to directly specify the list of
// 		 * receptors
// 		 * \param obj the object to which the sensor is attached. This cannot be
// 		 *            NULL
// 		 * \param mtr the transformation matrix relative to the object to which
// 		 *            the sensor is attached. The up vector is the local Z axis,
// 		 *            the camera points towards X and is on the XY plane.
// 		 * \param receptorsRanges the list of ranges for receptors. Angles are
// 		 *                        in radiants
// 		 * \param maxDistance the distance above which objects are not seen
// 		 *                    anymore
// 		 * \param backgroundColor the background color (used when no object is
// 		 *                        perceived by a given receptor)
// 		 */
// 		LinearCamera(WObject* obj, wMatrix mtr, QVector<SimpleInterval> receptorsRanges, double maxDistance, QColor backgroundColor);
//
// 		/**
// 		 * \brief Destructor
// 		 */
// 		virtual ~LinearCamera();
//
// 		/**
// 		 * \brief Updates the sensor reading
// 		 */
// 		void update();
//
// 		/**
// 		 * \brief Returns the color perceived by the i-th receptor
// 		 *
// 		 * \param i the index of the photoreceptor to return
// 		 * \return the color perceived by the receptor
// 		 */
// 		const QColor& colorForReceptor(int i) const
// 		{
// 			return m_receptors[i];
// 		}
//
// 		/**
// 		 * \brief Returns the number of receptors
// 		 *
// 		 * \return the number of receptors
// 		 */
// 		unsigned int getNumReceptors() const
// 		{
// 			return m_receptors.size();
// 		}
//
// 		/**
// 		 * \brief Returns the background color
// 		 *
// 		 * \return the background color (i.e. the color used when no object is
// 		 *         perceived by a given receptor)
// 		 */
// 		QColor getBackgroundColor() const
// 		{
// 			return m_backgroundColor;
// 		}
//
// 		/**
// 		 * \brief Sets the background color
// 		 *
// 		 * \param color the background color (i.e. the color used when no object
// 		 *              is perceived by a given receptor)
// 		 */
// 		void setBackgroundColor(QColor color)
// 		{
// 			m_backgroundColor = color;
// 		}
//
// 		/**
// 		 * \brief Sets whether to draw the linear camera or not
// 		 *
// 		 * \param d if true the camera is drawn, if false it is not
// 		 */
// 		void drawCamera(bool d);
//
// 		/**
// 		 * \brief Sets whether walls are ignored or not
// 		 *
// 		 * See the note in the class description
// 		 * \param ignore whether to ignore walls or not
// 		 */
// 		void ignoreWalls(bool ignore)
// 		{
// 			m_ignoreWalls = ignore;
// 		}
//
// 		/**
// 		 * \brief Whether walls are ignored or not
// 		 *
// 		 * See the note in the class description
// 		 * \return true if walls are ignored, false otherwise
// 		 */
// 		bool wallsIgnored() const
// 		{
// 			return m_ignoreWalls;
// 		}
//
// 	private:
// 		/**
// 		 * \brief The function called when a resource used here is changed
// 		 *
// 		 * \param resourceName the name of the resource that has changed.
// 		 * \param chageType the type of change the resource has gone through
// 		 *                  (whether it was created, modified or deleted)
// 		 */
// 		virtual void resourceChanged(QString resourceName, ResourceChangeType changeType);
//
// 		/**
// 		 * \brief The vector with perceived colors
// 		 */
// 		QVector<QColor> m_receptors;
//
// 		/**
// 		 * \brief The object to which the sensor is attached. This cannot be
// 		 *        NULL
// 		 */
// 		WObject* const m_object;
//
// 		/**
// 		 * \brief The transformation matrix relative to the object to which the
// 		 *        sensor is attached.
// 		 *
// 		 * The up vector is the local Z axis, the camera points towards X and is
// 		 * on the XY plane
// 		 */
// 		const wMatrix m_transformation;
//
// 		/**
// 		 * \brief The list of receptors
// 		 */
// 		const QVector<SimpleInterval> m_receptorsRanges;
//
// 		/**
// 		 * \brief The distance above which objects are not seen anymore
// 		 */
// 		const double m_maxDistance;
//
// 		/**
// 		 * \brief The background color
// 		 *
// 		 * This color is used when no object is perceived by a given receptor
// 		 */
// 		QColor m_backgroundColor;
//
// 		/**
// 		 * \brief The arena with objects to take into account
// 		 */
// 		Arena* m_arena;
//
// 		/**
// 		 * \brief Whether to draw the linear camera or not
// 		 */
// 		bool m_drawCamera;
//
// 		/**
// 		 * \brief Whether walls are ignored or not
// 		 *
// 		 * See the note in the class description
// 		 */
// 		bool m_ignoreWalls;
//
// 		/**
// 		 * \brief The graphical representation of the camera, if present
// 		 *
// 		 * This is NULL if m_drawCamera is false
// 		 */
// 		__LinearCamera_internal::LinearCameraGraphic* m_graphicalCamera;
// 	};
// }
//
// /**
//  * \brief Using the old implementation by default
//  */
// typedef LinearCameraOld::LinearCamera LinearCamera;
// // typedef LinearCameraNew::LinearCamera LinearCamera;

/**
 * \brief The exception thrown when an error occurs during IR sample files
 *        loading
 */
class FARSA_EXPERIMENTS_TEMPLATE SampleFileLoadingException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param filename the name of the file with samples. The buffer for
	 *                 this is at most 256 characters (including the '\0'
	 *                 terminator)
	 * \param description the description of the error. The buffer for this
	 *                    is at most 256 characters (including the '\0'
	 *                    terminator)
	 */
	SampleFileLoadingException(const char* filename, const char* description) throw() :
		BaseException()
	{
		strncpy(m_filename, filename, 256);
		m_filename[255] = '\0';
		strncpy(m_description, description, 256);
		m_description[255] = '\0';
		sprintf(m_errorMessage, "Error when loading the file with IR samples \"%s\". Description: %s", m_filename, m_description);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	SampleFileLoadingException(const SampleFileLoadingException& other) throw() :
		BaseException(other)
	{
		strncpy(m_filename, other.m_filename, 256);
		m_filename[255] = '\0';
		strncpy(m_description, other.m_description, 256);
		m_description[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	SampleFileLoadingException& operator=(const SampleFileLoadingException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_filename, other.m_filename, 256);
		m_filename[255] = '\0';
		strncpy(m_description, other.m_description, 256);
		m_description[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~SampleFileLoadingException() throw()
	{
	}

	/**
	 * \brief Returns a C string describing the exception
	 *
	 * \return a C string describing the exception
	 */
	virtual const char *what() const throw()
	{
		return m_errorMessage;
	}

	/**
	 * \brief Returns the name of the file with sampled data
	 *
	 * \return the name of the file with sample data
	 */
	const char *filename() const throw()
	{
		return m_filename;
	}

	/**
	 * \brief Returns the description of the error
	 *
	 * \return the description of the error
	 */
	const char *description() const throw()
	{
		return m_description;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(SampleFileLoadingException)

private:
	/**
	 * \brief The name of the file with sample data
	 */
	char m_filename[256];

	/**
	 * \brief The description of the error
	 */
	char m_description[256];

	/**
	 * \brief The message describing the error
	 */
	char m_errorMessage[1024];
};

/**
 * \brief An helper class to load sampled data from file
 *
 * This class loads a .sam file, the file used by Evorobot to store sensor
 * samplings. The file stores the recorded activations of infrared sensors when
 * the robot is in front of an object at variuos distances and orientations. A
 * .sam file if a text file with the following format:
 *
 * 	- The first line contains information about the sampling process. It is
 * 	  made up of 5 integer values separeted by a whitespace: the number of
 * 	  infrared sensors of the robot, the number of different sampling angles
 * 	  (if e.g there are 4 angles it means the sensors are sampled when the
 * 	  robot is at 0°, 90°, 180° and 270° with respect to the object), the
 * 	  number of different distances from the object, the initial distance
 * 	  and the distance interval (i.e. how many millimeters are there between
 * 	  two subsequent positions at different distances). All lengths are in
 * 	  millimiters.
 * 	- The file then contains a number of blocks equal to the number of
 * 	  different distances from the object. Each block starts with a line
 * 	  "TURN NN" where NN is an index starting from 0. The "TURN" line is
 * 	  followed by a number of lines equal to the number of different
 * 	  sampling angles. Each line is made up of a number of different integer
 * 	  values equal to the number of infrared sensors, separated by
 * 	  whitespaces. Each integer is the activation of the corresponding
 * 	  infrared sensor (range is 0-1023)
 *
 * All distances are taken from the nearest point of the robot (not from the
 * center), while orientations are relative to the robot forward direction.
 * Angles are in radiants.
 * \note The angles in the files are taken positive clockwise, while it is more
 *       common to take angles positive counterclockwise. This class takes care
 *       of the conversion, so positive angles are considered counterclockwise
 *       as usual
 */
class FARSA_EXPERIMENTS_API SampledIRDataLoader
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param filename The name of the file with data to load
	 */
	SampledIRDataLoader(QString filename);

	/**
	 * \brief Destructor
	 */
	~SampledIRDataLoader();

	/**
	 * \brief Returns the name of the file from which samples are loaded
	 *
	 * \return the name of the file from which samples are loaded
	 */
	const QString& filename() const
	{
		return m_filename;
	}

	/**
	 * \brief Returns the number of IR sensors as read from the file
	 *
	 * \return the number of IR sensors as read from the file
	 */
	unsigned int numIR() const
	{
		return m_numIR;
	}

	/**
	 * \brief Returns the number of different sampling angles
	 *
	 * \return the number of different sampling angles
	 */
	unsigned int numSamplingAngles() const
	{
		return m_numSamplingAngles;
	}

	/**
	 * \brief Returns the number of different distances from the object
	 *
	 * \return the number of different distances from the object
	 */
	unsigned int numDistances() const
	{
		return m_numDistances;
	}

	/**
	 * \brief Returns the initial distance in meters
	 *
	 * \return the initial distance in meters
	 */
	real initialDistance() const
	{
		return m_initialDistance;
	}

	/**
	 * \brief Returns the distance interval in meters
	 *
	 * \return the distance interval in meters
	 */
	real distanceInterval() const
	{
		return m_distanceInterval;
	}

	/**
	 * \brief Returns the furthest sampled distance in meters
	 *
	 * \return the furthest sampled distance in meters
	 */
	real finalDistance() const
	{
		return m_finalDistance;
	}

	/**
	 * \brief Returns the activation of the given sensor at the given
	 *        distance and angle
	 *
	 * Sensors activations range from 0 to 1023.
	 * \param i the id of the sensor
	 * \param dist the distance from the object to the robot
	 * \param ang the orientation respect to the object
	 * \return the activation of the sensor
	 */
	unsigned int getActivation(unsigned int i, real dist, real ang) const;

	/**
	 * \brief Returns the activation at the given distance and angle
	 *
	 * Sensors activations range from 0 to 1023. This function returns an
	 * iterator pointing at the first sensor (i.e. the sensor with id 0) for
	 * the given distance and angle. You can then move the iterator forward
	 * to access the other sensors
	 * \param dist the distance from the object to the robot
	 * \param ang the orientation respect to the object
	 * \return an iterator to the first sensor for the given distance and
	 *         angle
	 */
	QVector<unsigned int>::const_iterator getActivation(real dist, real ang) const;

private:
	/**
	 * \brief Returns the index in m_activations for the given IR id,
	 *        angle and distance
	 *
	 * \param id the id of the sensor
	 * \param ang the orientation index
	 * \param dist the distance index
	 */
	unsigned int getLinearIndex(unsigned int id, unsigned int ang, unsigned int dist) const;

	/**
	 * \brief The name of the file from which samples are loaded
	 */
	const QString m_filename;

	/**
	 * \brief The number of IR sensors as read from the file
	 */
	unsigned int m_numIR;

	/**
	 * \brief The number of different sampling angles
	 */
	unsigned int m_numSamplingAngles;

	/**
	 * \brief The number of different distances from the object
	 */
	unsigned int m_numDistances;

	/**
	 * \brief The initial distance in meters
	 */
	real m_initialDistance;

	/**
	 * \brief The distance interval in meters
	 */
	real m_distanceInterval;

	/**
	 * \brief The furthest sampled distance in meters
	 */
	real m_finalDistance;

	/**
	 * \brief The vector with activations
	 *
	 * This is a linear vector containing all the data. To convert to/from
	 * linear indexes use the getLinearIndex and getIndexes functions above
	 */
	QVector<unsigned int> m_activations;

	/**
	 * \brief The vector with null activations
	 *
	 * This is a vector with all 0 values, used when returning activations
	 * further than the limit distance
	 */
	QVector<unsigned int> m_nullActivations;
};

// /**
//  * \brief An helper function to implement ground sensors that only work on
//  *        objects in the arena
//  *
//  * This function takes a position and returns the color of the object in the
//  * arena under that position. Only the color of the plane and the ground areas
//  * are taken into account
//  * \param arena the arena
//  * \param pos the position to check
//  * \return the color on the ground at the given position
//  */
// QColor FARSA_EXPERIMENTS_API getColorAtArenaGroundPosition(Arena* arena, wVector pos);

} // end namespace farsa

#endif
