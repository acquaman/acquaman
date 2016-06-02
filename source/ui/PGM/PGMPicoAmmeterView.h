#ifndef PGMPICOAMMETERVIEW_H
#define PGMPICOAMMETERVIEW_H

#include <QWidget>

#include "beamline/AMPVControl.h"

#include <QLabel>

/// This is a simple view that takes the read only control of the picoammeter and provides a simple display.
class PGMPicoAmmeterView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes the picoammeter control.
	explicit PGMPicoAmmeterView(AMReadOnlyPVControl *control, QWidget *parent = 0);
	/// Destructor.
	~PGMPicoAmmeterView(){}

signals:

public slots:

protected slots:
	/// Updates the value label when the value or units change.
	void onValueChanged();

protected:
	/// The control we are connected to.
	AMReadOnlyPVControl *control_;

	/// The description label.
	QLabel *description_;
	/// The value label.
	QLabel *value_;
};

#endif // PGMPICOAMMETERVIEW_H
