#include "EntryWidget.h"

EntryWidget::EntryWidget(QWidget *parent) :
    QWidget(parent)
{
    lineEdit_ = new QLineEdit();
    connect( this, SIGNAL(entryEnabled(bool)), lineEdit_, SLOT(setEnabled(bool)) );
    connect( this, SIGNAL(clearEntry()), lineEdit_, SLOT(clear()) );
    connect( lineEdit_, SIGNAL(textChanged(QString)), this, SLOT(toSaveEntry(QString)) );
    connect( lineEdit_, SIGNAL(returnPressed()), this, SLOT(addClicked()) );

    button_ = new QPushButton();
    button_->setText("Add");
    connect( button_, SIGNAL(clicked()), this, SLOT(addClicked()) );
    connect( this, SIGNAL(entryEnabled(bool)), button_, SLOT(setEnabled(bool)) );

    QHBoxLayout *entryLayout = new QHBoxLayout();
    entryLayout->addWidget(lineEdit_);
    entryLayout->addWidget(button_);

    setLayout(entryLayout);

    connect( this, SIGNAL(entryComplete(QString)), this, SLOT(toTestSignal(QString)) );
}



EntryWidget::~EntryWidget()
{
}



void EntryWidget::setLineEditText(const QString &text)
{
    lineEdit_->setText(text);
}



void EntryWidget::setButtonText(const QString &text)
{
    button_->setText(text);
}



void EntryWidget::setWidthMax(int maxWidth)
{
    setMaximumWidth(maxWidth);
}



void EntryWidget::setWidthMin(int minWidth)
{
    setMinimumWidth(minWidth);
}



void EntryWidget::toSaveEntry(const QString &newText)
{
    textEntered_ = newText;
}



void EntryWidget::addClicked() {

    if (textEntered_ == "")
    {
        QMessageBox errorBox;
        errorBox.setText("Entry cannot be empty.");
        errorBox.exec();

    } else {

        emit entryComplete(textEntered_);
    }

    emit clearEntry();
}



void EntryWidget::toTestSignal(const QString &textEntered)
{
    qDebug() << "Entry was completed. Text saved :" << textEntered;
}
