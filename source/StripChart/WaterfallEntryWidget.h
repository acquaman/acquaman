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

protected:
    QCheckBox *waterfallCheckBox_;

protected slots:
    void setWaterfallCheckState(bool on);

};

#endif // WATERFALLENTRYWIDGET_H
