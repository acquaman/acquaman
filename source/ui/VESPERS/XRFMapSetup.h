#ifndef XRFMAPSETUP_H
#define XRFMAPSETUP_H

#include <QWidget>
#include <QRadioButton>
#include <QCheckBox>

/*!
	This widget sets up the arguments to boot up the NDMapper software without users having to go through the terminal application.
  */
class XRFMapSetup : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Builds the widget.
	explicit XRFMapSetup(QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles launching the program with the given arguments.
	void launchNDMapper();

protected:
	// Member variables.
	QRadioButton *dim2D_;
	QRadioButton *dim3D_;

	QRadioButton *vortex1El_;
	QRadioButton *vortex4El_;

	QCheckBox *ccd_;
	QCheckBox *multipleImages_;

};

#endif // XRFMAPSETUP_H
