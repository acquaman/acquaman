#include "BioXASZebraLogicBlockView.h"
#include "ui/BioXAS/BioXASZebraInputView.h"
#include "ui/BioXAS/BioXASZebraLogicBlockInputEnabledView.h"
#include "ui/BioXAS/BioXASZebraLogicBlockInputInvertedView.h"

BioXASZebraLogicBlockView::BioXASZebraLogicBlockView(BioXASZebraLogicBlock *control, const QString &title, QWidget *parent) :
	QGroupBox(title, parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	inputViewsLayout_ = new QGridLayout();

	QGroupBox *inputViewsBox = new QGroupBox();
	inputViewsBox->setTitle("Inputs");
	inputViewsBox->setFlat(true);
	inputViewsBox->setLayout(inputViewsLayout_);

	outputStatusLabel_ = new QLabel();

	QHBoxLayout *outputStatusLayout = new QHBoxLayout();
	outputStatusLayout->addStretch();
	outputStatusLayout->addWidget(outputStatusLabel_);
	outputStatusLayout->addStretch();

	QGroupBox *outputStatusBox = new QGroupBox();
	outputStatusBox->setTitle("Output");
	outputStatusBox->setFlat(true);
	outputStatusBox->setLayout(outputStatusLayout);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(inputViewsBox);
	layout->addWidget(outputStatusBox);

	setLayout(layout);

	// Current settings.

	setControl(control);

	refresh();
}

BioXASZebraLogicBlockView::~BioXASZebraLogicBlockView()
{

}

void BioXASZebraLogicBlockView::refresh()
{
	updateInputViews();
	updateOutputStatusLabel();
}

void BioXASZebraLogicBlockView::setControl(BioXASZebraLogicBlock *newControl)
{
	if (control_ != newControl) {
		
		if (control_)
			disconnect( control_, 0, this, 0 );
		
		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(outputStatusChanged(bool)), this, SLOT(updateOutputStatusLabel()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void BioXASZebraLogicBlockView::updateInputViews()
{
	// Clear the input views.

	clearInputViews();

	// Create new inputs view.

	if (control_) {

		// Add headings.

		inputViewsLayout_->addWidget(new QWidget(), 0, 0);
		inputViewsLayout_->addWidget(new QLabel("Enabled"), 0, 1, 1, 1, Qt::AlignCenter);
		inputViewsLayout_->addWidget(new QLabel("Configuration"), 0, 2, 1, 1, Qt::AlignCenter);
		inputViewsLayout_->addWidget(new QLabel("Inverted"), 0, 3, 1, 1, Qt::AlignCenter);

		// Create new widgets for each input control.

		QList<BioXASZebraLogicBlockInput*> inputControls = control_->inputControls();

		for (int i = 0, count = inputControls.count(); i < count; i++) {
			int rowIndex = i + 1;
			BioXASZebraLogicBlockInput *inputControl = qobject_cast<BioXASZebraLogicBlockInput*>(inputControls.at(i));

			if (inputControl) {
				inputViewsLayout_->addWidget(new QLabel(QString("%1: ").arg(rowIndex)), rowIndex, 0, 1, 1, Qt::AlignRight);

				BioXASZebraLogicBlockInputEnabledView *enabledBox = new BioXASZebraLogicBlockInputEnabledView(inputControl);
				inputViewsLayout_->addWidget(enabledBox, rowIndex, 1, 1, 1, Qt::AlignCenter);

				BioXASZebraInputView *inputView = new BioXASZebraInputView(inputControl);
				inputViewsLayout_->addWidget(inputView, rowIndex, 2, 1, 1, Qt::AlignCenter);

				BioXASZebraLogicBlockInputInvertedView *invertedBox = new BioXASZebraLogicBlockInputInvertedView(inputControl);
				inputViewsLayout_->addWidget(invertedBox, rowIndex, 3, 1, 1, Qt::AlignCenter);
			}
		}
	}
}

void BioXASZebraLogicBlockView::updateOutputStatusLabel()
{
	bool status = false;

	if (control_)
		status = control_->outputStatus();

	outputStatusLabel_->setPixmap(QIcon(status ? ":/22x22/greenLEDOn.png" : ":/22x22/greenLEDOff.png").pixmap(22));
}

void BioXASZebraLogicBlockView::clearInputViews()
{
	QLayoutItem *item;

	while (item = inputViewsLayout_->takeAt(0)) {
		QWidget *inputView = item->widget();

		if (inputView) {
			inputView->disconnect();
			delete inputView;
		}

		delete item;
	}
}
