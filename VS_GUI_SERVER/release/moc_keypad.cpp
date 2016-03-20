/****************************************************************************
** Meta object code from reading C++ file 'keypad.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../keypad.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'keypad.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KeyPad[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x05,

 // slots: signature, parameters, type, tag, flags
      27,    7,    7,    7, 0x08,
      38,    7,    7,    7, 0x08,
      49,    7,    7,    7, 0x08,
      60,    7,    7,    7, 0x08,
      71,    7,    7,    7, 0x08,
      82,    7,    7,    7, 0x08,
      93,    7,    7,    7, 0x08,
     104,    7,    7,    7, 0x08,
     115,    7,    7,    7, 0x08,
     126,    7,    7,    7, 0x08,
     137,    7,    7,    7, 0x08,
     149,    7,    7,    7, 0x08,
     162,    7,    7,    7, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_KeyPad[] = {
    "KeyPad\0\0signalAuthorized()\0slotKP_1()\0"
    "slotKP_2()\0slotKP_3()\0slotKP_4()\0"
    "slotKP_5()\0slotKP_6()\0slotKP_7()\0"
    "slotKP_8()\0slotKP_9()\0slotKP_0()\0"
    "slotKP_BS()\0slotKP_CLS()\0slotCheckPIN()\0"
};

void KeyPad::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KeyPad *_t = static_cast<KeyPad *>(_o);
        switch (_id) {
        case 0: _t->signalAuthorized(); break;
        case 1: _t->slotKP_1(); break;
        case 2: _t->slotKP_2(); break;
        case 3: _t->slotKP_3(); break;
        case 4: _t->slotKP_4(); break;
        case 5: _t->slotKP_5(); break;
        case 6: _t->slotKP_6(); break;
        case 7: _t->slotKP_7(); break;
        case 8: _t->slotKP_8(); break;
        case 9: _t->slotKP_9(); break;
        case 10: _t->slotKP_0(); break;
        case 11: _t->slotKP_BS(); break;
        case 12: _t->slotKP_CLS(); break;
        case 13: _t->slotCheckPIN(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData KeyPad::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KeyPad::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_KeyPad,
      qt_meta_data_KeyPad, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KeyPad::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KeyPad::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KeyPad::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KeyPad))
        return static_cast<void*>(const_cast< KeyPad*>(this));
    return QDialog::qt_metacast(_clname);
}

int KeyPad::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void KeyPad::signalAuthorized()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
