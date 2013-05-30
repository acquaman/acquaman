#include "AMCameraBrowserView.h"

#include <QComboBox>
#include <QCheckBox>
#include <QBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>

#include <QMessageBox>
#include <QStringBuilder>

#include "ui/AMShapeDataSetView.h"
#include "ui/AMColorPickerButton2.h"
#include "ui/AMCameraConfigurationView.h"
#include "ui/AMShapeDataView.h"
#include "AMCameraBrowser.h"
#include <QLineEdit>

#include <QDebug>

AMCameraBrowserView::AMCameraBrowserView(AMCameraBrowser *cameraBrowser, QWidget *parent, bool useOpenGlViewport) :
	QWidget(parent)
{
    if(useOpenGlViewport)qDebug()<<"Using openGlViewport"; // but not really
    init(cameraBrowser);

}

AMCameraBrowserView::AMCameraBrowserView(QWidget *parent, bool useOpenGlViewport) :
    QWidget(parent)
{
    if(useOpenGlViewport)qDebug()<<"Using openGlViewport";
    init(new AMCameraBrowser());
}



#include <QDebug>
void AMCameraBrowserView::onSourceComboBoxChanged(int index)
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
            videoWidget_->play();
		}
		else {
			QMessageBox::warning(this, "Invalid camera source address", "Sorry! That doesn't look like a valid URL for a camera or media source:\n\n" % stringUrl);
			sourceComboBox_->removeItem(index);
		}
	}
}

QStringList AMCameraBrowserView::previousSourceURLs() const
{
	QStringList rv;
	for(int i=0; i<sourceComboBox_->count(); i++) {
		rv << sourceComboBox_->itemText(i);
	}
	return rv;
}

void AMCameraBrowserView::setPreviousSourceURLs(const QStringList &sourceURLs)
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




QString AMCameraBrowserView::currentSourceURL() const
{
    return cameraBrowser_->currentURL();
}

void AMCameraBrowserView::setCurrentSourceURL(const QString &sourceURL)
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

void AMCameraBrowserView::onMediaPlayerError()
{
	QMessageBox::warning(this, "AcquaCam Error", "Sorry! There was an error trying to open that media URL.");
	sourceComboBox_->removeItem(sourceComboBox_->currentIndex());
}


void AMCameraBrowserView::setCrosshairColor(QColor crosshairColor)
{
    videoWidget_->setCrosshairColor(crosshairColor);
}

void AMCameraBrowserView::setCrosshairLineThickness(int crosshairThickness)
{
    videoWidget_->setCrosshairLineThickness(crosshairThickness);
}

void AMCameraBrowserView::setCrosshairVisible(bool crosshairVisible)
{
    videoWidget_->setCrosshairVisible(crosshairVisible);
}

void AMCameraBrowserView::setCrosshairLocked(bool crosshairLocked)
{
    videoWidget_->setCrosshairLocked(crosshairLocked);
}

void AMCameraBrowserView::setCrosshairPosition(QPointF crosshairPosition)
{
    videoWidget_->setCrosshairPosition(crosshairPosition);
}

QColor AMCameraBrowserView::crosshairColor() const
{
    return videoWidget_->crosshairPen().color();
}

int AMCameraBrowserView::crosshairLineThickness() const
{
    return videoWidget_->crosshairPen().width();
}

bool AMCameraBrowserView::crosshairVisible() const
{
    return videoWidget_->crosshairVisible();
}

QPointF AMCameraBrowserView::crosshairPosition() const
{
    return videoWidget_->crosshairPosition();
}

bool AMCameraBrowserView::crosshairLocked() const
{
    return videoWidget_->crosshairLocked();
}

void AMCameraBrowserView::init(AMCameraBrowser *cameraBrowser)
{
    cameraBrowser_ = cameraBrowser;
    videoWidget_ = new AMShapeDataSetView(cameraBrowser_->shapeDataSet());
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

        qDebug()<<"setting shape data view";


        // configure source combo box
        ///////////////////////////
        sourceComboBox_->setEditable(true);
        sourceComboBox_->setInsertPolicy(QComboBox::InsertAtTop);
        sourceComboBox_->setMaxCount(20);



        // Make conections:
        //////////////////////////

        connect(videoWidget_, SIGNAL(mouseDoubleClicked(QPointF)), cameraBrowser_, SLOT(onVideoWidgetDoubleClicked(QPointF)));
        connect(sourceComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSourceComboBoxChanged(int)));
        connect(videoWidget_->mediaPlayer(), SIGNAL(error(QMediaPlayer::Error)), this, SLOT(onMediaPlayerError()));








}

