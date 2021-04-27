//==========================================================================
//  STOPWATCH.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_STOPWATCH_H
#define __OMNETPP_ENVIR_STOPWATCH_H

#include "envirdefs.h"
#include "omnetpp/platdep/timeutil.h"

namespace omnetpp {
namespace envir {

/**
 * Internal class for keeping track of simulation elapsed time and CPU usage,
 * and implementing corresponding time limits. Modeled after a stopwatch with
 * Start/Stop/Reset buttons.
 */
class ENVIR_API Stopwatch
{
private:
    // configuration
    timeval realtimeLimit;
    int64_t cpuTimeLimitClocks;
    bool clockRunning;
    bool hasTimeLimit_;  // cached state

    // state for tracking elapsed time
    timeval elapsedTime; // accumulates real time spent simulating
    timeval lastTime;    // result of previous gettimeofday() call

    // state for tracking cpu usage
    int64_t elapsedClocks;  // needed because clock_t range might be just ~72 hours
    clock_t lastClock;

private:
    void addRealtimeDelta();
    void addClockDelta();

public:
    Stopwatch();
    void setRealTimeLimit(double seconds); // specify negative value to clear limit
    void setCPUTimeLimit(double seconds); // specify negative value to clear limit
    void resetClock(); // call on network setup
    void startClock(); // call when simulation is started or paused
    void stopClock(); // call when simulation is paused or terminated
    bool hasTimeLimits() {return hasTimeLimit_;}
    void checkTimeLimits();  // call every few events; throws appropriate exception
    timeval getElapsedTime();
    double getElapsedSec();
    double getCPUUsageSec();
};

}  // namespace envir
}  // namespace omnetpp

#endif

