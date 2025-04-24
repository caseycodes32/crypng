@REM Build for MINGW64 or 32 from MSYS2.
@set OUT_DIR=Output
@set OUT_EXE=crypng
@set INCLUDES=-I imgui/ -I imgui/backends
@set SOURCES=main.cpp image_handler.cpp aes.c imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_win32.cpp imgui/imgui*.cpp
@set LIBS=-lopengl32 -lgdi32 -ldwmapi
mkdir %OUT_DIR%
g++ -DUNICODE %INCLUDES% %SOURCES% -g -o %OUT_DIR%/%OUT_EXE%.exe --static -mwindows %LIBS% %LIBS%
pause
