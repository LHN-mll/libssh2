/****************************************************************************
** Meta object code from reading C++ file 'sshworker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/sshworker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sshworker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SshWorker_t {
    QByteArrayData data[37];
    char stringdata0[429];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SshWorker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SshWorker_t qt_meta_stringdata_SshWorker = {
    {
QT_MOC_LITERAL(0, 0, 9), // "SshWorker"
QT_MOC_LITERAL(1, 10, 9), // "connected"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 12), // "disconnected"
QT_MOC_LITERAL(4, 34, 15), // "connectionError"
QT_MOC_LITERAL(5, 50, 5), // "error"
QT_MOC_LITERAL(6, 56, 15), // "directoryListed"
QT_MOC_LITERAL(7, 72, 4), // "path"
QT_MOC_LITERAL(8, 77, 21), // "QList<RemoteFileInfo>"
QT_MOC_LITERAL(9, 99, 5), // "files"
QT_MOC_LITERAL(10, 105, 16), // "downloadProgress"
QT_MOC_LITERAL(11, 122, 10), // "remotePath"
QT_MOC_LITERAL(12, 133, 11), // "transferred"
QT_MOC_LITERAL(13, 145, 5), // "total"
QT_MOC_LITERAL(14, 151, 16), // "downloadFinished"
QT_MOC_LITERAL(15, 168, 14), // "uploadProgress"
QT_MOC_LITERAL(16, 183, 9), // "localPath"
QT_MOC_LITERAL(17, 193, 14), // "uploadFinished"
QT_MOC_LITERAL(18, 208, 14), // "deleteFinished"
QT_MOC_LITERAL(19, 223, 14), // "operationError"
QT_MOC_LITERAL(20, 238, 9), // "operation"
QT_MOC_LITERAL(21, 248, 13), // "connectToHost"
QT_MOC_LITERAL(22, 262, 4), // "host"
QT_MOC_LITERAL(23, 267, 4), // "port"
QT_MOC_LITERAL(24, 272, 4), // "user"
QT_MOC_LITERAL(25, 277, 8), // "password"
QT_MOC_LITERAL(26, 286, 18), // "disconnectFromHost"
QT_MOC_LITERAL(27, 305, 13), // "listDirectory"
QT_MOC_LITERAL(28, 319, 12), // "downloadFile"
QT_MOC_LITERAL(29, 332, 10), // "uploadFile"
QT_MOC_LITERAL(30, 343, 10), // "deleteFile"
QT_MOC_LITERAL(31, 354, 15), // "deleteDirectory"
QT_MOC_LITERAL(32, 370, 10), // "renameFile"
QT_MOC_LITERAL(33, 381, 7), // "oldPath"
QT_MOC_LITERAL(34, 389, 7), // "newPath"
QT_MOC_LITERAL(35, 397, 15), // "createDirectory"
QT_MOC_LITERAL(36, 413, 15) // "cancelOperation"

    },
    "SshWorker\0connected\0\0disconnected\0"
    "connectionError\0error\0directoryListed\0"
    "path\0QList<RemoteFileInfo>\0files\0"
    "downloadProgress\0remotePath\0transferred\0"
    "total\0downloadFinished\0uploadProgress\0"
    "localPath\0uploadFinished\0deleteFinished\0"
    "operationError\0operation\0connectToHost\0"
    "host\0port\0user\0password\0disconnectFromHost\0"
    "listDirectory\0downloadFile\0uploadFile\0"
    "deleteFile\0deleteDirectory\0renameFile\0"
    "oldPath\0newPath\0createDirectory\0"
    "cancelOperation"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SshWorker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  114,    2, 0x06 /* Public */,
       3,    0,  115,    2, 0x06 /* Public */,
       4,    1,  116,    2, 0x06 /* Public */,
       6,    2,  119,    2, 0x06 /* Public */,
      10,    3,  124,    2, 0x06 /* Public */,
      14,    1,  131,    2, 0x06 /* Public */,
      15,    3,  134,    2, 0x06 /* Public */,
      17,    1,  141,    2, 0x06 /* Public */,
      18,    1,  144,    2, 0x06 /* Public */,
      19,    2,  147,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      21,    4,  152,    2, 0x0a /* Public */,
      26,    0,  161,    2, 0x0a /* Public */,
      27,    1,  162,    2, 0x0a /* Public */,
      28,    2,  165,    2, 0x0a /* Public */,
      29,    2,  170,    2, 0x0a /* Public */,
      30,    1,  175,    2, 0x0a /* Public */,
      31,    1,  178,    2, 0x0a /* Public */,
      32,    2,  181,    2, 0x0a /* Public */,
      35,    1,  186,    2, 0x0a /* Public */,
      36,    0,  189,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 8,    7,    9,
    QMetaType::Void, QMetaType::QString, QMetaType::LongLong, QMetaType::LongLong,   11,   12,   13,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::QString, QMetaType::LongLong, QMetaType::LongLong,   16,   12,   13,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   20,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::UShort, QMetaType::QString, QMetaType::QString,   22,   23,   24,   25,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   11,   16,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   16,   11,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   33,   34,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void,

       0        // eod
};

void SshWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SshWorker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->connectionError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->directoryListed((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QList<RemoteFileInfo>(*)>(_a[2]))); break;
        case 4: _t->downloadProgress((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2])),(*reinterpret_cast< qint64(*)>(_a[3]))); break;
        case 5: _t->downloadFinished((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->uploadProgress((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2])),(*reinterpret_cast< qint64(*)>(_a[3]))); break;
        case 7: _t->uploadFinished((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->deleteFinished((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->operationError((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 10: _t->connectToHost((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 11: _t->disconnectFromHost(); break;
        case 12: _t->listDirectory((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->downloadFile((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 14: _t->uploadFile((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 15: _t->deleteFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: _t->deleteDirectory((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 17: _t->renameFile((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 18: _t->createDirectory((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 19: _t->cancelOperation(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<RemoteFileInfo> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SshWorker::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SshWorker::connected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SshWorker::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SshWorker::disconnected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SshWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SshWorker::connectionError)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (SshWorker::*)(const QString & , const QList<RemoteFileInfo> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SshWorker::directoryListed)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (SshWorker::*)(const QString & , qint64 , qint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SshWorker::downloadProgress)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (SshWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SshWorker::downloadFinished)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (SshWorker::*)(const QString & , qint64 , qint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SshWorker::uploadProgress)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (SshWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SshWorker::uploadFinished)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (SshWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SshWorker::deleteFinished)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (SshWorker::*)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SshWorker::operationError)) {
                *result = 9;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SshWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_SshWorker.data,
    qt_meta_data_SshWorker,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SshWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SshWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SshWorker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SshWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void SshWorker::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SshWorker::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void SshWorker::connectionError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SshWorker::directoryListed(const QString & _t1, const QList<RemoteFileInfo> & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void SshWorker::downloadProgress(const QString & _t1, qint64 _t2, qint64 _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void SshWorker::downloadFinished(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void SshWorker::uploadProgress(const QString & _t1, qint64 _t2, qint64 _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void SshWorker::uploadFinished(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void SshWorker::deleteFinished(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void SshWorker::operationError(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
