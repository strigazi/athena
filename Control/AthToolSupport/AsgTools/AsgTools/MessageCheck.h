#ifndef ASG_TOOLS__MESSAGE_CHECK_H
#define ASG_TOOLS__MESSAGE_CHECK_H

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <type_traits>

#ifdef ROOTCORE
#include <AsgTools/MsgStream.h>
#include <xAODRootAccess/tools/TReturnCode.h>
#else
#include "AthenaBaseComps/AthMessaging.h"
#endif

/// \brief for standalone code this creates a new message category
///
/// The idea is that this provides a way to use the "standard"
/// messaging macros in code that doesn't inherit from AsgTools.  All
/// you have to do is put
/// \code{.cpp}
///   using namespace NAME;
/// \endcode
/// at the beginning of a function and then you can use the messaging
/// macros as if you were inside an AsgTool.
///
/// Or almost, in RootCore this works just fine, but in Athena
/// ATH_MSG_* is defined slightly differently so that this won't work.
/// To that end we are providing ANA_MSG_* macros that work with this,
/// and in both RootCore and Athena.  This way you can also use
/// ATH_CHECK/ANA_CHECK in your code.
///
/// Besides allowing to use the standard messaging macros, you can
/// also set the message level similar to how you'd do it for an
/// AsgTool:
/// \code{.cpp}
///   NAME::setMsgLevel (MSG::DEBUG);
/// \endcode
///
/// \sa ANA_MSG_SOURCE
#define ANA_MSG_HEADER(NAME)		\
  namespace NAME {				\
  MsgStream& msg ();				\
  MsgStream& msg (MSG::Level level);		\
  bool msgLvl (MSG::Level lvl);			\
  void setMsgLevel (MSG::Level level); }


#ifdef ROOTCORE
#define ASG_TOOLS_MSG_STREAM(NAME,TITLE)	\
  static MsgStream NAME (TITLE);
#else
#define ASG_TOOLS_MSG_STREAM(NAME,TITLE)			\
  static MsgStream NAME (::asg::detail::getMessageSvcAthena(), TITLE);
#endif

/// \brief the source code part of \ref ANA_MSG_SOURCE
///
/// For every message category that you introduce via \ref
/// ANA_MSG_SOURCE you have to add this in one of your source files,
/// to add the source code needed for its function implementations
#define ANA_MSG_SOURCE(NAME,TITLE)		\
  namespace NAME				\
  {						\
  namespace					\
    {						\
  MSG::Level& msgLevel ()			\
    {						\
  static MSG::Level level = MSG::INFO;		\
  return level;					\
    }						\
  }						\
						\
						\
						\
  MsgStream& msg ()				\
  {						\
    ASG_TOOLS_MSG_STREAM (result, TITLE);	\
  return result;				\
  }						\
						\
						\
  MsgStream& msg (MSG::Level level)		\
  {						\
    return msg() << level;			\
  }						\
						\
						\
						\
  bool msgLvl (MSG::Level lvl)			\
  {						\
    return msgLevel() <= lvl;			\
  }						\
						\
						\
						\
  void setMsgLevel (MSG::Level level)		\
  {						\
    msgLevel() = level;				\
  }						\
  }

namespace asg
{
  ANA_MSG_HEADER (msgToolHandle)
  ANA_MSG_HEADER (msgUserCode)
}

namespace asg
{
  /// \brief this is an internal traits class for status codes used by
  /// the ANA_CHECK* macros
  template<typename T> struct CheckHelper;

  template<> struct CheckHelper<StatusCode>
  {
    /// \brief whether the status code reports a success
    static inline bool isSuccess (const StatusCode& sc) {
      return sc.isSuccess(); }

    /// \brief produce a status code to report success
    static inline decltype(StatusCode::SUCCESS) successCode () {
      return StatusCode::SUCCESS;}

    /// \brief produce a status code to report failure
    static inline decltype(StatusCode::FAILURE) failureCode () {
      return StatusCode::FAILURE;}
  };

#ifdef ROOTCORE
  template<> struct CheckHelper<xAOD::TReturnCode>
  {
    /// \brief whether the status code reports a success
    static inline bool isSuccess (const xAOD::TReturnCode& sc) {
      return sc.isSuccess(); }

    /// \brief produce a status code to report success
    static inline decltype (xAOD::TReturnCode::kSuccess) successCode () {
      return xAOD::TReturnCode::kSuccess;}

    /// \brief produce a status code to report failure
    static inline decltype (xAOD::TReturnCode::kFailure) failureCode () {
      return xAOD::TReturnCode::kFailure;}
  };
#endif

  template<> struct CheckHelper<int>
  {
    /// \brief produce a status code to report success
    static inline int successCode () {
      return 0;}

    /// \brief produce a status code to report failure
    static inline int failureCode () {
      return 1;}
  };

  template<> struct CheckHelper<bool>
  {
    /// \brief whether the status code reports a success
    static inline bool isSuccess (const bool& sc) {
      return sc; }

    /// \brief produce a status code to report success
    static inline bool successCode () {
      return true;}

    /// \brief produce a status code to report failure
    static inline bool failureCode () {
      return false;}
  };

  template<typename T> struct CheckHelper<T*>
  {
    /// \brief whether the status code reports a success
    static inline bool isSuccess (const T *sc) {
      return sc; }

    /// \brief produce a status code to report failure
    static inline T *failureCode () {
      return nullptr;}
  };

