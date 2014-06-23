#include "AMCurrentAmplifierView.h"

AMCurrentAmplifierView::AMCurrentAmplifierView(AMCurrentAmplifier *amplifier, QWidget *parent) :
    QWidget(parent)
{
    initialized_ = false;

    amplifier_ = amplifier;
    connect( amplifier_, SIGNAL(amplifierModeChanged()), this, SLOT(refreshView()) );
    connect( amplifier_, SIGNAL(valueChanged()), this, SLOT(onAmplifierValueChanged()) );

    name_ = new QLabel(amplifier_->name());

    minus_ = new QToolButton();
    minus_->show();
    minus_->setMaximumSize(25, 25);
    minus_->setIcon(QIcon(":/22x22/list-remove.png"));
    connect( minus_, SIGNAL(clicked()), this, SLOT(onMinusClicked()) );
    connect( amplifier_, SIGNAL(minimumValue(bool)), minus_, SLOT(setDisabled(bool)) );

    plus_ = new QToolButton();
    plus_->show();
    plus_->setMaximumSize(25, 25);
    plus_->setIcon(QIcon(":/22x22/list-add.png"));
    connect( plus_, SIGNAL(clicked()), this, SLOT(onPlusClicked()) );
    connect( amplifier_, SIGNAL(maximumValue(bool)), plus_, SLOT(setDisabled(bool)) );

    value_ = new QComboBox();
    value_->hide();
    connect( value_, SIGNAL(currentIndexChanged(QString)), this, SLOT(onValueComboBoxChanged(QString)) );

    setViewMode(Basic);
    showName(false);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)) );

    refreshView();

    QHBoxLayout *layout = new QHBoxLayout();

    layout->addWidget(name_);
    layout->addWidget(minus_);
    layout->addWidget(plus_);
    layout->addWidget(value_);

    setLayout(layout);

    initialized_ = true;
}

AMCurrentAmplifierView::~AMCurrentAmplifierView()
{

}

AMCurrentAmplifier* AMCurrentAmplifierView::currentAmplifier() const
{
    return amplifier_;
}

bool AMCurrentAmplifierView::initialized() const
{
    return initialized_;
}

AMCurrentAmplifierView::ViewMode AMCurrentAmplifierView::viewMode() const
{
    return mode_;
}

void AMCurrentAmplifierView::setViewMode(ViewMode newMode)
{
    if (newMode != mode_) {
        mode_ = newMode;

        if (mode_ == Basic) {
            value_->hide();
            plus_->show();
            minus_->show();

        } else {
            value_->show();
            plus_->hide();
            minus_->hide();
        }

        emit viewModeChanged(mode_);
    }
}

void AMCurrentAmplifierView::showName(bool show)
{
    if (show) {
        name_->show();

    } else {
        name_->hide();
    }
}

void AMCurrentAmplifierView::refreshView()
{
    refreshDisplayValues();
    refreshButtons();
}

void AMCurrentAmplifierView::onValueComboBoxChanged(const QString &newText)
{
    // the initialized_ boolean prevents the display from setting the amplifier value while view is initializing <- undesirable behavior.
    if (initialized_) {
        amplifier_->setValue(newText);
    }
}

void AMCurrentAmplifierView::onAmplifierValueChanged()
{
    amplifier_->blockSignals(true);
    int newIndex = value_->findText( valueToString(amplifier_->value(), amplifier_->units()) );
    if (newIndex != -1)
        value_->setCurrentIndex(newIndex);
    amplifier_->blockSignals(false);
}

void AMCurrentAmplifierView::onMinusClicked()
{
    amplifier_->decreaseValue();
}

void AMCurrentAmplifierView::onPlusClicked()
{
    amplifier_->increaseValue();
}

void AMCurrentAmplifierView::onCustomContextMenuRequested(QPoint position)
{
    // only show the menu if there is a valid amplifier and that amplifier supports multiple view options.

    QMenu menu(this);

    QAction *basic = menu.addAction("Basic view");
    basic->setDisabled(mode_ == Basic);

    QAction *advanced = menu.addAction("Advanced view");
    advanced->setDisabled(mode_ == Advanced);

    if (amplifier_ && amplifier_->supportsGainMode() && amplifier_->supportsSensitivityMode()) {
        menu.addSeparator();

        QAction *gain = menu.addAction("Gain view");
        gain->setDisabled(mode_ == Basic || amplifier_->inGainMode());

        QAction *sensitivity = menu.addAction("Sensitivity view");
        sensitivity->setDisabled(mode_ == Basic || amplifier_->inSensitivityMode());
    }

    QAction *selected = menu.exec(mapToGlobal(position));

    if (selected) {
        if (selected->text() == "Basic view")
            setViewMode(Basic);

        else if (selected->text() == "Advanced view")
            setViewMode(Advanced);

        else if (selected->text() == "Gain view")
            amplifier_->setAmplifierMode(AMCurrentAmplifier::Gain);

        else if (selected->text() == "Sensitivity view")
            amplifier_->setAmplifierMode(AMCurrentAmplifier::Sensitivity);
    }
}

QString AMCurrentAmplifierView::valueToString(double value, const QString &units) const
{
    return QString("%1 %2").arg(value, 0, 'e', 2).arg(units);
}

void AMCurrentAmplifierView::refreshDisplayValues()
{
    initialized_ = false;

    value_->clear();

    // (re)populate value_ with appropriate options provided by the amplifier.
    QStringList unitsList = amplifier_->unitsList();
    QList<double> valuesList = amplifier_->values();

    foreach (QString units, unitsList) {
        foreach (double value, valuesList) {
            QString item = valueToString(value, units);
            value_->addItem(item);
        }
    }

    // values displayed should represent the amplifier's current state.
    onAmplifierValueChanged();

    initialized_ = true;
}

void AMCurrentAmplifierView::refreshButtons()
{
    minus_->setDisabled(amplifier_->atMinimumValue());
    plus_->setDisabled(amplifier_->atMaximumValue());
}
