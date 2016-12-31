@echo off

rmdir /S /Q %1\debug
rmdir /S /Q %1\debug_win32
rmdir /S /Q %1\release
rmdir /S /Q %1\release_win32

rmdir /S /Q %1\.clang
rmdir /S /Q %1\.build-debug_win32
rmdir /S /Q %1\.build-release_win32

del %1\*.ncb
del %1\*.plg

del %1\*.user
del %1\*.opt
del /A:H %1\*.suo
