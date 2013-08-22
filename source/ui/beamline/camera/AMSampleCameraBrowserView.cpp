#include "ui/beamline/camera/AMSampleCameraBrowserView.h"

#include <QComboBox>
#include <QCheckBox>
#include <QBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>

#include <QMessageBox>
#include <QStringBuilder>

#include "ui/beamline/camera/AMSampleCameraView.h"
#include "ui/AMColorPickerButton2.h"
#include "ui/beamline/camera/AMCameraConfigurationView.h"
#include "ui/beamline/camera/AMShapeDataView.h"
#include "beamline/camera/AMSampleCameraBrowser.h"
#include <QLineEdit>
#include "ui/dataman/AMSampleContainerView.h"

#include <QDebug>

AMSampleCameraBrowserView::AMSampleCameraBrowserView(AMSampleCameraBrowser *cameraBrowser, QWidget *parent, bool useOpenGlViewport) :
	QWidget(parent)
{
	if(useOpenGlViewport)qDebug()<<"Using openGlViewport"; // but not really
	init(cameraBrowser);

}

AMSampleCameraBrowserView::AMSampleCameraBrowserView(QWidget *parent, bool useOpenGlViewport) :
	QWidget(parent)
{
	if(useOpenGlViewport)qDebug()<<"Using openGlViewport";
	init(new AMSampleCameraBrowser());
}



#include <QDebug>
#include <QApplication>
void AMSampleCameraBrowserView::onSourceComboBoxChanged(int index)
{
	qDebug() << "Here in onSourceComboBoxChanged";
	if(index < 0) {
		qDebug() << "Index is < 0";
		setWindowTitle("AcquaCam");
		videoWidget_->setMedia(QMediaContent());
	}

	else {
		qDebug() << "Valid index";
		QString stringUrl = sourceComboBox_->itemText(index);
		QUrl url = QUrl::fromUserInput(stringUrl);

		if(url.isValid()) {
			if(url.toString() != stringUrl)
				sourceComboBox_->setItemText(index, url.toString());
			setWindowTitle(url.toString());
			videoWidget_->setMedia(url);
			qDebug() << "AMBeamlineCameraBrowser2: Loading and playing" << url.toString();
			qDebug() << "Have a QApplication? " << QApplication::applicationName();
			videoWidget_->play();
		}
		else {
			QMessageBox::warning(this, "Invalid camera source address", "Sorry! That doesn't look like a valid URL for a camera or media source:\n\n" % stringUrl);
			sourceComboBox_->removeItem(index);
		}
	}
}

QStringList AMSampleCameraBrowserView::previousSourceURLs() const
{
	QStringList rv;
	for(int i=0; i<sourceComboBox_->count(); i++) {
		rv << sourceComboBox_->itemText(i);
	}
	return rv;
}

void AMSampleCameraBrowserView::setPreviousSourceURLs(const QStringList &sourceURLs)
{
	QString currentText = sourceComboBox_->currentText();
	int currentIndex = sourceComboBox_->currentIndex(); // could be -1 if nothing in the list

	// remove existing items, except for current
	for(int i=0; i<currentIndex; i++)
		sourceComboBox_->removeItem(0);// remove all up to current item
	while(sourceComboBox_->count() > 1)
		sourceComboBox_->removeItem(sourceComboBox_->count()-1);// remove all beyond current

	// add new items (unless duplicated)
	foreach(QString url, sourceURLs) {
		if(url != currentText)
			sourceComboBox_->addItem(url);
	}
}




QString AMSampleCameraBrowserView::currentSourceURL() const
{
	return cameraBrowser_->currentURL();
}

AMSampleCameraBrowser* AMSampleCameraBrowserView::sampleCameraBrowser(){
	return cameraBrowser_;
}

AMSampleCameraView* AMSampleCameraBrowserView::sampleCameraView(){
	return videoWidget_;
}

void AMSampleCameraBrowserView::setCurrentSourceURL(const QString &sourceURL)
{
	if(sourceURL.isEmpty())
		return;

	int existingIndex = sourceComboBox_->findText(sourceURL);
	if(existingIndex >= 0) {
		sourceComboBox_->setCurrentIndex(existingIndex);

	}
	else {
		sourceComboBox_->insertItem(0, sourceURL);
		sourceComboBox_->setCurrentIndex(0);
	}
	cameraBrowser_->setCurrentURL(sourceURL);
}

void AMSampleCameraBrowserView::onMediaPlayerError(QMediaPlayer::Error e)
{
	qDebug() << "Error was " << e;
	qDebug() << "Supported types? " << QMediaPlayer::supportedMimeTypes();
	QMessageBox::warning(this, "AcquaCam Error", "Sorry! There was an error trying to open that media URL.");
	sourceComboBox_->removeItem(sourceComboBox_->currentIndex());
}


