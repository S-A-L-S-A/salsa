# The script to compile Newton Game Dynamics from source

### set sources and headers files
set(SALSANEWTON_SRCS
	source/core/dgAABBPolygonSoup.cpp
	source/core/dgConvexHull3d.cpp
	source/core/dgConvexHull4d.cpp
	source/core/dg.cpp
	source/core/dgCRC.cpp
	source/core/dgDebug.cpp
	source/core/dgDelaunayTetrahedralization.cpp
	source/core/dgGeneralMatrix.cpp
	source/core/dgGeneralVector.cpp
	source/core/dgGoogol.cpp
	source/core/dgIntersections.cpp
	source/core/dgMatrix.cpp
	source/core/dgMemory.cpp
	source/core/dgNode.cpp
	source/core/dgPolygonSoupBuilder.cpp
	source/core/dgPolyhedra.cpp
	source/core/dgPolyhedraMassProperties.cpp
	source/core/dgQuaternion.cpp
	source/core/dgRandom.cpp
	source/core/dgRefCounter.cpp
	source/core/dgRef.cpp
	source/core/dgSmallDeterminant.cpp
	source/core/dgSPDMatrix.cpp
	source/core/dgSphere.cpp
	source/core/dgThreads.cpp
	source/core/dgTree.cpp
	source/core/dgTypes.cpp
	source/newton/NewtonClass.cpp
	source/newton/Newton.cpp
	source/physics/dgBallConstraint.cpp
	source/physics/dgBilateralConstraint.cpp
	source/physics/dgBody.cpp
	source/physics/dgBodyMasterList.cpp
	source/physics/dgBroadPhaseCollision.cpp
	source/physics/dgCollisionBox.cpp
	source/physics/dgCollisionBVH.cpp
	source/physics/dgCollisionCapsule.cpp
	source/physics/dgCollisionChamferCylinder.cpp
	source/physics/dgCollisionCompoundBreakable.cpp
	source/physics/dgCollisionCompound.cpp
	source/physics/dgCollisionCone.cpp
	source/physics/dgCollisionConvex.cpp
	source/physics/dgCollisionConvexHull.cpp
	source/physics/dgCollisionConvexModifier.cpp
	source/physics/dgCollision.cpp
	source/physics/dgCollisionCylinder.cpp
	source/physics/dgCollisionEllipse.cpp
	source/physics/dgCollisionHeightField.cpp
	source/physics/dgCollisionMesh.cpp
	source/physics/dgCollisionNull.cpp
	source/physics/dgCollisionScene.cpp
	source/physics/dgCollisionSphere.cpp
	source/physics/dgCollisionUserMesh.cpp
	source/physics/dgConnectorConstraint.cpp
	source/physics/dgConstraint.cpp
	source/physics/dgContact.cpp
	source/physics/dgCorkscrewConstraint.cpp
	source/physics/dgHingeConstraint.cpp
	source/physics/dgMeshEffect2.cpp
	source/physics/dgMeshEffect3.cpp
	source/physics/dgMeshEffect.cpp
	source/physics/dgMeshEffectSolidTree.cpp
	source/physics/dgMinkowskiConv.cpp
	source/physics/dgNarrowPhaseCollision.cpp
	source/physics/dgPointToCurveConstraint.cpp
	source/physics/dgSlidingConstraint.cpp
	source/physics/dgUniversalConstraint.cpp
	source/physics/dgUpVectorConstraint.cpp
	source/physics/dgUserConstraint.cpp
	source/physics/dgWorld.cpp
	source/physics/dgWorldDynamicUpdate.cpp)
