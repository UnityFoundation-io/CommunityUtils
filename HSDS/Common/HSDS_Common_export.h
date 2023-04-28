
// -*- C++ -*-
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl HSDS_Common
// ------------------------------
#ifndef HSDS_COMMON_EXPORT_H
#define HSDS_COMMON_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (HSDS_COMMON_HAS_DLL)
#  define HSDS_COMMON_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && HSDS_COMMON_HAS_DLL */

#if !defined (HSDS_COMMON_HAS_DLL)
#  define HSDS_COMMON_HAS_DLL 1
#endif /* ! HSDS_COMMON_HAS_DLL */

#if defined (HSDS_COMMON_HAS_DLL) && (HSDS_COMMON_HAS_DLL == 1)
#  if defined (HSDS_COMMON_BUILD_DLL)
#    define HSDS_Common_Export ACE_Proper_Export_Flag
#    define HSDS_COMMON_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define HSDS_COMMON_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* HSDS_COMMON_BUILD_DLL */
#    define HSDS_Common_Export ACE_Proper_Import_Flag
#    define HSDS_COMMON_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define HSDS_COMMON_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* HSDS_COMMON_BUILD_DLL */
#else /* HSDS_COMMON_HAS_DLL == 1 */
#  define HSDS_Common_Export
#  define HSDS_COMMON_SINGLETON_DECLARATION(T)
#  define HSDS_COMMON_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* HSDS_COMMON_HAS_DLL == 1 */

// Set HSDS_COMMON_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (HSDS_COMMON_NTRACE)
#  if (ACE_NTRACE == 1)
#    define HSDS_COMMON_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define HSDS_COMMON_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !HSDS_COMMON_NTRACE */

#if (HSDS_COMMON_NTRACE == 1)
#  define HSDS_COMMON_TRACE(X)
#else /* (HSDS_COMMON_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define HSDS_COMMON_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (HSDS_COMMON_NTRACE == 1) */

#endif /* HSDS_COMMON_EXPORT_H */

// End of auto generated file.
