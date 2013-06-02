set(LXD_SRCS 
    localxmltodo.cpp 
    localxmltodolist.cpp 
    localxmltodolistfactory.cpp
    localxmlbackend.cpp
)

set(LXD_HDRS
    localxmltodo.h
    localxmltodolist.h
    localxmltodolistfactory.h
    localxmlbackend.h
)

set(LXD_FILES
    LocalXmlBackend.json
)

add_library(LocalXmlBackend SHARED ${LXD_SRCS})

qt5_use_modules( LocalXmlBackend Core Xml)

include_directories(../../../OpenTodoListCore)

target_link_libraries( LocalXmlBackend OpenTodoListCore )

install( TARGETS LocalXmlBackend DESTINATION lib/opentodolist/plugins/opentodobackends )