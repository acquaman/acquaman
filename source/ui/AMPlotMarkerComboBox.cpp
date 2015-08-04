#include "AMPlotMarkerComboBox.h"

AMPlotMarkerComboBox::AMPlotMarkerComboBox(QWidget *parent) :
	QComboBox(parent)
{
	setToolTip(QString("Marker Style"));
	setIconSize(QSize(16,16));
	setMinimumWidth(16);

	addAllPlotMarkers();

	setCurrentIndex(0);

	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged()));
}

AMPlotMarkerComboBox::AMPlotMarkerComboBox(const QList<MPlotMarkerShape::Shape> &shapeOptions, QWidget *parent) :
	QComboBox(parent)
{
	setToolTip(QString("Marker Style"));
	setIconSize(QSize(16,16));
	setMinimumWidth(16);

	addItems(shapeOptions);

	setCurrentIndex(0);

	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged()) );
}

AMPlotMarkerComboBox::~AMPlotMarkerComboBox()
{

}

MPlotMarkerShape::Shape AMPlotMarkerComboBox::selectedMarkerShape() const
{
	int currentMarkerShape = itemData(currentIndex(), Qt::UserRole).toInt();
	return (MPlotMarkerShape::Shape)currentMarkerShape;
}

void AMPlotMarkerComboBox::setSelectedMarkerShape(MPlotMarkerShape::Shape shape)
{
	int passedShapeInteger = (int)shape;

	for (int iCurrentItem = 0, itemCount = this->count(); iCurrentItem < itemCount; ++iCurrentItem) {
		int currentItemShapeInteger = itemData(iCurrentItem, Qt::UserRole).toInt();

		if(currentItemShapeInteger == passedShapeInteger) {
			setCurrentIndex(iCurrentItem);
			return;
		}
	}
}

void AMPlotMarkerComboBox::addItem(const MPlotMarkerShape::Shape &newShape)
{
	if (!shapeOptions_.contains(newShape)) {

		// Set up the marker pixmap.
		// Should initially be blank.

		QPixmap currentMarkerPixmap(16, 16);
		currentMarkerPixmap.fill(Qt::white);

		QBrush markerPixmapBrush(Qt::black);
		QPen markerPixmapPen(markerPixmapBrush, 1);

		QPainter markerPixmapPainter(&currentMarkerPixmap);
		markerPixmapPainter.translate(QPoint(7, 8));
		markerPixmapPainter.setPen(markerPixmapPen);


		if (newShape == MPlotMarkerShape::None) {

			// If the new shape is 'None', no changes to the pixmap settings needed.

			QComboBox::addItem(QIcon(currentMarkerPixmap), QString(), QVariant(newShape));

		} else {

			// If the new shape is something not 'None', create the marker and paint it onto the pixmap.

			MPlotAbstractMarker* currentMarker = MPlotMarker::create(newShape, 12);
			currentMarker->paint((&markerPixmapPainter));
			QComboBox::addItem(QIcon(currentMarkerPixmap), QString(), QVariant(newShape));
			delete currentMarker;
		}

		shapeOptions_ << newShape;
	}
}

void AMPlotMarkerComboBox::addItems(const QList<MPlotMarkerShape::Shape> &newShapes)
{
	foreach (MPlotMarkerShape::Shape newShape, newShapes)
		addItem(newShape);
}

void AMPlotMarkerComboBox::clear()
{
	QComboBox::clear();
	shapeOptions_.clear();
}

void AMPlotMarkerComboBox::onCurrentIndexChanged()
{
	emit currentMarkerShapeChanged(selectedMarkerShape());
}

void AMPlotMarkerComboBox::addAllPlotMarkers()
{
	// Manually add each marker style to the combo box by iterating through the
	// MPlotMarkerShape::Shape enum (which increments by x2 for each marker shape)

	for (int iCurrentMarkerShape = 1; iCurrentMarkerShape <=512; iCurrentMarkerShape = iCurrentMarkerShape * 2)
		addItem(MPlotMarkerShape::Shape(iCurrentMarkerShape));

	for (int iCurrentMarkerShape = 513; iCurrentMarkerShape < 524; iCurrentMarkerShape++)
		addItem(MPlotMarkerShape::Shape(iCurrentMarkerShape));
}
