//==========================================================================
//  FIELDS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_FIELDS_H
#define __OMNETPP_SCAVE_FIELDS_H

#include <functional>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "common/commonutil.h"
#include "common/patternmatcher.h"
#include "common/stringutil.h"
#include "resultfilemanager.h"
#include "scaveutils.h"

namespace omnetpp {
namespace scave {


class SCAVE_API ResultItemField
{
    public:
        static const char * const FILE;
        static const char * const RUN;
        static const char * const MODULE;
        static const char * const NAME;

        static const int FILE_ID        = 1;
        static const int RUN_ID         = 2;
        static const int MODULE_ID      = 3;
        static const int NAME_ID        = 4;
        static const int ATTR_ID        = 5;
        static const int RUN_ATTR_ID    = 6;
        static const int RUN_PARAM_ID   = 7;
    private:
        int id;
        std::string name; // constant names above + attribute/param names
        static int getFieldID(const std::string& fieldName);
    public:
        explicit ResultItemField(const std::string& fieldName);
        int getID() const { return id; }
        const std::string& getName() const { return name; };
        const std::string& getFieldValue(const ResultItem& d) const ;
        bool equal(const ResultItem& d1, const ResultItem& d2) const;
        int compare(const ResultItem& d1, const ResultItem& d2) const;
};

class SCAVE_API ResultItemAttribute
{
    public:
        static const char * const TYPE;
        static const char * const ENUM;

        static const StringVector& getAttributeNames();
        static bool isAttributeName(const std::string& name);
};


class SCAVE_API RunAttribute
{
    public:
        static const char * const INIFILE;
        static const char * const CONFIGNAME;
        static const char * const RUNNUMBER;
        static const char * const NETWORK;
        static const char * const EXPERIMENT;
        static const char * const MEASUREMENT;
        static const char * const REPLICATION;
        static const char * const DATETIME;
        static const char * const PROCESSID;
        static const char * const RESULTDIR;
        static const char * const REPETITION;
        static const char * const SEEDSET;
        static const char * const ITERATIONVARS;
        static const char * const ITERATIONVARS2;

        static const StringVector& getAttributeNames();
        static bool isAttributeName(const std::string& name);
};

inline const std::string& getAttribute(const ResultItem& d, const std::string& attrName)
{
    return d.getAttribute(attrName);
}

inline const std::string& getRunAttribute(const ResultItem &d, const std::string& attrName)
{
    return d.fileRunRef->runRef->getAttribute(attrName);
}

inline const std::string& getRunParam(const ResultItem& d, const std::string& paramName)
{
    return d.fileRunRef->runRef->getModuleParam(paramName);
}

inline const std::string& ResultItemField::getFieldValue(const ResultItem& d) const
{
    switch(id) {
        case FILE_ID:       return d.fileRunRef->fileRef->filePath;
        case RUN_ID:        return d.fileRunRef->runRef->runName;
        case MODULE_ID:     return *d.moduleNameRef;
        case NAME_ID:       return *d.nameRef;
        case ATTR_ID:       return getAttribute(d, name);
        case RUN_ATTR_ID:   return getRunAttribute(d, name);
        case RUN_PARAM_ID:  return getRunParam(d, name);
        default: throw opp_runtime_error("unknown result item");
    }
}

inline bool ResultItemField::equal(const ResultItem& d1, const ResultItem& d2) const
{
    switch (id) {
        case FILE_ID:       return d1.fileRunRef->fileRef == d2.fileRunRef->fileRef;
        case RUN_ID:        return d1.fileRunRef->runRef == d2.fileRunRef->runRef;
        case MODULE_ID:     return d1.moduleNameRef == d2.moduleNameRef;
        case NAME_ID:       return d1.nameRef == d2.nameRef;
        case ATTR_ID:       return getAttribute(d1, name) == getAttribute(d2, name);
        case RUN_ATTR_ID:   return getRunAttribute(d1, name) == getRunAttribute(d2, name);
        case RUN_PARAM_ID:  return getRunParam(d1, name) == getRunParam(d2, name);
        default: throw opp_runtime_error("unknown result item");
    }
}

inline int ResultItemField::compare(const ResultItem& d1, const ResultItem& d2) const
{
    using omnetpp::common::strdictcmp;
    switch (id) {
        case FILE_ID:       return strdictcmp(d1.fileRunRef->fileRef->filePath.c_str(), d2.fileRunRef->fileRef->filePath.c_str());
        case RUN_ID:        return strdictcmp(d1.fileRunRef->runRef->runName.c_str(), d2.fileRunRef->runRef->runName.c_str());
        case MODULE_ID:     return strdictcmp(d1.moduleNameRef->c_str(), d2.moduleNameRef->c_str());
        case NAME_ID:       return strdictcmp(d1.nameRef->c_str(), d2.nameRef->c_str());
        case ATTR_ID:       return strdictcmp(getAttribute(d1, name).c_str(), getAttribute(d2, name).c_str());
        case RUN_ATTR_ID:   return strdictcmp(getRunAttribute(d1, name).c_str(), getRunAttribute(d2, name).c_str());
        case RUN_PARAM_ID:  return strdictcmp(getRunParam(d1, name).c_str(), getRunParam(d2, name).c_str());
        default: throw opp_runtime_error("unknown result item");
    }
}

class SCAVE_API ResultItemFields
{
    private:
        std::vector<ResultItemField> fields;
    public:
        typedef std::vector<ResultItemField>::const_iterator const_iterator;

