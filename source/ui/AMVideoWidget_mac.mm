#include "ui/AMVideoWidget.h"
#include <QMacCocoaViewContainer>
#include <QVBoxLayout>
#include "/System/Library/Frameworks/AppKit.framework/Headers/NSView.h"

#ifdef Q_WS_MAC

/// Macro to take a non-quoted expression \c s and place quotes around it ("Stringify" it)
#define stringify(s) #s

/// Macro to expand/evaluate a macro expression and THEN stringify it
#define stringify2(s) stringify(s)


AMVideoWidget::AMVideoWidget(QWidget *parent)
	: QFrame(parent) {

	setMinimumSize(100,100);
	setFrameStyle(QFrame::Box);
	QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sp.setHorizontalStretch(2);
	sp.setVerticalStretch(2);
	setSizePolicy(sp);

	QVBoxLayout* vl = new QVBoxLayout();
	macViewContainer_ = new QMacCocoaViewContainer(0);
	vl->addWidget(macViewContainer_);
	setLayout(vl);

	const char* const vlcArgs[] = {
		"--no-video-title",
		"--plugin-path=" stringify2(VLC_PLUGIN_PATH),
		"--vout=macosx"
	};

	// Create a vlc instance for this widget
	vlcInstance_ = libvlc_new (3, vlcArgs);
	// create the vlc player itself
	vlcPlayer_ = libvlc_media_player_new(vlcInstance_);


	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	NSView* nsView = [[NSView alloc] init];
	macViewContainer_->setCocoaView(nsView);

	libvlc_media_player_set_nsobject(vlcPlayer_, nsView);

	[nsView release];
	[pool release];
}

AMVideoWidget::~AMVideoWidget() {

	macViewContainer_->setCocoaView(0);

	libvlc_media_player_stop(vlcPlayer_);
	libvlc_media_player_release(vlcPlayer_);
	libvlc_release(vlcInstance_);
}


#endif
