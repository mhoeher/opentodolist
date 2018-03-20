call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat" || exit /b
set PATH=C:\Qt\5.10.0\msvc2015\bin;%PATH%
mkdir build-win32-vs || exit /b
cd build-win32-vs || exit /b
"C:\Program Files\CMake\bin\cmake.exe" ^
    -G"MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DOPENTODOLIST_WITH_UPDATE_SERVICE=ON ^
    -DUSE_CREDENTIAL_STORE=ON ^
    -DQTKEYCHAIN_STATIC=ON ^
    -DCMAKE_MAKE_PROGRAM=C:\Qt\Tools\QtCreator\bin\jom.exe ^
    -DCMAKE_C_COMPILER=cl ^
    -DCMAKE_CXX_COMPILER=cl ^
    .. || exit /b
"C:\Program Files\CMake\bin\cmake.exe" ^
    --build . -- -j3 || exit /b
mkdir bin || exit /b
copy app\OpenTodoList.exe bin || exit /b

mkdir openssl || exit /b
cd openssl || exit /b
"C:\Program Files\Git\mingw64\bin\curl.exe" ^
	-o openssl-1.0.2n-i386-win32.zip ^
	https://indy.fulgan.com/SSL/openssl-1.0.2n-i386-win32.zip || exit /b
"C:\Program Files\Git\usr\bin\sha256sum.exe" ^
    -c ..\..\templates\windows\openssl-win32.sha256sum || exit /b
"C:\Program Files\Git\usr\bin\unzip.exe" ^
    openssl-1.0.2n-i386-win32.zip || exit /b
copy ssleay32.dll ..\bin\ssleay32.dll || exit /b
copy libeay32.dll ..\bin\libeay32.dll || exit /b
cd .. || exit /b

windeployqt --qmldir ..\app --release --compiler-runtime bin\OpenTodoList.exe || exit /b
copy ..\templates\nsis\win32-installer.nsis . || exit /b
"C:\Program Files (x86)\NSIS\makensis.exe" win32-installer.nsis || exit /b
copy OpenTodoList-Windows-32bit.exe OpenTodoList-Windows-32bit-MSVC.exe || exit /b
