#ifndef IDEASXRFDETAILEDDETECTORVIEWWITHSAVE_H
#define IDEASXRFDETAILEDDETECTORVIEWWITHSAVE_H

#include "ui/beamline/AMXRFDetailedDetectorView.h"

#include "acquaman/IDEAS/IDEASXRFScanConfiguration.h"
#include "actions3/actions/AMScanAction.h"


#include <QSignalMapper>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QtGui/QComboBox>


class IDEASXRFDetailedDetectorViewWithSave : public AMXRFDetailedDetectorView
{
	Q_OBJECT

public:
	/// Constructor.
	IDEASXRFDetailedDetectorViewWithSave(AMXRFDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~IDEASXRFDetailedDetectorViewWithSave();

	/// Re-implementing but still going to use the base class buildDetectorView since this view is merely adding to it.
	virtual void buildDetectorView();


protected slots:
	void onSaveScanButtonClicked();
	void onNotesTextChanged();
	void onScanNameChanged(QString name);
	void onScanNumberChanged(int number);
	void onPeakingTimeBoxChanged(const QString &arg1);
	void onAcquisitionSucceeded();
	void onKETEKPeakingTimeChanged();

protected:
	/// Method that builds the Scan Save Button and associated things.
	void buildScanSaveViews();

	/// The button for saving the scan.
	QPushButton *saveScanButton_;

	/// A  scan configuration notes editor
	QPlainTextEdit *notesEdit;

	QGridLayout *scanInfoGridLayout;
	QLabel *scanNameLabel;
	QLineEdit *scanName;
	QLabel *scanNumberLabel;
	QSpinBox *scanNumber;
	QComboBox *peakingTimeBox;

	/// The scan configuration.  Contains some configuration details about the scan such as integration time, minimum energy, maximum energy.
	IDEASXRFScanConfiguration *config_;

        /// The scan action created when a scan is saved
        AMAction3* scanAction_;


};

#endif // IDEASXRFDETAILEDDETECTORVIEWWITHSAVE_H
