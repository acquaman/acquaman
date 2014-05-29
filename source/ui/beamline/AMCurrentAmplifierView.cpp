//#include "AMCurrentAmplifierView.h"

//AMCurrentAmplifierView::AMCurrentAmplifierView(AMCurrentAmplifier *amplifier, ValueMode mode, QWidget *parent) :
//    QWidget(parent)
//{
//    amplifier_ = amplifier;

//    minus_ = new QToolButton();
//    minus_->setMaximumSize(25, 25);
//    minus_->setIcon(QIcon(":/22x22/list-remove.png"));

//    connect( minus_, SIGNAL(clicked()), amplifier_, SLOT(decreaseSensitivity()) );
//    connect( amplifier_, SIGNAL(minimumSensitivity(bool)), minus_, SLOT(setDisabled(bool)) );

//    if (amplifier_->isConnected() && amplifier_->atMinimumSensitivity()) {
//        minus_->setDisabled(true);
//    }

//    plus_ = new QToolButton();
//    plus_->setMaximumSize(25, 25);
//    plus_->setIcon(QIcon(":/22x22/list-add.png"));

//    connect( plus_, SIGNAL(clicked()), amplifier_, SLOT(increaseSensitivity()) );
//    connect( amplifier_, SIGNAL(maximumSensitivity(bool)), plus_, SLOT(setDisabled(bool)) );

//    if (amplifier_->isConnected() && amplifier_->atMaximumSensitivity()) {
//        plus_->setDisabled(true);
//    }

//    value_ = new QComboBox();
//    value_->hide();
////    value_->addItems(amplifier_->valuesList());

//    connect( value_, SIGNAL(currentIndexChanged(int)), this, SLOT(onValueComboBoxChanged(int)) );
//    connect( amplifier_, SIGNAL(valueIndexChanged(int)), this, SLOT(onValueChanged(int)) );

//    if (amplifier_->isConnected()) {
////        onValueChanged(amplifier_->valueIndex());
//    }

//    units_ = new QComboBox();
//    units_->hide();
////    units_->addItems(amplifier_->unitsList());

//    if (amplifier_->isConnected()) {
////        onUnitsChanged(amplifier_->units());
//    }

//    setViewMode(Basic);
//    setValueMode(mode);

//    setContextMenuPolicy(Qt::CustomContextMenu);
//    connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)) );

//    QHBoxLayout *layout = new QHBoxLayout();
//    layout->addWidget(minus_);
//    layout->addWidget(plus_);
//    layout->addWidget(value_);
//    layout->addWidget(units_);

//    setLayout(layout);

//}

//AMCurrentAmplifierView::~AMCurrentAmplifierView()
//{

//}


//AMCurrentAmplifier* AMCurrentAmplifierView::currentAmplifier() const
//{
//    return amplifier_;
//}

//AMCurrentAmplifierView::ViewMode AMCurrentAmplifierView::viewMode() const
//{
//    return viewMode_;
//}

//AMCurrentAmplifierView::ValueMode AMCurrentAmplifierView::valueMode() const
//{
//    return valueMode_;
//}

//void AMCurrentAmplifierView::setViewMode(ViewMode newMode)
//{
//    if (newMode != viewMode_) {
//        viewMode_ = newMode;

//        if (viewMode_ == Basic) {
//            value_->hide();
//            units_->hide();
//            plus_->show();
//            minus_->show();

//            emit viewModeChanged(viewMode_);

//        } else if (viewMode_ == Advanced) {
//            plus_->hide();
//            minus_->hide();
//            value_->show();
//            units_->show();

//            emit viewModeChanged(viewMode_);

//        } else {
//            qDebug() << "AMCurrentAmplifierView : unknown view mode encountered. Setting mode to Basic.";
//            setViewMode(Basic);
//        }
//    }
//}

//void AMCurrentAmplifierView::setValueMode(ValueMode newMode)
//{
//    if (newMode != valueMode_) {
//        valueMode_ = newMode;
//        emit valueModeChanged(valueMode_);
//    }
//}

//void AMCurrentAmplifierView::onCustomContextMenuRequested(QPoint pos)
//{
//    QMenu popup(this);

//    QAction *temp = popup.addAction("Basic View");
//    temp->setDisabled(viewMode_ == Basic);

//    temp = popup.addAction("Advanced View");
//    temp->setDisabled(viewMode_ == Advanced);

//    temp = popup.exec(mapToGlobal(pos));

//    // If a valid action was selected.
//    if (temp){

//        if (temp->text() == "Basic View")
//            setViewMode(Basic);

//        else if (temp->text() == "Advanced View")
//            setViewMode(Advanced);
//    }
//}
