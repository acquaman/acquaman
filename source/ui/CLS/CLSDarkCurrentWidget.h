#ifndef CLSDARKCURRENTWIDGET_H
#define CLSDARKCURRENTWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QDebug>

class CLSDarkCurrentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CLSDarkCurrentWidget(double initialDwellTime, QWidget *parent = 0);
    ~CLSDarkCurrentWidget();

signals:
    void darkCurrentButtonClicked(double dwellTime);

public slots:
    void disableDarkCurrentButton();
    void enableDarkCurrentButton();
    void setDarkCurrentButtonEnabled(bool isEnabled);

protected slots:
    void onDwellTimeEntryChanged(const QString &entryText);
    void onDarkCurrentButtonClicked();

protected:
    double dwellTime_;
    QLineEdit* dwellTimeEntry_;
    QPushButton* darkCurrentButton_;

};

#endif // CLSDARKCURRENTWIDGET_H
