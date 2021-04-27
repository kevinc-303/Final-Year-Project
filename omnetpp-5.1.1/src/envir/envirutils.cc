//==========================================================================
//  ENVIRUTILS.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
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

#include "common/stringutil.h"
#include "common/unitconversion.h"
#include "common/opp_ctype.h"
#include "nedxml/nedparser.h"  // NEDParser::getBuiltInDeclarations()
#include "omnetpp/ccanvas.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cnedmathfunction.h"
#include "omnetpp/cnedfunction.h"
#include "omnetpp/cresultrecorder.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/cchannel.h"
#include "sim/resultfilters.h"  // ExpressionFilter
#include "sim/resultrecorders.h"  // ExpressionRecorder
#include "args.h"
#include "appreg.h"
#include "envirutils.h"

using namespace omnetpp::common;
using namespace omnetpp::nedxml;

namespace omnetpp {
namespace envir {

using std::ostream;

std::string EnvirUtils::getConfigOptionType(cConfigOption *option)
{
    if (option->isGlobal())
        return "Global setting (applies to all simulation runs)";
    else if (!option->isPerObject())
        return "Per-simulation-run setting";
    else {
        const char *typeComment = "";
        switch (option->getObjectKind()) {
            case cConfigOption::KIND_COMPONENT: typeComment = "modules and channels"; break;
            case cConfigOption::KIND_CHANNEL: typeComment = "channels"; break;
            case cConfigOption::KIND_MODULE: typeComment = "modules"; break;
            case cConfigOption::KIND_SIMPLE_MODULE: typeComment = "simple modules"; break;
            case cConfigOption::KIND_UNSPECIFIED_TYPE: typeComment = "modules and channels"; break; // note: this is currently only used for **.typename
            case cConfigOption::KIND_PARAMETER: typeComment = "module/channel parameters"; break;
            case cConfigOption::KIND_STATISTIC: typeComment = "statistics (@statistic)"; break;
            case cConfigOption::KIND_SCALAR: typeComment = "scalar results"; break;
            case cConfigOption::KIND_VECTOR: typeComment = "vector results"; break;
            case cConfigOption::KIND_OTHER: typeComment = "other objects"; break;
            case cConfigOption::KIND_NONE: ASSERT(false); break;
        }
        return std::string("Per-object setting for ") + typeComment;
    }

}

void EnvirUtils::dumpComponentList(std::ostream& out, const char *category, bool verbose)
{
    cConfigurationEx *config = getEnvir()->getConfigEx();
    bool wantAll = !strcmp(category, "all");
    bool processed = false;
    out << "\n";
    if (wantAll || !strcmp(category, "config") || !strcmp(category, "configdetails")) {
        processed = true;
        if (verbose)
            out << "Supported configuration options:\n";
        bool printDescriptions = strcmp(category, "configdetails") == 0;

        cRegistrationList *table = configOptions.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cConfigOption *obj = dynamic_cast<cConfigOption *>(table->get(i));
            ASSERT(obj);
            if (!printDescriptions)
                out << "  ";
            if (obj->isPerObject())
                out << "**.";
            out << obj->getName() << "=";
            out << "<" << cConfigOption::getTypeName(obj->getType()) << ">";
            if (obj->getUnit())
                out << ", unit=\"" << obj->getUnit() << "\"";
            if (obj->getDefaultValue())
                out << ", default:" << obj->getDefaultValue() << "";
            if (!printDescriptions)
                out << "; " << (obj->isGlobal() ? "global" : obj->isPerObject() ? "per-object" : "per-run") << " setting"; // TODO getOptionKindName()

            out << "\n";
            if (printDescriptions)
                out << "    " << getConfigOptionType(obj) << ".\n";
            if (printDescriptions && !opp_isempty(obj->getDescription()))
                out << opp_indentlines(opp_breaklines(obj->getDescription(), 75), "    ") << "\n";
            if (printDescriptions)
                out << "\n";
        }
        if (!wantAll && verbose)
            out << "Use '-h configvars' to print the list of dollar variables that can be used in configuration values.\n";
        out << "\n";
    }
    if (wantAll || !strcmp(category, "configvars")) {
        processed = true;
        if (verbose)
            out << "Predefined variables that can be used in config values:\n";
        std::vector<const char *> v = config->getPredefinedVariableNames();
        for (int i = 0; i < (int)v.size(); i++) {
            out << "${" << v[i] << "}\n";
            const char *desc = config->getVariableDescription(v[i]);
            out << opp_indentlines(opp_breaklines(desc, 75), "    ") << "\n";
        }
        out << "\n";
    }
    if (!strcmp(category, "latexconfig")) {  // internal undocumented option, for maintenance purposes
        // generate LaTeX code for the appendix in the User Manual
        processed = true;
        cRegistrationList *table = configOptions.getInstance();
        table->sort();
        out << "\\begin{description}\n";
        for (int i = 0; i < table->size(); i++) {
            cConfigOption *obj = dynamic_cast<cConfigOption *>(table->get(i));
            ASSERT(obj);
            out << "\\item[" << (obj->isPerObject() ? "**." : "") << opp_latexQuote(obj->getName()) << "] = ";
            out << "\\textit{<" << cConfigOption::getTypeName(obj->getType()) << ">}";
            if (obj->getUnit())
                out << ", unit=\\ttt{" << obj->getUnit() << "}";
            if (obj->getDefaultValue())
                out << ", default: \\ttt{" << opp_latexInsertBreaks(opp_latexQuote(obj->getDefaultValue())) << "}";
            out << "\\\\\n";
            out << "    \\textit{" << getConfigOptionType(obj) << ".}\\\\\n";
            out << opp_indentlines(opp_breaklines(opp_markup2Latex(opp_latexQuote(obj->getDescription())), 75), "    ") << "\n";
        }
        out << "\\end{description}\n\n";

        out << "Predefined variables that can be used in config values:\n\n";
        std::vector<const char *> v = config->getPredefinedVariableNames();
        out << "\\begin{description}\n";
        for (int i = 0; i < (int)v.size(); i++) {
            out << "\\item[\\$\\{" << v[i] << "\\}] : \\\\\n";
            const char *desc = config->getVariableDescription(v[i]);
            out << opp_indentlines(opp_breaklines(opp_markup2Latex(opp_latexQuote(desc)), 75), "    ") << "\n";
        }
        out << "\\end{description}\n\n";
    }
    if (!strcmp(category, "jconfig")) {  // internal undocumented option, for maintenance purposes
        // generate Java code for ConfigurationRegistry.java in the IDE
        processed = true;
        if (verbose)
            out << "Supported configuration options (as Java code):\n";
        cRegistrationList *table = configOptions.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cConfigOption *key = dynamic_cast<cConfigOption *>(table->get(i));
            ASSERT(key);

            std::string id = "CFGID_";
            for (const char *s = key->getName(); *s; s++)
                id.append(1, opp_isalpha(*s) ? opp_toupper(*s) : *s == '-' ? '_' : *s == '%' ? 'n' : *s);
            const char *method = key->isGlobal() ? "addGlobalOption" :
                                 !key->isPerObject() ? "addPerRunOption" :
                                 "addPerObjectOption";
            #define CASE(X)  case cConfigOption::X: typestring = #X; break;
            const char *typestring;
            switch (key->getType()) {
                CASE(CFG_BOOL)
                CASE(CFG_INT)
                CASE(CFG_DOUBLE)
                CASE(CFG_STRING)
                CASE(CFG_FILENAME)
                CASE(CFG_FILENAMES)
                CASE(CFG_PATH)
                CASE(CFG_CUSTOM)
            }
            #undef CASE

            #define CASE(X)  case cConfigOption::X: kindstring = #X; break;
            const char *kindstring;
            switch (key->getObjectKind()) {
                CASE(KIND_COMPONENT)
                CASE(KIND_CHANNEL)
                CASE(KIND_MODULE)
                CASE(KIND_SIMPLE_MODULE)
                CASE(KIND_UNSPECIFIED_TYPE)
                CASE(KIND_PARAMETER)
                CASE(KIND_STATISTIC)
                CASE(KIND_SCALAR)
                CASE(KIND_VECTOR)
                CASE(KIND_NONE)
                CASE(KIND_OTHER)
            }
            #undef CASE

            out << "    public static final ConfigOption " << id << " = ";
            out << method << (key->getUnit() ? "U" : "") << "(\n";
            out << "        \"" << key->getName() << "\", ";
            if (key->isPerObject())
                out << kindstring << ", ";
            if (!key->getUnit())
                out << typestring << ", ";
            else
                out << "\"" << key->getUnit() << "\", ";
            if (!key->getDefaultValue())
                out << "null";
            else
                out << "\"" << opp_replacesubstring(key->getDefaultValue(), "\"", "\\\"", true) << "\"";
            out << ",\n";

            std::string desc = key->getDescription();
            desc = opp_replacesubstring(desc, "\n", "\\n\n", true);  // keep explicit line breaks
            desc = opp_breaklines(desc, 75);  // break long lines
            desc = opp_replacesubstring(desc, "\"", "\\\"", true);
            desc = opp_replacesubstring(desc, "\n", " \" +\n\"", true);
            desc = opp_replacesubstring(desc, "\\n \"", "\\n\"", true);  // remove bogus space after explicit line breaks
            desc = "\"" + desc + "\"";

            out << opp_indentlines(desc, "        ") << ");\n";
        }
        out << "\n";

