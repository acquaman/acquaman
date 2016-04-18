#ifndef BIOXASVALUEPROGRESSLABEL_H
#define BIOXASVALUEPROGRESSLABEL_H

#include <QLabel>

class BioXASValueProgressLabel : public QLabel
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASValueProgressLabel(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASValueProgressLabel();

public slots:
	/// Sets the flag for whether progress is displayed.
	void setDisplayProgress(bool showProgress);
	/// Sets the progress value.
	void setProgressValue(double newValue);
	/// Sets the progress value minimum.
	void setProgressValueMinimum(double newValue);
	/// Sets the progress value maximum.
	void setProgressValueMaximum(double newValue);

protected:
	/// Paints the widget. Reimplemented to provide custom text.
	virtual void paintEvent(QPaintEvent *event);

	/// Returns the decimal progress percentage.
	virtual double progressPercentage(double min, double value, double max) const;

	/// Returns the absolute value of the given double. Added because std::abs requires an int argument.
	double abs(double arg) const;

protected:
	/// Flag indicating whether to display progress.
	bool displayProgress_;
	/// The progress value.
	double value_;
	/// The progress value minimum.
	double valueMin_;
	/// The progress value maximum.
	double valueMax_;
};

#endif // BIOXASVALUEPROGRESSLABEL_H
