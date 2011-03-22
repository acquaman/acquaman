#include "AMOverlayVideoWidget.h"

#include <QGLWidget>

#include <QPushButton>

AMOverlayVideoWidget::AMOverlayVideoWidget(QWidget *parent) :
	QGraphicsView(parent)
{

	//setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	//setViewport(new QGLWidget());
	//setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	setScene(new QGraphicsScene());

	videoPlayer_ = new Phonon::VideoPlayer();
	scene()->addWidget(videoPlayer_);

	resize(size());

	QPushButton* test = new QPushButton("Help!");
	scene()->addWidget(test);

	// phonon version is 4.4.0
}

