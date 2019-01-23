QT += widgets
#requires(qtConfig(filedialog))
CONFIG += C++11

QT += serialport

RC_FILE = myapp.rc

TARGET = shutter_control

HEADERS             = screenshot.h
SOURCES             = main.cpp \
                      screenshot.cpp

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/widgets/desktop/screenshot
#INSTALLS += target
