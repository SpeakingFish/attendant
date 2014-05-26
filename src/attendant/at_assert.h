#pragma once

#include <QtCore/QtGlobal>
#include <QtCore/QByteArray>

#include "at_common.h"

#if !defined(AT_ASSERT)

	#define AT_ASSERT_1(cond) ((!(cond)) ? qt_assert(#cond,__FILE__,__LINE__) : qt_noop())
	#define AT_ASSERT_2(cond, message) ((!(cond)) ? qt_assert(QByteArray(message) + ". Assert string: '" + QByteArray(#cond) + "'", __FILE__, __LINE__) : qt_noop())

	#define GET_3TH_ARG(arg1, arg2, arg3, ...) arg3

	#define AT_ASSERT_MACRO_CHOOSER(...) GET_3TH_ARG(__VA_ARGS__, AT_ASSERT_2, AT_ASSERT_1, )

	#define AT_ASSERT(...) AT_ASSERT_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
#else
	#error AT_ASSERT macro redifinition
#endif
