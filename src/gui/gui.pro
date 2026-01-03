TARGET     = QtGui
QPRO_PWD   = $$PWD
QT = core
DEFINES   += QT_BUILD_GUI_LIB QT_NO_USING_NAMESPACE
win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x65000000
irix-cc*:QMAKE_CXXFLAGS += -no_prelink -ptused

!win32:!embedded:!qpa:!mac:CONFIG      += x11

unix|win32-g++*:QMAKE_PKGCONFIG_REQUIRES = QtCore

include(../qbase.pri)

contains(QT_CONFIG, x11sm):CONFIG += x11sm

#platforms
x11:include(kernel/x11.pri)
mac:include(kernel/mac.pri)
win32:include(kernel/win.pri)
embedded:include(embedded/embedded.pri)

#modules
include(animation/animation.pri)
include(kernel/kernel.pri)
include(image/image.pri)
include(painting/painting.pri)
include(text/text.pri)
include(styles/styles.pri)
include(widgets/widgets.pri)
include(dialogs/dialogs.pri)
include(accessible/accessible.pri)
include(itemviews/itemviews.pri)
include(inputmethod/inputmethod.pri)
include(graphicsview/graphicsview.pri)
include(util/util.pri)
include(statemachine/statemachine.pri)
include(math3d/math3d.pri)
include(effects/effects.pri)

include(egl/egl.pri)
win32: DEFINES += QT_NO_EGL
embedded: QT += network

QMAKE_LIBS += $$QMAKE_LIBS_GUI

contains(DEFINES,QT_EVAL):include($$QT_SOURCE_TREE/src/corelib/eval.pri)

QMAKE_DYNAMIC_LIST_FILE = $$PWD/QtGui.dynlist

DEFINES += Q_INTERNAL_QAPP_SRC

neon:if(*-g++*|*-qcc*) {
    HEADERS += $$NEON_HEADERS

    neon_compiler.commands = $$QMAKE_CXX -c
    neon_compiler.commands += $(CXXFLAGS) $$QMAKE_CFLAGS_NEON $(INCPATH) ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
    neon_compiler.dependency_type = TYPE_C
    neon_compiler.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_BASE}$${first(QMAKE_EXT_OBJ)}
    neon_compiler.input = NEON_SOURCES
    neon_compiler.variable_out = OBJECTS
    neon_compiler.name = compiling[neon] ${QMAKE_FILE_IN}
    silent:neon_compiler.commands = @echo compiling[neon] ${QMAKE_FILE_IN} && $$neon_compiler.commands
    neon_assembler.commands = $$QMAKE_CC -c
    neon_assembler.commands += $(CFLAGS) $$QMAKE_CFLAGS_NEON $(INCPATH) ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
    neon_assembler.dependency_type = TYPE_C
    neon_assembler.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_BASE}$${first(QMAKE_EXT_OBJ)}
    neon_assembler.input = NEON_ASM
    neon_assembler.variable_out = OBJECTS
    neon_assembler.name = assembling[neon] ${QMAKE_FILE_IN}
    silent:neon_assembler.commands = @echo assembling[neon] ${QMAKE_FILE_IN} && $$neon_assembler.commands
    QMAKE_EXTRA_COMPILERS += neon_compiler neon_assembler
}

win32:!contains(QT_CONFIG, directwrite) {
    DEFINES += QT_NO_DIRECTWRITE
}

*-g++*|linux-icc*|*-clang  {
    sse2 {
        sse2_compiler.commands = $$QMAKE_CXX -c -Winline

        sse2_compiler.commands += $$QMAKE_CFLAGS_SSE2

        sse2_compiler.commands += $(CXXFLAGS) $(INCPATH) ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
        sse2_compiler.dependency_type = TYPE_C
        sse2_compiler.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_BASE}$${first(QMAKE_EXT_OBJ)}
        sse2_compiler.input = SSE2_SOURCES
        sse2_compiler.variable_out = OBJECTS
        sse2_compiler.name = compiling[sse2] ${QMAKE_FILE_IN}
        silent:sse2_compiler.commands = @echo compiling[sse2] ${QMAKE_FILE_IN} && $$sse2_compiler.commands
        QMAKE_EXTRA_COMPILERS += sse2_compiler
    }
    ssse3 {
        ssse3_compiler.commands = $$QMAKE_CXX -c -Winline

        ssse3_compiler.commands += $$QMAKE_CFLAGS_SSSE3

        ssse3_compiler.commands += $(CXXFLAGS) $(INCPATH) ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
        ssse3_compiler.dependency_type = TYPE_C
        ssse3_compiler.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_BASE}$${first(QMAKE_EXT_OBJ)}
        ssse3_compiler.input = SSSE3_SOURCES
        ssse3_compiler.variable_out = OBJECTS
        ssse3_compiler.name = compiling[ssse3] ${QMAKE_FILE_IN}
        silent:ssse3_compiler.commands = @echo compiling[ssse3] ${QMAKE_FILE_IN} && $$ssse3_compiler.commands
        QMAKE_EXTRA_COMPILERS += ssse3_compiler
    }
    avx {
        avx_compiler.commands = $$QMAKE_CXX -c $(CXXFLAGS)
        avx_compiler.commands += $$QMAKE_CFLAGS_AVX
        avx_compiler.commands += $(INCPATH) ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
        avx_compiler.dependency_type = TYPE_C
        avx_compiler.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_BASE}$${first(QMAKE_EXT_OBJ)}
        avx_compiler.input = AVX_SOURCES
        avx_compiler.variable_out = OBJECTS
        avx_compiler.name = compiling[avx] ${QMAKE_FILE_IN}
        silent:avx_compiler.commands = @echo compiling[avx] ${QMAKE_FILE_IN} && $$avx_compiler.commands
        QMAKE_EXTRA_COMPILERS += avx_compiler
    }
} else {
    sse2: SOURCES += $$SSE2_SOURCES
    ssse3: SOURCES += $$SSSE3_SOURCES
}
