/****************************************************************************
** Meta object code from reading C++ file 'pr_m_wnd.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../pr_m_wnd.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pr_m_wnd.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Pr_M_Wnd_t {
    QByteArrayData data[11];
    char stringdata0[145];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Pr_M_Wnd_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Pr_M_Wnd_t qt_meta_stringdata_Pr_M_Wnd = {
    {
QT_MOC_LITERAL(0, 0, 8), // "Pr_M_Wnd"
QT_MOC_LITERAL(1, 9, 11), // "set_cmd_txt"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 3), // "str"
QT_MOC_LITERAL(4, 26, 18), // "on_btn_cmd_clicked"
QT_MOC_LITERAL(5, 45, 27), // "on_tr_reg_itemDoubleClicked"
QT_MOC_LITERAL(6, 73, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(7, 90, 4), // "item"
QT_MOC_LITERAL(8, 95, 6), // "column"
QT_MOC_LITERAL(9, 102, 22), // "on_tr_reg_itemExpanded"
QT_MOC_LITERAL(10, 125, 19) // "on_btn_task_clicked"

    },
    "Pr_M_Wnd\0set_cmd_txt\0\0str\0on_btn_cmd_clicked\0"
    "on_tr_reg_itemDoubleClicked\0"
    "QTreeWidgetItem*\0item\0column\0"
    "on_tr_reg_itemExpanded\0on_btn_task_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Pr_M_Wnd[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x08 /* Private */,
       4,    0,   42,    2, 0x08 /* Private */,
       5,    2,   43,    2, 0x08 /* Private */,
       9,    1,   48,    2, 0x08 /* Private */,
      10,    0,   51,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6, QMetaType::Int,    7,    8,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,

       0        // eod
};

void Pr_M_Wnd::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Pr_M_Wnd *_t = static_cast<Pr_M_Wnd *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->set_cmd_txt((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->on_btn_cmd_clicked(); break;
        case 2: _t->on_tr_reg_itemDoubleClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->on_tr_reg_itemExpanded((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 4: _t->on_btn_task_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject Pr_M_Wnd::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_Pr_M_Wnd.data,
      qt_meta_data_Pr_M_Wnd,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Pr_M_Wnd::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Pr_M_Wnd::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Pr_M_Wnd.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int Pr_M_Wnd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE