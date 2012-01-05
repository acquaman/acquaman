Building a modern GStreamer on Scientific Linux 5.4

Yikes...

mark.boots@usask.ca
June 2011

=======================================

Motivation:

The GStreamer multimedia library on Scientific Linux (SL) 5.4 is version 0.10.9.  That's quite ancient as multimedia goes, and it has one big problem: it doesn't include the souphttpsrc input plugin that works to read mjpeg video from streaming web servers.  Newer binary packages of GStreamer (ex: packaged with SL 6.0) depend on a more recent version of GLib (min 2.22) than we have.




1) Flex dependency: resolved:  need newer than 2.5.31  (have 2.5.4)
================================
Compiled (no deps) and put into: /home/beamline/tools/gstreamer-0.10.35/deps


2) Newer GLib required:
========================
checking for GLIB... no
configure: Requested 'glib-2.0 >= 2.22' but version of GLib is 2.12.3
configure: error: This package requires GLib >= 2.22 to compile.


Creating GLib 2.28.8, in /home/beamline/tools/gstreamer-0.10.35/deps

Problem: needed to modify gio/tests/Makefile to remove test.mo target,
Because test failed:

make[4]: Entering directory `/home/beamline/tools/gstreamer-0.10.35/gstreamer-src-0.10.35/deps/glib-2.28.8/gio/tests'
/usr/bin/msgfmt -o test.mo ./de.po; \
	/bin/mkdir -p de/LC_MESSAGES; \
	cp -f test.mo de/LC_MESSAGES
./de.po:15: keyword "msgctxt" unknown
./de.po:15:8: parse error
/usr/bin/msgfmt: found 2 fatal errors
cp: cannot stat `test.mo': No such file or directory





3) Configure for gstreamer (core) source: Sets PKG_CONFIG_PATH so that we find the special Glib, and $PATH for flex:

[in gstreamer-0.10.35]
PATH="/home/beamline/tools/gstreamer-0.10.35/deps/bin:$PATH" CPPFLAGS="-I/home/beamline/tools/gstreamer-0.10.35/deps/include" LDFLAGS="-L/home/beamline/tools/gstreamer-0.10.35/deps/lib" PKG_CONFIG_PATH="/home/beamline/tools/gstreamer-0.10.35/deps/lib/pkgconfig" ./configure --prefix=/home/beamline/tools/gstreamer-0.10.35/gstreamer-install

make
make install


4) Get liboil (Orc) for faster plugins:

wget http://code.entropywave.com/download/orc/orc-0.4.14.tar.gz
tar -zxf orc-0.4.14.tar.gz
cd orc-0.4.14
./configure --prefix=/home/beamline/tools/gstreamer-0.10.35/deps


5) Build in [in gst-plugins-base-0.10.35]

#Note: didn't work to find Qt in configure: source /home/beamline/tools/qt/startup/startup.qt-4.7.3.sh

PATH="/home/beamline/tools/gstreamer-0.10.35/deps/bin:$PATH" CPPFLAGS="-I/home/beamline/tools/gstreamer-0.10.35/deps/include" LDFLAGS="-L/home/beamline/tools/gstreamer-0.10.35/deps/lib" PKG_CONFIG_PATH="/home/beamline/tools/gstreamer-0.10.35/deps/lib/pkgconfig:/home/beamline/tools/gstreamer-0.10.35/gstreamer-install/lib/pkgconfig" ./configure --prefix=/home/beamline/tools/gstreamer-0.10.35/gstreamer-install

make
make install


6) Getting shoutcast / icecast library:

wget http://downloads.us.xiph.org/releases/libshout/libshout-2.2.2.tar.gz
tar -zxf libshout-2.2.2.tar.gz
cd libshout-2.2.2
PKG_CONFIG_PATH="/home/beamline/tools/gstreamer-0.10.35/deps/lib/pkgconfig" ./configure --prefix=/home/beamline/tools/gstreamer-0.10.35/deps

make
make install



