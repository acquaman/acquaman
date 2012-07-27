#ifndef AMTOPFRAME2_H
#define AMTOPFRAME2_H

#include <QWidget>
#include <QIcon>

namespace Ui {
    class AMTopFrame2;
}

/// Standard header bar widget for main window panes
class AMTopFrame2 : public QWidget
{
    Q_OBJECT

public:
	explicit AMTopFrame2(const QString& title, const QIcon& icon = QIcon(), QWidget *parent = 0);
    ~AMTopFrame2();

	QString title() const;
	QString leftSubText() const;
	QString rightSubText() const;

public slots:
	void setTitle(const QString& title);
	void setIcon(const QIcon& icon);
	void setLeftSubText(const QString& subText);
	void setRightSubText(const QString& subText);

private:
    Ui::AMTopFrame2 *ui;
};

#endif // AMTOPFRAME2_H
