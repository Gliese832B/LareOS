@echo off
set KERNEL=%~dp0..\build\kernel8.img

if not exist "%KERNEL%" (
    echo Kernel not built. Run 'make' first.
    exit /b 1
)

where qemu-system-aarch64 >nul 2>&1
if errorlevel 1 (
    echo QEMU not found. Install from https://www.qemu.org/download/
    exit /b 1
)

if "%1"=="gui" (
    qemu-system-aarch64 -M raspi3b -kernel "%KERNEL%" -serial stdio -display sdl -no-reboot
) else if "%1"=="debug" (
    qemu-system-aarch64 -M raspi3b -kernel "%KERNEL%" -serial stdio -display none -no-reboot -S -gdb tcp::1234
) else (
    qemu-system-aarch64 -M raspi3b -kernel "%KERNEL%" -serial stdio -display none -no-reboot -monitor none
)
