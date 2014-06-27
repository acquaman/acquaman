#include "AMCurrentAmplifierView.h"


AMCurrentAmplifierView::AMCurrentAmplifierView(QWidget *parent) :
    QWidget(parent)
{
    initialized_ = false;

    viewMode_ = Basic;

    name_ = new QLabel();
    name_->hide();

    minus_ = new QToolButton();
    minus_->show();
    minus_->setMaximumSize(25, 25);
    minus_->setIcon(QIcon(":/22x22/list-remove.png"));
    connect( minus_, SIGNAL(clicked()), this, SLOT(onMinusClicked()) );

    plus_ = new QToolButton();
    plus_->show();
    plus_->setMaximumSize(25, 25);
    plus_->setIcon(QIcon(":/22x22/list-add.png"));
    connect( plus_, SIGNAL(clicked()), this, SLOT(onPlusClicked()) );

    value_ = new QComboBox();
    value_->setMaxVisibleItems(5);
    value_->hide();
    connect( value_, SIGNAL(currentIndexChanged(QString)), this, SLOT(onValueComboBoxChanged(QString)) );

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)) );

    layout_ = new QHBoxLayout();

    layout_->addWidget(name_);
    layout_->addWidget(minus_);
    layout_->addWidget(plus_);
    layout_->addWidget(value_);

    setLayout(layout_);

    setInitialized(true);
}

AMCurrentAmplifierView::~AMCurrentAmplifierView()
{

}

bool AMCurrentAmplifierView::initialized() const
{
    return initialized_;
}

AMCurrentAmplifierView::ViewMode AMCurrentAmplifierView::viewMode() const
{
    return viewMode_;
}

QString AMCurrentAmplifierView::name() const
{
    return name_->text();
}

void AMCurrentAmplifierView::setViewMode(ViewMode newMode)
{
    if (viewMode_ != newMode) {
        viewMode_ = newMode;

        if (viewMode_ == Basic) {
            value_->hide();
            plus_->show();
            minus_->show();

        } else {
            value_->show();
            plus_->hide();
            minus_->hide();
        }

        emit viewModeChanged(viewMode_);
    }
}

void AMCurrentAmplifierView::setName(const QString &newName)
{
    if (name_->text() != newName) {
        name_->setText(newName);
        emit nameChanged(name_->text());
    }
}

void AMCurrentAmplifierView::showName(bool show)
{
    if (show)
        name_->show();
    else
        name_->hide();
}

void AMCurrentAmplifierView::setViewableValuesMax(int newMax)
{
    value_->setMaxVisibleItems(newMax);
}

void AMCurrentAmplifierView::setInitialized(bool isInitialized)
{
    if (initialized_ != isInitialized) {
        initialized_ = isInitialized;
        emit initialized(initialized_);
    }
}

void AMCurrentAmplifierView::onValueComboBoxChanged(const QString &newText)
{
    if (initialized_ && isValid())
        onValueComboBoxChangedImplementation(newText);
}

void AMCurrentAmplifierView::onMinusClicked()
{
    if (isValid())
        onMinusClickedImplementation();
}

void AMCurrentAmplifierView::onPlusClicked()
{
    if (isValid())
        onPlusClickedImplementation();
}

void AMCurrentAmplifierView::refreshView()
{
    // initialized_ = false here prevents the view from changing the amplifier value while populating <-- undesireable behavior.
    if (isValid()) {
        initialized_ = false;
        refreshViewImplementation();
        initialized_ = true;
    }
}

void AMCurrentAmplifierView::onCustomContextMenuRequested(QPoint position)
{
    QMenu menu(this);

    QAction *basic = menu.addAction("Basic view");
    basic->setDisabled(viewMode_ == Basic);

    QAction *advanced = menu.addAction("Advanced view");
    advanced->setDisabled(viewMode_ == Advanced);

    QAction *selected = menu.exec(mapToGlobal(position));

    if (selected) {
        if (selected->text() == "Basic view")
            setViewMode(Basic);

        else if (selected->text() == "Advanced view")
            setViewMode(Advanced);
    }
}
