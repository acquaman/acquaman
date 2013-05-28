#ifndef AMBEAMLINECAMERABROWSER2_H
#define AMBEAMLINECAMERABROWSER2_H

#include <QWidget>

class QPushButton;
class QComboBox;
class QCheckBox;
class AMShapeDataSetView;
class AMColorPickerButton2;
class QSlider;
class AMShapeOverlayVideoWidgetView2;
class QLineEdit;
class AMCameraConfigurationView;
class AMShapeDataView;
class AMCameraBrowser;
/// This class provides a general-purpose widget that people can use to monitor the video from different network camera sources.
class AMCameraBrowserView : public QWidget
{
	Q_OBJECT
public:
    explicit AMCameraBrowserView(AMCameraBrowser *cameraBrowser, QWidget *parent = 0, bool useOpenGlViewport = true);
    AMCameraBrowserView(QWidget *parent = 0, bool useOpenGlViewport = true);

    /// Returns a list of the URLs of all the video sources in the history (including the current one)
    QStringList previousSourceURLs() const;

    /// Returns the current video source URL that is currently playing (or loading, or attempting to play, etc.)
    QString currentSourceURL() const;


signals:
    void coordinateChange(double,double,double);



public slots:

    /// Set the history list of previous sources (URLs of cameras or files visited).
    /*! If there is currently a source playing, it will be left as the most recent item in the new list.*/
    void setPreviousSourceURLs(const QStringList& sourceURLs);

    /// Add a new video source URL and start playing it, or switch to an existing source URL (if this already exists in the history)
    void setCurrentSourceURL(const QString& sourceURL);

    /// Mode Selection
    void drawMode();
    void moveMode();
    void editMode();
    void shiftMode();
    void operationMode();
    void groupMode();

    /// Respond to change in motor coordinates
    void setMotorCoordinate();

    /// Change the shape information
    void nameChanged(QString);
    void currentChanged();

    void xChanged(QString);
    void yChanged(QString);
    void zChanged(QString);
    void rotationChanged(QString);
    void tiltChanged(QString);
    void setCoordinate();
    void applyDistortion();

    void setCrosshairColor(QColor crosshairColor);
    void setCrosshairLineThickness(int crosshairThickness);
    void setCrosshairVisible(bool crosshairVisible);
    void setCrosshairLocked(bool crosshairLocked);
    void setCrosshairPosition(QPointF crosshairPosition);


    QColor crosshairColor() const;
    int crosshairLineThickness() const;
    bool crosshairVisible() const;
    QPointF crosshairPosition() const;
    bool crosshairLocked() const;

protected:
    void init(AMCameraBrowser *, bool);

protected:

    AMCameraBrowser *cameraBrowser_;

    AMShapeDataSetView* videoWidget_;
    QCheckBox* showCrosshairCheckBox_, *lockCrosshairCheckBox_;
    AMColorPickerButton2* crosshairColorPicker_;
    QComboBox* sourceComboBox_;
    QSlider* crosshairThicknessSlider_;



    /// Mode buttons
    QPushButton* drawButton_;
    QPushButton* moveButton_;
    QPushButton* editButton_;
    QPushButton* shiftButton_;
    QPushButton* operationButton_;
    QPushButton* groupButton_;
    /// Motor coordinate control
    QLineEdit* motorXEdit_;
    QLineEdit* motorYEdit_;
    QLineEdit* motorZEdit_;
    QLineEdit* motorREdit_;
    /// Motor coordinate set button
    QPushButton* setMotorCoordinate_;



    /// Configuration Window
    AMCameraConfigurationView* cameraConfiguration_;

    /// AMShapeDataView \todo move to AMShapeDataSetView
    AMShapeDataView *shapeView_ ;







protected slots:

	/// called when the user selects a different source from the combobox
	void onSourceComboBoxChanged(int index);



	/// Called when the media player has an error (ex: invalid URL specified)
	void onMediaPlayerError();

};

#endif // AMBEAMLINECAMERABROWSER_H
