#include "AMXRFDetailedDetectorView.h"

#include <QHBoxLayout>
#include <QStringBuilder>

#include "util/AMDataSourcePlotSettings.h"

AMXRFDetailedDetectorView::AMXRFDetailedDetectorView(AMXRFDetector *detector, QWidget *parent)
	: AMXRFBaseDetectorView(detector, parent)
{
	emissionLineValidator_ = new AMNameAndRangeValidator(this);

	periodicTable_ = new AMSelectablePeriodicTable(this);
	periodicTable_->buildPeriodicTable();

	emissionLineLegendColors_.insert("K", QColor(0, 255, 0));
	emissionLineLegendColors_.insert("L", QColor(255, 255, 0));
	emissionLineLegendColors_.insert("M", QColor(255, 0, 0));
	emissionLineLegendColors_.insert("Default", palette().button().color());
}

void AMXRFDetailedDetectorView::buildDetectorView()
{
	// This is going to extend the base view, so we will simply call it's build method too.
	AMXRFBaseDetectorView::buildDetectorView();

	periodicTableView_ = new AMSelectablePeriodicTableView(periodicTable_);
	periodicTableView_->buildPeriodicTableView();

	elementView_ = new AMSelectableElementView(qobject_cast<AMSelectableElement *>(periodicTable_->elementBySymbol("Fe")));
	elementView_->setAbsorptionEdgeVisibility(false);

	QFont font(this->font());
	font.setBold(true);

	QLabel *legend = new QLabel(QString("Legend"));
	legend->setFont(font);
	QLabel *kLines = new QLabel(QString("K-lines"));
	kLines->setFont(font);
	kLines->setStyleSheet(QString("QLabel { background-color: rgb(%1,%2,%3) ; border-width: 2px ; border-style: solid }").arg(emissionLineLegendColors_.value("K").red()).arg(emissionLineLegendColors_.value("K").green()).arg(emissionLineLegendColors_.value("K").blue()));
	QLabel *lLines = new QLabel(QString("L-lines"));
	lLines->setFont(font);
	lLines->setStyleSheet(QString("QLabel { background-color: rgb(%1,%2,%3) ; border-width: 2px ; border-style: solid }").arg(emissionLineLegendColors_.value("L").red()).arg(emissionLineLegendColors_.value("L").green()).arg(emissionLineLegendColors_.value("L").blue()));
	QLabel *mLines = new QLabel(QString("M-lines"));
	mLines->setFont(font);
	mLines->setStyleSheet(QString("QLabel { background-color: rgb(%1,%2,%3) ; border-width: 2px ; border-style: solid }").arg(emissionLineLegendColors_.value("M").red()).arg(emissionLineLegendColors_.value("M").green()).arg(emissionLineLegendColors_.value("M").blue()));

	QVBoxLayout *legendLayout = new QVBoxLayout;
	legendLayout->addWidget(legend);
	legendLayout->addWidget(kLines);
	legendLayout->addWidget(lLines);
	legendLayout->addWidget(mLines);
	legendLayout->addStretch();

	QHBoxLayout *periodicTableAndElementViewLayout = new QHBoxLayout;
	periodicTableAndElementViewLayout->addLayout(legendLayout);
	periodicTableAndElementViewLayout->addWidget(periodicTableView_);
	periodicTableAndElementViewLayout->addWidget(elementView_);

	bottomLayout_->addLayout(periodicTableAndElementViewLayout);

	connect(emissionLineValidator_, SIGNAL(validatorChanged()), this, SLOT(updateEmissionLineMarkers()));
	connect(periodicTable_, SIGNAL(elementSelected(AMElement*)), this, SLOT(onElementSelected(AMElement*)));
	connect(periodicTable_, SIGNAL(elementDeselected(AMElement*)), this, SLOT(onElementDeselected(AMElement*)));
	connect(periodicTable_, SIGNAL(emissionLineSelected(AMEmissionLine)), this, SLOT(onEmissionLineSelected(AMEmissionLine)));
	connect(periodicTable_, SIGNAL(emissionLineDeselected(AMEmissionLine)), this, SLOT(onEmissionLineDeselected(AMEmissionLine)));
	connect(periodicTableView_, SIGNAL(elementSelected(AMElement*)), this, SLOT(onElementClicked(AMElement*)));
}

