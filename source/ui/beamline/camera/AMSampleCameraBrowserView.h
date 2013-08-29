#ifndef AMBEAMLINECAMERABROWSER2_H
#define AMBEAMLINECAMERABROWSER2_H

#include <QWidget>
#include <QVector3D>

class QPushButton;
class QComboBox;
class QCheckBox;
class AMSampleCameraView;
class AMColorPickerButton2;
class QSlider;
class AMShapeOverlayVideoWidgetView2;
class QLineEdit;
class AMCameraConfigurationView;
class AMShapeDataView;
class AMSampleCameraBrowser;
class AMSamplePlate;
class QVector3D;

#include <QMediaPlayer>

/// This class provides a general-purpose widget that people can use to monitor the video from different network camera sources.
class AMSampleCameraBrowserView : public QWidget
{
	Q_OBJECT
public:
	explicit AMSampleCameraBrowserView(AMSampleCameraBrowser *cameraBrowser, QWidget *parent = 0, bool useOpenGlViewport = true);
	AMSampleCameraBrowserView(QWidget *parent = 0, bool useOpenGlViewport = true);

	/// \todo put into model
	/// Returns a list of the URLs of all the video sources in the history (including the current one)
	QStringList previousSourceURLs() const;

	/// \todo put into model
	/// Returns the current video source URL that is currently playing (or loading, or attempting to play, etc.)
	QString currentSourceURL() const;

	AMSampleCameraBrowser* sampleCameraBrowser();
	AMSampleCameraView *sampleCameraView();

	/// for saving crosshair settings
	QColor crosshairColor() const;
	int crosshairLineThickness() const;
	bool crosshairVisible() const;
	QPointF crosshairPosition() const;
	bool crosshairLocked() const;


public slots:

	/// Set the history list of previous sources (URLs of cameras or files visited).
	/*! If there is currently a source playing, it will be left as the most recent item in the new list.*/
	void setPreviousSourceURLs(const QStringList& sourceURLs);

	/// Add a new video source URL and start playing it, or switch to an existing source URL (if this already exists in the history)
	void setCurrentSourceURL(const QString& sourceURL);

	/// for loading crosshair settings
	void setCrosshairColor(QColor crosshairColor);
	void setCrosshairLineThickness(int crosshairThickness);
	void setCrosshairVisible(bool crosshairVisible);
	void setCrosshairLocked(bool crosshairLocked);
	void setCrosshairPosition(QPointF crosshairPosition);



	void setSamplePlateSelected();

signals:

    void requestLoadBeam();
    void requestLoadCamera();
    void requestLoadSamplePlate();

	void beamWizardPressed();
	void cameraWizardPressed();
	void samplePlateWizardPressed();

	void beamWizardFinished();
	void cameraWizardFinished();
	void samplePlateWizardFinished();

	void samplePlateLoaded(AMSamplePlate*);
	void samplePlateSelected();
protected:
	/// initialization
	void init(AMSampleCameraBrowser *);

protected:

	AMSampleCameraBrowser *cameraBrowser_;

	AMSampleCameraView* videoWidget_;

	QComboBox* sourceComboBox_;




protected slots:

	/// called when the user selects a different source from the combobox
	void onSourceComboBoxChanged(int index);



	/// Called when the media player has an error (ex: invalid URL specified)
	void onMediaPlayerError(QMediaPlayer::Error e);

};

#endif // AMBEAMLINECAMERABROWSER_H
