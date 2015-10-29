#ifndef AMVALIDATORICON_H
#define AMVALIDATORICON_H

#include <QWidget>
#include <QLabel>
class AMValidator;
/*!
  * A widget class which is used to notify users of the status of a validator.
  * When the provided validator is fails, the widget will display the provided
  * icon, with a tooltip message describing the failure messages.
  */
class AMValidatorIcon : public QWidget
{
    Q_OBJECT
public:
	/*!
	  * Creates a validator icon which will visualize the provided validator.
	  */
	AMValidatorIcon(AMValidator* validator,
					const QString& iconPath,
					QWidget *parent = 0);

signals:

public slots:
protected slots:
	/// Slot which handles the fail count of the validator changing.
	void onFailCountChanged(int);
protected:
	QLabel* iconLabel_;
	AMValidator* validator_;
};

#endif // AMVALIDATORICON_H
