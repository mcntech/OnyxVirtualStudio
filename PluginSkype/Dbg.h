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

#define Dump(tsz) \
    OutputDebugString(tsz);

#define DumpAndReturnFalse(tsz) \
    {OutputDebugString(tsz);    \
    return false;}              \

// dump a string with a parameter value to debug output
#define Dump1(tsz, arg)                         \
    { TCHAR dbgsup_tszDump[1024];               \
      wsprintf(dbgsup_tszDump, (tsz), (arg));   \
      OutputDebugString(dbgsup_tszDump); }


#define CHECK_ERROR(tsz,hr)                     \
{   if( !SUCCEEDED(hr)  )                       \
    {                                           \
        TCHAR dbgsup_tszDump[1024];             \
        wsprintf(dbgsup_tszDump, (tsz), (hr));  \
        OutputDebugString(dbgsup_tszDump);      \
        return hr;                              \
    }                                           \
}

#define RETURN_FALSE_IF_FAILED(tsz,hr)          \
{   if( S_OK != hr)                             \
    {                                           \
        TCHAR dbgsup_tszDump[1024];             \
        wsprintf(dbgsup_tszDump, (tsz), (hr));  \
        OutputDebugString(dbgsup_tszDump);      \
        return FALSE;                           \
    }                                           \
}

#define CHECK_BADPTR(tsz,ptr)                   \
{                                               \
    TCHAR dbgsup_tszDump[1024];                 \
    if( ptr == 0)                               \
    {                                           \
        wsprintf(dbgsup_tszDump, (tsz), (ptr)); \
        OutputDebugString(dbgsup_tszDump);      \
        return E_FAIL;                          \
    }                                           \
}

#define RETURN_FALSE_IF_BADPTR(tsz,ptr)         \
{                                               \
    TCHAR dbgsup_tszDump[1024];                 \
    if( ptr == 0)                               \
    {                                           \
        wsprintf(dbgsup_tszDump, (tsz), (ptr)); \
        OutputDebugString(dbgsup_tszDump);      \
        return FALSE;                           \
    }                                           \
}
extern "C" void DbgOutL(const char *szError,...);

#if defined(DEBUG)
#define DbgOut(level, _x_) \
    if((level) <= gDbgLvl) { \
        DbgOutL _x_; \
    }
#else
#define DbgOut(level, _x_)
#endif

#endif //__DBG_H__