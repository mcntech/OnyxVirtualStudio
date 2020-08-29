//------------------------------------------------------------------------------
// File: Dbg.h
//
// Desc: DirectShow sample code - Helper file for the PSIParser filter.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

// dump a string to debug output
#ifndef __DBG_H__
#define __DBG_H__
#include "stdio.h"


#define DBG_LVL_STRM	5
#define DBG_LVL_TRACE	4
#define DBG_LVL_MSG		3
#define DBG_LVL_SETUP	2
#define DBG_LVL_WARN	1
#define DBG_LVL_ERR		0

extern "C" int gDbgLvl;

extern "C" void DbgOutL(char *szError,...);

#if defined(DEBUG) || defined(FORCE_LOG)
#define DbgOut(level, _x_) \
    if((level) <= gDbgLvl) { \
        DbgOutL _x_; \
    }
#else
#define DbgOut(level, _x_)
#endif

#endif //__DBG_H__