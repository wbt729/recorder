/****************************************************************************
** Meta object code from reading C++ file 'qeye.h'
**
** Created: Fri 24. Feb 10:55:23 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qeye.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qeye.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QEye[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
       6,    5,    5,    5, 0x05,
      24,    5,    5,    5, 0x05,
      35,    5,    5,    5, 0x05,
      46,    5,    5,    5, 0x05,

 // slots: signature, parameters, type, tag, flags
      58,    5,    5,    5, 0x08,
      71,    5,    5,    5, 0x0a,
      86,    5,    5,    5, 0x0a,
     100,    5,    5,    5, 0x0a,
     113,    5,    5,    5, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QEye[] = {
    "QEye\0\0newImage(QImage*)\0starting()\0"
    "stopping()\0errors(int)\0frameDelay()\0"
    "startCapture()\0stopCapture()\0onNewFrame()\0"
    "onError(int)\0"
};

void QEye::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QEye *_t = static_cast<QEye *>(_o);
        switch (_id) {
        case 0: _t->newImage((*reinterpret_cast< QImage*(*)>(_a[1]))); break;
        case 1: _t->starting(); break;
        case 2: _t->stopping(); break;
        case 3: _t->errors((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->frameDelay(); break;
        case 5: _t->startCapture(); break;
        case 6: _t->stopCapture(); break;
        case 7: _t->onNewFrame(); break;
        case 8: _t->onError((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QEye::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QEye::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QEye,
      qt_meta_data_QEye, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QEye::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QEye::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QEye::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QEye))
        return static_cast<void*>(const_cast< QEye*>(this));
    return QObject::qt_metacast(_clname);
}

int QEye::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void QEye::newImage(QImage * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QEye::starting()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QEye::stopping()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void QEye::errors(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
