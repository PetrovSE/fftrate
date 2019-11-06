TEMPLATE = subdirs
CONFIG  += ordered

SUBDIRS += \
	cmdline		\
	common		\
	convert		\
	stretch		\
	fft		\
	mathex		\
	profiler	\
	riffio		\
	thr		\
	\
	fft_test	\
	ini_test	\
	\
	arateconf	\
	\
	pcm_common	\
	pcm_conv	\
	pcm_info	\
	pcm_mse		\
	pcm_stretch


cmdline.file	= ../src/lib/cmdline.pro
common.file	= ../src/lib/common.pro
convert.file	= ../src/lib/convert.pro
stretch.file	= ../src/lib/stretch.pro
fft.file	= ../src/lib/fft.pro
mathex.file	= ../src/lib/mathex.pro
profiler.file	= ../src/lib/profiler.pro
riffio.file	= ../src/lib/riffio.pro
thr.file	= ../src/lib/thr.pro

fft_test.file	= ../src/tests/fft_test/fft_test.pro
ini_test.file	= ../src/tests/ini_test/ini_test.pro

arateconf.file	= ../src/tools/alsa/arateconf/arateconf.pro

pcm_common.file		= ../src/apps/pcm_common/pcm_common.pro
pcm_conv.file		= ../src/apps/pcm_conv/pcm_conv.pro
pcm_info.file		= ../src/apps/pcm_info/pcm_info.pro
pcm_mse.file		= ../src/apps/pcm_mse/pcm_mse.pro
pcm_stretch.file	= ../src/apps/pcm_stretch/pcm_stretch.pro
