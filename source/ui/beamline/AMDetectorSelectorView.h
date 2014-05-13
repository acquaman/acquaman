#ifndef AMDETECTORSELECTORVIEW_H
#define AMDETECTORSELECTORVIEW_H

#include <QGroupBox>
#include <QMap>

class QScrollArea;
class QVBoxLayout;
class QLabel;
class AMDetectorSelectorViewInternal;
class AMDetectorGeneralBriefView;
class AMDetector;
class AMDetectorSelector;

class AMDetectorSelectorView : public QGroupBox
{
Q_OBJECT
public:
	/// Constructor for the top view. Going to put the internal view into a scroll area
 	virtual ~AMDetectorSelectorView();
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
 	virtual ~AMDetectorSelectorViewInternal();
	AMDetectorSelectorViewInternal(AMDetectorSelector *detectorSelector, QWidget *parent = 0);

protected slots:
	/// Handles changes in the detector selector where detectors become connected
	void onDetectorBecameConnected(AMDetector *detector);
	/// Handles changes in the detector selector where detectors become unconnected
	void onDetectorBecameUnconnected(AMDetector *detector);

	/// Handles changes from the lineviews to the checked state (selected state)
	void onDetectorCheckedChanged(bool selected);
	/// Handles changes from the detectorSelected to the selected state
	void onDetectorSelectorSelectedChanged(AMDetector *detector);

protected:
	/// Helper to locate insertion index for preferential ordering
	int preferentialInsertionIndex(int preferentialListIndex, QVBoxLayout *layout);

protected:
	/// The model object
	AMDetectorSelector *detectorSelector_;

	/// The map of detector names to individual view widgets
	QMap<QString, AMDetectorSelectorViewInternalLineView*> allDetectorViews_;

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

class AMDetectorSelectorViewInternalLineView : public QWidget
{
Q_OBJECT
public:
	/// Constructor. Creates a "single line" to view a single detector
 	virtual ~AMDetectorSelectorViewInternalLineView();
	AMDetectorSelectorViewInternalLineView(AMDetector *detector, bool isSelected = false, QWidget *parent = 0);

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
	/// A check box to show the (de)selected state of this detector
	QCheckBox *detectorSelectedCheckBox_;

	/// Holds a pointer to the detector being visualized
	AMDetector *detector_;
	/// The general brief-style view for this detector
	AMDetectorGeneralBriefView *briefView_;
};

#endif // AMDETECTORSELECTORVIEW_H
