#include "AMBeamlineCameraBrowser.h"

#include <QComboBox>
#include <QCheckBox>
#include <QBoxLayout>
#include <QFrame>
#include <QLabel>

#include "ui/AMCrosshairOverlayVideoWidget.h"
#include "ui/AMColorPickerButton.h"

AMBeamlineCameraBrowser::AMBeamlineCameraBrowser(QWidget *parent, bool useOpenGlViewport) :
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
	chl->addWidget(crosshairColorPicker_ = new AMColorPickerButton(Qt::red));
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

	vl->addWidget(crosshairFrame);
	vl->addWidget(videoWidget_ = new AMCrosshairOverlayVideoWidget(0, useOpenGlViewport));
	vl->addWidget(sourceFrame, 0);
	setLayout(vl);

	// configure source combo box
	///////////////////////////
	sourceComboBox_->setEditable(true);
	sourceComboBox_->setInsertPolicy(QComboBox::InsertAtTop);
	sourceComboBox_->setMaxCount(20);


	// Make conections:
	//////////////////////////

	connect(videoWidget_, SIGNAL(mouseDoubleClicked(QPointF)), videoWidget_, SLOT(setCrosshairPosition(QPointF)));

	connect(crosshairColorPicker_, SIGNAL(colorChanged(QColor)), this, SLOT(onCrosshairPickerColorChanged(QColor)));
	connect(showCrosshairCheckBox_, SIGNAL(clicked(bool)), videoWidget_, SLOT(setCrosshairVisible(bool)));
	connect(lockCrosshairCheckBox_, SIGNAL(clicked(bool)), this, SLOT(setCrosshairLocked(bool)));

	connect(sourceComboBox_, SIGNAL(currentIndexChanged(QString)), this, SLOT(onSourceComboBoxChanged(QString)));

}

void AMBeamlineCameraBrowser::setCrosshairLocked(bool doLock)
{
	if(doLock && !crosshairLocked_) {
		disconnect(videoWidget_, SIGNAL(mouseDoubleClicked(QPointF)), videoWidget_, SLOT(setCrosshairPosition(QPointF)));
		crosshairLocked_ = true;
	}
	else if(!doLock && crosshairLocked_){
		connect(videoWidget_, SIGNAL(mouseDoubleClicked(QPointF)), videoWidget_, SLOT(setCrosshairPosition(QPointF)));
		crosshairLocked_ = false;
	}
}

void AMBeamlineCameraBrowser::onCrosshairPickerColorChanged(const QColor &newColor)
{
	videoWidget_->setCrosshairPen(QPen(newColor));
}

void AMBeamlineCameraBrowser::onSourceComboBoxChanged(const QString &newSourceURL)
{
	QUrl url(newSourceURL);
	if(url.isValid()) {
		setWindowTitle(url.toString());
		videoWidget_->mediaPlayer()->setMedia(url);
		videoWidget_->mediaPlayer()->play();
	}
}
