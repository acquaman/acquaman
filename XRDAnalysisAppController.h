#ifndef XRDANALYSISAPPCONTROLLER_H
#define XRDANALYSISAPPCONTROLLER_H

#include <QObject>

/// Class that holds the guts of classes that do the work and manages the overall GUI.
class XRDAnalysisAppController : public QObject
{
	Q_OBJECT
public:
	/// Constructor.
	explicit XRDAnalysisAppController(QObject *parent = 0);

signals:

public slots:

};

#endif // XRDANALYSISAPPCONTROLLER_H
