#include "AMPlotMarkerComboBox.h"

AMPlotMarkerComboBox::AMPlotMarkerComboBox(QWidget *parent) :
	QComboBox(parent)
{
	// Manually add each marker style to the combo box by iterating through the
	// MPlotMarkerShape::Shape enum (which increments by x2 for each marker shape)

	setToolTip(QString("Marker Style"));
	setIconSize(QSize(16,16));
	setMinimumWidth(16);

	QPixmap currentMarkerPixmap(16, 16);
	QBrush markerPixmapBrush(Qt::black);
	QPen markerPixmapPen(markerPixmapBrush, 1);
	QPainter markerPixmapPainter(&currentMarkerPixmap);
	markerPixmapPainter.translate(QPoint(7, 8));
	markerPixmapPainter.setPen(markerPixmapPen);
	MPlotAbstractMarker* currentMarker = 0;
	currentMarkerPixmap.fill(Qt::white);
	addItem(QIcon(currentMarkerPixmap), QString(), QVariant(0));

	for (int iCurrentMarkerShape = 1; iCurrentMarkerShape <=512; iCurrentMarkerShape = iCurrentMarkerShape * 2)
	{
		currentMarker = MPlotMarker::create((MPlotMarkerShape::Shape)iCurrentMarkerShape, 12);
		currentMarkerPixmap.fill(Qt::white);
		currentMarker->paint((&markerPixmapPainter));
		addItem(QIcon(currentMarkerPixmap), QString(), QVariant(iCurrentMarkerShape));
		delete currentMarker;
	}

	setCurrentIndex(0);
}

MPlotMarkerShape::Shape AMPlotMarkerComboBox::selectedMarkerShape() const
{
	int currentMarkerShape = itemData(currentIndex()).toInt();
	return (MPlotMarkerShape::Shape)currentMarkerShape;
}

void AMPlotMarkerComboBox::onCurrentIndexChanged(int /*newIndex*/)
{
	emit currentMarkerShapeChanged(selectedMarkerShape());
}




