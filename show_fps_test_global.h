#ifndef SHOW_FPS_TEST_GLOBAL_H
#define SHOW_FPS_TEST_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SHOW_FPS_TEST_LIBRARY)
#  define SHOW_FPS_TESTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SHOW_FPS_TESTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SHOW_FPS_TEST_GLOBAL_H