void AMSampleCameraBrowserView::setCrosshairColor(QColor crosshairColor)
{
	videoWidget_->setCrosshairColor(crosshairColor);
}

void AMSampleCameraBrowserView::setCrosshairLineThickness(int crosshairThickness)
{
	videoWidget_->setCrosshairLineThickness(crosshairThickness);
}

void AMSampleCameraBrowserView::setCrosshairVisible(bool crosshairVisible)
{
	videoWidget_->setCrosshairVisible(crosshairVisible);
}

void AMSampleCameraBrowserView::setCrosshairLocked(bool crosshairLocked)
{
	videoWidget_->setCrosshairLocked(crosshairLocked);
}

void AMSampleCameraBrowserView::setCrosshairPosition(QPointF crosshairPosition)
{
	videoWidget_->setCrosshairPosition(crosshairPosition);
}

QColor AMSampleCameraBrowserView::crosshairColor() const
{
	return videoWidget_->crosshairPen().color();
}

int AMSampleCameraBrowserView::crosshairLineThickness() const
{
	return videoWidget_->crosshairPen().width();
}

bool AMSampleCameraBrowserView::crosshairVisible() const
{
	return videoWidget_->crosshairVisible();
}

QPointF AMSampleCameraBrowserView::crosshairPosition() const
{
	return videoWidget_->crosshairPosition();
}

bool AMSampleCameraBrowserView::crosshairLocked() const
{
	return videoWidget_->crosshairLocked();
}

void AMSampleCameraBrowserView::init(AMSampleCameraBrowser *cameraBrowser)
{
	cameraBrowser_ = cameraBrowser;
	videoWidget_ = new AMSampleCameraView(cameraBrowser_->shapeDataSet());
	sampleView_ = new AMSampleContainerView();
	sampleView_->setSampleContainer(cameraBrowser_->sampleContainer());
	sampleView_->show();

	//	crosshairLocked_ = false;

	setWindowTitle("Video");

	// GUI setup:
	//////////////////////////
	QVBoxLayout* vl = new QVBoxLayout();
	vl->setSpacing(0);
	vl->setContentsMargins(0,0,0,0);


	QFrame* sourceFrame = new QFrame();
	QHBoxLayout* shl = new QHBoxLayout();
	shl->setContentsMargins(12,4,12,4);
	shl->addWidget(new QLabel("Video URL:"), 0);
	shl->addWidget(sourceComboBox_ = new QComboBox(), 2);
	sourceComboBox_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	sourceFrame->setLayout(shl);


	vl->addWidget(videoWidget_);
	vl->addWidget(sourceFrame, 0);
	setLayout(vl);

	//	crosshairLocked_ = false;
/*
	setWindowTitle("Video");

	// GUI setup:
	//////////////////////////
	QVBoxLayout* vl = new QVBoxLayout();
	vl->setSpacing(0);
	vl->setContentsMargins(0,0,0,0);


	QFrame* sourceFrame = new QFrame();
	QHBoxLayout* shl = new QHBoxLayout();
	shl->setContentsMargins(12,4,12,4);
	shl->addWidget(new QLabel("Video URL:"), 0);
	shl->addWidget(sourceComboBox_ = new QComboBox(), 2);
	sourceComboBox_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	sourceFrame->setLayout(shl);


	vl->addWidget(videoWidget_);
	vl->addWidget(sourceFrame, 0);
	setLayout(vl);
*/
	qDebug()<<"setting shape data view";


	// configure source combo box
	///////////////////////////
	sourceComboBox_->setEditable(true);
	sourceComboBox_->setInsertPolicy(QComboBox::InsertAtTop);
	sourceComboBox_->setMaxCount(20);



	// Make conections:
	//////////////////////////

	connect(sourceComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSourceComboBoxChanged(int)));

	connect(videoWidget_->mediaPlayer(), SIGNAL(error(QMediaPlayer::Error)), this, SLOT(onMediaPlayerError()));
	connect(sampleView_, SIGNAL(indexChanged(int)), cameraBrowser_, SLOT(sampleIndexChanged(int)));
	connect(sampleView_, SIGNAL(indexChanged(int)), videoWidget_, SLOT(currentSelectionChanged()));
	connect(cameraBrowser_, SIGNAL(changeSampleIndex(int)), sampleView_, SLOT(setCurrentSelection(int)));
	//        connect(videoWidget_, SIGNAL(changeSampleName(int,QString)), sampleView_, SLOT(updateSampleName(int,QString)));
}

