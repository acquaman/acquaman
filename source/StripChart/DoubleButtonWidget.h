#ifndef DOUBLEBUTTONWIDGET_H
#define DOUBLEBUTTONWIDGET_H

#include <QWidget>
#include <QtGui>

class DoubleButtonWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DoubleButtonWidget(QWidget *parent = 0);
    ~DoubleButtonWidget();

signals:
    void leftButtonClicked();
    void rightButtonClicked();

protected:
    QPushButton *leftButton_;
    QPushButton *rightButton_;

public:
    void setLeftButtonText(const QString &text);
    void setLeftButtonDefault(bool isDefault);
    void setRightButtonText(const QString &text);
    void setRightButtonDefault(bool isDefault);

    // here is a change so I can commit something!
};

#endif // DOUBLEBUTTONWIDGET_H
