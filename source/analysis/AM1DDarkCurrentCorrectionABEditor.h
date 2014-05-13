#ifndef AM1DDARKCURRENTCORRECTIONABEDITOR_H
#define AM1DDARKCURRENTCORRECTIONABEDITOR_H

#include <QWidget>
#include <QComboBox>
#include <QFormLayout>

#include "analysis/AM1DDarkCurrentCorrectionAB.h"

class AM1DDarkCurrentCorrectionABEditor : public QWidget
{
    Q_OBJECT

public:
    explicit AM1DDarkCurrentCorrectionABEditor(AM1DDarkCurrentCorrectionAB *analysisBlock, QWidget *parent = 0);
    virtual ~AM1DDarkCurrentCorrectionABEditor();

signals:

public slots:

protected slots:
    /// Handles setting the data name for the analysis blockbased on the new choice of input data source.
    void onDataNameChoiceChanged(int index);
    /// Helper slot. Populates the names combo box.
    void populateComboBox();

protected:
    /// Pointer to the dark current analysis block.
    AM1DDarkCurrentCorrectionAB *analysisBlock_;

    /// The combo box that contains all of the names. Used for the data input source.
    QComboBox *dataNames_;

};

#endif // AM1DDARKCURRENTCORRECTIONABEDITOR_H
