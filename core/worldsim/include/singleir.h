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

#ifndef SINGLEIR_H
#define SINGLEIR_H

#include "ownerfollower.h"
#include "worldhelpers.h"

namespace salsa {

class World;
class RenderSingleIR;

/**
 * \brief The shared data for the SingleIR
 */
class SALSA_WSIM_TEMPLATE SingleIRShared : public OwnerFollowerShared
{
public:
	/**
	 * \brief Constructor
	 *
	 * This initializes all fields to the default value
	 */
	SingleIRShared()
		: OwnerFollowerShared()
		, minDist(0.0)
		, maxDist(0.0)
		, aperture(0.0)
		, numRays(0)
		, drawSensor(false)
		, drawRay(false)
		, drawRealRay(false)
		, startingRayPoints()
		, endingRayPoints()
		, rayCastHit()
	{
	}

	/**
	 * \brief The distance below which collisions are not reported
	 */
	double minDist;

	/**
	 * \brief The distance above which collision are not reported
	 */
	double maxDist;

	/**
	 * \brief The aperture of the sensor in degrees
	 */
	double aperture;

	/**
	 * \brief The number of rays used to simulate the sensor
	 */
	unsigned int numRays;

	/**
	 * \brief If true the sensor is drawn
	 */
	bool drawSensor;

	/**
	 * \brief If true sensor rays are drawn
	 */
	bool drawRay;

	/**
	 * \brief If true the actual ray range is drawn, otherwise only the
	 *        direction of the rays is given
	 */
	bool drawRealRay;

	/**
	 * \brief The vector with starting points of the rays
	 *
	 * The points are in the frame of reference of the solid to which this
	 * sensor is attached
	 */
	QVector<wVector> startingRayPoints;

	/**
	 * \brief The vector with ending points of the rays
	 *
	 * The points are in the frame of reference of the solid to which this
	 * sensor is attached
	 */
	QVector<wVector> endingRayPoints;

	/**
	 * \brief Information about the last ray cast hit
	 *
	 * This is the structure with information about the ray cast hit that
	 * was used to compute the activation (i.e. the ray cast hit with
	 * minimal distance)
	 */
	RayCastHit rayCastHit;
};

/**
 * \brief A single proximity IR sensor
 *
 * This class models a single IR sensor. The sensor is attached to its owner
 * (see the description of the OwnerFollower class). The sensor has an aperture
 * (given in degrees) and all the rays lie on the local XZ plane. The center of
 * the aperture is in the direction of the positive Z axis. The sensor is
 * simulated using ray casts. It is possible to specify the number of rays to
 * use to simulate the sensor. Of course more rays give a more accurate response
 * but are more computationally expensive. Using a single ray implies a zero
 * aperture. The upper part of the sensor is towards positive Y
 */
class SALSA_WSIM_API SingleIR : public OwnerFollower
{
public:
	/**
	 * \brief The class with shared data
	 */
	typedef SingleIRShared Shared;

	/**
	 * \brief The class for rendering
	 */
	typedef RenderSingleIR Renderer;

	/**
	 * \brief The structure with initialization parameters
	 *
	 * A list of this is passed to sensor controllers that create SingleIRs
	 */
	struct InitParams
	{
		/**
		 * \brief Constructor
		 *
		 * This creates an invalid ir
		 */
		InitParams()
			: owner(NULL)
			, displacement()
			, minDist(0.0)
			, maxDist(0.0)
			, aperture(0.0)
			, numRays(0)
		{
		}

		/**
		 * \brief Constructor
		 *
		 * \param o the owner (i.e. the object to which we are attached)
		 * \param disp the displacement with respect to the owner
		 * \param minD the distance below which collisions are not
		 *             reported
		 * \param maxD the distance above which collisions are not
		 *             reported
		 * \param a the aperture of the sensor in degrees
		 * \param n the number of rays used to simulate the sensor. If
		 *          set to 0, creates an invalid sensor
		 */
		InitParams(WObject* o, const wMatrix& disp, double minD, double maxD, double a, unsigned int n)
			: owner(o)
			, displacement(disp)
			, minDist(minD)
			, maxDist(maxD)
			, aperture(a)
			, numRays(n)
		{
		}

		/**
		 * \brief The owner (i.e. the object to which we are attached)
		 */
		WObject* owner;

		/**
		 * \brief The displacement with respect to the owner
		 */
		wMatrix displacement;

