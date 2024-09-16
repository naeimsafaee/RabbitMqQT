# Qt Modules
QT += core gui widgets

# Paths to your source files
SOURCES += main.cpp \
           mainwindow.cpp \
           worker.cpp

HEADERS += mainwindow.h \
    worker.h

# Include paths for RabbitMQ-C (Update this path according to your setup)
INCLUDEPATH += c:/rabbitmq-c/include

# Linking RabbitMQ-C dynamic library
LIBS += -LC:/rabbitmq-c-build/build/librabbitmq/Release -lrabbitmq.4

# Windows specific: Add external libraries like OpenSSL if required
#win32:LIBS += -lssl -lcrypto
