TARGET = tp_math_utils
TEMPLATE = lib

DEFINES += TP_MATH_UTILS_LIBRARY

HEADERS += inc/tp_math_utils/Globals.h

#SOURCES += src/JSONUtils.cpp
HEADERS += inc/tp_math_utils/JSONUtils.h

SOURCES += src/Plane.cpp
HEADERS += inc/tp_math_utils/Plane.h

SOURCES += src/PlaneFromPoints.cpp
HEADERS += inc/tp_math_utils/PlaneFromPoints.h

SOURCES += src/DistanceToPlane.cpp
HEADERS += inc/tp_math_utils/DistanceToPlane.h

SOURCES += src/DistanceToRay.cpp
HEADERS += inc/tp_math_utils/DistanceToRay.h

SOURCES += src/DistanceBetweenLines.cpp
HEADERS += inc/tp_math_utils/DistanceBetweenLines.h

#SOURCES += src/Ray.cpp
HEADERS += inc/tp_math_utils/Ray.h

SOURCES += src/Intersection.cpp
HEADERS += inc/tp_math_utils/Intersection.h

SOURCES += src/Transformation.cpp
HEADERS += inc/tp_math_utils/Transformation.h

#SOURCES += src/Ellipse.cpp
HEADERS += inc/tp_math_utils/Ellipse.h

SOURCES += src/Polygon.cpp
HEADERS += inc/tp_math_utils/Polygon.h

SOURCES += src/ClosestPointsOnLines.cpp
HEADERS += inc/tp_math_utils/ClosestPointsOnLines.h

SOURCES += src/MidPointBetweenLines.cpp
HEADERS += inc/tp_math_utils/MidPointBetweenLines.h

SOURCES += src/AngleBetweenVectors.cpp
HEADERS += inc/tp_math_utils/AngleBetweenVectors.h

SOURCES += src/StandardDeviation.cpp
HEADERS += inc/tp_math_utils/StandardDeviation.h

SOURCES += src/MatrixComposition.cpp
HEADERS += inc/tp_math_utils/MatrixComposition.h
