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

#ifndef RENDERINGPROXY_H
#define RENDERINGPROXY_H

#include "wentity.h"
#include "glcontextanddata.h"
#include <QSet>
#include "dataexchange.h"

#ifdef FARSA_DEBUG
	#include <cstdlib>
#endif

namespace farsa {

class GLContextAndData;
class WObject;
class PhyObject;
class PhyJoint;
class SensorController;
class MotorController;

/**
 * \brief The namespace with utility functions for the rending proxies
 *
 * Implementations are at the end of this file
 * \internal
 */
namespace __RenderingProxy_internal {

	/**
	 * \brief The function calling the rendering functions for WEntities
	 *
	 * This function performs the needed rendering steps depending on flags
	 * in contextAndData and in sharedData
	 * \param dummy a pointer used only to perform function overloading
	 * \param renderer the renderer
	 * \param sharedData the pointer to shared data
	 * \param contextAndData the OpenGL context and data
	 * \internal
	 */
	template <class T>
	void FARSA_WSIM_TEMPLATE render(WEntity* dummy, typename T::Renderer* renderer, const typename T::Shared* sharedData, GLContextAndData* contextAndData);

	/**
	 * \brief The function calling the rendering functions for WObjects
	 *
	 * This function performs the needed rendering steps depending on flags
	 * in contextAndData and in sharedData
	 * \param dummy a pointer used only to perform function overloading
	 * \param renderer the renderer
	 * \param sharedData the pointer to shared data
	 * \param contextAndData the OpenGL context and data
	 * \internal
	 */
	template <class T>
	void FARSA_WSIM_TEMPLATE render(WObject* dummy, typename T::Renderer* renderer, const typename T::Shared* sharedData, GLContextAndData* contextAndData);

	/**
	 * \brief The function calling the rendering functions for PhyObjects
	 *
	 * This function performs the needed rendering steps depending on flags
	 * in contextAndData and in sharedData
	 * \param dummy a pointer used only to perform function overloading
	 * \param renderer the renderer
	 * \param sharedData the pointer to shared data
	 * \param contextAndData the OpenGL context and data
	 * \internal
	 */
	template <class T>
	void FARSA_WSIM_TEMPLATE render(PhyObject* dummy, typename T::Renderer* renderer, const typename T::Shared* sharedData, GLContextAndData* contextAndData);

	/**
	 * \brief The function calling the rendering functions for PhyJoints
	 *
	 * This function performs the needed rendering steps depending on flags
	 * in contextAndData and in sharedData
	 * \param dummy a pointer used only to perform function overloading
	 * \param renderer the renderer
	 * \param sharedData the pointer to shared data
	 * \param contextAndData the OpenGL context and data
	 * \internal
	 */
	template <class T>
	void FARSA_WSIM_TEMPLATE render(PhyJoint* dummy, typename T::Renderer* renderer, const typename T::Shared* sharedData, GLContextAndData* contextAndData);

	/**
	 * \brief The function calling the rendering functions for
	 *        SensorControllers
	 *
	 * This function performs the needed rendering steps depending on flags
	 * in contextAndData and in sharedData
	 * \param dummy a pointer used only to perform function overloading
	 * \param renderer the renderer
	 * \param sharedData the pointer to shared data
	 * \param contextAndData the OpenGL context and data
	 * \internal
	 */
	template <class T>
	void FARSA_WSIM_TEMPLATE render(SensorController* dummy, typename T::Renderer* renderer, const typename T::Shared* sharedData, GLContextAndData* contextAndData);

	/**
	 * \brief The function calling the rendering functions for
	 *        MotorControllers
	 *
	 * This function performs the needed rendering steps depending on flags
	 * in contextAndData and in sharedData
	 * \param dummy a pointer used only to perform function overloading
	 * \param renderer the renderer
	 * \param sharedData the pointer to shared data
	 * \param contextAndData the OpenGL context and data
	 * \internal
	 */
	template <class T>
	void FARSA_WSIM_TEMPLATE render(MotorController* dummy, typename T::Renderer* renderer, const typename T::Shared* sharedData, GLContextAndData* contextAndData);