void AMXRFDetailedDetectorView::onElementClicked(AMElement *element)
{
	// This is always safe because we know the periodic table is an AMSelectablePeriodicTable.
	elementView_->setElement(qobject_cast<AMSelectableElement *>(element));
	highlightCurrentElementRegionOfInterest(element);
}

void AMXRFDetailedDetectorView::onElementSelected(AMElement *element)
{
	QColor color = AMDataSourcePlotSettings::nextColor();

	foreach (AMEmissionLine emissionLine, element->emissionLines()){

		if (emissionLineValidator_->isValid(emissionLine.name(), emissionLine.energy())){

			MPlotPoint *newLine = new MPlotPoint(QPointF(emissionLine.energy(), 0));
			newLine->setMarker(MPlotMarkerShape::VerticalBeam, 1e6, QPen(color), QBrush(color));
			newLine->setDescription(emissionLine.greekName() % ": " % emissionLine.energyString() % " eV");
			plot_->addItem(newLine);
			emissionLineMarkers_ << newLine;
		}
	}
}

void AMXRFDetailedDetectorView::onElementDeselected(AMElement *element)
{
	QString symbol = element->symbol();

	foreach(MPlotItem *item, emissionLineMarkers_){

		if (item->description().contains(QRegExp(symbol % " (K|L|M)")))
			if (plot_->removeItem(item)){

				emissionLineMarkers_.removeOne(item);
				delete item;
			}
	}
}

void AMXRFDetailedDetectorView::updateEmissionLineMarkers()
{
	foreach (AMElement *element, periodicTable_->selectedElements())
		onElementDeselected(element);

	foreach (AMElement *element, periodicTable_->selectedElements())
		onElementSelected(element);
}

void AMXRFDetailedDetectorView::addEmissionLineNameFilter(const QString &newNameFilter)
{
	emissionLineValidator_->addNameFilter(newNameFilter);
	elementView_->addEmissionLineNameFilter(newNameFilter);
}

bool AMXRFDetailedDetectorView::removeEmissionLineNameFilter(int index)
{
	return emissionLineValidator_->removeNameFilter(index) && elementView_->removeEmissionLineNameFilter(index);;
}

bool AMXRFDetailedDetectorView::removeEmissionLineNameFilter(const QString &filter)
{
	return emissionLineValidator_->removeNameFilter(filter) && elementView_->removeEmissionLineNameFilter(filter);
}

void AMXRFDetailedDetectorView::setEnergyRange(const AMRange &newRange)
{
	emissionLineValidator_->setRange(newRange);
	periodicTableView_->setEnergyRange(newRange);
	elementView_->setEnergyRange(newRange);
}

void AMXRFDetailedDetectorView::setEnergyRange(double minimum, double maximum)
{
	setEnergyRange(AMRange(minimum, maximum));
}

void AMXRFDetailedDetectorView::setMinimumEnergy(double newMinimum)
{
	emissionLineValidator_->setMinimum(newMinimum);
	periodicTableView_->setMinimumEnergy(newMinimum);
	elementView_->setMinimumEnergy(newMinimum);
}

void AMXRFDetailedDetectorView::setMaximumEnergy(double newMaximum)
{
	emissionLineValidator_->setMaximum(newMaximum);
	periodicTableView_->setMaximumEnergy(newMaximum);
	elementView_->setMaximumEnergy(newMaximum);
}

void AMXRFDetailedDetectorView::onEmissionLineSelected(const AMEmissionLine &emissionLine)
{
	detector_->addRegionOfInterest(emissionLine);
	AMRegionOfInterest *newRegion = detector_->regionOfInterest(emissionLine);

	MPlotMarkerTransparentVerticalRectangle *newMarker = new MPlotMarkerTransparentVerticalRectangle(newRegion->name(), newRegion->energy(), newRegion->lowerBound(), newRegion->upperBound());
	plot_->insertItem(newMarker);
	newMarker->setYAxisTarget(plot_->axisScale(MPlot::VerticalRelative));
	regionOfInterestMarkers_.insert(emissionLine, newMarker);

	updatePeriodicTableButtonColors(emissionLine);
}

