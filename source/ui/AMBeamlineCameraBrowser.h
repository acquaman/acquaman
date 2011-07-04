#ifndef AMBEAMLINECAMERABROWSER_H
#define AMBEAMLINECAMERABROWSER_H

#include <QWidget>

class QComboBox;
class QCheckBox;
class AMCrosshairOverlayVideoWidget;
class AMColorPickerButton;

class AMBeamlineCameraBrowser : public QWidget
{
	Q_OBJECT
public:
	explicit AMBeamlineCameraBrowser(QWidget *parent = 0, bool useOpenGlViewport = true);

signals:

public slots:

	/// Disable the capability to move the cross-hair by double-clicking
	void setCrosshairLocked(bool doLock = true);


protected:

	AMCrosshairOverlayVideoWidget* videoWidget_;
	QCheckBox* showCrosshairCheckBox_, *lockCrosshairCheckBox_;
	AMColorPickerButton* crosshairColorPicker_;
	QComboBox* sourceComboBox_;

	bool crosshairLocked_;


protected slots:
	/// when the color picker is changed to set a new color for the crosshair
	void onCrosshairPickerColorChanged(const QColor& newColor);
	/// When the lock checkbox is activated or de-activated
	void onCrosshairLockCheckboxClicked(bool doLock) { setCrosshairLocked(doLock); }

	/// called when the user selects a different source from the combobox
	void onSourceComboBoxChanged(const QString& newSourceURL);

};

#endif // AMBEAMLINECAMERABROWSER_H
