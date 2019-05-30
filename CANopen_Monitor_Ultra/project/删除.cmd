@echo off
color 2e
chdir /d %cd%

for %%i in (%chdir%*.*) do (
 if not "%%~xi"==".cmd" (
 if not "%%~xi"==".hex" (
 if not "%%~xi"==".uvoptx" (
 if not "%%~xi"==".uvprojx" (
 if not "%%~xi"==".s" (
 if not "%%~xi"==".c" (
 if not "%%~xi"==".h" @del *%%~xi
)
)
)
)
)
)
)
del/Q Project
exit