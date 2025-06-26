win32:HEADERS += arch/qatomic_windows.h

HEADERS += \
    arch/qatomic_armv5.h \
    arch/qatomic_armv6.h \
    arch/qatomic_armv7.h \
    arch/qatomic_bootstrap.h \
    arch/qatomic_ia64.h \
    arch/qatomic_x86.h \
    arch/qatomic_gcc.h \
    arch/qatomic_cxx11.h

QT_ARCH_CPP = $$QT_SOURCE_TREE/src/corelib/arch/$$QT_ARCH
DEPENDPATH += $$QT_ARCH_CPP
include($$QT_ARCH_CPP/arch.pri, "", true)
