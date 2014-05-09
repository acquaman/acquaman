#ifndef AMSCANSEARCHVIEW_H
#define AMSCANSEARCHVIEW_H

#include <QWidget>

/// A class representing a Dialog Window for searching scan's for:
///		Sample name
///		Sample date
///		Scan Exit Slit Value
///		Scan Grating Value
///		Scan Harmonic Value
class AMScanSearchView : public QWidget
{
	Q_OBJECT
public:
	explicit AMScanSearchView(QWidget *parent = 0);


protected:
	/// Initialises all the needed settings for the window (constructs Widgets, sets title etc.)
	void initDialog();
signals:
	
public slots:
	
};

#endif // AMSCANSEARCHVIEW_H
