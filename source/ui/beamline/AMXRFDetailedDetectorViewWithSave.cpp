#include "AMXRFDetailedDetectorViewWithSave.h"
#include "actions3/actions/AMScanAction.h"
#include "beamline/IDEAS/IDEASBeamline.h"
#include "acquaman/IDEAS/IDEASXRFScanConfiguration.h"


AMXRFDetailedDetectorViewWithSave::AMXRFDetailedDetectorViewWithSave(AMXRFDetector *detector, QWidget *parent) :
    AMXRFDetailedDetectorView(detector, parent)
{
    config_ = new IDEASXRFScanConfiguration();
}

void AMXRFDetailedDetectorViewWithSave::buildDetectorView()
{
    // This is going to extend the base view, so we will simply call it's build method too.
    AMXRFBaseDetectorView::buildDetectorView();

    AMXRFDetailedDetectorView::buildPeriodicTableViewAndElementView();
    AMXRFDetailedDetectorView::buildShowSpectraButtons();
    AMXRFDetailedDetectorView::buildPileUpPeakButtons();
    AMXRFDetailedDetectorView::buildDeadTimeView();
    AMXRFDetailedDetectorView::buildRegionOfInterestViews();

    buildScanSaveViews();

}

void AMXRFDetailedDetectorViewWithSave::buildScanSaveViews()
{

    scanInfoGridLayout = new QGridLayout();
    scanInfoGridLayout->setObjectName(QString::fromUtf8("scanInfoGridLayout"));
    scanInfoGridLayout->setVerticalSpacing(4);
    scanInfoGridLayout->setContentsMargins(12, -1, -1, -1);




    notesEdit = new QPlainTextEdit(this);
    notesEdit->setObjectName(QString::fromUtf8("notesEdit"));
    QSizePolicy sizePolicy2(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    sizePolicy2.setHorizontalStretch(4);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(notesEdit->sizePolicy().hasHeightForWidth());
    notesEdit->setSizePolicy(sizePolicy2);
    notesEdit->setStyleSheet(QString::fromUtf8( "#notesEdit {\n"
                                                "background-image: url(:/notepadBackground.png);\n"
                                                "font: bold 15px \"Marker Felt\";\n"
                                                "}"));
    notesEdit->setBackgroundVisible(false);

    scanNameLabel = new QLabel("Name");
    scanNameLabel->setObjectName(QString::fromUtf8("scanNameLabel"));
    scanNameLabel->setStyleSheet(QString::fromUtf8("color: rgb(121, 121, 121);\n"
"font-weight: bold;\n"
"font-family: \"Lucida Grande\";"));

    scanInfoGridLayout->addWidget(scanNameLabel, 0, 0, 1, 1);

    scanName = new QLineEdit(this);
    scanName->setObjectName(QString::fromUtf8("scanName"));
    scanName->setFrame(false);
    scanName->setText("XRF Scan");

    scanInfoGridLayout->addWidget(scanName, 0, 1, 1, 1);

    scanNumberLabel = new QLabel("Number");
    scanNumberLabel->setObjectName(QString::fromUtf8("scanNumberLabel"));
    scanNumberLabel->setStyleSheet(QString::fromUtf8("color: rgb(121, 121, 121);\n"
"font-weight: bold;\n"
"font-family: \"Lucida Grande\";"));

    scanInfoGridLayout->addWidget(scanNumberLabel, 1, 0, 1, 1);


    scanNumber = new QSpinBox(this);
    scanNumber->setObjectName(QString::fromUtf8("scanNumber"));
    scanNumber->setMaximumSize(QSize(16777215, 18));
    scanNumber->setFrame(false);
    scanNumber->setButtonSymbols(QAbstractSpinBox::NoButtons);
    scanNumber->setMaximum(9999);
    scanNumber->setValue(1);

    scanInfoGridLayout->addWidget(scanNumber, 1, 1, 1, 1);

    rightLayout_->addStretch();

    rightLayout_->addLayout(scanInfoGridLayout);
    rightLayout_->addWidget(notesEdit);

    saveScanButton_ = new QPushButton("Save Scan");

    rightLayout_->addWidget(saveScanButton_);

    connect(saveScanButton_, SIGNAL(clicked()), this, SLOT(onSaveScanButtonClicked()));
    connect(notesEdit, SIGNAL(textChanged()), this, SLOT(onNotesTextChanged()));
    connect(scanName, SIGNAL(textChanged(QString)), this, SLOT(onScanNameChanged(QString)));
    connect(scanNumber, SIGNAL(valueChanged(int)), this, SLOT(onScanNumberChanged(int)));

}

void AMXRFDetailedDetectorViewWithSave::onSaveScanButtonClicked()
{
    config_->setDetectorInfo(IDEASBeamline::ideas()->ketek()->toInfo());
    AMAction3* action = new AMScanAction(new AMScanActionInfo(config_->createCopy()));
    action->start();
}


/*

  AMAction3* action = new AMScanAction(new AMScanActionInfo(view_->configuration()->createCopy()))
  action()->start;

  */
