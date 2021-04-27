//==========================================================================
//  SQLITESCALARFILEEXPORTER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <cstdio>
#include <memory>
#include "common/stringutil.h"
#include "common/stringtokenizer.h"
#include "common/stlutil.h"
#include "xyarray.h"
#include "resultfilemanager.h"
#include "datasorter.h"
#include "datatable.h"
#include "exportutils.h"
#include "sqlitescalarfileexporter.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

class SqliteScalarFileExporterType : public ExporterType
{
    public:
        virtual std::string getFormatName() const {return "SqliteScalarFile";}
        virtual std::string getDisplayName() const {return "SQLite Scalar File";}
        virtual std::string getDescription() const {return "Export results in SQLite scalar file (.sca) format.";}
        virtual int getSupportedResultTypes() {return ResultFileManager::SCALAR | ResultFileManager::HISTOGRAM;}
        virtual std::string getFileExtension() {return "sca";}
        virtual StringMap getSupportedOptions() const;
        virtual std::string getXswtForm() const;
        virtual Exporter *create() const {return new SqliteScalarFileExporter();}
};

string SqliteScalarFileExporterType::getXswtForm() const
{
    return
            "<?xml version='1.0' encoding='UTF-8'?>\n"
            "<xswt xmlns:x='http://sweet_swt.sf.net/xswt'>\n"
            "  <import xmlns='http://sweet_swt.sf.net/xswt'>\n"
            "    <package name='java.lang'/>\n"
            "    <package name='org.eclipse.swt.widgets' />\n"
            "    <package name='org.eclipse.swt.graphics' />\n"
            "    <package name='org.eclipse.swt.layout' />\n"
            "    <package name='org.eclipse.swt.custom' />\n"
            "  </import>\n"
            "  <layout x:class='GridLayout' numColumns='2'/>\n"
            "  <x:children>\n"
            "    <group text='Options'>\n"
            "      <layoutData x:class='GridData' horizontalSpan='2' horizontalAlignment='FILL' grabExcessHorizontalSpace='true'/>\n"
            "      <layout x:class='GridLayout' numColumns='2'/>\n"
            "      <x:children>\n"
            "         <label text='Simtime scale exponent:'/>\n"
            "         <spinner x:id='simtimeScaleExp' x:style='BORDER' minimum='-14' maximum='0' textLimit='3' selection='-12'>\n"
            "           <layoutData x:class='GridData' widthHint='100'/>\n"
            "         </spinner>\n"
            "      </x:children>\n"
            "    </group>\n"
            "  </x:children>\n"
            "</xswt>\n";
}

StringMap SqliteScalarFileExporterType::getSupportedOptions() const
{
    StringMap options {
        {"simtimeScaleExp", "Simulation time scale exponent. "}, //TODO explain: simtime-resolution, raw int64's in the db, etc
    };
    return options;
}

//---

ExporterType *SqliteScalarFileExporter::getDescription()
{
    static SqliteScalarFileExporterType desc;
    return &desc;
}

void SqliteScalarFileExporter::setOption(const std::string& key, const std::string& value)
{
    checkOptionKey(getDescription(), key);
    if (key == "simtimeScaleExp")
        setSimtimeScaleExp(opp_atol(value.c_str()));
    else
        throw opp_runtime_error("Exporter: unhandled option '%s'", key.c_str());
}

void SqliteScalarFileExporter::saveResults(const std::string& fileName, ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor)
{
    //TODO progress reporting

    writer.open(fileName.c_str());

    RunList *runList = manager->getUniqueRuns(idlist);
    std::unique_ptr<RunList> tmp(runList);

    for (Run *run : *runList) {
        writer.beginRecordingForRun(run->runName, simtimeScaleExp, run->attributes, run->moduleParams);
        IDList filteredList = manager->filterIDList(idlist, run, nullptr, nullptr);
        for (int i=0; i<filteredList.size(); i++) {
            ID id = filteredList.get(i);
            if (ResultFileManager::getTypeOf(id) == ResultFileManager::SCALAR) {
                const ScalarResult& scalar = manager->getScalar(id);
                writer.recordScalar(*scalar.moduleNameRef, *scalar.nameRef, scalar.value, scalar.attributes);
            }
            if (ResultFileManager::getTypeOf(id) == ResultFileManager::HISTOGRAM) {
                const HistogramResult& histogram = manager->getHistogram(id);
                writer.recordHistogram(*histogram.moduleNameRef, *histogram.nameRef, histogram.getStatistics(), histogram.getBins(), histogram.attributes);
            }
        }
        writer.endRecordingForRun();
    }
    writer.close();
}

}  // namespace scave
}  // namespace omnetpp

