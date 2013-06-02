set(OTLC_SRCS
    abstracttodo.cpp
    abstracttodolist.cpp
    objectmodel.cpp
    opentodolistcore.cpp
    opentodolistinterfaces.cpp
    pluginsloader.cpp
    todolistfactory.cpp
    todolistlibrary.cpp
    todosortfiltermodel.cpp
)

set(OTLC_HDRS
    abstracttodo.h
    abstracttodolist.h
    objectmodel.h
    opentodolistcore_global.h
    opentodolistinterfaces.h
    pluginsloader.h
    todolistlibrary.h
    todolistfactory.h
    opentodolistcore.h
    todosortfiltermodel.h
)

add_library(OpenTodoListCore SHARED ${OTLC_SRCS})

qt5_use_modules( OpenTodoListCore Core )

install(TARGETS OpenTodoListCore DESTINATION lib)
install(FILES ${OTLC_HDRS} DESTINATION include/OpenTodoList)
