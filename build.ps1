Write-Host "Building CGUI Demo..." -ForegroundColor Cyan

if (-not (Test-Path -Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

Set-Location build

Write-Host "`nConfiguring with CMake..." -ForegroundColor Yellow

if ($env:VCPKG_ROOT) {
    Write-Host "Using vcpkg toolchain from: $env:VCPKG_ROOT" -ForegroundColor Cyan
    cmake .. -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
} else {
    Write-Host "vcpkg not found, using default configuration" -ForegroundColor Yellow
    cmake .. -DCMAKE_BUILD_TYPE=Release
}

if ($LASTEXITCODE -ne 0) {
    Write-Host "`nCMake configuration failed!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

Write-Host "`nBuilding..." -ForegroundColor Yellow
cmake --build . --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "`nBuild failed!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

Set-Location ..

Write-Host "`nBuild complete! Run: build\Release\cgui_demo.exe" -ForegroundColor Green

