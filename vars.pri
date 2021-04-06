TARGET = tp_math_utils
TEMPLATE = lib

DEFINES += TP_MATH_UTILS_LIBRARY

SOURCES += src/Globals.cpp
HEADERS += inc/tp_math_utils/Globals.h

#SOURCES += src/JSONUtils.cpp
HEADERS += inc/tp_math_utils/JSONUtils.h

SOURCES += src/Plane.cpp
HEADERS += inc/tp_math_utils/Plane.h

SOURCES += src/DistanceToPlane.cpp
HEADERS += inc/tp_math_utils/DistanceToPlane.h

SOURCES += src/DistanceToRay.cpp
HEADERS += inc/tp_math_utils/DistanceToRay.h

SOURCES += src/DistanceBetweenLines.cpp
HEADERS += inc/tp_math_utils/DistanceBetweenLines.h

SOURCES += src/DistanceBetweenCones.cpp
HEADERS += inc/tp_math_utils/DistanceBetweenCones.h

#SOURCES += src/Ray.cpp
HEADERS += inc/tp_math_utils/Ray.h

#SOURCES += src/Cone.cpp
HEADERS += inc/tp_math_utils/Cone.h

#SOURCES += src/Region.cpp
HEADERS += inc/tp_math_utils/Region.h

SOURCES += src/Intersection.cpp
HEADERS += inc/tp_math_utils/Intersection.h

SOURCES += src/Transformation.cpp
HEADERS += inc/tp_math_utils/Transformation.h

#SOURCES += src/Ellipse.cpp
HEADERS += inc/tp_math_utils/Ellipse.h

SOURCES += src/Polygon.cpp
HEADERS += inc/tp_math_utils/Polygon.h

SOURCES += src/Material.cpp
HEADERS += inc/tp_math_utils/Material.h

SOURCES += src/Light.cpp
HEADERS += inc/tp_math_utils/Light.h

SOURCES += src/Geometry3D.cpp
HEADERS += inc/tp_math_utils/Geometry3D.h

SOURCES += src/ClosestPointsOnLines.cpp
HEADERS += inc/tp_math_utils/ClosestPointsOnLines.h

SOURCES += src/ClosestPointsOnRays.cpp
HEADERS += inc/tp_math_utils/ClosestPointsOnRays.h

SOURCES += src/MidPointBetweenLines.cpp
HEADERS += inc/tp_math_utils/MidPointBetweenLines.h

SOURCES += src/AngleBetweenVectors.cpp
HEADERS += inc/tp_math_utils/AngleBetweenVectors.h

SOURCES += src/StandardDeviation.cpp
HEADERS += inc/tp_math_utils/StandardDeviation.h

#SOURCES += src/BoxPacker.cpp
HEADERS += inc/tp_math_utils/BoxPacker.h

SOURCES += src/Sphere.cpp
HEADERS += inc/tp_math_utils/Sphere.h
