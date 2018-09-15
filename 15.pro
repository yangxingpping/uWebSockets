TEMPLATE = app
CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.cpp \
    uSockets/src/eventing/epoll.c \
    uSockets/src/context.c \
    uSockets/src/socket.c \
    uSockets/src/eventing/libuv.c \
    uSockets/src/ssl.c \
    uSockets/src/loop.c

HEADERS += \
    src/HttpRouter.h \
    src/HttpParser.h \
    src/websocket/libwshandshake.hpp \
    src/websocket/WebSocketProtocol.h \
    src/websocket/WebSocket.h \
    src/websocket/WebSocketApp.h \
    src/HttpContext.h \
    src/HttpContextData.h \
    src/HttpResponseData.h \
    src/HttpResponse.h \
    src/StaticDispatch.h \
    src/LoopData.h \
    src/AsyncSocket.h \
    src/AsyncSocketData.h \
    src/Loop.h \
    src/App.h

INCLUDEPATH += uSockets/src src
QMAKE_CXXFLAGS += -fsanitize=address
LIBS += -lasan -lssl -lcrypto
