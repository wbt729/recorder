/****************************************************************************
** Meta object code from reading C++ file 'tiffconverter.h'
**
** Created: Fri 24. Feb 18:05:17 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../tiffconverter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tiffconverter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TiffConverter[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      26,   21,   15,   14, 0x0a,
      74,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_TiffConverter[] = {
    "TiffConverter\0\0char*\0,,,,\0"
    "convertLine(char*,int,int,tsize_t,QTextStream*)\0"
    "convert()\0"
};

void TiffConverter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TiffConverter *_t = static_cast<TiffConverter *>(_o);
        switch (_id) {
        case 0: { char* _r = _t->convertLine((*reinterpret_cast< char*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< tsize_t(*)>(_a[4])),(*reinterpret_cast< QTextStream*(*)>(_a[5])));
            if (_a[0]) *reinterpret_cast< char**>(_a[0]) = _r; }  break;
        case 1: _t->convert(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData TiffConverter::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TiffConverter::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_TiffConverter,
      qt_meta_data_TiffConverter, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TiffConverter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TiffConverter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TiffConverter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TiffConverter))
        return static_cast<void*>(const_cast< TiffConverter*>(this));
    return QObject::qt_metacast(_clname);
}

int TiffConverter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
