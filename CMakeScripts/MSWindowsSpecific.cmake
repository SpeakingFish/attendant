if (WIN32 AND MINGW)

    # Windows
    add_definitions(-D_WIN32_WINNT=0x0501 -DWIN32)

    # MinGW
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=attributes -Wno-error=unused-variable -Wno-error=cpp")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=attributes -Wno-error=unused-variable -Wno-error=cpp")

    #Qt
    set(QT_QMAKE_EXECUTABLE c:/windevel/build/qt/4.8.5/bin/qmake.exe)

endif (WIN32 AND MINGW)