		/**
		 * \brief The distance below which collisions are not reported
		 */
		double minDist;

		/**
		 * \brief The distance above which collisions are not reported
		 */
		double maxDist;

		/**
		 * \brief The aperture of the sensor in degrees
		 */
		double aperture;

		/**
		 * \brief The number of rays used to simulate the sensor. If set
		 *        to 0, creates an invalid sensor
		 */
		unsigned int numRays;
	};

protected:
	/**
	 * \brief Constructor
	 *
	 * \param world the world where object lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               object has been deleted)
	 * \param params the structure with initialization values
	 * \param name the name of the object
	 * \param tm rotation and position in global coordinate frame
	 */
	SingleIR(World* world, SharedDataWrapper<Shared> shared, const InitParams& params, QString name = "unamed", const wMatrix& tm = wMatrix::identity());

	/**
	 * \brief Destructor
	 */
	virtual ~SingleIR();

public:
	/**
	 * \brief Updates sensor reading
	 *
	 * This function effectively executes the ray casts and store the sensor
	 * activation and the structure with information about the ray cast hit
	 */
	void update();

	/**
	 * \brief Returns information about the last ray cast hit
	 *
	 * This function returns a const reference to the structure with
	 * information about the ray cast hit that was used to compute the
	 * activation (i.e. the ray cast hit with minimal distance). The
	 * reference is valid for the whole lifetime of this object (although
	 * the values can change after calls to update()). Use the update()
	 * function to perform a sensor reading prior to calling this function.
	 * \return the activation of the sensor (valid for the whole lifetime of
	 *         this object)
	 */
	const RayCastHit& getRayCastHit() const
	{
		return m_shared->rayCastHit;
	}

	/**
	 * \brief Configures the sensor
	 *
	 * \param minDist the distance below which collisions are not reported
	 * \param maxDist the distance above which collision are not reported
	 * \param aperture the aperture of the sensor in degrees
	 * \param numRays the number of rays used to simulate the sensor. If set
	 *                to 0, creates an invalid sensor
	 */
	void set(double minDist, double maxDist, double aperture, unsigned int numRays);

	/**
	 * \brief Sets graphical properties, i.e. whether sensors are drawn and
	 *        how
	 *
	 * By default the sensor is not drawn
	 * \param drawSensor if true the sensor is drawn
	 * \param drawRay if true sensor rays are drawn
	 * \param drawRealRay if true the actual ray range is drawn, otherwise
	 *                    only the direction of the rays is given
	 */
	void setGraphicalProperties(bool drawSensor, bool drawRay = false, bool drawRealRay = false);

	/**
	 * \brief Returns the distance below which collisions are not reported
	 *
	 * \return the distance below which collisions are not reported
	 */
	double minDistance() const
	{
		return m_shared->minDist;
	}

	/**
	 * \brief Returns the distance above which collision are not reported
	 *
	 * \return the distance above which collision are not reported
	 */
	double maxDistance() const
	{
		return m_shared->maxDist;
	}

	/**
	 * \brief Returns the aperture of the sensor in degrees
	 *
	 * \return the aperture of the sensor in degrees
	 */
	double aperture() const
	{
		return m_shared->aperture;
	}

	/**
	 * \brief Returns the number of rays used to simulate the sensor
	 *
	 * \return the number of rays used to simulate the sensor
	 */
	unsigned int numRays() const
	{
		return m_shared->numRays;
	}

	/**
	 * \brief Returns true if the sensor is valid
	 *
	 * \return true if the sensor is valid
	 */
	bool isValid() const
	{
		return (m_shared->numRays != 0);
	}

private:
	/**
	 * \brief Computes the starting and ending point of rays in the object
	 *        frame of reference
	 *
	 * This only does something if the sensor is valid.
	 */
	void computeRayPoints();

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
 * \brief The class rendering the SingleIR
 *
 * We only implement the render function, we don't provide the rendering of the
 * AABB
 */
class SALSA_WSIM_API RenderSingleIR : public RenderOwnerFollower
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
	RenderSingleIR(const SingleIR* entity);

	/**
	 * \brief Destructor
	 *
	 * Here just to declare it as virtual
	 */
	virtual ~RenderSingleIR();

	/**
	 * \brief The function performing the rendering
	 *
	 * \param sharedData the object with data from WObject to use for
	 *                   rendering
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	void render(const SingleIRShared* sharedData, GLContextAndData* contextAndData);
};

} // end namespace salsa

#endif
