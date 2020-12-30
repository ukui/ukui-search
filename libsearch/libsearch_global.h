#ifndef LIBSEARCH_GLOBAL_H
#define LIBSEARCH_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBSEARCH_LIBRARY)
#  define LIBSEARCH_EXPORT Q_DECL_EXPORT
#else
#  define LIBSEARCH_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBSEARCH_GLOBAL_H
