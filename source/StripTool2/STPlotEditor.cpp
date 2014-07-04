#include "STPlotEditor.h"

STPlotEditor::STPlotEditor(STPlotWidget *plot, QWidget *parent) : STEditor(parent)
{
    // Set variables.

    plot_ = plot;

    nameChanged_ = false;
    nameShownChanged_ = false;

    // Set up UI.

    QGridLayout *mainLayout = new QGridLayout(this);

    QLabel *nameLabel = new QLabel(this);
    nameLabel->setText("Name :");
    nameEntry_ = new QLineEdit(this);
    nameShown_ = new QCheckBox("Name shown", this);
    mainLayout->addWidget(nameLabel, 0, 0);
    mainLayout->addWidget(nameEntry_, 0, 1);
    mainLayout->addWidget(nameShown_, 1, 1);
\
    QLabel *timeLabel = new QLabel(this);
    timeLabel->setText("Time :");
    timeEntry_ = new QLineEdit(this);
    timeEntry_->setEnabled(false);
    mainLayout->addWidget(timeLabel, 2, 0);
    mainLayout->addWidget(timeEntry_, 2, 1);

    QLabel *unitsLabel = new QLabel("Units :", this);
    timeUnits_ = new QComboBox();
    timeUnits_->setEnabled(false);
    mainLayout->addWidget(unitsLabel, 3, 0);
    mainLayout->addWidget(timeUnits_, 3, 1);

    setLayout(mainLayout);

    // Make connections.

    connect( nameEntry_, SIGNAL(textChanged(QString)), this, SLOT(onNameEntryChanged()) );
    connect( nameShown_, SIGNAL(stateChanged(int)), this, SLOT(onNameShownChanged()) );

    // Get current settings.

    getPlotInfo();
}

STPlotEditor::~STPlotEditor()
{

}

void STPlotEditor::applyChanges()
{
    if (plot_) {

        qDebug() << "Applying changes to plot.";

        if (nameChanged_)
            plot_->setPlotName(nameEntry_->text());

        if (nameShownChanged_)
            plot_->showPlotName(nameShown_->checkState() == Qt::Checked);
    }
}

void STPlotEditor::setNameEntry(const QString &oldName)
{
    nameEntry_->setText(oldName);
}

void STPlotEditor::setNameShown(bool oldState)
{
    nameShown_->setChecked(oldState);
}

void STPlotEditor::setTimeEntry(int oldValue)
{
    timeEntry_->setText(QString::number(oldValue));
}

//void STPlotEditor::setTimeUnits(STPlot::TimeUnits newUnits)
//{
//    Q_UNUSED(newUnits)
//}

void STPlotEditor::onNameEntryChanged()
{
    nameChanged_ = true;
}

void STPlotEditor::onNameShownChanged()
{
    qDebug() << "Name shown changed.";

    nameShownChanged_ = true;
}

void STPlotEditor::getPlotInfo()
{
    if (plot_) {
        setNameEntry(plot_->plotName());
        setNameShown(plot_->plotNameVisible());
    }
}

void STPlotEditor::clearPlotInfo()
{
    nameEntry_->clear();
    nameShown_->setChecked(false);
}
