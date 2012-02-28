/****************************************************************************
** Meta object code from reading C++ file 'converter.h'
**
** Created: Mon 27. Feb 16:26:58 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../converter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'converter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Converter[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      29,   10,   10,   10, 0x0a,
      49,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Converter[] = {
    "Converter\0\0newImage(QImage*)\0"
    "charToQImage(char*)\0blockToTIFFs()\0"
};

void Converter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Converter *_t = static_cast<Converter *>(_o);
        switch (_id) {
        case 0: _t->newImage((*reinterpret_cast< QImage*(*)>(_a[1]))); break;
        case 1: _t->charToQImage((*reinterpret_cast< char*(*)>(_a[1]))); break;
        case 2: _t->blockToTIFFs(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Converter::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Converter::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Converter,
      qt_meta_data_Converter, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Converter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Converter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Converter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Converter))
        return static_cast<void*>(const_cast< Converter*>(this));
    return QObject::qt_metacast(_clname);
}

int Converter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void Converter::newImage(QImage * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
