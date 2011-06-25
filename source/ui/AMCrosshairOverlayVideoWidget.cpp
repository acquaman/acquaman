#include "AMCrosshairOverlayVideoWidget.h"
#include <QGraphicsLineItem>
#include <QResizeEvent>

#include <QMediaObject>

AMCrosshairOverlayVideoWidget::AMCrosshairOverlayVideoWidget(QWidget *parent) :
	AMOverlayVideoWidget(parent)
{
	crosshairX_ = 0.5;
	crosshairY_ = 0.5;

	QPen pen(QColor(Qt::red));

	crosshairXLine_ = scene()->addLine(0.5,0,0.5,1,pen);
	crosshairYLine_ = scene()->addLine(0,0.5,0,1,pen);

	reviewCrosshairLinePositions();

	connect(videoItem_, SIGNAL(nativeSizeChanged(QSizeF)), this, SLOT(reviewCrosshairLinePositions()));
}


#include <QDebug>

void AMCrosshairOverlayVideoWidget::reviewCrosshairLinePositions()
{

	QSizeF viewSize = videoItem_->size();
	// first we need to find out the native size of the video. (Well, actually just the aspect ratio, but...)
	QSizeF videoSize = videoItem_->nativeSize();

//	QMediaObject* mediaObject = videoWidget_->mediaObject();
//	if(mediaObject && mediaObject->isMetaDataAvailable()) {
//		qDebug() << "meta data avail" << mediaObject->metaData(QtMultimediaKit::VideoCodec).toString() << mediaObject->metaData(QtMultimediaKit::Resolution).toSize();
//		QVariant resolutionV = mediaObject->metaData(QtMultimediaKit::Resolution);
//		if(resolutionV.isValid()) {
//			videoSize = resolutionV.toSize();
//			qDebug() << "Crosshair: got valid resolution" << videoSize;
//		}
//	}

	// scale the video size to the view size, obeying the transformation mode
	QSizeF scaledSize = videoSize;
	scaledSize.scale(viewSize, videoItem_->aspectRatioMode());

	// now, scaledSize will either be:
		// same as viewSize, if view and video have same aspect ratio, or the video is being stretched with Qt::IgnoreAspectRatio
		// One dimension the same, other dimension smaller than viewSize, if Qt::KeepAspectRatio
		// or One dimension the same, other dimension LARGER than viewSize, if Qt::KeepAspectRatioByExpanding

	QRectF activeRect = QRectF(QPointF((viewSize.width()-scaledSize.width())/2,
									   (viewSize.height()-scaledSize.height())/2),
							   scaledSize);

	// activeRect is now a rectangle in scene coordinates that covers the actual area of the video [not the area of the videoWidget, which may be smaller or larger depending on the aspect ratio mode and aspect ratio of the actual video feed]

	qreal xSceneCoord = activeRect.left() + crosshairX_*activeRect.width();
	qreal ySceneCoord = activeRect.top() + crosshairY_*activeRect.height();

	crosshairXLine_->setLine(xSceneCoord, activeRect.top(), xSceneCoord, activeRect.bottom());
	crosshairYLine_->setLine(activeRect.left(), ySceneCoord, activeRect.right(), ySceneCoord);
}

QPen AMCrosshairOverlayVideoWidget::crosshairPen() const
{
	return crosshairXLine_->pen();
}

bool AMCrosshairOverlayVideoWidget::crosshairVisible() const
{
	return crosshairXLine_->isVisible();
}

void AMCrosshairOverlayVideoWidget::setCrosshairPen(const QPen &pen)
{
	crosshairXLine_->setPen(pen);
	crosshairYLine_->setPen(pen);
}

void AMCrosshairOverlayVideoWidget::setCrosshairVisible(bool crosshairVisible)
{
	crosshairXLine_->setVisible(crosshairVisible);
	crosshairYLine_->setVisible(crosshairVisible);
}

void AMCrosshairOverlayVideoWidget::resizeEvent(QResizeEvent *event)
{
	AMOverlayVideoWidget::resizeEvent(event);

	reviewCrosshairLinePositions();
}

void AMCrosshairOverlayVideoWidget::mousePressEvent(QMouseEvent *e)
{
	AMOverlayVideoWidget::mousePressEvent(e);
}

void AMCrosshairOverlayVideoWidget::mouseReleaseEvent(QMouseEvent *e)
{
	AMOverlayVideoWidget::mouseReleaseEvent(e);
}

void AMCrosshairOverlayVideoWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
	AMOverlayVideoWidget::mouseDoubleClickEvent(e);
}
