@echo off

call clear.bat src\apps\pcm_common
call clear.bat src\apps\pcm_conv
call clear.bat src\apps\pcm_info
call clear.bat src\apps\pcm_mse
call clear.bat src\apps\pcm_stretch

call clear.bat src\tests\fft_test
call clear.bat src\tests\ini_test

call clear.bat src\tools\alsa\arateconf
call clear.bat src\lib

call clear.bat workspaces

for /d %%G in ("build-*") do rmdir "%%~G" /s /q
