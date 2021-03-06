//=========================================================================
//  CEXCEPTION.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Exception classes
//
//  Author: Andras Varga
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cstring>
#include <cstdlib>  // for exit()
#include <cstdarg>  // for va_list
#include <csignal>  // raise()
#include "common/stringutil.h"
#include "common/commonutil.h"
#include "omnetpp/cexception.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/ccomponent.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/platdep/platmisc.h"  // INT64_PRINTF_FORMAT and DEBUG_TRAP

using namespace omnetpp::common;

namespace omnetpp {

#define BUFLEN 1024
static char buffer[BUFLEN];
static char buffer2[BUFLEN];

#define LL  INT64_PRINTF_FORMAT

cException::cException() : std::exception()
{
    errorCode = E_CUSTOM;
    storeContext();
    msg = "n/a";
}

cException::cException(ErrorCodeInt errorcode...) : std::exception()
{
    va_list va;
    va_start(va, errorcode);
    init(nullptr, (ErrorCode)errorcode, cErrorMessages::get((ErrorCode)errorcode), va);
    va_end(va);
}

cException::cException(const char *msgformat...) : std::exception()
{
    va_list va;
    va_start(va, msgformat);
    init(nullptr, E_CUSTOM, msgformat, va);
    va_end(va);
}

cException::cException(const cObject *where, ErrorCodeInt errorcode...) : std::exception()
{
    va_list va;
    va_start(va, errorcode);
    init(where, (ErrorCode)errorcode, cErrorMessages::get((ErrorCode)errorcode), va);
    va_end(va);
}

cException::cException(const cObject *where, const char *msgformat...) : std::exception()
{
    va_list va;
    va_start(va, msgformat);
    init(where, E_CUSTOM, msgformat, va);
    va_end(va);
}

cException::cException(const cException& e) : std::exception(e)
{
    errorCode = e.errorCode;
    msg = e.msg;

    simulationStage = e.simulationStage;
    eventNumber = e.eventNumber;
    simtime = e.simtime;

    hasContext_ = e.hasContext_;
    contextClassName = e.contextClassName;
    contextFullPath = e.contextFullPath;
    contextComponentId = e.contextComponentId;
    contextComponentKind = e.contextComponentKind;
}

void cException::storeContext()
{
    cSimulation *sim = cSimulation::getActiveSimulation();

    if (!sim) {
        simulationStage = CTX_NONE;
        eventNumber = 0;
        simtime = SIMTIME_ZERO;
    }
    else {
        simulationStage = sim->getSimulationStage();
        eventNumber = sim->getEventNumber();
        simtime = sim->getSimTime();
    }

    if (!sim || !sim->getContext()) {
        hasContext_ = false;
        contextComponentId = -1;
    }
    else {
        hasContext_ = true;
        contextClassName = sim->getContext()->getClassName();
        contextFullPath = sim->getContext()->getFullPath().c_str();
        contextComponentId = sim->getContext()->getId();
        contextComponentKind = sim->getContext()->getComponentKind();
    }
}

void cException::exitIfStartupError()
{
    if (!cStaticFlag::insideMain()) {
        // note: ev may not be available at this time
        fprintf(stderr, "<!> Error during startup/shutdown: %s. Aborting.", what());
        abort();
    }
}

void cException::init(const cObject *where, ErrorCode errorcode, const char *fmt, va_list va)
{
    // store error code
    this->errorCode = errorcode;

    // prefix message with "where" object conditionally, as "(%s)%s:"
    //  - if object is the module itself: skip
    //  - if object is local in module: use getFullName()
    //  - if object is somewhere else: use getFullPath()
    buffer[0] = '\0';
    cSimulation *sim = cSimulation::getActiveSimulation();
    cComponent *context = sim ? sim->getContext() : nullptr;
    if (where && where != context) {
        // try: if context's fullpath is same as module fullpath + object fullname, no need to print path
        sprintf(buffer2, "%s.%s", (context ? context->getFullPath().c_str() : ""), where->getFullName());
        bool needpath = strcmp(buffer2, where->getFullPath().c_str()) != 0;
        sprintf(buffer, "(%s)%s: ", where->getClassName(), needpath ? where->getFullPath().c_str() : where->getFullName());
    }

    vsnprintf(buffer + strlen(buffer), BUFLEN - strlen(buffer), fmt, va);
    buffer[BUFLEN-1] = '\0';
    msg = buffer;

    // store context
    storeContext();

    // if a global object's ctor/dtor throws an exception, there won't be
    // anyone around to catch it, so print it and abort here.
    exitIfStartupError();
}

static const char *getKindStr(cComponent::ComponentKind kind, bool capitalized)
{
    switch (kind) {
        case cComponent::KIND_MODULE: return capitalized ? "Module" : "module";
        case cComponent::KIND_CHANNEL: return capitalized ? "Channel" : "channel";
        default: return capitalized ? "Component" : "component";
    }
}

std::string cException::getFormattedMessage() const
{
    std::string when, where;
    switch (getSimulationStage()) {
        case CTX_NONE: when = ""; break;
        case CTX_BUILD: when = " during network setup"; break; // note leading spaces
        case CTX_INITIALIZE: when = " during network initialization"; break;
        case CTX_EVENT: when = opp_stringf(" at t=%ss, event #%" LL "d", SIMTIME_STR(getSimtime()), getEventNumber()); break;
        case CTX_FINISH: when = " during finalization"; break;
        case CTX_CLEANUP: when = " during network cleanup"; break;
    }

    //TODO include event name
    if (hasContext()) {
        where = opp_stringf(" in %s (%s) %s (id=%d)",
                getKindStr((cComponent::ComponentKind)getContextComponentKind(), false),
                getContextClassName(), getContextFullPath(), getContextComponentId());
    }

    std::string whereWhen = opp_join(",", where.c_str(), when.c_str());
    std::string result = opp_join(" --", what(), whereWhen.c_str());  // note: isError() is not used
    return result;
}

//---

cTerminationException::cTerminationException(ErrorCodeInt errorcode...)
{
    va_list va;
    va_start(va, errorcode);
    init(nullptr, (ErrorCode)errorcode, cErrorMessages::get((ErrorCode)errorcode), va);
    va_end(va);
}

cTerminationException::cTerminationException(const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    init(nullptr, E_CUSTOM, msgformat, va);
    va_end(va);
}

//---

cRuntimeError::cRuntimeError(ErrorCodeInt errorcode...)
{
    va_list va;
    va_start(va, errorcode);
    init(nullptr, (ErrorCode)errorcode, cErrorMessages::get((ErrorCode)errorcode), va);
    va_end(va);
    breakIntoDebuggerIfRequested();
}

cRuntimeError::cRuntimeError(const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    init(nullptr, E_CUSTOM, msgformat, va);
    va_end(va);
    breakIntoDebuggerIfRequested();
}

cRuntimeError::cRuntimeError(const cObject *where, ErrorCodeInt errorcode...)
{
    va_list va;
    va_start(va, errorcode);
    init(where, (ErrorCode)errorcode, cErrorMessages::get((ErrorCode)errorcode), va);
    va_end(va);
    breakIntoDebuggerIfRequested();
}

cRuntimeError::cRuntimeError(const cObject *where, const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    init(where, E_CUSTOM, msgformat, va);
    va_end(va);

    breakIntoDebuggerIfRequested();
}

void cRuntimeError::breakIntoDebuggerIfRequested()
{
    if (getEnvir()->attachDebuggerOnErrors) {
        getEnvir()->attachDebugger();
    }
    else if (getEnvir()->debugOnErrors) {
#ifdef NDEBUG
        printf("\n[Warning: Program was compiled without debug info, ignoring debug-on-error=true setting.]\n");
#else
        printf("\n"
               "RUNTIME ERROR. A cRuntimeError exception is about to be thrown, and you\n"
               "requested (by setting debug-on-errors=true in the ini file) that errors\n"
               "abort execution and break into the debugger.\n\n"
#ifdef _MSC_VER
                "If you see a [Debug] button on the Windows crash dialog and you have\n"
                "just-in-time debugging enabled, select it to get into the Visual Studio\n"
                "debugger. Otherwise, you should already have attached to this process from\n"
                "Visual Studio. Once in the debugger, see you can browse to the context of\n"
                "the error in the \"Call stack\" debug view.\n\n"
#else
                "You should now probably be running the simulation under gdb or another\n"
                "debugger. The simulation kernel will now raise a SIGINT signal which will\n"
                "get you into the debugger. If you are not running under a debugger, you can\n"
                "still use the core dump for post-mortem debugging. Once in the debugger,\n"
                "view the call stack (in gdb: \"bt\" command) to see the context of the\n"
                "runtime error.\n\n"
#endif
                );

        printf("<!> %s\n", getFormattedMessage().c_str());
        printf("\nTRAPPING on the exception above, due to a debug-on-errors=true configuration option. Is your debugger ready?\n");
        fflush(stdout);

        // cause debugger interrupt or signal
        DEBUG_TRAP;
#endif
    }
}

}  // namespace omnetpp

