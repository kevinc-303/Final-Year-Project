/****************************************************************************
** Meta object code from reading C++ file 'loginspector.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "loginspector.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'loginspector.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_omnetpp__qtenv__LogInspector_t {
    QByteArrayData data[22];
    char stringdata0[291];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_omnetpp__qtenv__LogInspector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_omnetpp__qtenv__LogInspector_t qt_meta_stringdata_omnetpp__qtenv__LogInspector = {
    {
QT_MOC_LITERAL(0, 0, 28), // "omnetpp::qtenv::LogInspector"
QT_MOC_LITERAL(1, 29, 16), // "selectionChanged"
QT_MOC_LITERAL(2, 46, 0), // ""
QT_MOC_LITERAL(3, 47, 8), // "cObject*"
QT_MOC_LITERAL(4, 56, 8), // "runUntil"
QT_MOC_LITERAL(5, 65, 12), // "fastRunUntil"
QT_MOC_LITERAL(6, 78, 14), // "stopSimulation"
QT_MOC_LITERAL(7, 93, 13), // "onFontChanged"
QT_MOC_LITERAL(8, 107, 12), // "onFindButton"
QT_MOC_LITERAL(9, 120, 9), // "findAgain"
QT_MOC_LITERAL(10, 130, 16), // "findAgainReverse"
QT_MOC_LITERAL(11, 147, 14), // "onFilterButton"
QT_MOC_LITERAL(12, 162, 12), // "onCaretMoved"
QT_MOC_LITERAL(13, 175, 9), // "lineIndex"
QT_MOC_LITERAL(14, 185, 6), // "column"
QT_MOC_LITERAL(15, 192, 14), // "onRightClicked"
QT_MOC_LITERAL(16, 207, 9), // "globalPos"
QT_MOC_LITERAL(17, 217, 14), // "toMessagesMode"
QT_MOC_LITERAL(18, 232, 9), // "toLogMode"
QT_MOC_LITERAL(19, 242, 16), // "saveColumnWidths"
QT_MOC_LITERAL(20, 259, 19), // "restoreColumnWidths"
QT_MOC_LITERAL(21, 279, 11) // "saveContent"

    },
    "omnetpp::qtenv::LogInspector\0"
    "selectionChanged\0\0cObject*\0runUntil\0"
    "fastRunUntil\0stopSimulation\0onFontChanged\0"
    "onFindButton\0findAgain\0findAgainReverse\0"
    "onFilterButton\0onCaretMoved\0lineIndex\0"
    "column\0onRightClicked\0globalPos\0"
    "toMessagesMode\0toLogMode\0saveColumnWidths\0"
    "restoreColumnWidths\0saveContent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_omnetpp__qtenv__LogInspector[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   94,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   97,    2, 0x08 /* Private */,
       5,    0,   98,    2, 0x08 /* Private */,
       6,    0,   99,    2, 0x08 /* Private */,
       7,    0,  100,    2, 0x08 /* Private */,
       8,    0,  101,    2, 0x09 /* Protected */,
       9,    0,  102,    2, 0x09 /* Protected */,
      10,    0,  103,    2, 0x09 /* Protected */,
      11,    0,  104,    2, 0x09 /* Protected */,
      12,    2,  105,    2, 0x09 /* Protected */,
      15,    3,  110,    2, 0x09 /* Protected */,
      17,    0,  117,    2, 0x09 /* Protected */,
      18,    0,  118,    2, 0x09 /* Protected */,
      19,    0,  119,    2, 0x09 /* Protected */,
      20,    0,  120,    2, 0x09 /* Protected */,
      21,    0,  121,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   13,   14,
    QMetaType::Void, QMetaType::QPoint, QMetaType::Int, QMetaType::Int,   16,   13,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void omnetpp::qtenv::LogInspector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LogInspector *_t = static_cast<LogInspector *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->selectionChanged((*reinterpret_cast< cObject*(*)>(_a[1]))); break;
        case 1: _t->runUntil(); break;
        case 2: _t->fastRunUntil(); break;
        case 3: _t->stopSimulation(); break;
        case 4: _t->onFontChanged(); break;
        case 5: _t->onFindButton(); break;
        case 6: _t->findAgain(); break;
        case 7: _t->findAgainReverse(); break;
        case 8: _t->onFilterButton(); break;
        case 9: _t->onCaretMoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 10: _t->onRightClicked((*reinterpret_cast< QPoint(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 11: _t->toMessagesMode(); break;
        case 12: _t->toLogMode(); break;
        case 13: _t->saveColumnWidths(); break;
        case 14: _t->restoreColumnWidths(); break;
        case 15: _t->saveContent(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (LogInspector::*_t)(cObject * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LogInspector::selectionChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject omnetpp::qtenv::LogInspector::staticMetaObject = {
    { &Inspector::staticMetaObject, qt_meta_stringdata_omnetpp__qtenv__LogInspector.data,
      qt_meta_data_omnetpp__qtenv__LogInspector,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *omnetpp::qtenv::LogInspector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *omnetpp::qtenv::LogInspector::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_omnetpp__qtenv__LogInspector.stringdata0))
        return static_cast<void*>(const_cast< LogInspector*>(this));
    return Inspector::qt_metacast(_clname);
}

int omnetpp::qtenv::LogInspector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Inspector::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void omnetpp::qtenv::LogInspector::selectionChanged(cObject * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
