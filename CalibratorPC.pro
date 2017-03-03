#-------------------------------------------------
#
# Project created by QtCreator 2014-05-06T13:55:09
#
#-------------------------------------------------

DEFINES += IRS_USE_HID_WIN_API=1
DEFINES += IRS_USE_JSON_CPP=1
DEFINES += IRS_LIB_IRSMBUS_DEBUG_TYPE=IRS_LIB_DEBUG_NONE

QT       += core gui
QT       += axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CalibratorPC
TEMPLATE = app
QMAKE_TARGET_PRODUCT = N4-25
QMAKE_TARGET_COMPANY = ООО "Радиоэлектронные системы"

INCLUDEPATH += .
INCLUDEPATH += ../boost/
INCLUDEPATH += ../mxsrclib/
INCLUDEPATH += ../mxsrclib/arch/qt/
INCLUDEPATH += ../extern_libs/winapi/hid
INCLUDEPATH += ../extern_libs/jsoncpp/include

win32-g++ {
  QMAKE_CXXFLAGS += -finput-charset=CP1251
  QMAKE_CXXFLAGS += -fno-strict-aliasing
  QMAKE_CXXFLAGS += -Wno-deprecated
  QMAKE_CXXFLAGS += -Wno-unused-parameter
  QMAKE_CXXFLAGS += -Wextra
  QMAKE_CXXFLAGS += -Woverloaded-virtual
  QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
  QMAKE_CXXFLAGS += -Wno-unused-function
  QMAKE_CXXFLAGS += -std=c++11
  #QMAKE_CXXFLAGS += -Wno-enum-compare
}

win32-msvc2010 {
  QMAKE_CXXFLAGS -= -Zc:wchar_t-
  QMAKE_CXXFLAGS -= -w34100
  QMAKE_CXXFLAGS += -wd4800
  QMAKE_CXXFLAGS += -w44355
  QMAKE_CXXFLAGS += -wd4996
  QMAKE_CFLAGS_RELEASE = -Zi -MDd
  QMAKE_CXXFLAGS += -MP8
}

win32-msvc2012 {
  QMAKE_CXXFLAGS -= -Zc:wchar_t-
  QMAKE_CXXFLAGS -= -w34100
  QMAKE_CXXFLAGS += -wd4800
  QMAKE_CXXFLAGS += -w44355
  QMAKE_CXXFLAGS += -wd4996
  QMAKE_CXXFLAGS += -wd4250
  QMAKE_CFLAGS_RELEASE = -Zi -MDd
  QMAKE_CXXFLAGS += -MP8
}

LIBS += -lws2_32
LIBS += -lwinmm
LIBS += -lHid
LIBS += -lSetupapi
LIBS += -L"../boost/stage/lib/"
# Для hid.lib
LIBS += -L"C:/Program Files (x86)/Windows Kits/8.0/Lib/win8/um/x86"

SOURCES += main.cpp\
        mainwindow.cpp \
    ../mxsrclib/correct_alg.cpp \
    ../mxsrclib/csvwork.cpp \
    ../mxsrclib/dbgprn.cpp \
    ../mxsrclib/hardflowg.cpp \
    ../mxsrclib/irsadc.cpp \
    ../mxsrclib/irsalg.cpp \
    ../mxsrclib/irscalc.cpp \
    ../mxsrclib/irscpu.cpp \
    ../mxsrclib/irsdbgutil.cpp \
    ../mxsrclib/irsdcp.cpp \
    ../mxsrclib/irsdev.cpp \
    ../mxsrclib/irsdevices.cpp \
    ../mxsrclib/irsdsp.cpp \
    ../mxsrclib/irserror.cpp \
    ../mxsrclib/irsexcept.cpp \
    ../mxsrclib/irsfilter.cpp \
    ../mxsrclib/irsfunnel.cpp \
    ../mxsrclib/irsgpio.cpp \
    ../mxsrclib/irshfftp.cpp \
    ../mxsrclib/irsinit.cpp \
    ../mxsrclib/irsint.cpp \
    ../mxsrclib/irskbd.cpp \
    ../mxsrclib/irslocale.cpp \
    ../mxsrclib/irslwip.cpp \
    ../mxsrclib/irsmath.cpp \
    ../mxsrclib/irsmatrix.cpp \
    ../mxsrclib/irsmbus.cpp \
    ../mxsrclib/irsmcutil.cpp \
    ../mxsrclib/irsmem.cpp \
    ../mxsrclib/irsmenu.cpp \
    ../mxsrclib/irsprop.cpp \
    ../mxsrclib/irsstd.cpp \
    ../mxsrclib/irsstring.cpp \
    ../mxsrclib/irsstrm.cpp \
    ../mxsrclib/irssysutils.cpp \
    ../mxsrclib/irstcpip.cpp \
    ../mxsrclib/irstime.cpp \
    ../mxsrclib/irsvariant.cpp \
    ../mxsrclib/measmul.cpp \
    ../mxsrclib/meassup.cpp \
    ../mxsrclib/mxdata.cpp \
    ../mxsrclib/mxextbase.cpp \
    ../mxsrclib/mxextchart.cpp \
    ../mxsrclib/mxifar.cpp \
    ../mxsrclib/mxnet.cpp \
    ../mxsrclib/mxnetc.cpp \
    ../mxsrclib/mxnetr.cpp \
    ../mxsrclib/niusbgpib.cpp \
    ../mxsrclib/niusbgpib_hardflow.cpp \
    ../mxsrclib/timer.cpp \
    ../mxsrclib/arch/qt/counter.cpp \
    ../mxsrclib/arch/qt/mxifa.cpp \
    settings.cpp \
    calibrator.cpp \
    settingsdialog.cpp \
    edittemplatedialog.cpp \
    makeresultdialog.cpp \
    msword.cpp \
    msword_document.cpp \
    ../extern_libs/jsoncpp/src/lib_json/json_reader.cpp \
    ../extern_libs/jsoncpp/src/lib_json/json_value.cpp \
    ../extern_libs/jsoncpp/src/lib_json/json_writer.cpp \
    DeviceParametersForm.cpp \
    GeneratorModeForm.cpp \
    ParamsTreeModel.cpp

