#include "AMBeamlineCameraWidgetWithSourceTabs.h"

#include <QTabBar>
#include <QVBoxLayout>

AMBeamlineCameraWidgetWithSourceTabs::AMBeamlineCameraWidgetWithSourceTabs(const QUrl& initialSource, const QString& initialDescription, QWidget *parent, bool useOpenGl) :
	AMBeamlineCameraWidget(parent, useOpenGl)
{
	// Add GUI element: the tab bar:
	tabBar_ = new QTabBar();
	tabBar_->setDocumentMode(false);
	QVBoxLayout* vl = qobject_cast<QVBoxLayout*>(layout());
	vl->insertWidget(1, tabBar_, 0);

	// Make connections
	connect(tabBar_, SIGNAL(currentChanged(int)), this, SLOT(playSourceAt(int)));
	// Not used for now: Should users be able to close tabs intentionally?
		// connect(tabBar_, SIGNAL(tabCloseRequested(int)), this, SLOT(removeSource(int));

	// if we have an initial source, add it.
	if(initialSource.isValid()) {
		addSource(initialDescription, initialSource);
	}
}

void AMBeamlineCameraWidgetWithSourceTabs::addSource(const QString &description, const QUrl &sourceUrl)
{
	sourceUrls_ << sourceUrl;
	tabBar_->addTab(description);
}

void AMBeamlineCameraWidgetWithSourceTabs::removeSource(int index)
{
	if(index<0 || index>=sourceCount())
		return;
	sourceUrls_.removeAt(index);
	tabBar_->removeTab(index);
}