        static const StringVector& getFieldNames();

        ResultItemFields() {};
        explicit ResultItemFields(ResultItemField field);
        explicit ResultItemFields(const std::string& fieldName);
        explicit ResultItemFields(const StringVector& fieldNames);

        ResultItemFields complement() const;
        bool hasField(ResultItemField field) const;
        bool hasField(const std::string& fieldName) const;
        const_iterator begin() const { return fields.begin(); };
        const_iterator end() const { return fields.end(); };
        bool less(ID id1, ID id2, ResultFileManager *manager) const;
        bool less(const ResultItem& d1, const ResultItem& d2) const;
        bool equal(ID id1, ID id2, ResultFileManager *manager) const;
        bool equal(const ResultItem& d1, const ResultItem& d2) const;
};

#ifndef SWIG
/*
 * Equality test of ResultItems that compares the specified fields.
 */
struct ResultItemFieldsEqual : public std::binary_function<ResultItem, ResultItem, bool>
{
    ResultItemFields fields;
    ResultItemFieldsEqual(const ResultItemFields fields) : fields(fields) {}
    bool operator()(const ResultItem& d1, const ResultItem& d2) const { return fields.equal(d1, d2); }
};

struct IDFieldsEqual : public std::binary_function<ID,ID,bool>
{
    ResultItemFields fields;
    ResultFileManager *manager;
    IDFieldsEqual(const ResultItemFields& fields, ResultFileManager *manager) : fields(fields), manager(manager) {}
    bool operator()(ID id1, ID id2) const { return fields.equal(id1, id2, manager); }
};

/*
 * Comparator of ResultItems that compares the specified fields.
 */
struct ResultItemFieldsLess : public std::binary_function<ResultItem, ResultItem, bool>
{
    ResultItemFields fields;
    ResultItemFieldsLess() {}
    ResultItemFieldsLess(const ResultItemFields& fields) : fields(fields) {}
    bool operator()(const ResultItem& d1, const ResultItem& d2) const { return fields.less(d1, d2); }
};

struct IDFieldsLess : public std::binary_function<ID,ID,bool>
{
    ResultItemFields fields;
    ResultFileManager *manager;
    IDFieldsLess(const ResultItemFields& fields, ResultFileManager *manager) : fields(fields), manager(manager) {}
    bool operator()(ID id1, ID id2) const { return fields.less(id1, id2, manager); }
};
#endif

} // namespace scave
}  // namespace omnetpp


#endif

