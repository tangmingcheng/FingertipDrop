find_package(Qt6 REQUIRED COMPONENTS Gui WebSockets)



qt_add_qml_module(backend
    URI Backend
    VERSION 1.0
    SOURCES
        backend.cpp
        backend.h
        syncserver.cpp
        syncserver.h
        mylistmodel.cpp
        mylistmodel.h
)

qt_add_resources(backend "htmlfiles"
    PREFIX "/"
    FILES
        css/style.css
        html/index.html
        js/script.js)

target_link_libraries(backend PRIVATE Qt6::Gui Qt6::WebSockets)