  namespace detail
  {
#ifndef ROOTCORE
    /// Get the Athena message service
    /// TODO: Look into using AthenaKernel/MsgStreamMember.h
    IMessageSvc* getMessageSvcAthena();
#endif

    /// \brief throw an error for a failed check
    ///
    /// the main reason to have a separate function for this is to cut
    /// down on the number of includes in the header file.
    [[noreturn]] void throw_check_fail (const std::string& str);
  }
}


// This is a GCC extension for getting the name of the current function.
#if defined( __GNUC__ )
#   define MSGSTREAM_FNAME __PRETTY_FUNCTION__
#else
#   define MSGSTREAM_FNAME ""
#endif


// This is a GCC extension for getting the name of the current function.
#if defined( __GNUC__ )
#   define ASG_TOOLS_MSGSTREAM_FNAME __PRETTY_FUNCTION__
#else
#   define ASG_TOOLS_MSGSTREAM_FNAME ""
#endif

/// Common prefix for the non-usual messages
///
/// The idea is that a regular user usually only wants to see DEBUG, INFO
/// and some WARNING messages. So those should be reasonably short. On the other
/// hand serious warnings (ERROR, FATAL) messages should be as precise
/// as possible to make debugging the issue easier.
///
#define ASG_TOOLS_MSGSTREAM_PREFIX \
   __FILE__ << ":" << __LINE__ << " (" << ASG_TOOLS_MSGSTREAM_FNAME << "): "

/// Macro used to print "serious" messages
#define ANA_MSG_LVL_SERIOUS( lvl, xmsg )                    \
   msg( lvl ) << ASG_TOOLS_MSGSTREAM_PREFIX << xmsg << endmsg

/// Macro used to print "regular" messages
#define ANA_MSG_LVL_NOCHK( lvl, xmsg )          \
   msg( lvl ) << xmsg << endmsg

/// Macro used to print "protected" messages
#define ANA_MSG_LVL( lvl, xmsg )                \
   do {                                         \
      if( msg().level() <= lvl ) {             \
         ANA_MSG_LVL_NOCHK( lvl, xmsg );        \
      }                                         \
   } while( 0 )

/// Macro printing verbose messages
#define ANA_MSG_VERBOSE( xmsg )  ANA_MSG_LVL( MSG::VERBOSE, xmsg )
/// Macro printing debug messages
#define ANA_MSG_DEBUG( xmsg )    ANA_MSG_LVL( MSG::DEBUG, xmsg )
/// Macro printing info messages
#define ANA_MSG_INFO( xmsg )     ANA_MSG_LVL_NOCHK( MSG::INFO,  xmsg )
/// Macro printing warning messages
#define ANA_MSG_WARNING( xmsg )  ANA_MSG_LVL_NOCHK( MSG::WARNING, xmsg )
/// Macro printing error messages
#define ANA_MSG_ERROR( xmsg )    ANA_MSG_LVL_SERIOUS( MSG::ERROR, xmsg )
/// Macro printing fatal messages
#define ANA_MSG_FATAL( xmsg )    ANA_MSG_LVL_SERIOUS( MSG::FATAL, xmsg )
/// Macro printing messages that should always appear
#define ANA_MSG_ALWAYS( xmsg )   ANA_MSG_LVL_NOCHK( MSG::ALWAYS, xmsg )



/// \brief the return type used by ANA_CHECK
///
/// this defaults to the regular StatusCode, but can be overriden via
/// \ref ANA_CHECK_SET_TYPE
typedef StatusCode AsgToolsCheckResultType;


/// \brief set the type for \ref ANA_CHECK to report failures
///
/// normally ANA_CHECK will report failures via a StatusCode object,
/// but this macro can be called inside a function to tell it to
/// report via a different type.
#define ANA_CHECK_SET_TYPE(TYPE)			\
  typedef TYPE AsgToolsCheckResultType;


/// \brief check whether the given expression was successful
///
/// This works like ATH_CHECK except it can handle all kinds of status
/// codes, not just the regular ones.  In particular the user can also
/// set the type of the status code returned via \ref
/// ANA_CHECK_SET_TYPE
#define ANA_CHECK(EXP)							\
  { const auto sc__ = EXP;						\
    typedef typename std::decay<decltype(sc__)>::type scType__;		\
    if (!::asg::CheckHelper<scType__>::isSuccess (sc__)) {		\
      ANA_MSG_ERROR ("Failed to call \"" << #EXP << "\"");		\
      return ::asg::CheckHelper<AsgToolsCheckResultType>::failureCode(); \
    } }


/// \brief check whether the given expression was successful, throwing
/// an exception on failure
///
/// This works like \ref ANA_CHECK except it throws when an error
/// occurs.  This is meant for situations in which we perform a check
/// inside a function that does not return a StatusCode.
#define ANA_CHECK_THROW(EXP)						\
  { const auto sc__ = EXP;						\
    typedef typename std::decay<decltype(sc__)>::type scType__;		\
    if (!::asg::CheckHelper<scType__>::isSuccess (sc__)) {		\
      std::ostringstream str;						\
      str << #EXP;							\
      ANA_MSG_ERROR ("Failed to call \"" << str.str() << "\", throwing exception"); \
      ::asg::detail::throw_check_fail (str.str());			\
    } }

#endif
