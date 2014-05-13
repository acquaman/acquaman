#ifndef AMDETECTORSELECTORREQUIREDVIEW_H
#define AMDETECTORSELECTORREQUIREDVIEW_H

#include <QGroupBox>
#include <QMap>

class QScrollArea;
class QVBoxLayout;
class QLabel;
class AMDetectorSelectorRequiredViewInternal;
class AMDetector;
class AMDetectorSelector;

class AMDetectorSelectorRequiredView : public QGroupBox
{
Q_OBJECT
public:
	/// Constructor for the top view. Going to put the internal view into a scroll area
 	virtual ~AMDetectorSelectorRequiredView();
	AMDetectorSelectorRequiredView(AMDetectorSelector *requiredDetectorSelector, QWidget *parent = 0);

protected:
	/// The internal view (doing all the actual drawing)
	AMDetectorSelectorRequiredViewInternal *internalView_;
	/// The scroll area that the internal view is going to be placed in
	QScrollArea *scrollArea_;
};

class AMDetectorSelectorRequiredViewInternalLineView;

class AMDetectorSelectorRequiredViewInternal : public QWidget
{
Q_OBJECT
public:
	/// Constructor
 	virtual ~AMDetectorSelectorRequiredViewInternal();
	AMDetectorSelectorRequiredViewInternal(AMDetectorSelector *detectorSelector, QWidget *parent = 0);

protected slots:
	/// Handles changes in the detector selector where detectors become connected
	void onDetectorBecameConnected(AMDetector *detector);
	/// Handles changes in the detector selector where detectors become unconnected
	void onDetectorBecameUnconnected(AMDetector *detector);

	void onDetectorCheckedChanged(bool selected);
	void onDetectorSelectorSelectedChanged(AMDetector *detector);

protected:
	/// Helper to locate insertion index for preferential ordering
	int preferentialInsertionIndex(int preferentialListIndex, QVBoxLayout *layout);

protected:
	/// The model object
	AMDetectorSelector *detectorSelector_;

	/// The map of detector names to individual view widgets
	QMap<QString, AMDetectorSelectorRequiredViewInternalLineView*> allDetectorViews_;

	/// The groupbox for the connected detectors
	QGroupBox *connectedGroupBox_;
	/// The layout for the connected detectors
	QVBoxLayout *connectedVL_;
	/// Label for the case where there are no connected detectors
	QLabel *noneConnectedLabel_;
	/// The groupbox for the unconnected detectors
	QGroupBox *unconnectedGroupBox_;
	/// The layout for the unconnected detectors
	QVBoxLayout *unconnectedVL_;
	/// Label for the case where there are no unconnected detectors
	QLabel *noneUnconnectedLabel_;
};

class QCheckBox;

class AMDetectorSelectorRequiredViewInternalLineView : public QWidget
{
Q_OBJECT
public:
	/// Constructor. Creates a "single line" to view a single detector
 	virtual ~AMDetectorSelectorRequiredViewInternalLineView();
	AMDetectorSelectorRequiredViewInternalLineView(AMDetector *detector, bool isSelected = false, QWidget *parent = 0);

	/// Returns the detector name as a string
	QString detectorName() const;
	/// Returns whether or not the detector selected box is checked
	bool detectorChecked();

public slots:
	/// Sets this detector as selected (or deselected)
	void setDetectorSelected(bool selected);

signals:
	/// Emitted when the check box is (de)selected
	void detectorCheckedChanged(bool selected);

protected slots:
	void onDetectorSelectedCheckBoxToggled(bool isChecked);

protected:
	/// A check box to show the (de)selected state of this detector
	QCheckBox *detectorSelectedCheckBox_;

	/// Holds a pointer to the detector being visualized
	AMDetector *detector_;

	QLabel *detectorNameLabel_;
};

#endif // AMDETECTORSELECTORREQUIREDVIEW_H
