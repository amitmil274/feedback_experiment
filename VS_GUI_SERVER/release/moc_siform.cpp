/****************************************************************************
** Meta object code from reading C++ file 'siform.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../siform.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'siform.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SIForm[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_SIForm[] = {
    "SIForm\0"
};

void SIForm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SIForm::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SIForm::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SIForm,
      qt_meta_data_SIForm, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SIForm::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SIForm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SIForm::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SIForm))
        return static_cast<void*>(const_cast< SIForm*>(this));
    if (!strcmp(_clname, "Ui::SIForm"))
        return static_cast< Ui::SIForm*>(const_cast< SIForm*>(this));
    return QDialog::qt_metacast(_clname);
}

int SIForm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_ServerNForm[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      44,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      32,   12,   12,   12, 0x08,
      44,   12,   12,   12, 0x08,
      56,   12,   12,   12, 0x08,
      77,   12,   12,   12, 0x08,
      98,   12,   12,   12, 0x08,
     119,   12,   12,   12, 0x08,
     140,   12,   12,   12, 0x08,
     161,   12,   12,   12, 0x08,
     182,   12,   12,   12, 0x08,
     203,   12,   12,   12, 0x08,
     224,   12,   12,   12, 0x08,
     252,  246,   12,   12, 0x08,
     276,   12,   12,   12, 0x08,
     294,  290,   12,   12, 0x08,
     319,   12,   12,   12, 0x08,
     339,   12,   12,   12, 0x08,
     370,   12,   12,   12, 0x08,
     407,   12,   12,   12, 0x08,
     429,   12,   12,   12, 0x08,
     452,   12,   12,   12, 0x08,
     470,   12,   12,   12, 0x08,
     487,   12,   12,   12, 0x08,
     503,   12,   12,   12, 0x08,
     520,   12,   12,   12, 0x08,
     537,   12,   12,   12, 0x08,
     556,  549,   12,   12, 0x08,
     574,  549,   12,   12, 0x08,
     594,  246,   12,   12, 0x08,
     613,   12,   12,   12, 0x08,
     641,  635,   12,   12, 0x08,
     662,  246,   12,   12, 0x08,
     688,  246,   12,   12, 0x08,
     713,   12,   12,   12, 0x08,
     736,   12,   12,   12, 0x08,
     756,  751,   12,   12, 0x08,
     779,   12,   12,   12, 0x08,
     802,   12,   12,   12, 0x08,
     822,   12,   12,   12, 0x08,
     846,   12,   12,   12, 0x08,
     867,   12,   12,   12, 0x08,
     886,  884,   12,   12, 0x08,
     905,   12,   12,   12, 0x08,
     924,  918,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ServerNForm[] = {
    "ServerNForm\0\0signalFinalClose()\0"
    "slotStart()\0slotClose()\0slotPScaleDecrease()\0"
    "slotPScaleIncrease()\0slotGScaleDecrease()\0"
    "slotGScaleIncrease()\0slotgripKpDecrease()\0"
    "slotgripKpIncrease()\0slotgripKdDecrease()\0"
    "slotgripKdIncrease()\0slotEnableGripForce()\0"
    "index\0slotgripForceStyle(int)\0"
    "slotZeroXYZ()\0cnt\0updateGUIFromMaster(int)\0"
    "slotGUInewConnect()\0slotGUIupdateStopButtonImage()\0"
    "slotGUIupdateConnectionButtonImage()\0"
    "slotReadClientatGUI()\0slotDataExchangeInit()\0"
    "slotUpdateTimer()\0slotZeroButton()\0"
    "slotTabChange()\0slotCheckLogin()\0"
    "slotSelectView()\0slotDCNew()\0toggle\0"
    "slotDCStart(bool)\0slotDCDictate(bool)\0"
    "slotIPChanged(int)\0slotCamAngleChanged()\0"
    "check\0slotITPCheckBox(int)\0"
    "slotRightToolChanged(int)\0"
    "slotLeftToolChanged(int)\0"
    "slotKeyPadAuthorized()\0updateMaster()\0"
    "data\0sendTCP(stUI2MA_DATA*)\0"
    "slotDisconnectMaster()\0slotEnableGripper()\0"
    "slotEnableOrientation()\0slotEnablePosition()\0"
    "slotFullscreen()\0b\0slotHttpRead(bool)\0"
    "slotCamOff()\0event\0closeEvent(QCloseEvent*)\0"
};

void ServerNForm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ServerNForm *_t = static_cast<ServerNForm *>(_o);
        switch (_id) {
        case 0: _t->signalFinalClose(); break;
        case 1: _t->slotStart(); break;
        case 2: _t->slotClose(); break;
        case 3: _t->slotPScaleDecrease(); break;
        case 4: _t->slotPScaleIncrease(); break;
        case 5: _t->slotGScaleDecrease(); break;
        case 6: _t->slotGScaleIncrease(); break;
        case 7: _t->slotgripKpDecrease(); break;
        case 8: _t->slotgripKpIncrease(); break;
        case 9: _t->slotgripKdDecrease(); break;
        case 10: _t->slotgripKdIncrease(); break;
        case 11: _t->slotEnableGripForce(); break;
        case 12: _t->slotgripForceStyle((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->slotZeroXYZ(); break;
        case 14: _t->updateGUIFromMaster((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->slotGUInewConnect(); break;
        case 16: _t->slotGUIupdateStopButtonImage(); break;
        case 17: _t->slotGUIupdateConnectionButtonImage(); break;
        case 18: _t->slotReadClientatGUI(); break;
        case 19: _t->slotDataExchangeInit(); break;
        case 20: _t->slotUpdateTimer(); break;
        case 21: _t->slotZeroButton(); break;
        case 22: _t->slotTabChange(); break;
        case 23: _t->slotCheckLogin(); break;
        case 24: _t->slotSelectView(); break;
        case 25: _t->slotDCNew(); break;
        case 26: _t->slotDCStart((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 27: _t->slotDCDictate((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 28: _t->slotIPChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 29: _t->slotCamAngleChanged(); break;
        case 30: _t->slotITPCheckBox((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 31: _t->slotRightToolChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 32: _t->slotLeftToolChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 33: _t->slotKeyPadAuthorized(); break;
        case 34: _t->updateMaster(); break;
        case 35: _t->sendTCP((*reinterpret_cast< stUI2MA_DATA*(*)>(_a[1]))); break;
        case 36: _t->slotDisconnectMaster(); break;
        case 37: _t->slotEnableGripper(); break;
        case 38: _t->slotEnableOrientation(); break;
        case 39: _t->slotEnablePosition(); break;
        case 40: _t->slotFullscreen(); break;
        case 41: _t->slotHttpRead((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 42: _t->slotCamOff(); break;
        case 43: _t->closeEvent((*reinterpret_cast< QCloseEvent*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ServerNForm::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ServerNForm::staticMetaObject = {
    { &SIForm::staticMetaObject, qt_meta_stringdata_ServerNForm,
      qt_meta_data_ServerNForm, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ServerNForm::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ServerNForm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ServerNForm::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ServerNForm))
        return static_cast<void*>(const_cast< ServerNForm*>(this));
    return SIForm::qt_metacast(_clname);
}

int ServerNForm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SIForm::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 44)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 44;
    }
    return _id;
}

// SIGNAL 0
void ServerNForm::signalFinalClose()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
