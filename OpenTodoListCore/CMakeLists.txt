set(OTLC_SRCS
    abstracttodo.cpp
    abstracttodolist.cpp
    opentodolistcore.cpp
    pluginsloader.cpp
    todolistfactory.cpp
    todolistlibrary.cpp
)

set(OTLC_HDRS
    abstracttodo.h
    abstracttodolist.h
    opentodolistcore_global.h
    opentodolistinterfaces.h
    pluginsloader.h
    todolistlibrary.h
    todolistfactory.h
    opentodolistcore.h
)

add_library(OpenTodoListCore SHARED ${OTLC_SRCS})

qt5_use_modules( OpenTodoListCore Core )

install(TARGETS OpenTodoListCore DESTINATION bin)
install(FILES ${OTLC_HDRS} DESTINATION include/OpenTodoList)
