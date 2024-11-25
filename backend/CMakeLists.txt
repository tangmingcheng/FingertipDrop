find_package(Qt6 REQUIRED COMPONENTS Gui WebSockets)

qt_add_qml_module(backend
    URI Backend
    VERSION 1.0
    SOURCES
        backend.cpp
        backend.h
        syncserver.cpp
        syncserver.h
    RESOURCES
        resources.qrc
)

target_link_libraries(backend PRIVATE Qt6::Gui Qt6::WebSockets)