	/**
	 * \brief An utility function calling one of the functions above with
	 *        the correct arguments
	 *
	 * \param renderer the renderer
	 * \param sharedData the pointer to shared data
	 * \param contextAndData the OpenGL context and data
	 * \internal
	 */
	template <class T>
	void FARSA_WSIM_TEMPLATE render(typename T::Renderer* renderer, const typename T::Shared* sharedData, GLContextAndData* contextAndData);
}

/**
 * \brief The abstract base for classes which get data from a WEntity and calls
 *        functions of the renderers
 *
 * Instances of subclasses may live in two different threads: the simulation one
 * in which the copyDataFromWEntity() function is called and the rendering one
 * (which may be the same as the simulation one) in which the render() function
 * is called
 */
class FARSA_WSIM_TEMPLATE AbstractRenderingProxy
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~AbstractRenderingProxy()
	{
	}

	/**
	 * \brief The function to get data from a WEntity for rendering
	 */
	virtual void copyDataFromWEntity() = 0;

	/**
	 * \brief The function performing the rendering
	 *
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	virtual void render(GLContextAndData* contextAndData) = 0;

	/**
	 * \brief Returns a const pointer to the renderer we use here
	 *
	 * \return a const pointer to the renderer we use here
	 */
	virtual RenderWEntity* renderer() const = 0;
};

/**
 * \brief The subclass of AbstractRenderingProxy to use when the render can take
 *        data directly from the WEntity
 *
 * This class performs no copy of data, the renderer takes data directly from
 * the WEntity. Use this only when the renderer and WEntity live in the same
 * thread. Type is the WEntity to render
 */
template <class Type>
class FARSA_WSIM_TEMPLATE DataFromWEntityRenderingProxy : public AbstractRenderingProxy
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param entity the entity that will be rendered
	 * \param renderer the renderer rendering the entity
	 */
	DataFromWEntityRenderingProxy(const Type* entity, typename Type::Renderer* renderer)
		: m_entity(entity)
		, m_renderer(renderer)
	{
	}

	/**
	 * \brief Destructor
	 */
	virtual ~DataFromWEntityRenderingProxy()
	{
	}

	/**
	 * \brief Copies data from the WEntity
	 *
	 * This function does nothing, the renderer takes data directly from the
	 * WEntity
	 */
	virtual void copyDataFromWEntity()
	{
	}

	/**
	 * \brief The function performing the rendering
	 *
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	virtual void render(GLContextAndData* contextAndData)
	{
		__RenderingProxy_internal::render<Type>(m_renderer, static_cast<const typename Type::Shared*>(m_entity->getShared()), contextAndData);
	}

	/**
	 * \brief Returns a const pointer to the renderer we use here
	 *
	 * \return a const pointer to the renderer we use here
	 */
	virtual RenderWEntity* renderer() const
	{
		return m_renderer;
	}

private:
	/**
	 * \brief The entity that is rendered
	 */
	const Type* const m_entity;

	/**
	 * \brief The renderer
	 */
	typename Type::Renderer* const m_renderer;
};

/**
 * \brief The subclass of AbstractRenderingProxy to use when the render has to
 *        use copied data
 *
 * This class copies shared data from the WEntity and the renderer works on
 * copied data. Use this when the renderer and WEntity live in different threads
 */
