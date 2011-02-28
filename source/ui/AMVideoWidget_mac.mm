#include "ui/AMVideoWidget.h"
#include <QMacCocoaViewContainer>
#include <QVBoxLayout>
#include "/System/Library/Frameworks/AppKit.framework/Headers/NSView.h"
#include "/System/Library/Frameworks/AppKit.framework/Headers/NSSearchField.h"
// #include <VLCVideoView.h>
// #include "/System/Library/Frameworks/Cocoa.framework/Headers/Cocoa.h"

#include <QLabel>

/// Macro to take a non-quoted expression \c s and place quotes around it ("Stringify" it)
#define stringify(s) #s

/// Macro to expand/evaluate a macro expression and THEN stringify it
#define stringify2(s) stringify(s)



@interface VideoView : NSView
		- (void)addVoutSubview:(NSView *)view;
- (void)removeVoutSubview:(NSView *)view;
@end

@implementation VideoView
		- (void)addVoutSubview:(NSView *)view
{
	[view setFrame:[self bounds]];
	[self addSubview:view];
	[view setAutoresizingMask: NSViewHeightSizable | NSViewWidthSizable];
}
- (void)removeVoutSubview:(NSView *)view
{
	[view removeFromSuperview];
}
@end





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
		"--plugin-path=" stringify2(VLC_PLUGIN_PATH)
	};

	// Create a vlc instance for this widget
	vlcInstance_ = libvlc_new (2, vlcArgs);
	// create the vlc player itself
	vlcPlayer_ = libvlc_media_player_new(vlcInstance_);


	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	VideoView *nsView = [[VideoView alloc] init];
	macViewContainer_->setCocoaView(nsView);

	libvlc_media_player_set_nsobject(vlcPlayer_, nsView);

	[nsView release];
	[pool release];
}

AMVideoWidget::~AMVideoWidget() {
	libvlc_media_player_stop(vlcPlayer_);
	libvlc_media_player_release(vlcPlayer_);
	libvlc_release(vlcInstance_);
}

bool AMVideoWidget::openVideoUrl(const QString &videoUrl) {

	libvlc_media_t* media = libvlc_media_new_path( vlcInstance_, videoUrl.toAscii().constData() );
	if(media == 0)
		return false;

	/// \todo Look at:
	// libvlc_state_t state = libvlc_media_get_state(media);

	libvlc_media_player_stop(vlcPlayer_);
	libvlc_media_player_set_media(vlcPlayer_, media);
	libvlc_media_release(media);


	return true;
}

void AMVideoWidget::play() {
	libvlc_media_player_play(vlcPlayer_);
}

void AMVideoWidget::pause() {
	libvlc_media_player_set_pause(vlcPlayer_, true);
}

void AMVideoWidget::unPause() {
	libvlc_media_player_set_pause(vlcPlayer_, false);
}

void AMVideoWidget::togglePause() {
	libvlc_media_player_pause(vlcPlayer_);
}

void AMVideoWidget::stop() {
	// if(isPlaying())	/// \todo change to all non-stopped states?
	libvlc_media_player_stop(vlcPlayer_);
}
