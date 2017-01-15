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
	arateconf	\
	\
	pcm_conv	\
	pcm_info	\
	pcm_mse		\
	pcm_stretch
	

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
arateconf.file	= ../packets/src/arateconf.pro

pcm_conv.file		= ../apps/pcm_conv/pcm_conv.pro
pcm_info.file		= ../apps/pcm_info/pcm_info.pro
pcm_mse.file		= ../apps/pcm_mse/pcm_mse.pro
pcm_stretch.file	= ../apps/pcm_stretch/pcm_stretch.pro
