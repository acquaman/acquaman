#include "CamWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QUrl>

#include <QDebug>


CamWidget::CamWidget(const QString& cameraName, const QUrl& cameraAddress, QWidget* parent) : QWidget(parent) {
	
	// Setup UI:
	/////////////////////
	
	// Layouts: full vertical, with horizontal sub-layout at bottom.
	QVBoxLayout* vl1 = new QVBoxLayout(this);
	QHBoxLayout* hl1 = new QHBoxLayout();
	
	// VideoWidget takes up main space:
	videoWidget_ = new AMCrosshairVideoWidget();
	
	vl1->addWidget(videoWidget_);
	
	// add horizontal layout at bottom:
	vl1->addLayout(hl1);
	
	hl1->addStretch();
	QLabel* label = new QLabel("Source");
	hl1->addWidget(label);
	
	// Add combobox with list of camera names/addresses:
	cameraList_ = new QComboBox();
	hl1->addWidget(cameraList_);
	
	cameraList_->insertItem(0, cameraName, cameraAddress);
	
	
	
	setupVideo();
	
	
}

CamWidget::~CamWidget()
{
}


void CamWidget::setupVideo() {
	

	mediaObject_ = new Phonon::MediaObject(this);	
	Phonon::createPath(mediaObject_, videoWidget_);
	
	// TODO: connect error signals from mediaObject to somewhere visible...
	
	// Play the first camera:
	onSourceChanged( cameraList_->currentIndex() );
	
	connect(cameraList_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSourceChanged(int)));
	
}

#include "AMErrorMonitor.h"

void CamWidget::onSourceChanged(int index) {

	mediaObject_->stop();
	
	QUrl source( cameraList_->itemData(index).toUrl() );
	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Information, 0, QString("Connecting to video server: '%1%2' with user name: '%3' and password: '%4'").arg(source.host()).arg(source.path()).arg(source.userName()).arg(source.password())));

	mediaObject_->setCurrentSource( source );
	
	mediaObject_->play();
	
}

void CamWidget::addSource(const QString& cameraName, const QUrl& cameraAddress) {

	cameraList_->addItem(cameraName, QVariant::fromValue(cameraAddress));
}
