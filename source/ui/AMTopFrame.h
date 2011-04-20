#ifndef AMTOPFRAME_H
#define AMTOPFRAME_H

#include <QFrame>

class QLabel;
class QHBoxLayout;

class AMTopFrame : public QFrame{
	Q_OBJECT
public:
	AMTopFrame(const QString &title, QWidget *parent = 0);

	QHBoxLayout* frameLayout();

public slots:
	void setTitle(const QString &title);
	void setIcon(const QIcon &icon);

protected:
	QLabel *iconLabel_;
	QLabel *titleLabel_;
	QFrame *separator_;
	QHBoxLayout *hl_;
};

#endif // AMTOPFRAME_H