        std::vector<const char *> vars = config->getPredefinedVariableNames();
        for (int i = 0; i < (int)vars.size(); i++) {
            opp_string id = vars[i];
            opp_strupr(id.buffer());
            const char *desc = config->getVariableDescription(vars[i]);
            out << "    public static final String CFGVAR_" << id << " = addConfigVariable(";
            out << "\"" << vars[i] << "\", \"" << opp_replacesubstring(desc, "\"", "\\\"", true) << "\");\n";
        }
        out << "\n";
    }
    if (wantAll || !strcmp(category, "classes")) {
        processed = true;
        if (verbose)
            out << "Registered C++ classes, including modules, channels and messages:\n";
        cRegistrationList *table = classes.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cObject *obj = table->get(i);
            out << "  class " << obj->getFullName() << "\n";
        }
        if (verbose) {
            out << "Note: if your class is not listed, it needs to be registered in the\n";
            out << "C++ code using Define_Module(), Define_Channel() or Register_Class().\n";
        }
        out << "\n";
    }
    if (wantAll || !strcmp(category, "classdesc")) {
        processed = true;
        if (verbose)
            out << "Classes that have associated reflection information (needed for Tkenv inspectors):\n";
        cRegistrationList *table = classDescriptors.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cObject *obj = table->get(i);
            out << "  class " << obj->getFullName() << "\n";
        }
        out << "\n";
    }
    if (wantAll || !strcmp(category, "nedfunctions")) {
        processed = true;
        if (verbose)
            out << "Functions that can be used in NED expressions and in omnetpp.ini:\n";
        cRegistrationList *table = nedFunctions.getInstance();
        table->sort();
        std::set<std::string> categories;
        for (int i = 0; i < table->size(); i++) {
            cNEDFunction *nf = dynamic_cast<cNEDFunction *>(table->get(i));
            cNEDMathFunction *mf = dynamic_cast<cNEDMathFunction *>(table->get(i));
            categories.insert(nf ? nf->getCategory() : mf ? mf->getCategory() : "???");
        }
        for (std::set<std::string>::iterator ci = categories.begin(); ci != categories.end(); ++ci) {
            std::string category = (*ci);
            out << "\n Category \"" << category << "\":\n";
            for (int i = 0; i < table->size(); i++) {
                cObject *obj = table->get(i);
                cNEDFunction *nf = dynamic_cast<cNEDFunction *>(table->get(i));
                cNEDMathFunction *mf = dynamic_cast<cNEDMathFunction *>(table->get(i));
                const char *fcat = nf ? nf->getCategory() : mf ? mf->getCategory() : "???";
                const char *desc = nf ? nf->getDescription() : mf ? mf->getDescription() : "???";
                if (fcat == category) {
                    out << "  " << obj->getFullName() << " : " << obj->str() << "\n";
                    if (desc)
                        out << "    " << desc << "\n";
                }
            }
        }
        out << "\n";
    }
    if (wantAll || !strcmp(category, "neddecls")) {
        processed = true;
        if (verbose) {
            out << "Built-in NED declarations:\n\n";
            out << "---START---\n";
        }
        out << NEDParser::getBuiltInDeclarations();
        if (verbose)
            out << "---END---\n";
        out << "\n";
    }
    if (wantAll || !strcmp(category, "units")) {
        processed = true;
        if (verbose) {
            out << "Recognized physical units (note: other units can be used as well, only\n";
            out << "no automatic conversion will be available for them):\n";
        }
        std::vector<const char *> units = UnitConversion::getAllUnits();
        for (int i = 0; i < (int)units.size(); i++) {
            const char *u = units[i];
            const char *bu = UnitConversion::getBaseUnit(u);
            out << "  " << u << "\t" << UnitConversion::getLongName(u);
            if (omnetpp::opp_strcmp(u, bu) != 0)
                out << "\t" << UnitConversion::convertUnit(1, u, bu) << bu;
            out << "\n";
        }
        out << "\n";
    }
    if (wantAll || !strcmp(category, "enums")) {
        processed = true;
        if (verbose)
            out << "Enums defined in .msg files\n";
        cRegistrationList *table = enums.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cObject *obj = table->get(i);
            out << "  " << obj->getFullName() << " : " << obj->str() << "\n";
        }
        out << "\n";
    }
    if (wantAll || !strcmp(category, "userinterfaces")) {
        processed = true;
        if (verbose)
            out << "User interfaces loaded:\n";
        cRegistrationList *table = omnetapps.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cObject *obj = table->get(i);
            out << "  " << obj->getFullName() << " : " << obj->str() << "\n";
        }
        out << "\n";
    }

    if (wantAll || !strcmp(category, "resultfilters")) {
        processed = true;
        if (verbose)
            out << "Result filters that can be used in @statistic properties:\n";
        cRegistrationList *table = resultFilters.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cObject *obj = table->get(i);
            out << "  " << obj->getFullName() << " : " << obj->str() << "\n";
        }
        out << "\n";
    }

    if (wantAll || !strcmp(category, "resultrecorders")) {
        processed = true;
        if (verbose)
            out << "Result recorders that can be used in @statistic properties:\n";
        cRegistrationList *table = resultRecorders.getInstance();
        table->sort();
        for (int i = 0; i < table->size(); i++) {
            cObject *obj = table->get(i);
            out << "  " << obj->getFullName() << " : " << obj->str() << "\n";
        }
        out << "\n";
    }

    if (wantAll || !strcmp(category, "figures")) {
        processed = true;
        if (verbose)
            out << "Figure types and their accepted @figure property keys:\n";
        for (auto it : figureTypes) {
            std::string type = it.first;
            std::string className = it.second;
            cFigure *figure = check_and_cast<cFigure *>(createOne(className.c_str()));
            out << "  " << type << " (" << className << "): " << opp_join(figure->getAllowedPropertyKeys(), ", ") << "\n";
            delete figure;
        }
        out << "\n";
    }

    if (!processed)
        throw cRuntimeError("Unrecognized category for '-h' option: %s", category);
}

