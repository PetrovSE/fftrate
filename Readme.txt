1. Fftrate is a set of libraries, programs and plugins designed to change sound parameters:
  - bit depth
  - number of channels
  - sampling rate (this is the most important thing!)

2. Project tree.
---                             top
  |
  |--- packets                  binary package builder for Debian
  |
  |--- src
  |    |
  |    |--- apps
  |    |    |
  |    |    |--- pcm_common     shared library for "pcm_*" programs
  |    |    |--- pcm_conv       standalone file converter
  |    |    |--- pcm_info       wave-file information
  |    |    |--- pcm_mse        measurement of the mean square error
  |    |    |--- pcm_stretch    "stretching" the sound (in development)
  |    |
  |    |--- lib                 the library itself
  |    |--- tests               test programs
  |    |--- tools
  |         |
  |         |--- arateconf      utility for generating a configuration file for the ALSA system
  |         |--- plugin         ALSA plugin
  |
  |--- workspaces               solutions for VS and QtCreator

3. Project build.

3.1 Essential Libraries and Tools.
For build, it is enough to have a gcc/g++ compiler with a standard library.
However, for the ALSA plugin, you must install the ALSA development package (in the case of Debian: libasound2-dev).

3.2 Build order.
Build all:
    make

Libraries only:
    make libs

Console programs:
    make libs apps

ALSA tools:
    make libs tools
