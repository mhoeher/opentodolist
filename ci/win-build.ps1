$ErrorActionPreference = "Stop"

# Configuration:
$QT_ARCHIVE_URL="https://gitlab.com/api/v4/projects/46171955/packages/generic/Qt6/$env:QT_VERSION/Qt-mingw-w64.zip"

# Expected paths:
$CMAKE_PATH="C:\Qt\Tools\CMake_64\bin"
$NINJA_PATH="C:\Qt\Tools\Ninja"
$MINGW_PATH="C:\Qt\Tools\mingw1120_64\bin\"
$QT_PATH="C:\Qt\$env:QT_VERSION\mingw_64\"
$PERL_PATH="C:\Strawberry\perl\bin"
$NSIS_PATH="C:\Program Files (x86)\NSIS"


if(-Not (Test-Path -Path "$QT_PATH")) {
    # Install Qt
    Invoke-WebRequest -o "Qt-mingw-w64.zip" $QT_ARCHIVE_URL
    New-Item -Path 'C:\Qt' -ItemType Directory
    Expand-Archive -Path "Qt-mingw-w64.zip" -DestinationPath "C:\Qt"
}

if(-Not (Test-Path -Path "$PERL_PATH")) {
    # Install Strawberry Perl (needed for KDE Syntax Highlighting)
    choco install -y strawberryperl

    if (-not $?) {
        Write-Error -Message "Failed to install Perl."
    }
}

if(-Not (Test-Path -Path "$NSIS_PATH")) {
    # Install NSIS installer framework:
    choco install -y nsis

    if (-not $?) {
        Write-Error -Message "Failed to install NSIS."
    }
}

# Setup search paths (important - order matters!):
$env:Path="$QT_PATH\bin;$CMAKE_PATH;$NINJA_PATH;$MINGW_PATH;$PERL_PATH;$NSIS_PATH;$env:Path"

# Build OpenTodoList:
cmake `
    -S . -B build-win64 `
    -GNinja `
    -DCMAKE_PREFIX_PATH=$QT_PATH `
    -DCMAKE_INSTALL_PREFIX=deploy-win64 `
    -DOPENTODOLIST_WITH_UPDATE_SERVICE=ON `
    --fresh

if (-not $?) {
    Write-Error -Message "Failed to configure OpenTodoList."
}

cmake --build build-win64 --target OpenTodoList

if (-not $?) {
    Write-Error -Message "Failed to build OpenTodoList."
}

cmake --install build-win64

if (-not $?) {
    Write-Error -Message "Failed to install OpenTodoList."
}

windeployqt --qmldir app --compiler-runtime  deploy-win64\bin

if (-not $?) {
    Write-Error -Message "Failed to deploy Qt binaries for OpenTodoList."
}

# Prepare portable version of the app:
$OPENTODOLIST_VERSION = git describe --tags
New-Item -Type Directory -Path OpenTodoList-$OPENTODOLIST_VERSION-Windows-64bit
Copy-Item -Path deploy-win64\bin\* -Destination .\OpenTodoList-$OPENTODOLIST_VERSION-Windows-64bit\ -Recurse
Compress-Archive `
    -Path .\OpenTodoList-$OPENTODOLIST_VERSION-Windows-64bit `
    -DestinationPath deploy-win64\OpenTodoList-$OPENTODOLIST_VERSION-Windows-64bit.zip

# Copy NSIS config and build installer:
Copy-Item -Path templates\nsis\win64-installer.nsis -Destination deploy-win64

Set-Location -Path deploy-win64

makensis win64-installer.nsis

if (-not $?) {
    Write-Error -Message "Failed to create installer."
}

Rename-Item OpenTodoList-Windows-64bit.exe OpenTodoList-$OPENTODOLIST_VERSION-Windows-64bit.exe

# Check if the build was successful. Sometimes, installations fail, but
# it seems we get no indication in the form of a non-zero return code.
# In this case, as a last resort, check if we were able to build
# our desired deployables and - if not - error out.
if(-Not (Test-Path -Path "OpenTodoList-$OPENTODOLIST_VERSION-Windows-64bit.zip")) {
    Write-Error -Message "No portable OpenTodoList found - the build probably failed!"
}
if(-Not (Test-Path -Path "OpenTodoList-$OPENTODOLIST_VERSION-Windows-64bit.exe")) {
    Write-Error -Message "No OpenTodoList installer found - the build probably failed!"
}
