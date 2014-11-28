#ifndef WATERVALVEVIEW_H
#define WATERVALVEVIEW_H

#include <QWidget>
#include <QLabel>
#include <QIcon>
#include "WaterValve.h"

class WaterValveView : public QWidget
{
    Q_OBJECT
public:
    explicit WaterValveView(WaterValve *beamline, QWidget *parent = 0);



signals:



public slots:
    void updatePixmap(bool value);



protected:
    QLabel *name_;
    QLabel *waterLight_;

    WaterValve *model_;



};

#endif // WATERVALVEVIEW_H
