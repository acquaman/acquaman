#ifndef SGMFASTSCANCREATESAMPLESCANLISTVIEW_H
#define SGMFASTSCANCREATESAMPLESCANLISTVIEW_H

#include <QWidget>

class QComboBox;
class QDoubleSpinBox;
class QSpinBox;
class QPushButton;
class QCheckBox;

class SGMFastScanConfiguration2013;

class SGMFastScanCreateSampleScanListView : public QWidget
{
Q_OBJECT

public:
	/// Constructor takes a pointer to the configuration to be run and a parent
	SGMFastScanCreateSampleScanListView(SGMFastScanConfiguration2013 *configuration, QWidget *parent = 0);

protected:
	/// Overrides the regular close event to clean up
	virtual void closeEvent(QCloseEvent *closeEvent);

protected slots:
	/// Handles adding this list/loop to the workflow
	void onAddToWorkflowButtonClicked();

protected:
	/// The fast scan configuration we're going to use
	SGMFastScanConfiguration2013 *configuration_;

	/// A combo box to select the sample to be scanned
	QComboBox *sampleComboBox_;
	/// A double spin box to choose the rotation
	QDoubleSpinBox *rotationDoubleSpinBox_;
	/// A check box to determine if we'll do initialization before starting any scans
	QCheckBox *oneTimeInitializationCheckBox_;
	/// A spin box to choose the number of rows
	QSpinBox *rowsSpinBox_;
	/// A double spin box to choose the size of the veritcal steps
	QDoubleSpinBox *verticalStepSpinBox_;
	/// A spin box to choose the number of columns
	QSpinBox *columnsSpinBox_;
	/// A double spin box to choose the size of the horizontal steps
	QDoubleSpinBox *horizontalStepSpinBox_;

	/// Button to confirm and add to workflow
	QPushButton *addToWorkflowButton_;
	/// Button to cancel and close window
	QPushButton *cancelButton_;
};

#endif // SGMFASTSCANCREATESAMPLESCANLISTVIEW_H
