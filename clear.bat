@echo off

rmdir /S /Q %1\debug
rmdir /S /Q %1\debug_win32
rmdir /S /Q %1\release
rmdir /S /Q %1\release_win32
rmdir /S /Q %1\ipch

del %1\*.ncb
del %1\*.plg
del %1\*.pdb
del %1\*.wav

del %1\*.debug
del %1\*.release

del %1\*.user
del %1\*.opt
del %1\*.xml
del %1\*.sdf

del /A:H %1\*.suo

for %%f in ("%1\Makefile.*") do (
	if not "%%~xf" == ".st" del %%f
)
