QT       += widgets
TEMPLATE  = app
CONFIG   += c++17
TARGET    = libssh2Test2

# ---- vcpkg paths (adjust if your vcpkg root differs) ----
VCPKG_ROOT = D:/Install/vcpkg/vcpkg
VCPKG_TRIPLET = x64-windows

INCLUDEPATH += $${VCPKG_ROOT}/installed/$${VCPKG_TRIPLET}/include

CONFIG(debug, debug|release) {
    LIBS += -L$${VCPKG_ROOT}/installed/$${VCPKG_TRIPLET}/debug/lib
} else {
    LIBS += -L$${VCPKG_ROOT}/installed/$${VCPKG_TRIPLET}/lib
}

LIBS += -llibssh2 -lws2_32

# ---- Sources ----
SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/sshworker.cpp

HEADERS += \
    src/mainwindow.h \
    src/sshworker.h