7a) Getting GnuTLS (required for glib-networking)
configure: error: "Requested 'gnutls >= 2.1.7' but version of GnuTLS is 1.4.1"

wget http://ftp.gnu.org/gnu/gnutls/gnutls-2.12.7.tar.bz2
tar -jxf gnutls-2.12.7.tar.bz2
cd gnutls-2.12.7

# Must specify --with-libgcrypt, because we don't have libnettle:
./configure --prefix=/home/beamline/tools/gstreamer-0.10.35/deps --with-libgcrypt
make
make install



7b) Getting intltool (required for glib-networking build)

We could have it, but we don't.  Installing official SL54 version:

yum install intltool

Dependencies Resolved

================================================================================
 Package               Arch         Version                 Repository     Size
================================================================================
Installing:
 intltool              x86_64       0.35.0-2                sl-base       117 k
Installing for dependencies:
 perl-XML-Parser       x86_64       2.34-6.1.2.2.1          sl-base       209 k


7c) Getting libproxy (recm'd for glib-networking)

-- Skipping -- requires cmake.

Wish we would have proxy support, but nope.

7c) Faking a pkgconfig file for zlib, so that glib-networking accepts the Requirement from GnuTLS

Put this into /home/beamline/tools/gstreamer-0.10.35/deps/lib/pkgconfig/zlib.pc: (Since already in our PKG_CONFIG_PATH)

# pkg-config source file manually created for zlib

prefix=/usr
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include
cppflags= -I/usr/include
cflags_only=

Name: Zlib
Description: Zlib compression library
Version: 1.2.3
Requires: 
Libs: -L${libdir} -lz
Cflags: -I${includedir}



7d) Getting glib-networking (required for libsoup)

wget http://ftp.acc.umu.se/pub/gnome/sources/glib-networking/2.28/glib-networking-2.28.7.tar.gz
tar -zxf glib-networking-2.28.7.tar.gz 
cd glib-networking-2.28.7


# Need new version of glib-genmarshal in path:
export LD_LIBRARY_PATH="/home/beamline/tools/gstreamer-0.10.35/deps/lib:$LD_LIBRARY_PATH"
export PATH="/home/beamline/tools/gstreamer-0.10.35/deps/bin:$PATH"

PKG_CONFIG_PATH="/home/beamline/tools/gstreamer-0.10.35/deps/lib/pkgconfig" GNUTLS_CFLAGS=-I/home/beamline/tools/gstreamer-0.10.35/deps/include GNUTLS_LIBS=-L/home/beamline/tools/gstreamer-0.10.35/deps/lib\ -lgnutls ./configure --prefix=/home/beamline/tools/gstreamer-0.10.35/deps

# Note: GNUTLS_CFLAGS and GNUTLS_LIBS don't actually work the way configure --help says they should.  (Probably borken in glib-networking automake). Had to manually edit Makefile in:

cd tls/gnutls
nano Makefile
# add -I/home/beamline/tools/gstreamer-0.10.35/deps/include to DEFAULT_INCLUDES
# add "-L/home/beamline/tools/gstreamer-0.10.35/deps/lib -lgnutls" to LINK right before -o
make
cd ../../
make
make install


7) Getting libsoup library (our version 2.2.98 is too old)

wget http://ftp.acc.umu.se/pub/gnome/sources/libsoup/2.34/libsoup-2.34.2.tar.gz
tar -zxf libsoup-2.34.2.tar.gz
cd libsoup-2.34.2
LD_LIBRARY_PATH="/home/beamline/tools/gstreamer-0.10.35/deps/lib" PKG_CONFIG_PATH="/home/beamline/tools/gstreamer-0.10.35/deps/lib/pkgconfig" ./configure --prefix=/home/beamline/tools/gstreamer-0.10.35/deps

make
make install


7) Build in [in gst-plugins-good-0.10.30]

# assuming PATH still set...
PKG_CONFIG_PATH="/home/beamline/tools/gstreamer-0.10.35/deps/lib/pkgconfig:/home/beamline/tools/gstreamer-0.10.35/gstreamer-install/lib/pkgconfig" ./configure --prefix=/home/beamline/tools/gstreamer-0.10.35/gstreamer-install