set(SALSANEWTON_HDRS
	source/core/dgAABBPolygonSoup.h
	source/core/dgArray.h
	source/core/dgConvexHull3d.h
	source/core/dgConvexHull4d.h
	source/core/dgCRC.h
	source/core/dgDebug.h
	source/core/dgDelaunayTetrahedralization.h
	source/core/dgGeneralMatrix.h
	source/core/dgGeneralVector.h
	source/core/dgGoogol.h
	source/core/dgGraph.h
	source/core/dg.h
	source/core/dgHeap.h
	source/core/dgIntersections.h
	source/core/dgList.h
	source/core/dgMatrix.h
	source/core/dgMemory.h
	source/core/dgNode.h
	source/core/dgPathFinder.h
	source/core/dgPlane.h
	source/core/dgPolygonSoupBuilder.h
	source/core/dgPolygonSoupDatabase.h
	source/core/dgPolyhedra.h
	source/core/dgPolyhedraMassProperties.h
	source/core/dgQuaternion.h
	source/core/dgRandom.h
	source/core/dgRefCounter.h
	source/core/dgRef.h
	source/core/dgRtti.h
	source/core/dgSimd_Instrutions.h
	source/core/dgSmallDeterminant.h
	source/core/dgSPDMatrix.h
	source/core/dgSphere.h
	source/core/dgStack.h
	source/core/dgStdafx.h
	source/core/dgThreads.h
	source/core/dgTree.h
	source/core/dgTypes.h
	source/core/dgVector.h
	source/newton/NewtonClass.h
	source/newton/Newton.h
	source/newton/NewtonStdAfx.h
	source/physics/dgBallConstraint.h
	source/physics/dgBilateralConstraint.h
	source/physics/dgBody.h
	source/physics/dgBodyMasterList.h
	source/physics/dgBroadPhaseCollision.h
	source/physics/dgCollisionBox.h
	source/physics/dgCollisionBVH.h
	source/physics/dgCollisionCapsule.h
	source/physics/dgCollisionChamferCylinder.h
	source/physics/dgCollisionCompoundBreakable.h
	source/physics/dgCollisionCompound.h
	source/physics/dgCollisionCone.h
	source/physics/dgCollisionConvex.h
	source/physics/dgCollisionConvexHull.h
	source/physics/dgCollisionConvexModifier.h
	source/physics/dgCollisionCylinder.h
	source/physics/dgCollisionEllipse.h
	source/physics/dgCollision.h
	source/physics/dgCollisionHeightField.h
	source/physics/dgCollisionMesh.h
	source/physics/dgCollisionNull.h
	source/physics/dgCollisionScene.h
	source/physics/dgCollisionSphere.h
	source/physics/dgCollisionUserMesh.h
	source/physics/dgConnectorConstraint.h
	source/physics/dgConstraint.h
	source/physics/dgContact.h
	source/physics/dgCorkscrewConstraint.h
	source/physics/dgHingeConstraint.h
	source/physics/dgMeshEffect.h
	source/physics/dgMeshEffectSolidTree.h
	source/physics/dgMinkowskiConv.h
	source/physics/dgPhysics.h
	source/physics/dgPhysicsStdafx.h
	source/physics/dgPointToCurveConstraint.h
	source/physics/dgSlidingConstraint.h
	source/physics/dgUniversalConstraint.h
	source/physics/dgUpVectorConstraint.h
	source/physics/dgUserConstraint.h
	source/physics/dgWorldDynamicUpdate.h
	source/physics/dgWorld.h)

add_library(salsanewton SHARED ${SALSANEWTON_SRCS} ${SALSANEWTON_HDRS})
add_salsa_version(salsanewton)

# Specifying the the include directories: they are used both here and  exported
# by this library (so that targets linking this one will automatically import
# the include directories declared here)
target_include_directories(salsanewton PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/source/core" "${CMAKE_CURRENT_SOURCE_DIR}/source/physics" "${CMAKE_CURRENT_BINARY_DIR}/source/newton")

# QtCore is needed because we added a QMutex to solve a bug
target_link_libraries(salsanewton Qt5::Core)

# Additional compiler settings. Always use maximum optimizations
if (APPLE)
	target_compile_definitions(salsanewton PRIVATE _MAC_VER _NEWTON_USE_LIB _SCALAR_ARITHMETIC_ONLY)
	target_compile_options(salsanewton PRIVATE -Wno-strict-aliasing -g -O0 -fpic -msse -msse2 -ffloat-store -funsafe-math-optimizations)
elseif (UNIX)
	target_compile_definitions(salsanewton PRIVATE _LINUX_VER)
	target_compile_options(salsanewton PRIVATE -Wno-strict-aliasing -O3 -fpic -msse -msse2 -ffloat-store -funsafe-math-optimizations -fsingle-precision-constant)
elseif (WIN32)
	target_compile_definitions(salsanewton PRIVATE _NEWTON_BUILD_DLL)
endif ()

# Specifying the public headers of this target
set_property(TARGET salsanewton PROPERTY PUBLIC_HEADER ${SALSANEWTON_HDRS})

# Installation paths
install(TARGETS salsanewton
        EXPORT salsa
        ARCHIVE DESTINATION lib/
        LIBRARY DESTINATION lib/
        PUBLIC_HEADER DESTINATION include/salsa/worldsim/newton
        RUNTIME DESTINATION bin/)
