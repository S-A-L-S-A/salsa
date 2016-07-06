// /********************************************************************************
//  *  FARSA Experimentes Library                                                  *
//  *  Copyright (C) 2007-2012                                                     *
//  *  Gianluca Massera <emmegian@yahoo.it>                                        *
//  *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
//  *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
//  *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
//  *                                                                              *
//  *  This program is free software; you can redistribute it and/or modify        *
//  *  it under the terms of the GNU General Public License as published by        *
//  *  the Free Software Foundation; either version 2 of the License, or           *
//  *  (at your option) any later version.                                         *
//  *                                                                              *
//  *  This program is distributed in the hope that it will be useful,             *
//  *  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
//  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
//  *  GNU General Public License for more details.                                *
//  *                                                                              *
//  *  You should have received a copy of the GNU General Public License           *
//  *  along with this program; if not, write to the Free Software                 *
//  *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA  *
//  ********************************************************************************/
//
// #ifndef MOTORS_H
// #define MOTORS_H
//
// #include "experimentsconfig.h"
// #include "neuroninterfaces.h"
// #include "musclepair.h"
// #include "robots.h"
// #include "helperresources.h"
//
// namespace farsa {
//
// /**
//  * \brief A class to add output neurons that can be used for custom operations
//  *
//  * This class allows to add a given number of outputs to the controller and then
//  * provides a resource to access the value of the new neurons
//  *
//  * In addition to all parameters defined by the parent class (Motor), this
//  * class also defines the following parameters:
//  * - additionalOutputs: the number of outputs that will be added to the
//  *                      controller (default 1)
//  * - neuronsIterator: the name of the resource associated with the neural
//  *                    network iterator (default is "neuronsIterator")
//  * - additionalOutputsResource: the name of the resource that can be used to
//  *                              access the additional outputs (default is
//  *                              "additionalOutputs")
//  *
//  * The resources required by this Motor are:
//  * - name defined by the neuronsIterator parameter: the object to iterate over
//  *                                                  outputs of the controller
//  *
//  * This motor also defines the following resources:
//  * - name defined by the additionalOutputsResource parameter: the name of the
//  *                   resource that can be used to access the additional outputs.
//  *                   This can be accessed as a farsa::ResourceVector\<real\>
//  */
// class FARSA_EXPERIMENTS_API FakeMotor : public Motor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Motor
// 	 */
// 	FakeMotor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	~FakeMotor();
//
// 	/**
// 	 * \brief Saves the parameters of the FakeMotor into the provided
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
// 	 * \brief Updates the state of the Motor every time step
// 	 */
// 	virtual void update();
//
// 	/**
// 	 * \brief Returns the number of outputs
// 	 *
// 	 * This returns the value set for the paramenter additionalOutputs
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
// 	void resourceChanged(QString resourceName, ResourceChangeType changeType);
//
// 	/**
// 	 * \brief The vector with additional outputs
// 	 */
// 	ResourceVector<real> m_additionalOutputs;
//
// 	/**
// 	 * \brief The name of th resource associated with the neural network
// 	 *        iterator
// 	 */
// 	const QString m_neuronsIteratorResource;
//
// 	/**
// 	 * \brief The name of the resource associated with the vector of
// 	 *        additional outputs
// 	 */
// 	const QString m_additionalOutputsResource;
//
// 	/**
// 	 * \brief The object to iterate over neurons of the neural network
// 	 */
// 	NeuronsIterator* m_neuronsIterator;
// };
//
// /**
//  * \brief Implements a prorportional controller
//  *
//  * This class implements a simple proportional controller. It has two
//  * parameters: the gain of the controller k and the absolute value of the
//  * maximum allowed velocity. It is used to get a velocity to apply to a joint to
//  * reach a desired position
//  */
// class FARSA_EXPERIMENTS_API ProportionalController
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * The parameters are set to default values:
// 	 * 	- k = 0.3
// 	 * 	- maxVelocity = 20.0
// 	 */
// 	ProportionalController();
//
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * \param k the value of the gain of the controller
// 	 * \param maxVelocity the value of the maximum velocity parameter
// 	 */
// 	ProportionalController(double k, double maxVelocity);
//
// 	/**
// 	 * \brief Copy constructor
// 	 *
// 	 * \param other the object to copy
// 	 */
// 	ProportionalController(const ProportionalController& other);
//
// 	/**
// 	 * \brief Copy operator
// 	 *
// 	 * \param other the object to copy
// 	 * \return a reference to this
// 	 */
// 	ProportionalController& operator=(const ProportionalController& other);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	~ProportionalController();
//
// 	/**
// 	 * \brief Returns the value of the gain
// 	 *
// 	 * \return the value of the gain
// 	 */
// 	double getK() const
// 	{
// 		return m_k;
// 	}
//
// 	/**
// 	 * \brief Sets the value of the gain
// 	 *
// 	 * \param k the new value of the gain
// 	 */
// 	void setK(double k)
// 	{
// 		m_k = k;
// 	}
//
// 	/**
// 	 * \brief Returns the maximum velocity
// 	 *
// 	 * \return the maximum velocity
// 	 */
// 	double getMaxVelocity() const
// 	{
// 		return m_maxVelocity;
// 	}
//
// 	/**
// 	 * \brief Sets the maximum velocity
// 	 *
// 	 * \param maxVelocity the new maximum velocity
// 	 */
// 	void setMaxVelocity(double maxVelocity)
// 	{
// 		m_maxVelocity = maxVelocity;
// 	}
//
// 	/**
// 	 * \brief Returns the velocity to apply to a joint to reach the desired
// 	 *        position
// 	 *
// 	 * \param desired the desired position of the joint
// 	 * \param current the current position of the joint
// 	 * \return the velocity to apply to the joint
// 	 */
// 	double velocityForJoint(double desired, double current) const;
//
// protected:
// 	/**
// 	 * \brief The gain of the controller
// 	 */
// 	double m_k;
//
// 	/**
// 	 * \brief The maximum allowed velocity
// 	 */
// 	double m_maxVelocity;
// };
//
// } // end namespace farsa
//
// #endif
