#ifndef AMSTRIPTOOLVIEW_H
#define AMSTRIPTOOLVIEW_H

#include <QWidget>

#include "ui/AMStripTool.h"
#include "ui/dataman/AMScanView.h"
#include "ui/dataman/AMDataSourcesEditor.h"

class AMStripToolView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMStripToolView(QWidget *parent = 0);
	/// Destructor.
	virtual ~AMStripToolView();

	/// Returns the striptool being viewed.
	virtual AMStripTool* stripTool() const = 0;

signals:
	/// Notifier that the striptool being viewed has changed.
	void stripToolChanged(AMStripTool *newTool);

public slots:
	/// Sets the striptool being viewed.
	virtual void setStripTool(AMStripTool *newTool) = 0;

};

#endif // AMSTRIPTOOLVIEW_H