void AMXRFDetailedDetectorView::onEmissionLineDeselected(const AMEmissionLine &emissionLine)
{
	MPlotItem *itemToBeRemoved = regionOfInterestMarkers_.value(emissionLine);

	if (itemToBeRemoved){

		detector_->removeRegionOfInterest(emissionLine);
		plot_->removeItem(itemToBeRemoved);
		regionOfInterestMarkers_.remove(emissionLine);
		delete itemToBeRemoved;

		updatePeriodicTableButtonColors(emissionLine);
	}
}

void AMXRFDetailedDetectorView::removeAllEmissionLineMarkers()
{

}

void AMXRFDetailedDetectorView::removeAllRegionsOfInterest()
{

}

void AMXRFDetailedDetectorView::updatePeriodicTableButtonColors(const AMEmissionLine &line)
{
	AMSelectableElement *element = qobject_cast<AMSelectableElement *>(periodicTable_->elementBySymbol(line.elementSymbol()));

	if (element){

		if (element->hasSelectedEmissionLines()){

			QStringList keyString;

			foreach (AMEmissionLine line, element->selectedEmissionLines())
				keyString << line.lineName().left(1);

			keyString.removeDuplicates();
			periodicTableView_->button(periodicTable_->elementBySymbol(line.elementSymbol()))->setStyleSheet(buildStyleSheet(keyString.join("")));
		}

		else{

			periodicTableView_->button(periodicTable_->elementBySymbol(line.elementSymbol()))->setStyleSheet(buildStyleSheet("Default"));
		}
	}
}

const QString AMXRFDetailedDetectorView::buildStyleSheet(const QString &colorMapKey) const
{
	QString styleSheet;

	// Default is unselected.  It can only have one value in the key.
	if (colorMapKey.contains("Default")){

		QColor color = emissionLineLegendColors_.value(colorMapKey);
		styleSheet = QString("QToolButton { background-color: rgb(%1,%2,%3); }").arg(color.red()).arg(color.green()).arg(color.blue());
	}

	else {

		QColor color = emissionLineLegendColors_.value(colorMapKey.left(1));
		styleSheet = QString("QToolButton { background-color: rgb(%1,%2,%3); }").arg(color.red()).arg(color.green()).arg(color.blue());
	}

	return styleSheet;
}

void AMXRFDetailedDetectorView::highlightCurrentElementRegionOfInterest(AMElement *element)
{
	foreach (MPlotMarkerTransparentVerticalRectangle *marker, regionOfInterestMarkers_)
		marker->setHighlighted(marker->description().contains(QRegExp(element->symbol() % " (K|L|M)")));
}

void AMXRFDetailedDetectorView::setLineColors(const QColor &kColor, const QColor &lColor, const QColor &mColor, const QColor &defaultColor)
{
	emissionLineLegendColors_.clear();
	emissionLineLegendColors_.insert("K", kColor);
	emissionLineLegendColors_.insert("L", lColor);
	emissionLineLegendColors_.insert("M", mColor);
	emissionLineLegendColors_.insert("Default", defaultColor);
}

void AMXRFDetailedDetectorView::setKEmissionLineColor(const QColor &color)
{
	emissionLineLegendColors_.remove("K");
	emissionLineLegendColors_.insert("K", color);
}

void AMXRFDetailedDetectorView::setLEmissionLineColor(const QColor &color)
{
	emissionLineLegendColors_.remove("L");
	emissionLineLegendColors_.insert("L", color);
}

void AMXRFDetailedDetectorView::setMEmissionLineColor(const QColor &color)
{
	emissionLineLegendColors_.remove("M");
	emissionLineLegendColors_.insert("M", color);
}

void AMXRFDetailedDetectorView::setDefaultEmissionLineColor(const QColor &color)
{
	emissionLineLegendColors_.remove("Default");
	emissionLineLegendColors_.insert("Default", color);
}