void EnvirUtils::dumpResultRecorders(std::ostream& out, cComponent *component)
{
    dumpComponentResultRecorders(out, component);
    if (component->isModule()) {
        cModule *module = (cModule *)component;
        for (cModule::SubmoduleIterator it(module); !it.end(); ++it)
            dumpResultRecorders(out, *it);
        for (cModule::ChannelIterator it(module); !it.end(); ++it)
            dumpResultRecorders(out, *it);
    }
}

void EnvirUtils::dumpComponentResultRecorders(std::ostream& out, cComponent *component)
{
    bool componentPathPrinted = false;
    std::vector<simsignal_t> signals = component->getLocalListenedSignals();
    for (unsigned int i = 0; i < signals.size(); i++) {
        bool signalNamePrinted = false;
        simsignal_t signalID = signals[i];
        std::vector<cIListener *> listeners = component->getLocalSignalListeners(signalID);
        for (unsigned int j = 0; j < listeners.size(); j++) {
            if (dynamic_cast<cResultListener *>(listeners[j])) {
                if (!componentPathPrinted) {
                    out << component->getFullPath() << " (" << component->getNedTypeName() << "):\n";
                    componentPathPrinted = true;
                }
                if (!signalNamePrinted) {
                    out << "    \"" << cComponent::getSignalName(signalID) << "\" (signalID="  << signalID << "):\n";
                    signalNamePrinted = true;
                }
                dumpResultRecorderChain(out, (cResultListener *)listeners[j], 0);
            }
        }
    }
}

void EnvirUtils::dumpResultRecorderChain(std::ostream& out, cResultListener *listener, int depth)
{
    std::string indent(4*depth+8, ' ');
    out << indent;
    if (ExpressionRecorder *expressionRecorder = dynamic_cast<ExpressionRecorder *>(listener))
        out << expressionRecorder->getExpression().str() << " (" << listener->getClassName() << ")";
    else if (ExpressionFilter *expressionFilter = dynamic_cast<ExpressionFilter *>(listener))
        out << expressionFilter->getExpression().str() << " (" << listener->getClassName() << ")";
    else
        out << listener->getClassName();

    if (cResultRecorder *resultRecorder = dynamic_cast<cResultRecorder *>(listener))
        out << " ==> " << resultRecorder->getResultName();
    out << "\n";

    if (cResultFilter *resultFilter = dynamic_cast<cResultFilter *>(listener)) {
        std::vector<cResultListener *> delegates = resultFilter->getDelegates();
        for (unsigned int i = 0; i < delegates.size(); i++)
            dumpResultRecorderChain(out, delegates[i], depth+1);
    }
}

}  // namespace envir
}  // namespace omnetpp

