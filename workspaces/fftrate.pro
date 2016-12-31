TEMPLATE = subdirs
CONFIG  += ordered

SUBDIRS += \
	cmdline		\
	common		\
	convert		\
	fft			\
	mathex		\
	profiler	\
	riffio		\
	thr			\
	\
	fft_test	\
	ini_test	\
	\
	pcm_conv	\
	pcm_info	\
	pcm_mse
	

cmdline.file	= ../lib/cmdline.pro
common.file		= ../lib/common.pro
convert.file	= ../lib/convert.pro
fft.file		= ../lib/fft.pro
mathex.file		= ../lib/mathex.pro
profiler.file	= ../lib/profiler.pro
riffio.file		= ../lib/riffio.pro
thr.file		= ../lib/thr.pro

fft_test.file	= ../tests/fft_test/fft_test.pro
ini_test.file	= ../tests/ini_test/ini_test.pro

pcm_conv.file	= ../apps/pcm_conv/pcm_conv.pro
pcm_info.file	= ../apps/pcm_info/pcm_info.pro
pcm_mse.file	= ../apps/pcm_mse/pcm_mse.pro
