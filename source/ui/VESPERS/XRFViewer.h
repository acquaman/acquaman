#ifndef XRFVIEWER_H
#define XRFVIEWER_H

#include <QWidget>

class XRFViewer : public QWidget
{
	Q_OBJECT
public:
	/// Builds the viewer.  The general layout, control buttons and information is built here.
	explicit XRFViewer(QWidget *parent = 0);

signals:

public slots:

};

#endif // XRFVIEWER_H
