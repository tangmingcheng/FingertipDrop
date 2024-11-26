find_package(Qt6 REQUIRED COMPONENTS Gui WebSockets)



qt_add_qml_module(backend
    URI Backend
    VERSION 1.0
    RESOURCE_PREFIX "/web"
    RESOURCES
        "html/index.html"
        "js/script.js"
        "css/style.css"
    SOURCES
        backend.cpp
        backend.h
        syncserver.cpp
        syncserver.h
        mylistmodel.cpp
        mylistmodel.h
)

target_link_libraries(backend PRIVATE Qt6::Gui Qt6::WebSockets)