template <class Type>
class FARSA_WSIM_TEMPLATE CopiedDataRenderingProxy : public AbstractRenderingProxy
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param entity the entity that will be rendered
	 * \param renderer the renderer rendering the entity
	 */
	CopiedDataRenderingProxy(const Type* entity, typename Type::Renderer* renderer)
		: m_entity(entity)
		, m_renderer(renderer)
		, m_shared()
		, m_copyNeededChecker()
	{
	}

	/**
	 * \brief Destructor
	 */
	virtual ~CopiedDataRenderingProxy()
	{
	}

	/**
	 * \brief Copies data from the WEntity
	 *
	 * This function copies the WEntity shared data into an internal buffer
	 */
	virtual void copyDataFromWEntity()
	{
		// Only copying data if needed
		if (m_entity->updateNeeded(m_copyNeededChecker)) {
			m_shared = *(static_cast<const typename Type::Shared*>(m_entity->getShared()));
		}
	}

	/**
	 * \brief The function performing the rendering
	 *
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	virtual void render(GLContextAndData* contextAndData)
	{
		__RenderingProxy_internal::render<Type>(m_renderer, &m_shared, contextAndData);
	}

	/**
	 * \brief Returns a const pointer to the renderer we use here
	 *
	 * \return a const pointer to the renderer we use here
	 */
	virtual RenderWEntity* renderer() const
	{
		return m_renderer;
	}

private:
	/**
	 * \brief The entity that is rendered
	 */
	const Type* const m_entity;

	/**
	 * \brief The renderer
	 */
	typename Type::Renderer* const m_renderer;

	/**
	 * \brief Data copied from the WEntity
	 */
	typename Type::Shared m_shared;

	/**
	 * \brief The object to check if data really needs to be copied
	 */
	UpdateCheckerLong m_copyNeededChecker;
};

/**
 * \brief The class to generate renderers and AbstractRenderingProxy on demand
 *
 * This is the base class that is used to generate the renderers and the
 * corresponding AbstractRenderingProxy objects for a given WEntity when needed.
 * This class is NOT thread safe, use it only in the same thread as World
 */
class FARSA_WSIM_TEMPLATE AbstractRenderWEntityCreator
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~AbstractRenderWEntityCreator()
	{
	}

	/**
	 * \brief Creates a renderer and returns it
	 *
	 * \return the created renderer
	 */
	virtual RenderWEntity* generateRenderer() = 0;

	/**
	 * \brief Generates a rendering proxy that does not perform the copy of
	 *        data
	 *
	 * The renderer should have been generated by this class, otherwise a
	 * wrong cast is performed.
	 * \param renderer the renderer for which the proxy has to be generated
	 * \return the proxy for the renderer
	 */
	virtual AbstractRenderingProxy* generateRenderingProxyWithoutCopy(RenderWEntity* renderer) = 0;

	/**
	 * \brief Generates a rendering proxy that copies data from the entity
	 *        to render
	 *
	 * The renderer should have been generated by this class, otherwise a
	 * wrong cast is performed.
	 * \param renderer the renderer for which the proxy has to be generated
	 * \return the proxy for the renderer
	 */
	virtual AbstractRenderingProxy* generateRenderingProxyWithCopy(RenderWEntity* renderer) = 0;
};

/**
 * \brief The concrete class to generate renderers and AbstractRenderingProxy on
 *        demand
 *
 * The Type template parameter is the WEntity for which renderers have to be
 * generated.
 */
template <class Type>
class FARSA_WSIM_TEMPLATE RenderWEntityCreator : public AbstractRenderWEntityCreator
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param entity a pointer to the entity for which renderers are
	 *              generated
	 */
	RenderWEntityCreator(const Type* entity)
		: AbstractRenderWEntityCreator()
		, m_entity(entity)
	{
	}

	/**
	 * \brief Creates a renderer and returns it
	 *
	 * \return the created renderer
	 */
	virtual RenderWEntity* generateRenderer()
	{
		return new typename Type::Renderer(m_entity);
	}

	/**
	 * \brief Generates a rendering proxy that does not perform the copy of
	 *        data
	 *
	 * The renderer should have been generated by this class, otherwise a
	 * wrong cast is performed.
	 * \param renderer the renderer for which the proxy has to be generated
	 * \return the proxy for the renderer
	 */
	virtual AbstractRenderingProxy* generateRenderingProxyWithoutCopy(RenderWEntity* renderer)
	{
		return new DataFromWEntityRenderingProxy<Type>(m_entity, static_cast<typename Type::Renderer*>(renderer));
	}

	/**
	 * \brief Generates a rendering proxy that copies data from the entity
	 *        to render
	 *
	 * The renderer should have been generated by this class, otherwise a
	 * wrong cast is performed.
	 * \param renderer the renderer for which the proxy has to be generated
	 * \return the proxy for the renderer
	 */
	virtual AbstractRenderingProxy* generateRenderingProxyWithCopy(RenderWEntity* renderer)
	{
		return new CopiedDataRenderingProxy<Type>(m_entity, static_cast<typename Type::Renderer*>(renderer));
	}

