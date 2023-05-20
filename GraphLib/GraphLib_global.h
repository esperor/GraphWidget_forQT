
#pragma once

#include <QtCore/qglobal.h>

#if defined(GRAPHLIB_LIBRARY)
#  define GRAPHLIB_EXPORT Q_DECL_EXPORT
#else
#  define GRAPHLIB_EXPORT Q_DECL_IMPORT
#endif
