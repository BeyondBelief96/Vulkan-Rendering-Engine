@echo off
for %%f in (*.vert *.frag) do (
    C:\VulkanSDK\1.3.275.0\Bin\glslc.exe "%%f" -o "%%~nf.spv"
)
pause