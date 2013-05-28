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
    init(cameraBrowser, useOpenGlViewport);

}

AMCameraBrowserView::AMCameraBrowserView(QWidget *parent, bool useOpenGlViewport) :
    QWidget(parent)
{
    init(new AMCameraBrowser, useOpenGlViewport);
}



#include <QDebug>
void AMCameraBrowserView::onSourceComboBoxChanged(int index)
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
	return sourceComboBox_->currentText();
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
}

void AMCameraBrowserView::onMediaPlayerError()
{
	QMessageBox::warning(this, "AcquaCam Error", "Sorry! There was an error trying to open that media URL.");
	sourceComboBox_->removeItem(sourceComboBox_->currentIndex());
}





void AMCameraBrowserView::drawMode()
{
    drawButton_->setDown(true);
    videoWidget_->setDrawMode();

}

void AMCameraBrowserView::moveMode()
{
    moveButton_->setDown(true);
    videoWidget_->setMoveMode();
}

void AMCameraBrowserView::editMode()
{
    editButton_->setDown(true);
    videoWidget_->setEditMode();
}

void AMCameraBrowserView::shiftMode()
{
    shiftButton_->setDown(true);
    videoWidget_->setShiftMode();
}

void AMCameraBrowserView::operationMode()
{
    operationButton_->setDown(true);
    videoWidget_->setOperationMode();
    /// Window testing
   cameraConfiguration_->show();
}

void AMCameraBrowserView::groupMode()
{
    groupButton_->setDown(true);
    videoWidget_->setGroupMode();
}

void AMCameraBrowserView::setMotorCoordinate()
{
    double x = motorXEdit_->text().toDouble();
    double y = motorYEdit_->text().toDouble();
    double z = motorZEdit_->text().toDouble();
    double r = motorREdit_->text().toDouble();
    videoWidget_->setMotorCoordinate(x,y,z,r);
}

void AMCameraBrowserView::nameChanged(QString name)
{
    qDebug()<<"Name changed";
    videoWidget_->setCurrentName(name);
    shapeView_->setName(videoWidget_->currentName());
}

void AMCameraBrowserView::tiltChanged(QString tilt)
{
   videoWidget_->setTilt(tilt.toDouble());
  // tiltEdit_->setText(videoWidget_->currentTilt());
}

void AMCameraBrowserView::currentChanged()
{
    shapeView_->setName(videoWidget_->currentName());
    shapeView_->setTilt(QString::number(videoWidget_->tilt()));
    shapeView_->setX(QString::number(videoWidget_->xCoordinate()));
    shapeView_->setY(QString::number(videoWidget_->yCoordinate()));
    shapeView_->setZ(QString::number(videoWidget_->zCoordinate()));
    shapeView_->setRotation(QString::number(videoWidget_->rotation()));
    motorREdit_->setText(QString::number(videoWidget_->motorRotation()));
    motorXEdit_->setText(QString::number(videoWidget_->motorX()));
    motorYEdit_->setText(QString::number(videoWidget_->motorY()));
    motorZEdit_->setText(QString::number(videoWidget_->motorZ()));
}

void AMCameraBrowserView::xChanged(QString text)
{
    double x = text.toDouble();
    videoWidget_->setX(x);
}

void AMCameraBrowserView::yChanged(QString text)
{
   double y = text.toDouble();
   videoWidget_->setY(y);
}

void AMCameraBrowserView::zChanged(QString text)
{
    double z = text.toDouble();
    videoWidget_->setZ(z);
}

#include "dataman/AMSample.h"
void AMCameraBrowserView::rotationChanged(QString text)
{
    double rotation = text.toDouble();
    videoWidget_->setRotation(rotation);
}

void AMCameraBrowserView::setCoordinate()
{
    videoWidget_->moveCurrentToCoordinate();
}

void AMCameraBrowserView::applyDistortion()
{
    videoWidget_->toggleDistortion();
}

void AMCameraBrowserView::setCrosshairColor(QColor crosshairColor)
{
    cameraBrowser_->setCrosshairColor(crosshairColor);
}