make
make install



Result:

Src in: /home/beamline/tools/gstreamer-0.10.35/gstreamer-src-0.10.35/gstreamer-0.10.35

Installed in: /home/beamline/tools/gstreamer-0.10.35/gstreamer-install
Dependencies in: /home/beamline/tools/gstreamer-0.10.35/deps


8) Reference and link when building Qt Mobility...

# Starting from scratch: new shell, new environment.

# Qt path config:
source /home/beamline/tools/qt/startup/startup.qt-4.7.3.sh

# important: now we need gstreamer and the new glib stuff in the path:
export PATH=/home/beamline/tools/gstreamer-0.10.35/gstreamer-install/bin:/home/beamline/tools/gstreamer-0.10.35/deps/bin:$PATH
export LD_LIBRARY_PATH=/home/beamline/tools/gstreamer-0.10.35/gstreamer-install/lib:/home/beamline/tools/gstreamer-0.10.35/deps/lib:$LD_LIBRARY_PATH

#check:

bash-3.2$ gst-launch --version
gst-launch-0.10 version 0.10.35
GStreamer 0.10.35
Unknown package origin

# source folder:
cd /home/beamline/tools/qt/source/qt-mobility-opensource-src-1.1.3



# Set PKG_CONFIG_PATH when running configure to point to new Glib and new GStreamer

PKG_CONFIG_PATH=/home/beamline/tools/gstreamer-0.10.35/gstreamer-install/lib/pkgconfig:/home/beamline/tools/gstreamer-0.10.35/deps/lib/pkgconfig ./configure -modules multimedia -prefix /home/beamline/tools/qt/qt-mobility-1.1.3-SL54-x86_64




Test mjpg source: http://v2e1607-001.clsi.ca/mjpg/1/video.mjpg




gst-plugins-good-0.30: Summary of Plugins Built:

*** Plug-ins without external dependencies that will be built:
	alpha
	apetag
	audiofx
	audioparsers
	auparse
	autodetect
	avi
	cutter
	debugutils
	deinterlace
	effectv
	equalizer
	flv
	flx
	goom
	goom2k1
	icydemux
	id3demux
	imagefreeze
	interleave
	isomp4
	law
	level
	matroska
	multifile
	multipart
	replaygain
	rtp
	rtpmanager
	rtsp
	shapewipe
	smpte
	spectrum
	udp
	videobox
	videocrop
	videofilter
	videomixer
	wavenc
	wavparse
	y4m

configure: *** Plug-ins without external dependencies that will NOT be built:
	monoscope

configure: *** Plug-ins with dependencies that will be built:
	annodex
	cairo
	esdsink
	gconfelements
	gdkpixbuf
	halelements
	jpeg
	oss4
	ossaudio
	png
	shout2
	souphttpsrc
	v4l2src
	ximagesrc

configure: *** Plug-ins with dependencies that will NOT be built:
	1394
	aasink
	cacasink
	cairooverlay
	directsoundsink
	dv
	flac
	jack
	osxaudio
	osxvideosink
	pulseaudio
	speex
	sunaudio
	taglib
	wavpack

configure: *** Orc acceleration enabled.






gst-plugins-base-0.10.35: Summary of plugins built:

*** Plug-ins without external dependencies that will be built:
	adder
	app
	audioconvert
	audiorate
	audioresample
	audiotestsrc
	encoding
	ffmpegcolorspace
	gdp
	playback
	subparse
	tcp
	typefind
	videorate
	videoscale
	videotestsrc
	volume

configure: *** Plug-ins without external dependencies that will NOT be built:

configure: *** Plug-ins with dependencies that will be built:
	alsa
	gio
	gnomevfs
	ogg
	video4linux
	vorbis
	ximagesink
	xvimagesink

configure: *** Plug-ins with dependencies that will NOT be built:
	cdparanoia
	ivorbisdec
	libvisual
	pango
	theora

configure: *** Orc acceleration enabled.


