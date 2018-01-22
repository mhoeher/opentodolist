call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
set PATH=C:\Qt\5.10.0\msvc2015\bin;%PATH%
mkdir build-win32-vs
cd build-win32-vs
echo %PATH%
"C:\Program Files\CMake\bin\cmake.exe" ^
    -G"NMake Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DOPENTODOLIST_WITH_UPDATE_SERVICE=ON ^
    -DUSE_CREDENTIAL_STORE=ON ^
    -DQTKEYCHAIN_STATIC=ON ^
    ..
nmake
mkdir bin
copy app\OpenTodoList.exe bin
copy ..\pre-build\windows\openssl-1.0.2l-i386-win32\ssleay32.dll bin\ssleay32.dll
copy ..\pre-build\windows\openssl-1.0.2l-i386-win32\libeay32.dll bin\libeay32.dll
windeployqt --qmldir ..\app --release --compiler-runtime bin\OpenTodoList.exe
