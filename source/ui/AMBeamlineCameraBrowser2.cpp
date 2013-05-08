#include "AMBeamlineCameraBrowser2.h"

#include <QComboBox>
#include <QCheckBox>
#include <QBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>

#include <QMessageBox>
#include <QStringBuilder>

#include "ui/AMCrosshairOverlayVideoWidget2.h"
#include "ui/AMColorPickerButton2.h"
#include "ui/AMShapeOverlayVideoWidgetView2.h"
#include <QLineEdit>

AMBeamlineCameraBrowser2::AMBeamlineCameraBrowser2(QWidget *parent, bool useOpenGlViewport) :
	QWidget(parent)
{
	crosshairLocked_ = false;

	setWindowTitle("Video");

	// GUI setup:
	//////////////////////////
	QVBoxLayout* vl = new QVBoxLayout();
	vl->setSpacing(0);
	vl->setContentsMargins(0,0,0,0);

	QFrame* crosshairFrame = new QFrame();
	QHBoxLayout* chl = new QHBoxLayout();
	chl->setContentsMargins(12,4,12,4);
	chl->addWidget(showCrosshairCheckBox_ = new QCheckBox("Crosshair:"));
	chl->addSpacing(20);
	chl->addWidget(new QLabel("Color:"));
    chl->addWidget(crosshairColorPicker_ = new AMColorPickerButton2(Qt::red));
	chl->addWidget(new QLabel("Line:"));
	chl->addWidget(crosshairThicknessSlider_ = new QSlider(Qt::Horizontal));
	crosshairThicknessSlider_->setMaximumWidth(80);
	crosshairThicknessSlider_->setRange(1,6);
	crosshairThicknessSlider_->setValue(1);
	chl->addSpacing(20);
	chl->addWidget(lockCrosshairCheckBox_ = new QCheckBox("Lock position"));
	chl->addStretch();
	crosshairFrame->setLayout(chl);
	showCrosshairCheckBox_->setChecked(true);


	QFrame* sourceFrame = new QFrame();
	QHBoxLayout* shl = new QHBoxLayout();
	shl->setContentsMargins(12,4,12,4);
	shl->addWidget(new QLabel("Video URL:"), 0);
	shl->addWidget(sourceComboBox_ = new QComboBox(), 2);
	sourceComboBox_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	sourceFrame->setLayout(shl);

    QFrame* toolFrame  = new QFrame();
    QHBoxLayout* tfl = new QHBoxLayout();
    tfl->setContentsMargins(12,4,12,4);
    tfl->addWidget(drawButton_ = new QPushButton("Draw"));
    tfl->addSpacing(20);
    tfl->addWidget(moveButton_ = new QPushButton("Move"));
    tfl->addSpacing(20);
    tfl->addWidget(editButton_ = new QPushButton("Edit"));
    tfl->addSpacing(20);
    tfl->addWidget(shiftButton_ = new QPushButton("Shift"));
    tfl->addStretch(20);
    toolFrame->setLayout(tfl);

    QFrame* infoFrame = new QFrame();
    QHBoxLayout* ihl = new QHBoxLayout();
    ihl->setContentsMargins(12,4,12,4);
    ihl->addWidget(nameEdit_ = new QLineEdit());
    ihl->addSpacing(20);
    ihl->addWidget(infoEdit_ = new QLineEdit());
    ihl->addStretch();
    infoFrame->setLayout(ihl);


	vl->addWidget(crosshairFrame);
    vl->addWidget(videoWidget_ = new AMCrosshairOverlayVideoWidget2(0, useOpenGlViewport));
    vl->addWidget(infoFrame);
    vl->addWidget(toolFrame);
	vl->addWidget(sourceFrame, 0);
	setLayout(vl);

	// configure source combo box
	///////////////////////////
	sourceComboBox_->setEditable(true);
	sourceComboBox_->setInsertPolicy(QComboBox::InsertAtTop);
	sourceComboBox_->setMaxCount(20);


	// Make conections:
	//////////////////////////

	connect(videoWidget_, SIGNAL(mouseDoubleClicked(QPointF)), this, SLOT(onVideoWidgetDoubleClicked(QPointF)));

	connect(crosshairColorPicker_, SIGNAL(colorChanged(QColor)), this, SLOT(setCrosshairColor(QColor)));
	connect(showCrosshairCheckBox_, SIGNAL(clicked(bool)), this, SLOT(setCrosshairVisible(bool)));
	connect(lockCrosshairCheckBox_, SIGNAL(clicked(bool)), this, SLOT(setCrosshairLocked(bool)));

	connect(sourceComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSourceComboBoxChanged(int)));

	connect(videoWidget_->mediaPlayer(), SIGNAL(error(QMediaPlayer::Error)), this, SLOT(onMediaPlayerError()));
	connect(crosshairThicknessSlider_, SIGNAL(valueChanged(int)), this, SLOT(setCrosshairLineThickness(int)));

    connect(drawButton_, SIGNAL(clicked()), this, SLOT(drawMode()));
    connect(moveButton_, SIGNAL(clicked()), this, SLOT(moveMode()));
    connect(editButton_, SIGNAL(clicked()), this, SLOT(editMode()));
    connect(shiftButton_, SIGNAL(clicked()), this, SLOT(shiftMode()));

    connect(nameEdit_, SIGNAL(textChanged(QString)), this, SLOT(nameChanged(QString)));
    connect(infoEdit_, SIGNAL(textChanged(QString)), this, SLOT(infoChanged(QString)));
    connect(videoWidget_, SIGNAL(currentChanged()), this, SLOT(currentChanged()));
}



