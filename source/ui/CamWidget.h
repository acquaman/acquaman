#ifndef ACQMAN_CAMWIDGET_H_
#define ACQMAN_CAMWIDGET_H_

#include <QWidget>
#include <QComboBox>

#include <mediaobject.h>
#include <videowidget.h>

// A widget that provides a view onto multiple Axis-server MJPEG streams,
// using the Phonon multimedia library

class CamWidget : public QWidget {
	
	Q_OBJECT
	
public:
	CamWidget(const QString& cameraName, const QUrl& cameraAddress, QWidget* parent = 0);
	virtual ~CamWidget();
	
	void setupVideo();
	
	void addSource(const QString& cameraName, const QUrl& cameraAddress);
	
public slots:

	void onSourceChanged(int index);
	
	
protected:
	Phonon::VideoWidget* videoWidget_;
	Phonon::MediaObject* mediaObject_;
	
	QComboBox* cameraList_;
	
	
};

#endif /*CAMWIDGET_H_*/
