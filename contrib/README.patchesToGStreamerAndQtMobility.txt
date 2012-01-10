Customizing GStreamer and QtMobility for proper working MJPEG http video streams, without crazy delays on startup

(Note: Build instructions for GStreamer and QtMobility on CLS machines are in a different note.)
============================

0) gstreamer-plugins-good contains the souphttpsource and multipartdemux plugins, which are used by the playbin element when it finds an http stream.  Until version 0.10.31, multipartdemux doesn't include a patch that is required to add the "single-stream" property, which can be used to avoid delays when playing back low-framerate MJPEG camera streams.  If using a version prior to 0.10.31, need to apply the patch here: https://bugzilla.gnome.org/show_bug.cgi?id=616686

1) To get QtMobility to build on SL4, had to comment out a section related to our really old version of video4linux.  See plugins/multimedia/gstreamer/mediacapture/qgstreamerv4l2input.cpp  inside /home/beamline/tools/qt/source/qt-mobility-opensource-src-1.1.3  and look for my comment/note.  This probably breaks video4linux if you're using it, but we aren't.


2) To use the GStreamer patch, need to manually turn the "single-stream" property on in Qt mobility's QGStreamerPlayerSession.cpp:

plugins/multimedia/gstreamer/mediaplayer/qgstreamerplayersession.cpp 

For example code, see my edits inside /home/beamline/tools/qt/source/qt-mobility-opensource-src-1.1.3/plugins/multimedia/gstreamer/mediaplayer/qgstreamerplayersession.h/cpp 

Added the functions:

static void playbinElementAdded(GstBin* playbin, GstElement* element, QGstreamerPlayerSession* session);
    static void uriDecodeBinElementAdded(GstBin* uriDecodeBin, GstElement* element, QGstreamerPlayerSession* session);
    static void decodeBinElementAdded(GstBin* decodeBin, GstElement* element, QGstreamerPlayerSession* session);
    static gboolean gObjectPropertyExists(GObject* object, const gchar* property_name);


Inside Constructor: QGstreamerPlayerSession::QGstreamerPlayerSession()
	-- added 
		g_signal_connect(G_OBJECT(m_playbin), "element-added", G_CALLBACK(playbinElementAdded...
	which will trigger the remaining functions to eventually set the "single-source=true" property, if available.










Email to CLS dev staff: RE: release of Acquacam in /home/beamline/AcquaCam
========================
Reusing these components:

This is a good example of a program using just a few of the classes from the Acquaman framework to do a specific task.  If you want to embed video inside a larger Qt application, check out the classes AMOverlayVideoWidget, AMCrosshairOverlayVideoWidget, or AMBeamlineCameraBrowser, depending on the level of abstraction you need.   (You can browse these in /home/beamline/AcquaCam/acquaman-source/source/ui folder, or on the web.)  The rest of the program consists of just acquaman-source/source/application/AcquaCam/AcquaCamMain.cpp

Implementation details:

These classes use the QMultimediaKit module from the QtMobility extensions, which will provide the future of video support in Qt.   On Linux, it depends on the GStreamer multimedia backend.  

Our version of GStreamer that came with SL54 is super-ancient; To get this working, I had to build a compile a recent version, as well as all of its dependencies, from source.  If you need it, it's in /home/beamline/tools/gstreamer-0.10.35/gstreamer-install.   The dependencies, including a new version of GLib, are in /home/beamline/tools/gstreamer-0.10.35/deps

Additionally, I've patched both GStreamer and QtMobility (/home/beamline/tools/qt/qt-mobility-1.1.3-SL54-x86_64) to fix a bug that was causing a 30-second delay before video would appear, when accessing HTTP-streaming MJPEG cameras with low framerates ( https://bugzilla.gnome.org/show_bug.cgi?id=616686 ).   The GStreamer patch will be included in the next release of GStreamer.  I haven't submitted the Qt change as a bug report yet, so for now our version of QtMobility is custom.

Performance and OpenGl

AcquaCam can use OpenGl for accelerated video rendering, which really reduces the CPU load (from about 20% down to 4%, when I tested.)  However, OpenGl support is broken in the video driver we currently have installed on the beamline OPIs.  Therefore, the startup script (runAcquaCam.sh) starts the program with the --disable-opengl option.  If you update your video driver to the latest version from NVidia (I haven't done this yet... Glenn and I are checking it out...) you can remove that option and benefit from the performance boost.