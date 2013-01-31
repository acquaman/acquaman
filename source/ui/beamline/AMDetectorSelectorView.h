#ifndef AMDETECTORSELECTORVIEW_H
#define AMDETECTORSELECTORVIEW_H

#include <QGroupBox>

#include "beamline/AMDetectorSet.h"

class QScrollArea;
class QVBoxLayout;
class AMDetectorSelectorViewInternal;

class AMDetectorSelectorView : public QGroupBox
{
Q_OBJECT
public:
	/// Constructor for the top view. Going to put the internal view into a scroll area
	AMDetectorSelectorView(AMDetectorSelector *detectorSelector, QWidget *parent = 0);

protected:
	/// The internal view (doing all the actual drawing)
	AMDetectorSelectorViewInternal *internalView_;
	/// The scroll area that the internal view is going to be placed in
	QScrollArea *scrollArea_;
};

class AMDetectorSelectorViewInternalLineView;

class AMDetectorSelectorViewInternal : public QWidget
{
Q_OBJECT
public:
	/// Constructor
	AMDetectorSelectorViewInternal(AMDetectorSelector *detectorSelector, QWidget *parent = 0);

protected slots:
	/// Handles changes in the detector selector where detectors become connected
	void onDetectorBecameConnected(AMDetector *detector);
	/// Handles changes in the detector selector where detectors become unconnected
	void onDetectorBecameUnconnected(AMDetector *detector);

protected:
	/// The model object
	AMDetectorSelector *detectorSelector_;

	/// The map of detector names to individual view widgets
	QMap<QString, AMDetectorSelectorViewInternalLineView*> allDetectorViews_;

	/// The layout for the connected detectors
	QVBoxLayout *connectedVL_;
	/// The layout for the unconnected detectors
	QVBoxLayout *unconnectedVL_;
};

class QLabel;
class QCheckBox;

class AMDetectorSelectorViewInternalLineView : public QWidget
{
Q_OBJECT
public:
	/// Constructor. Creates a "single line" to view a single detector
	AMDetectorSelectorViewInternalLineView(const QString &detectorName, bool isSelected = false, QWidget *parent = 0);

	/// Returns the detector name as a string
	QString detectorName() const;
	/// Returns whether or not the detector selected box is checked
	bool detectorChecked();

public slots:
	/// Sets this detector as selected (or deselected)
	void setDetectorSelected(bool selected);
	/// Paints the name in the view enabled or disabled based on connected state
	void setDetectorConnected(bool connected);

signals:
	/// Emitted when the check box is (de)selected
	void detectorCheckedChanged(bool selected);

protected:
	/// A label to hold the name of the detector
	QLabel *detectorNameLabel_;
	/// A check box to show the (de)selected state of this detector
	QCheckBox *detectorSelectedCheckBox_;
};

#endif // AMDETECTORSELECTORVIEW_H
