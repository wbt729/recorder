/****************************************************************************
** Meta object code from reading C++ file 'qeye.h'
**
** Created: Mon 27. Feb 14:27:19 2012
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
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
       6,    5,    5,    5, 0x05,
      24,    5,    5,    5, 0x05,
      35,    5,    5,    5, 0x05,
      46,    5,    5,    5, 0x05,
      58,    5,    5,    5, 0x05,

 // slots: signature, parameters, type, tag, flags
      74,    5,    5,    5, 0x08,
     100,    5,    5,    5, 0x0a,
     115,    5,    5,    5, 0x0a,
     129,    5,    5,    5, 0x0a,
     142,    5,    5,    5, 0x0a,
     155,    5,    5,    5, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QEye[] = {
    "QEye\0\0newImage(QImage*)\0starting()\0"
    "stopping()\0errors(int)\0newFrame(char*)\0"
    "onConversionDone(QImage*)\0startCapture()\0"
    "stopCapture()\0onNewFrame()\0onError(int)\0"
    "convertBlock()\0"
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
        case 4: _t->newFrame((*reinterpret_cast< char*(*)>(_a[1]))); break;
        case 5: _t->onConversionDone((*reinterpret_cast< QImage*(*)>(_a[1]))); break;
        case 6: _t->startCapture(); break;
        case 7: _t->stopCapture(); break;
        case 8: _t->onNewFrame(); break;
        case 9: _t->onError((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->convertBlock(); break;
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
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
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

// SIGNAL 4
void QEye::newFrame(char * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
