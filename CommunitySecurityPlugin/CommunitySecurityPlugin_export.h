
// -*- C++ -*-
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl CommunitySecurityPlugin
// ------------------------------
#ifndef COMMUNITYSECURITYPLUGIN_EXPORT_H
#define COMMUNITYSECURITYPLUGIN_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (COMMUNITYSECURITYPLUGIN_HAS_DLL)
#  define COMMUNITYSECURITYPLUGIN_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && COMMUNITYSECURITYPLUGIN_HAS_DLL */

#if !defined (COMMUNITYSECURITYPLUGIN_HAS_DLL)
#  define COMMUNITYSECURITYPLUGIN_HAS_DLL 1
#endif /* ! COMMUNITYSECURITYPLUGIN_HAS_DLL */

#if defined (COMMUNITYSECURITYPLUGIN_HAS_DLL) && (COMMUNITYSECURITYPLUGIN_HAS_DLL == 1)
#  if defined (COMMUNITYSECURITYPLUGIN_BUILD_DLL)
#    define CommunitySecurityPlugin_Export ACE_Proper_Export_Flag
#    define COMMUNITYSECURITYPLUGIN_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define COMMUNITYSECURITYPLUGIN_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* COMMUNITYSECURITYPLUGIN_BUILD_DLL */
#    define CommunitySecurityPlugin_Export ACE_Proper_Import_Flag
#    define COMMUNITYSECURITYPLUGIN_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define COMMUNITYSECURITYPLUGIN_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* COMMUNITYSECURITYPLUGIN_BUILD_DLL */
#else /* COMMUNITYSECURITYPLUGIN_HAS_DLL == 1 */
#  define CommunitySecurityPlugin_Export
#  define COMMUNITYSECURITYPLUGIN_SINGLETON_DECLARATION(T)
#  define COMMUNITYSECURITYPLUGIN_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* COMMUNITYSECURITYPLUGIN_HAS_DLL == 1 */

// Set COMMUNITYSECURITYPLUGIN_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (COMMUNITYSECURITYPLUGIN_NTRACE)
#  if (ACE_NTRACE == 1)
#    define COMMUNITYSECURITYPLUGIN_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define COMMUNITYSECURITYPLUGIN_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !COMMUNITYSECURITYPLUGIN_NTRACE */

#if (COMMUNITYSECURITYPLUGIN_NTRACE == 1)
#  define COMMUNITYSECURITYPLUGIN_TRACE(X)
#else /* (COMMUNITYSECURITYPLUGIN_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define COMMUNITYSECURITYPLUGIN_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (COMMUNITYSECURITYPLUGIN_NTRACE == 1) */

#endif /* COMMUNITYSECURITYPLUGIN_EXPORT_H */

// End of auto generated file.
