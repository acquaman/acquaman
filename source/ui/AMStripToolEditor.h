#ifndef AMSTRIPTOOLEDITOR_H
#define AMSTRIPTOOLEDITOR_H

#include "dataman/AMScan.h"

#include "ui/AMStripToolView.h"
#include "ui/dataman/AMScanView.h"
#include "ui/dataman/AMDataSourcesEditor.h"

class AMStripToolEditor : public AMStripToolView
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMStripToolEditor(QWidget *parent = 0);
	/// Destructor.
	virtual ~AMStripToolEditor();

signals:

public slots:

protected:
	/// The striptool being viewed.
	AMStripTool *stripTool_;
	/// The striptool 'scan'.
	AMScan *scan_;
	/// The scan view used to visualize the strip tool data.
	AMScanView *stripToolView_;
	/// The editor used to editor the strip tool sources.
	AMDataSourcesEditor *sourcesEditor_;
};

#endif // AMSTRIPTOOLEDITOR_H