void AMCameraBrowserView::setCrosshairLineThickness(int crosshairThickness)
{
    cameraBrowser_->setCrosshairLineThickness(crosshairThickness);
}

void AMCameraBrowserView::setCrosshairVisible(bool crosshairVisible)
{
    cameraBrowser_->setCrosshairVisible(crosshairVisible);
}

void AMCameraBrowserView::setCrosshairLocked(bool crosshairLocked)
{
    cameraBrowser_->setCrosshairLocked(crosshairLocked);
}

void AMCameraBrowserView::setCrosshairPosition(QPointF crosshairPosition)
{
    cameraBrowser_->setCrosshairPosition(crosshairPosition);
}

QColor AMCameraBrowserView::crosshairColor() const
{
    return cameraBrowser_->crosshairColor();
}

int AMCameraBrowserView::crosshairLineThickness() const
{
    return cameraBrowser_->crosshairLineThickness();
}

bool AMCameraBrowserView::crosshairVisible() const
{
    return cameraBrowser_->crosshairVisible();
}

QPointF AMCameraBrowserView::crosshairPosition() const
{
    return cameraBrowser_->crosshairPosition();
}

bool AMCameraBrowserView::crosshairLocked() const
{
    return cameraBrowser_->crosshairLocked();
}

void AMCameraBrowserView::init(AMCameraBrowser *cameraBrowser, bool useOpenGlViewport)
{
    cameraBrowser_ = cameraBrowser;
    //	crosshairLocked_ = false;

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
        tfl->addSpacing(20);
        tfl->addWidget(operationButton_ = new QPushButton("Operation"));
        tfl->addSpacing(20);
        tfl->addWidget(groupButton_ = new QPushButton("Group"));
        tfl->addSpacing(20);
        tfl->addWidget(motorXEdit_ = new QLineEdit());
        tfl->addSpacing(10);
        tfl->addWidget(motorYEdit_ = new QLineEdit());
        tfl->addSpacing(10);
        tfl->addWidget(motorZEdit_ = new QLineEdit());
        tfl->addSpacing(10);
        tfl->addWidget(motorREdit_ = new QLineEdit());
        tfl->addSpacing(20);
        tfl->addWidget(setMotorCoordinate_ = new QPushButton("Set"));
        tfl->addStretch(20);
        toolFrame->setLayout(tfl);

    //    QFrame* infoFrame = new QFrame();
    //    QHBoxLayout* ihl = new QHBoxLayout();
    //    ihl->setContentsMargins(12,4,12,4);
    //    ihl->addWidget(nameEdit_ = new QLineEdit());
    //    ihl->addSpacing(20);
    //    ihl->addWidget(tiltEdit_ = new QLineEdit());
    //    ihl->addSpacing(10);
    //    ihl->addWidget(xEdit_ = new QLineEdit());
    //    ihl->addSpacing(10);
    //    ihl->addWidget(yEdit_ = new QLineEdit());
    //    ihl->addSpacing(10);
    //    ihl->addWidget(zEdit_ = new QLineEdit());
    //    ihl->addSpacing(10);
    //    ihl->addWidget(rotationEdit_ = new QLineEdit());
    //    ihl->addSpacing(20);
    //    ihl->addWidget(setCoordinate_ = new QPushButton("Set"));
    //    ihl->addSpacing(20);
    //    ihl->addWidget(distortionButton_ = new QPushButton("Distortion"));
    //    ihl->addStretch();
    //    infoFrame->setLayout(ihl);


        vl->addWidget(crosshairFrame);
        vl->addWidget(videoWidget_ = new AMShapeDataSetView(0, useOpenGlViewport));
        vl->addWidget(shapeView_ = new AMShapeDataView());
        vl->addWidget(toolFrame);
        vl->addWidget(sourceFrame, 0);
        setLayout(vl);

        qDebug()<<"setting shaped data view";
        cameraBrowser_->setShapeDataView(videoWidget_);

        // configure source combo box
        ///////////////////////////
        sourceComboBox_->setEditable(true);
        sourceComboBox_->setInsertPolicy(QComboBox::InsertAtTop);
        sourceComboBox_->setMaxCount(20);

        // create the configuration window
        cameraConfiguration_ = new AMCameraConfigurationView();


        // Make conections:
        //////////////////////////

        connect(videoWidget_, SIGNAL(mouseDoubleClicked(QPointF)), cameraBrowser_, SLOT(onVideoWidgetDoubleClicked(QPointF)));

        connect(crosshairColorPicker_, SIGNAL(colorChanged(QColor)), cameraBrowser_, SLOT(setCrosshairColor(QColor)));
        connect(showCrosshairCheckBox_, SIGNAL(clicked(bool)), cameraBrowser_, SLOT(setCrosshairVisible(bool)));
        connect(lockCrosshairCheckBox_, SIGNAL(clicked(bool)), cameraBrowser_, SLOT(setCrosshairLocked(bool)));

        connect(sourceComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSourceComboBoxChanged(int)));

        connect(videoWidget_->mediaPlayer(), SIGNAL(error(QMediaPlayer::Error)), this, SLOT(onMediaPlayerError()));
        connect(crosshairThicknessSlider_, SIGNAL(valueChanged(int)), cameraBrowser_, SLOT(setCrosshairLineThickness(int)));

        connect(drawButton_, SIGNAL(clicked()), this, SLOT(drawMode()));
        connect(moveButton_, SIGNAL(clicked()), this, SLOT(moveMode()));
        connect(editButton_, SIGNAL(clicked()), this, SLOT(editMode()));
        connect(shiftButton_, SIGNAL(clicked()), this, SLOT(shiftMode()));
        connect(operationButton_, SIGNAL(clicked()), this, SLOT(operationMode()));
        connect(groupButton_, SIGNAL(clicked()), this, SLOT(groupMode()));
        connect(setMotorCoordinate_, SIGNAL(clicked()), this, SLOT(setMotorCoordinate()));

        connect(shapeView_, SIGNAL(nameChanged(QString)), this, SLOT(nameChanged(QString)));
        connect(shapeView_, SIGNAL(tiltChanged(QString)), this, SLOT(tiltChanged(QString)));
        connect(videoWidget_, SIGNAL(currentChanged()), this, SLOT(currentChanged()));

        connect(shapeView_, SIGNAL(xChanged(QString)), this, SLOT(xChanged(QString)));
        connect(shapeView_, SIGNAL(yChanged(QString)), this, SLOT(yChanged(QString)));
        connect(shapeView_, SIGNAL(zChanged(QString)), this, SLOT(zChanged(QString)));
        connect(shapeView_, SIGNAL(rotationChanged(QString)), this, SLOT(rotationChanged(QString)));
        connect(shapeView_, SIGNAL(setCoordinate()), this, SLOT(setCoordinate()));
        connect(this, SIGNAL(coordinateChange(double,double,double)), videoWidget_, SIGNAL(setCoordinate(double,double,double)));
        connect(shapeView_, SIGNAL(applyDistortion()), this, SLOT(applyDistortion()));

        connect(cameraConfiguration_, SIGNAL(update(AMCameraConfiguration*)), videoWidget_, SLOT(setCameraModel(AMCameraConfiguration*)));

        connect(cameraConfiguration_, SIGNAL(oneSelect()), videoWidget_, SIGNAL(oneSelect()));
        connect(cameraConfiguration_, SIGNAL(twoSelect()), videoWidget_, SIGNAL(twoSelect()));

        connect(videoWidget_, SIGNAL(beamChanged(QObject*)), cameraConfiguration_, SIGNAL(beamChanged(QObject*)));

        connect(cameraConfiguration_, SIGNAL(intersection()), videoWidget_, SLOT(intersection()));


        cameraConfiguration_->updateAll();
}

//void AMBeamlineCameraBrowser2::changeCoordinate(QString)
//{
//    double x = xEdit_->text().toDouble();
//    double y = yEdit_->text().toDouble();
//    double z = zEdit_->text().toDouble();
//    emit coordinateChange(x,y,z);
//}
