//==NL=============================================================NL======
//  CDETECT.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//         File designed and written by the Hollandiaba Team
//
//   Member functions of
//     cTransientDetection  :  virtual base class for transient detection
//     cAccuracyDetection  :  virtual base class for result accuracy detection
//
//     cTDExpandingWindows    :  an algorithm for transient detection
//     cADByStddev    :  an algorithm for result accuracy detection
//
//   Authors: Jar Heijmans et al.
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cmath>
#include <cstdio>
#include "omnetpp/csimulation.h"  // simTime()
#include "omnetpp/cdetect.h"

using namespace std;

namespace omnetpp {

cTDExpandingWindows::cTDExpandingWindows(const cTDExpandingWindows& r) : cTransientDetection(r)
{
    func = nullptr;
    copy(r);
}

cTDExpandingWindows::cTDExpandingWindows(const char *name,
        int reps, int minw, double wind, double acc,
        PostTDFunc f, void *p) :
    cTransientDetection(name)
{
    callback = f;
    callbackData = p;
    setParameters(reps, minw, wind, acc);
    func = nullptr;
    size = 0;
    go = true;
}

cTDExpandingWindows::~cTDExpandingWindows()
{
    reset();
}

void cTDExpandingWindows::copy(const cTDExpandingWindows& res)
{
    // setHostObject(res.getHostObject());
    go = res.go;
    transval = res.transval;
    accuracy = res.accuracy;
    minwinds = res.minwinds;
    windexp = res.windexp;
    repeats = res.repeats;
    detreps = res.detreps;
    callback = res.callback;
    callbackData = res.callbackData;

    // copy actual contents  --VA
    size = res.size;
    if (res.func == nullptr) {
        func = nullptr;
    }
    else {
        func = new xy;
        func->x = res.func->x;
        func->y = res.func->y;
        xy *res_p = res.func, *p = func;
        while (res_p->next) {
            p->next = new xy;
            p = p->next;
            res_p = res_p->next;
            p->x = res_p->x;
            p->y = res_p->y;
        }
        p->next = nullptr;
    }
}

cTDExpandingWindows& cTDExpandingWindows::operator=(const cTDExpandingWindows& res)
{
    if (this == &res)
        return *this;
    cOwnedObject::operator=(res);
    copy(res);
    return *this;
}

void cTDExpandingWindows::reset()
{
    detreps = repeats;
    transval = false;

    while (func) {
        xy *p = func;
        func = func->next;
        delete p;
    }
    size = 0;
}

void cTDExpandingWindows::setParameters(int reps, int minw, double wind, double acc)
{
    repeats = detreps = reps;
    minwinds = minw;
    windexp = wind;
    accuracy = acc;
}

void cTDExpandingWindows::collect(double val)
{
    if (go) {
        int maxsize = minwinds + (int)(minwinds * windexp + 0.5);
        xy *pair = new xy;  // create new element
        pair->x = SIMTIME_DBL(getSimulation()->getSimTime());
        pair->y = val;
        pair->next = nullptr;
        if (size == 0) {  // and add it at the end
            func = pair;
            size++;
        }
        else {
            xy *dum = func;
            while (dum->next)
                dum = dum->next;  // not very efficient! --VA
            dum->next = pair;
            if (size < maxsize)
                size++;
            else {  // if size == maxsize
                dum = func;  // delete first element
                func = func->next;
                delete dum;
            }
        }
        detectTransient();  // compute the new value of transval
    }
}

void cTDExpandingWindows::detectTransient()
{
    int ws1 = minwinds;
    int ws2 = (int)(0.5 + ws1 * windexp);  // round or something
    int maxsize = ws1 + ws2;
    if (size == maxsize) {
        xy *dum = func->next;
        double minval = func->y;
        // Get the minimum in order to move the axes
        while ((dum = dum->next) != nullptr)  // chged from while(dum=dum->next) --VA
            if (dum->y < minval)
                minval = dum->y;


        // Computation of the average of the values in the first window
        double avg1 = 0.0;
        double prevx = func->x;
        int i;
        dum = func->next;
        for (i = 2; i <= ws1; i++) {
            avg1 += (dum->y - minval) * (dum->x - prevx);
            prevx = dum->x;
            dum = dum->next;
        }
        avg1 = avg1 / (ws1 - 1);

        // Computation of the average of the values in the next window
        double avg2 = 0.0;
        prevx = dum->x;
        dum = dum->next;
        for (i = 2; i <= ws2; i++) {
            avg2 += (dum->y - minval) * (dum->x - prevx);
            prevx = dum->x;
            dum = dum->next;
        }
        avg2 = avg2 / (ws2 - 1);

        // Comparison of the two averages.
        double value = fabs(avg2 != 0 ? (avg1 - avg2) / avg2 : avg1);
        if (value < accuracy)
            detreps--;
        else
            detreps = repeats;
        transval = (detreps <= 0);

        // If the transient period over, call registered function
        if (transval && (callback != nullptr)) {
            (*callback)(this, callbackData);
        }
    }
}

//----

cADByStddev::cADByStddev(const cADByStddev& r) : cAccuracyDetection(r)
{
    go = resaccval = false;
    detreps = sctr = 0;
    ssum = sqrsum = 0.0;
    copy(r);
}

cADByStddev::cADByStddev(const char *name,
        double acc, int reps, PostADFunc f, void *p) :
    cAccuracyDetection(name)
{
    callback = f;
    callbackData = p;
    accuracy = acc;
    repeats = detreps = reps;
    go = resaccval = false;
    sctr = 0;
    ssum = sqrsum = 0.0;
}

void cADByStddev::copy(const cADByStddev& res)
{
    go = res.go;
    resaccval = res.resaccval;
    accuracy = res.accuracy;
    sctr = res.sctr;
    ssum = res.ssum;
    sqrsum = res.sqrsum;
    repeats = res.repeats;
    detreps = res.detreps;
    callback = res.callback;
    callbackData = res.callbackData;
}

cADByStddev& cADByStddev::operator=(const cADByStddev& res)
{
    if (this == &res)
        return *this;
    cOwnedObject::operator=(res);
    copy(res);
    return *this;
}

double cADByStddev::getStddev() const
{
    if (!sctr)
        return 0.0;
    else {
        double devsqr = (sqrsum - ssum * ssum / sctr) / sctr;
        if (devsqr <= 0)
            return 0.0;
        else
            return sqrt(devsqr);
    }
}

void cADByStddev::reset()
{
    ssum = sqrsum = 0.0;
    sctr = 0;
    resaccval = false;
}

void cADByStddev::collect(double val)
{
    if (go) {
        ssum += val;
        sqrsum += val * val;
        sctr++;
        if (sctr > 6)
            detectAccuracy();
    }
}

void cADByStddev::detectAccuracy()
{
    // the actual algorithm: divide the standard deviation by the square of
    // the number of values and check if this is small enough

    double sdvmean = (getStddev() / (sctr * sctr));
    if (sdvmean <= accuracy)
        detreps--;
    else
        detreps = repeats;
    resaccval = (detreps <= 0);

    // If we have the accuracy, call registered function
    if (resaccval && callback != nullptr) {
        (*callback)(this, callbackData);
    }
}

}  // namespace omnetpp

