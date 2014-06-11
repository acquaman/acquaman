#ifndef WATERFALLENTRYWIDGET_H
#define WATERFALLENTRYWIDGET_H

#include <QWidget>
#include <QtGui>


class WaterfallEntryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WaterfallEntryWidget(QWidget *parent = 0);

signals:
    void waterfallOn(bool on);

public slots:
    void setWaterfallCheckState(bool on);

private:
    QCheckBox *waterfallCheckBox_;

};

#endif // WATERFALLENTRYWIDGET_H
