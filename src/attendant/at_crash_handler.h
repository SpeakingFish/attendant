#pragma once

#ifdef __WIN32__
#include <windows.h>
#include <excpt.h>
#endif

#include <stdio.h>
#include <setjmp.h>

#include "at_log.h"

void print_backtrace(void);

#ifdef __WIN32__

class __SEH_HANDLER;

typedef struct tag__SEH_EXCEPTION_REGISTRATION
{
	tag__SEH_EXCEPTION_REGISTRATION* prev;
	PEXCEPTION_HANDLER handler;
	__SEH_HANDLER* exthandler;
} __SEH_EXCEPTION_REGISTRATION;



class __SEH_HANDLER
{
public:

	// This is the main exception handling function.  This is called
	// for each exception raised using this method.
	static EXCEPTION_DISPOSITION ExceptionRouter(PEXCEPTION_RECORD pRecord,
		__SEH_EXCEPTION_REGISTRATION* pReg,
		PCONTEXT pContext,
		PEXCEPTION_RECORD pRecord2);

	// This is the exception handler for this specific instance.  This is called by the
	// ExceptionRouter class function.
	virtual EXCEPTION_DISPOSITION ExceptionHandler(PEXCEPTION_RECORD pRecord,
		__SEH_EXCEPTION_REGISTRATION* pReg,
		PCONTEXT pContext,
		PEXCEPTION_RECORD pRecord2);

	virtual ~__SEH_HANDLER();

	// This is the context buffer used by setjmp.  This stores the context at a given point
	// in the program so that it can be resumed.
	jmp_buf context;

	// This is a copy of the EXCEPTION_RECORD structure passed to the exception handler.
	EXCEPTION_RECORD excRecord;
	// This is a copy of the CONTEXT structure passed to the exception handler.
	CONTEXT excContext;
};

__SEH_HANDLER::~__SEH_HANDLER()
{

}

// The main exception handler.
EXCEPTION_DISPOSITION __SEH_HANDLER::ExceptionRouter(PEXCEPTION_RECORD pRecord,
	__SEH_EXCEPTION_REGISTRATION* pReg,
	PCONTEXT pContext,
	PEXCEPTION_RECORD pRecord2)
{
	// Retrieve the actual __SEH_HANDLER object from the registration, and call the
	// specific exception handling function.  Everything could have been done from this
	// function alone, but I decided to use an instance method instead.
	return pReg->exthandler->ExceptionHandler(pRecord, pReg, pContext, pRecord2);
}

EXCEPTION_DISPOSITION __SEH_HANDLER::ExceptionHandler(PEXCEPTION_RECORD pRecord,
	__SEH_EXCEPTION_REGISTRATION* pReg,
	PCONTEXT pContext,
	PEXCEPTION_RECORD pRecord2)
{
	Q_UNUSED(pReg);
	Q_UNUSED(pRecord2);
	// The objects pointed to by the pointers live on the stack, so a copy of them is required,
	// or they may get overwritten by the time we've hit the real exception handler code
	// back in the offending function.
	CopyMemory(&excContext, pContext, sizeof(_CONTEXT));
	CopyMemory(&excRecord, pRecord, sizeof(_EXCEPTION_RECORD));

	// Jump back to the function where the exception actually occurred.  The 1 is the
	// return code that will be returned by set_jmp.
	longjmp(context, 1);
}

 // Note the unmatched braces in these macros.  These are to allow one to use
	  // the same variable name more than once (new scope).
	#define __seh_try															\
	{																			\
		__SEH_EXCEPTION_REGISTRATION _lseh_er;									\
		__SEH_HANDLER _lseh_handler;											\
																				\
		_lseh_er.handler =														\
		reinterpret_cast<PEXCEPTION_HANDLER>(__SEH_HANDLER::ExceptionRouter);	\
		_lseh_er.exthandler = &_lseh_handler;									\
		asm volatile ("movl %%fs:0, %0" : "=r" (_lseh_er.prev));				\
		asm volatile ("movl %0, %%fs:0" : : "r" (&_lseh_er));					\
		int _lseh_setjmp_res = setjmp((int*)(_lseh_handler.context));			\
		while(true)																\
		{																		\
			if(_lseh_setjmp_res != 0)											\
			{																	\
				break;															\
			}																	\


	#define __seh_except(rec, ctx)												\
			break;																\
		}																		\
		PEXCEPTION_RECORD rec = &_lseh_handler.excRecord;						\
		PCONTEXT ctx = &_lseh_handler.excContext;								\
																				\
		Q_UNUSED(rec);										\
		Q_UNUSED(ctx);										\
																				\
		asm volatile ("movl %0, %%fs:0" : : "r" (_lseh_er.prev));				\
		if(_lseh_setjmp_res != 0)

	#define __seh_end }

	#define __seh_default_except_and_end										\
		__seh_except(info, context)												\
		{																		\
			if(info->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)				\
			{																	\
				AT_LOG << QObject::tr("Access Violation Exception caught:");	\
			}																	\
		}																		\
		__seh_end

#else

#define __seh_try
#define __seh_except(arg_1, arg_2) if (false)
#define __seh_end
#define __seh_default_except_and_end __seh_end
#endif
