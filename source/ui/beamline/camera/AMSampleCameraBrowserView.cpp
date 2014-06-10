#include "ui/beamline/camera/AMSampleCameraBrowserView.h"

#include <QComboBox>
#include <QCheckBox>
#include <QBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QUrl>
#include <QApplication>
#include <QMessageBox>
#include <QStringBuilder>

#include "ui/beamline/camera/AMSampleCameraView.h"
#include "ui/AMColorPickerButton2.h"
#include "ui/beamline/camera/AMCameraConfigurationView.h"
#include "ui/beamline/camera/AMShapeDataView.h"
#include "beamline/camera/AMSampleCameraBrowser.h"
#include <QLineEdit>

AMSampleCameraBrowserView::AMSampleCameraBrowserView(AMSampleCameraBrowser *cameraBrowser, QWidget *parent, bool useOpenGlViewport) :
	QWidget(parent)
{
	Q_UNUSED(useOpenGlViewport)
	videoWidget_ = 0;
	init(cameraBrowser);

}

AMSampleCameraBrowserView::AMSampleCameraBrowserView(QWidget *parent, bool useOpenGlViewport) :
	QWidget(parent)
{
	Q_UNUSED(useOpenGlViewport)
	videoWidget_ = 0;
	init(new AMSampleCameraBrowser());
}

void AMSampleCameraBrowserView::onSourceComboBoxChanged(int index)
{
	if(index < 0) {
		setWindowTitle("AcquaCam");
		#ifdef AM_MOBILITY_VIDEO_ENABLED
		videoWidget_->setMedia(QMediaContent());
		#endif
	}

	else {
		QString stringUrl = sourceComboBox_->itemText(index);
		QUrl url = QUrl::fromUserInput(stringUrl);

		if(url.isValid()) {
			if(url.toString() != stringUrl)
				sourceComboBox_->setItemText(index, url.toString());
			setWindowTitle(url.toString());
			#ifdef AM_MOBILITY_VIDEO_ENABLED
			videoWidget_->setMedia(url);
			videoWidget_->play();
			#endif
		}
		else {
			QMessageBox::warning(this, "Invalid camera source address", "Sorry! That doesn't look like a valid URL for a camera or media source:\n\n" % stringUrl);
			sourceComboBox_->removeItem(index);
		}
	}
}

void AMSampleCameraBrowserView::onRowsInserted(const QModelIndex &index, int start, int end){
	Q_UNUSED(end)
	cameraBrowser_->addURL(sourceComboBox_->model()->data(sourceComboBox_->model()->index(start, 0, index)).toString());
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

#ifdef AM_MOBILITY_VIDEO_ENABLED
void AMSampleCameraBrowserView::onMediaPlayerError(QMediaPlayer::Error e)
{
	Q_UNUSED(e)
	QMessageBox::warning(this, "AcquaCam Error", "Sorry! There was an error trying to open that media URL.");
	cameraBrowser_->removeURL(sourceComboBox_->currentText());
	sourceComboBox_->removeItem(sourceComboBox_->currentIndex());
}
#endif

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

void AMSampleCameraBrowserView::setSamplePlateSelected()
{
	emit samplePlateSelected();
}

void AMSampleCameraBrowserView::requestAdvancedCameraOptionsWindow(){
	if(videoWidget_)
		videoWidget_->showAdvancedWindow();
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


	// configure source combo box
	///////////////////////////
	sourceComboBox_->setEditable(true);
	sourceComboBox_->setInsertPolicy(QComboBox::InsertAtTop);
	sourceComboBox_->setMaxCount(20);

	QList<AMSampleCameraURL*> urls = cameraBrowser_->urls();
	QStringList urlStrings;
	for(int x = 0; x < urls.count(); x++)
		urlStrings << urls.at(x)->urlString();
	setPreviousSourceURLs(urlStrings);
	sourceComboBox_->setCurrentIndex(-1);

	// Make conections:
	//////////////////////////

	connect(sourceComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSourceComboBoxChanged(int)));
	connect(sourceComboBox_->model(), SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onRowsInserted(QModelIndex,int,int)));

	#ifdef AM_MOBILITY_VIDEO_ENABLED
	connect(videoWidget_->mediaPlayer(), SIGNAL(error(QMediaPlayer::Error)), this, SLOT(onMediaPlayerError(QMediaPlayer::Error)));
	#endif

	if(urls.count() != 0)
		sourceComboBox_->setCurrentIndex(0);

	// wizard signals
	connect(this, SIGNAL(beamWizardPressed()), videoWidget_, SLOT(startBeamWizard()));
	connect(this, SIGNAL(cameraWizardPressed()), videoWidget_, SLOT(startCameraWizard()));
	connect(this, SIGNAL(samplePlateWizardPressed()), videoWidget_, SLOT(startSampleWizard()));
	connect(this, SIGNAL(rotationWizardPressed()), videoWidget_, SLOT(startRotationWizard()));

	connect(this, SIGNAL(requestLoadBeam()), videoWidget_, SLOT(requestLoadBeam()));
	connect(this, SIGNAL(requestLoadCamera()), videoWidget_, SLOT(requestLoadCamera()));
	connect(this, SIGNAL(requestLoadSamplePlate()), videoWidget_, SLOT(requestLoadSamplePlate()));
	connect(this, SIGNAL(requestLoadRotationConfiguration()), videoWidget_, SLOT(requestLoadRotationConfiguration()));

	connect(this, SIGNAL(requestLoadBeamFromDatabase()), videoWidget_, SLOT(requestLoadBeamFromDatabase()));
	connect(this, SIGNAL(requestLoadCameraFromDatabase()), videoWidget_, SLOT(requestLoadCameraFromDatabase()));
	connect(this, SIGNAL(requestLoadSamplePlateFromDatabase()), videoWidget_, SLOT(requestLoadSamplePlateFromDatabase()));
	connect(this, SIGNAL(requestLoadRotationConfigurationFromDatabase()), videoWidget_, SLOT(requestLoadRotationConfigurationFromDatabase()));

	connect(videoWidget_, SIGNAL(beamWizardFinished()), this, SIGNAL(beamWizardFinished()));
	connect(videoWidget_, SIGNAL(cameraWizardFinished()), this, SIGNAL(cameraWizardFinished()));
	connect(videoWidget_, SIGNAL(samplePlateWizardFinished(bool)), this, SIGNAL(samplePlateWizardFinished()));
	connect(videoWidget_, SIGNAL(rotationWizardFinished()), this, SIGNAL(rotationWizardFinished()));

	connect(this, SIGNAL(samplePlateSelected()), videoWidget_, SLOT(samplePlateSelected()));
}

