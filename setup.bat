
setlocal

echo Updating git submodules ...
where /q git
if errorlevel 1 (
    echo Cannot find git on PATH! Please initialize submodules manually and rerun.
    exit /b 1
) ELSE (
    git submodule sync --recursive
    git submodule update --init --recursive
)

echo Fetching dependencies by vcpkg ...
where /q vcpkg
if not exist %~dp0\tools\vcpkg (
    echo vcpkg is not found. Try to install it...
    cd %~dp0\tools
    git clone https://github.com/Microsoft/vcpkg.git
    call .\vcpkg\bootstrap-vcpkg.bat
)

echo Setup complete. 

endlocal
