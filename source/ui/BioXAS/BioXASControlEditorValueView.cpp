#include "BioXASControlEditorValueView.h"
#include "beamline/AMControl.h"

BioXASControlEditorValueView::BioXASControlEditorValueView(AMControl *control, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	label_ = new QLabel(this);

	// Current settings.

//	setControl(control);
}

BioXASControlEditorValueView::~BioXASControlEditorValueView()
{

}

void BioXASControlEditorValueView::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(repaint()) );
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(repaint()) );
		}

		repaint();

		emit controlChanged(control_);
	}
}

void BioXASControlEditorValueView::paintEvent(QPaintEvent *event)
{
	// Create painter.

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	// Identify the text to be displayed.

	QString controlText = "[Invalid]";

	QStyleOptionButton labelOption;
	labelOption.text = controlText;
	labelOption.rect = event->rect();

	QApplication::style()->drawControl(QStyle::CE_PushButtonLabel, &labelOption, &painter);

//	if (control_) {

//		controlText = "[Not measurable]";

//		if (control_->canMeasure()) {
//			controlText = QString("%1").arg(control_->value());

//			QString units = control_->units();
//			if (!units.isEmpty())
//				controlText.append(QString(" %1").arg(units));
//		}
//	}

//	// If the control is moving, the label displays the control's move progress.
//	// Else, the label displays as an ordinary label.

//	if (control_ && control_->isMoving()) {

//		QStyleOptionProgressBar progressBarOption;
//		progressBarOption.minimum = 0;
//		progressBarOption.maximum = 100;
//		progressBarOption.rect = QRect(50, 25, 100, 50);
//		progressBarOption.progress = 0;
//		progressBarOption.text = controlText;
//		progressBarOption.textVisible = true;

//		QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, &painter);

//	} else {

//		QStyleOptionButton buttonOption;
//		buttonOption.text = controlText;
//		buttonOption.rect = QRect(50, 25, 100, 50);
//		buttonOption.features = QStyleOptionButton::Flat;

//		QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonOption, &painter);
//	}
}
