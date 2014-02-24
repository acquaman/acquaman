#ifndef AMXRFDETAILEDDETECTORVIEWWITHSAVE_H
#define AMXRFDETAILEDDETECTORVIEWWITHSAVE_H

#include "ui/beamline/AMXRFBaseDetectorView.h"
#include "ui/beamline/AMXRFDetailedDetectorView.h"

#include "MPlot/MPlotPoint.h"
#include "MPlot/MPlotMarkerTransparentVerticalRectangle.h"

#include "util/AMSelectablePeriodicTable.h"
#include "ui/util/AMSelectablePeriodicTableView.h"
#include "ui/util/AMSelectableElementView.h"
#include "util/AMNameAndRangeValidator.h"

#include "acquaman/IDEAS/IDEASXRFScanConfiguration.h"


#include <QSignalMapper>
#include <QtGui/QPlainTextEdit>
#include <QLineEdit>


class AMXRFDetailedDetectorViewWithSave : public AMXRFDetailedDetectorView
{
    Q_OBJECT
public:
    explicit AMXRFDetailedDetectorViewWithSave(AMXRFDetector *detector, QWidget *parent = 0);

    /// Re-implementing but still going to use the base class buildDetectorView since this view is merely adding to it.
    virtual void buildDetectorView();

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



    /// The scan configuration.  Contains some configuration details about the scan such as integration time, minimum energy, maximum energy.
    IDEASXRFScanConfiguration *config_;



signals:

public slots:

private slots:
    void onSaveScanButtonClicked();
    void onNotesTextChanged(){ config_->setScanNotes(notesEdit->toPlainText()); }
    void onScanNameChanged(QString name){ config_->setScanName(name); }
    void onScanNumberChanged(int number){ config_->setScanNumber(number); }

};

#endif // AMXRFDETAILEDDETECTORVIEWWITHSAVE_H