private:
	/**
	 * \brief The entity for which renderers are generated
	 */
	const Type* const m_entity;
};

} // end namespace farsa

#include "wobject.h"
#include "phyobject.h"
#include "phyjoint.h"
#include "sensorcontrollers.h"
#include "motorcontrollers.h"

namespace farsa {

namespace __RenderingProxy_internal {
	template <class T>
	void render(WEntity*, typename T::Renderer* renderer, const typename T::Shared* sharedData, GLContextAndData* contextAndData)
	{
		// For WEntities we can only call render, no flag is present
		renderer->render(sharedData, contextAndData);
	}

	template <class T>
	void render(WObject*, typename T::Renderer* renderer, const typename T::Shared* sharedData, GLContextAndData* contextAndData)
	{
		// For WObjects we have to take few flags into account
		if ((sharedData->flags & WObjectShared::Visible) && (contextAndData->drawObjects())) {
			renderer->render(sharedData, contextAndData);
		}

		if ((sharedData->flags & WObjectShared::DrawLabel) || contextAndData->drawLabels()) {
			renderer->drawLabel(sharedData, contextAndData);
		}

		if ((sharedData->flags & WObjectShared::DrawLocalReferenceFrame) || contextAndData->drawLocalAxes()) {
			renderer->drawAxes(sharedData, contextAndData);
		}
	}

	template <class T>
	void render(PhyObject*, typename T::Renderer* renderer, const typename T::Shared* sharedData, GLContextAndData* contextAndData)
	{
		// For PhyObject we have to take few flags into account (those of WObjects plus some more)
		if ((sharedData->flags & WObjectShared::Visible) && (contextAndData->drawObjects())) {
			renderer->render(sharedData, contextAndData);
		}

		if ((sharedData->flags & WObjectShared::DrawLabel) || contextAndData->drawLabels()) {
			renderer->drawLabel(sharedData, contextAndData);
		}

		if ((sharedData->flags & WObjectShared::DrawLocalReferenceFrame) || contextAndData->drawLocalAxes()) {
			renderer->drawAxes(sharedData, contextAndData);
		}

		if ((sharedData->flags & PhyObjectShared::DrawForces) || contextAndData->drawForces()) {
			renderer->drawForces(sharedData, contextAndData);
		}
	}

	template <class T>
	void render(PhyJoint*, typename T::Renderer* renderer, const typename T::Shared* sharedData, GLContextAndData* contextAndData)
	{
		// For PhyJoints we can only call render, no flag is present
		if (contextAndData->drawJoints()) {
			renderer->render(sharedData, contextAndData);
		}
	}

	template <class T>
	void render(SensorController*, typename T::Renderer* renderer, const typename T::Shared* sharedData, GLContextAndData* contextAndData)
	{
		// For SensorControllers we can only call render, no flag is present
		renderer->render(sharedData, contextAndData);
	}

	template <class T>
	void render(MotorController*, typename T::Renderer* renderer, const typename T::Shared* sharedData, GLContextAndData* contextAndData)
	{
		// For MotorControllers we can only call render, no flag is present
		renderer->render(sharedData, contextAndData);
	}

	template <class T>
	void render(typename T::Renderer* renderer, const typename T::Shared* sharedData, GLContextAndData* contextAndData)
	{
		// The first parameter is only to resolve the overloading
		render<T>((T*)(0), renderer, sharedData, contextAndData);
	}
}

}

#endif
