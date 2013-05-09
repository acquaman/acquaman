#ifndef AMBEAMLINECAMERABROWSER2_H
#define AMBEAMLINECAMERABROWSER2_H

#include <QWidget>

class QPushButton;
class QComboBox;
class QCheckBox;
class AMCrosshairOverlayVideoWidget2;
class AMColorPickerButton2;
class QSlider;
class AMShapeOverlayVideoWidgetView2;
class QLineEdit;

/// This class provides a general-purpose widget that people can use to monitor the video from different network camera sources.
class AMBeamlineCameraBrowser2 : public QWidget
{
	Q_OBJECT
public:
    explicit AMBeamlineCameraBrowser2(QWidget *parent = 0, bool useOpenGlViewport = true);


	/// Is the crosshair locked?
	bool crosshairLocked() const { return crosshairLocked_; }
	/// Returns the crosshair color
	QColor crosshairColor() const;
	/// Returns the crosshair line thickness
	int crosshairLineThickness() const;
	/// Is the crosshair visible?
	bool crosshairVisible() const;
	/// Returns the crosshair position (relative X-Y position on the video display, from (0,0)[top left] to (1,1)[bottom right] )
	QPointF crosshairPosition() const;

	/// Returns a list of the URLs of all the video sources in the history (including the current one)
	QStringList previousSourceURLs() const;

	/// Returns the current video source URL that is currently playing (or loading, or attempting to play, etc.)
	QString currentSourceURL() const;

signals:
    void coordinateChange(double,double,double);

public slots:

	/// Set the crosshair color
	void setCrosshairColor(const QColor& color);
	/// Set the crosshair line thickness
	void setCrosshairLineThickness(int thickness);
	/// Set whether the crosshair is visible or not
	void setCrosshairVisible(bool isVisible);
	/// Disable the capability to move the cross-hair by double-clicking
	void setCrosshairLocked(bool doLock = true);
	/// Set the crosshair position (relative X-Y position on the video display, from (0,0)[top left] to (1,1)[bottom right] )
	void setCrosshairPosition(const QPointF& pos) const;

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

    /// Change the shape information
    void nameChanged(QString);
    void infoChanged(QString);
    void currentChanged();

    void xChanged(QString);
    void yChanged(QString);
    void zChanged(QString);
    void rotationChanged(QString);
    void setCoordinate();




protected:

    AMCrosshairOverlayVideoWidget2* videoWidget_;
	QCheckBox* showCrosshairCheckBox_, *lockCrosshairCheckBox_;
    AMColorPickerButton2* crosshairColorPicker_;
	QComboBox* sourceComboBox_;
	QSlider* crosshairThicknessSlider_;

	bool crosshairLocked_;

    /// Mode buttons
    QPushButton* drawButton_;
    QPushButton* moveButton_;
    QPushButton* editButton_;
    QPushButton* shiftButton_;

    /// Line Edits
    QLineEdit* nameEdit_;
    QLineEdit* infoEdit_;
    QLineEdit* xEdit_;
    QLineEdit* yEdit_;
    QLineEdit* zEdit_;
    QLineEdit* rotationEdit_;
    QPushButton* setCoordinate_;







protected slots:

	/// called when the user selects a different source from the combobox
	void onSourceComboBoxChanged(int index);

	/// Called when double-clicking on the video widget (move the cursor if not locked)
	void onVideoWidgetDoubleClicked(const QPointF& clickPoint);

	/// Called when the media player has an error (ex: invalid URL specified)
	void onMediaPlayerError();

};

#endif // AMBEAMLINECAMERABROWSER_H