HEADERS  += mainwindow.h \
    ../mxsrclib/correct_alg.h \
    ../mxsrclib/csvwork.h \
    ../mxsrclib/dbgprn.h \
    ../mxsrclib/hardflowg.h \
    ../mxsrclib/irsadc.h \
    ../mxsrclib/irsadcabs.h \
    ../mxsrclib/irsalg.h \
    ../mxsrclib/irscalc.h \
    ../mxsrclib/irschartwin.h \
    ../mxsrclib/irsconfig.h \
    ../mxsrclib/irsconsole.h \
    ../mxsrclib/irsconsolestd.h \
    ../mxsrclib/irscpp.h \
    ../mxsrclib/irscpu.h \
    ../mxsrclib/irsdacabs.h \
    ../mxsrclib/irsdbgutil.h \
    ../mxsrclib/irsdcp.h \
    ../mxsrclib/irsdefs.h \
    ../mxsrclib/irsdefsbase.h \
    ../mxsrclib/irsdev.h \
    ../mxsrclib/irsdevices.h \
    ../mxsrclib/irsdsp.h \
    ../mxsrclib/irserror.h \
    ../mxsrclib/irsexcept.h \
    ../mxsrclib/irsfilter.h \
    ../mxsrclib/irsfinal.h \
    ../mxsrclib/irsfunnel.h \
    ../mxsrclib/irsgpio.h \
    ../mxsrclib/irshfftp.h \
    ../mxsrclib/irsinit.h \
    ../mxsrclib/irsint.h \
    ../mxsrclib/irskbd.h \
    ../mxsrclib/irslimits.h \
    ../mxsrclib/irslocale.h \
    ../mxsrclib/irslwip.h \
    ../mxsrclib/irsmath.h \
    ../mxsrclib/irsmatrix.h \
    ../mxsrclib/irsmbus.h \
    ../mxsrclib/irsmcutil.h \
    ../mxsrclib/irsmem.h \
    ../mxsrclib/irsmenu.h \
    ../mxsrclib/irsnetdefs.h \
    ../mxsrclib/irsparamabs.h \
    ../mxsrclib/irspch.h \
    ../mxsrclib/irsprop.h \
    ../mxsrclib/irsrect.h \
    ../mxsrclib/irsspi.h \
    ../mxsrclib/irsstd.h \
    ../mxsrclib/irsstrconv.h \
    ../mxsrclib/irsstrdefs.h \
    ../mxsrclib/irsstring.h \
    ../mxsrclib/irsstrm.h \
    ../mxsrclib/irsstrmstd.h \
    ../mxsrclib/irssysutils.h \
    ../mxsrclib/irstable.h \
    ../mxsrclib/irstcpip.h \
    ../mxsrclib/irstest.h \
    ../mxsrclib/irstime.h \
    ../mxsrclib/irstree.h \
    ../mxsrclib/irsvariant.h \
    ../mxsrclib/lwipopts.h \
    ../mxsrclib/measdef.h \
    ../mxsrclib/measmul.h \
    ../mxsrclib/meassup.h \
    ../mxsrclib/mxdata.h \
    ../mxsrclib/mxdatastd.h \
    ../mxsrclib/mxextbase.h \
    ../mxsrclib/mxextchart.h \
    ../mxsrclib/mxifa.h \
    ../mxsrclib/mxifar.h \
    ../mxsrclib/mxnet.h \
    ../mxsrclib/mxnetc.h \
    ../mxsrclib/mxnetd.h \
    ../mxsrclib/mxnetr.h \
    ../mxsrclib/niusbgpib.h \
    ../mxsrclib/niusbgpib_hardflow.h \
    ../mxsrclib/timer.h \
    ../mxsrclib/tstlan4abs.h \
    ../mxsrclib/arch/qt/counter.h \
    ../mxsrclib/arch/qt/Decl-32.h \
    ../mxsrclib/arch/qt/irsdefsarch.h \
    ../mxsrclib/arch/qt/mxifal.h \
    ../mxsrclib/arch/qt/mxnetda.h \
    ../mxsrclib/arch/qt/ni488.h \
    settings.h \
    calibrator.h \
    settingsdialog.h \
    ../mxsrclib/irs_codecvt.h \
    edittemplatedialog.h \
    makeresultdialog.h \
    msword.h \
    msword_document.h \
    DeviceParametersForm.h \
    GeneratorModeForm.h \
    ParamsTreeModel.h

FORMS    += mainwindow.ui \
    settingsdialog.ui \
    edittemplatedialog.ui \
    makeresultdialog.ui \
    generatormodeform.ui \
    deviceparametersform.ui

RESOURCES += \
    Res.qrc
