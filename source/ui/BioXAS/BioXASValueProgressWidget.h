#ifndef BIOXASVALUEPROGRESSWIDGET_H
#define BIOXASVALUEPROGRESSWIDGET_H

#include <QLabel>

class BioXASValueProgressWidget : public QLabel
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASValueProgressWidget(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASValueProgressWidget();

public slots:
	/// Sets the flag for whether progress is displayed.
	void setDisplayProgress(bool showProgress);
	/// Sets the progress value.
	void setValue(double newValue);
	/// Sets the progress value minimum.
	void setValueMinimum(double newValue);
	/// Sets the progress value maximum.
	void setValueMaximum(double newValue);

protected:
	/// Paints the widget. Reimplemented to provide custom text.
	virtual void paintEvent(QPaintEvent *event);

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

#endif // BIOXASVALUEPROGRESSWIDGET_H