#include <QDebug>
void AMBeamlineCameraBrowser2::onSourceComboBoxChanged(int index)
{
    qDebug() << "Here in onSourceComboBoxChanged";
	if(index < 0) {
        qDebug() << "Index is < 0";
		setWindowTitle("AcquaCam");
		videoWidget_->mediaPlayer()->setMedia(QMediaContent());
	}

	else {
        qDebug() << "Valid index";
		QString stringUrl = sourceComboBox_->itemText(index);
		QUrl url = QUrl::fromUserInput(stringUrl);

		if(url.isValid()) {
			if(url.toString() != stringUrl)
				sourceComboBox_->setItemText(index, url.toString());
			setWindowTitle(url.toString());
			videoWidget_->mediaPlayer()->setMedia(url);
            qDebug() << "AMBeamlineCameraBrowser2: Loading and playing" << url.toString();
			videoWidget_->mediaPlayer()->play();
		}
		else {
			QMessageBox::warning(this, "Invalid camera source address", "Sorry! That doesn't look like a valid URL for a camera or media source:\n\n" % stringUrl);
			sourceComboBox_->removeItem(index);
		}
	}
}

QStringList AMBeamlineCameraBrowser2::previousSourceURLs() const
{
	QStringList rv;
	for(int i=0; i<sourceComboBox_->count(); i++) {
		rv << sourceComboBox_->itemText(i);
	}
	return rv;
}

void AMBeamlineCameraBrowser2::setPreviousSourceURLs(const QStringList &sourceURLs)
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

void AMBeamlineCameraBrowser2::setCrosshairColor(const QColor &color)
{
	QPen pen = videoWidget_->crosshairPen();
	pen.setColor(color);
	videoWidget_->setCrosshairPen(pen);

	if(crosshairColorPicker_->color() != color) {
		crosshairColorPicker_->blockSignals(true);
		crosshairColorPicker_->setColor(color);
		crosshairColorPicker_->blockSignals(false);
	}
}

void AMBeamlineCameraBrowser2::setCrosshairVisible(bool isVisible)
{
	videoWidget_->setCrosshairVisible(isVisible);

	if(showCrosshairCheckBox_->isChecked() != isVisible) {
		showCrosshairCheckBox_->blockSignals(true);
		showCrosshairCheckBox_->setChecked(isVisible);
		showCrosshairCheckBox_->blockSignals(false);
	}
}
void AMBeamlineCameraBrowser2::setCrosshairLocked(bool doLock)
{
	crosshairLocked_ = doLock;

	if(lockCrosshairCheckBox_->isChecked() != doLock) {
		lockCrosshairCheckBox_->blockSignals(true);
		lockCrosshairCheckBox_->setChecked(doLock);
		lockCrosshairCheckBox_->blockSignals(false);
	}
}

void AMBeamlineCameraBrowser2::onVideoWidgetDoubleClicked(const QPointF &clickPoint)
{
	if(!crosshairLocked_)
		videoWidget_->setCrosshairPosition(clickPoint);
}

QColor AMBeamlineCameraBrowser2::crosshairColor() const
{
	return videoWidget_->crosshairPen().color();
}

bool AMBeamlineCameraBrowser2::crosshairVisible() const
{
	return videoWidget_->crosshairVisible();
}

QPointF AMBeamlineCameraBrowser2::crosshairPosition() const
{
	return videoWidget_->crosshairPosition();
}

void AMBeamlineCameraBrowser2::setCrosshairPosition(const QPointF &pos) const
{
	videoWidget_->setCrosshairPosition(pos);
}

QString AMBeamlineCameraBrowser2::currentSourceURL() const
{
	return sourceComboBox_->currentText();
}

void AMBeamlineCameraBrowser2::setCurrentSourceURL(const QString &sourceURL)
{
    qDebug() << "Here in setCurrentSourceURL";
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
}

void AMBeamlineCameraBrowser2::onMediaPlayerError()
{
	QMessageBox::warning(this, "AcquaCam Error", "Sorry! There was an error trying to open that media URL.");
	sourceComboBox_->removeItem(sourceComboBox_->currentIndex());
}

void AMBeamlineCameraBrowser2::setCrosshairLineThickness(int thickness)
{
	QPen pen = videoWidget_->crosshairPen();
	pen.setWidth(thickness);
	videoWidget_->setCrosshairPen(pen);

	if(crosshairThicknessSlider_->value() != thickness) {
		crosshairThicknessSlider_->blockSignals(true);
		crosshairThicknessSlider_->setValue(thickness);
		crosshairThicknessSlider_->blockSignals(false);
	}
}

int AMBeamlineCameraBrowser2::crosshairLineThickness() const
{
	return videoWidget_->crosshairPen().width();
}


void AMBeamlineCameraBrowser2::drawMode()
{
    drawButton_->setDown(true);
    videoWidget_->setDrawMode();

}

void AMBeamlineCameraBrowser2::moveMode()
{
    moveButton_->setDown(true);
    videoWidget_->setMoveMode();
}

void AMBeamlineCameraBrowser2::editMode()
{
    editButton_->setDown(true);
    videoWidget_->setEditMode();
}

void AMBeamlineCameraBrowser2::shiftMode()
{
    shiftButton_->setDown(true);
    videoWidget_->setShiftMode();
}

void AMBeamlineCameraBrowser2::nameChanged(QString name)
{
    videoWidget_->setCurrentName(name);
    nameEdit_->setText(videoWidget_->currentName());
}

void AMBeamlineCameraBrowser2::infoChanged(QString info)
{
   videoWidget_->setCurrentInfo(info);
   infoEdit_->setText(videoWidget_->currentInfo());
}

void AMBeamlineCameraBrowser2::currentChanged()
{
    qDebug()<<"CurrentChanged";
    nameEdit_->setText(videoWidget_->currentName());
    infoEdit_->setText(videoWidget_->currentInfo());
}
