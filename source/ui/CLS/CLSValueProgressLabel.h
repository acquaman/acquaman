#ifndef CLSVALUEPROGRESSLABEL_H
#define CLSVALUEPROGRESSLABEL_H

#include <QLabel>

class CLSValueProgressLabel : public QLabel
{
	Q_OBJECT

public:
	/// Constructor.
	explicit CLSValueProgressLabel(QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSValueProgressLabel();

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

#endif // CLSVALUEPROGRESSLABEL_H
