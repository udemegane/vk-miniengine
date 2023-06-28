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
if errorlevel 1 (
    echo Cannot find vcpkg on PATH! Please install vcpkg and set the PATH, or install dependencies manually.
    exit /b 1
) ELSE (
    vcpkg install quill tinyobjloader vk-bootstrap stb glm fastgltf --triplet x64-windows
)
