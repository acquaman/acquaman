#ifndef AMSLIT_H
#define AMSLIT_H

#include "beamline/AMControl.h"
#include "beamline/AMControlSet.h"

class AMSlit : public AMControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMSlit(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMSlit();

	/// Returns true if connected, false otherwise.
	bool isConnected() const;

	/// Returns the current gap value.
	double gapValue() const;
	/// Returns the current center value.
	double centerValue() const;

	/// Returns the gap control.
	AMControl* gap() const { return gap_; }
	/// Returns the center control.
	AMControl* center() const { return center_; }

signals:
	/// Notifier that the gap control value has changed.
	void gapValueChanged(double newValue);
	/// Notifier that the gap control has changed.
	void gapChanged(AMControl *newControl);
	/// Notifier that the center control value has changed.
	void centerValueChanged(double newValue);
	/// Notifier that the center control has changed.
	void centerChanged(AMControl *newControl);

public slots:
	/// Sets the gap control.
	void setGapControl(AMControl *gap);
	/// Sets the center control.
	void setCenterControl(AMControl *center);

protected slots:

protected:
	/// The gap control.
	AMControl *gap_;
	/// The center control.
	AMControl *center_;
};

#endif // AMSLIT_H
