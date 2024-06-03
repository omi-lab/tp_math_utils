DEPENDENCIES += lib_glm
DEPENDENCIES += lib_nanoflann
DEPENDENCIES += tp_utils

INCLUDEPATHS += tp_math_utils/inc/
LIBRARIES    += tp_math_utils

# Microsoft sometimes define min and max. This turn that behaviour off.
DEFINES      += NOMINMAX
