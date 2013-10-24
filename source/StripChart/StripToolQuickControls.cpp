#include "StripToolQuickControls.h"

StripToolQuickControls::StripToolQuickControls(QWidget *parent, StripToolModel *newModel) :
    QWidget(parent)
{
    model_ = newModel;
    connect( this, SIGNAL(addPV(QString, QString, QString)), model_, SLOT(addPV(QString, QString, QString)) );

    listView_ = new StripToolListView(this);
    listView_->setPVModel(model_);

    pvNameLineEdit_ = new QLineEdit();
    connect( pvNameLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(clearMessage()) );
    connect( pvNameLineEdit_, SIGNAL(returnPressed()), this, SLOT(addClicked()) );

    addButton_ = new QPushButton("Add");
    connect( addButton_, SIGNAL(clicked()), this, SLOT(addClicked()) );

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(pvNameLineEdit_);
    buttonLayout->addWidget(addButton_);

    message_ = new QLabel("");
    connect(this, SIGNAL(isValid(bool)), this, SLOT(displayMessage(bool)) );

    QVBoxLayout *controlsLayout = new QVBoxLayout();
    controlsLayout->addWidget(listView_);
    controlsLayout->addLayout(buttonLayout);
    controlsLayout->addWidget(message_);

    setLayout(controlsLayout);
    setMaximumWidth(200);
}



void StripToolQuickControls::onControlsEnabled(bool enabled)
{
    pvNameLineEdit_->setEnabled(enabled);
    addButton_->setEnabled(enabled);
}



void StripToolQuickControls::clearMessage()
{
    message_->setText("");
}



void StripToolQuickControls::displayMessage(bool isValid)
{
    if (isValid)
        emit clearMessage();
    else
        message_->setText("Invalid pv name entered.");

    emit controlsEnabled(true);
}



void StripToolQuickControls::addClicked()
{
    emit controlsEnabled(false);

    pvName_ = pvNameLineEdit_->text();

    if (pvName_ == "")
        emit isValid(false);
    else
        testValidity(pvName_);
}



void StripToolQuickControls::testValidity(const QString &pvName)
{
    pvControl_ = new AMReadOnlyPVControl(pvName, pvName, this);
    connect( pvControl_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)) );
    connect( pvControl_, SIGNAL(error(int)), this, SLOT(onPVError(int)) );
}



void StripToolQuickControls::onPVConnected(bool isConnected)
{
    if (isConnected)
    {
        emit isValid(true);
        emit addPV(pvName_, "", "");
        pvNameLineEdit_->clear();

    } else {
        emit isValid(false);
    }
}



void StripToolQuickControls::onPVError(int errorNum)
{
    if (errorNum == 1)
        emit isValid(false);
    else
        emit isValid(false);
}
