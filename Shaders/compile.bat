@echo off

set compiler_glslangValidator=%VULKAN_SDK%\Bin\glslangValidator.exe
set compiler=%VULKAN_SDK%\Bin\glslc.exe

rem for /r %%i in (*.frag, *.vert) do %compiler1% -V %%i
for /r %%i in (*.frag, *.vert) do %compiler% %%i -o %%~ni.spv

pause