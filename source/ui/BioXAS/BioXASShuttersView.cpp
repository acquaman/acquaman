#include "BioXASShuttersView.h"
#include "ui/BioXAS/BioXASControlEditor.h"

BioXASShuttersView::BioXASShuttersView(BioXASShutters *shutters, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	shutters_ = 0;

	// Create UI elements.

	shuttersEditor_ = new BioXASControlEditor(0);

	childViewsLayout_ = new QVBoxLayout();

	childViews_ = new QGroupBox();
	childViews_->setFlat(true);
	childViews_->setLayout(childViewsLayout_);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(shuttersEditor_);
	layout->addWidget(childViews_);

	setLayout(layout);

	// Current settings.

	setControl(shutters);
	refresh();
}

BioXASShuttersView::~BioXASShuttersView()
{

}

void BioXASShuttersView::refresh()
{
	updateShuttersEditor();
	updateChildViews();
}

void BioXASShuttersView::setControl(BioXASShutters *newControl)
{
	if (shutters_ != newControl) {

		if (shutters_)
			disconnect( shutters_, 0, this, 0 );

		shutters_ = newControl;

		if (shutters_)
			connect( shutters_, SIGNAL(shuttersChanged()), this, SLOT(refresh()) );

		refresh();

		emit controlChanged(shutters_);
	}
}

void BioXASShuttersView::setTitle(const QString &newTitle)
{
	shuttersEditor_->setTitle(newTitle);
}

void BioXASShuttersView::updateShuttersEditor()
{
	shuttersEditor_->setControl(shutters_);
}

void BioXASShuttersView::updateChildViews()
{
	// Clear the child views.

	QLayoutItem *item = 0;

	while ( (item = childViewsLayout_->takeAt(0)) ) {
		QWidget *childView = item->widget();

		if (childView) {
			childView->disconnect();
			delete childView;
		}

		delete item;
	}

	// Create new child views.

	if (shutters_) {
		foreach (AMControl *child, shutters_->childControls()) {
			if (child)
				childViewsLayout_->addWidget(new BioXASControlEditor(child));
		}
	}

	// Updates the child views visibility.

	if (shutters_ && shutters_->childControlCount() > 0)
		childViews_->show();
	else
		childViews_->hide();
}
