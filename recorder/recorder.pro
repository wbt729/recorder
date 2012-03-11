TEMPLATE = app
TARGET = recorder
QT += core \
    gui
HEADERS += converter.h grabber.h qeye.h recorder.h storage.h tiffconverter.h imagelabel.h
SOURCES += main.cpp converter.cpp grabber.cpp qeye.cpp recorder.cpp storage.cpp tiffconverter.cpp imagelabel.cpp
LIBS += /usr/lib/libtiff.so /usr/lib/libueye_api.so 
RESOURCES += 
