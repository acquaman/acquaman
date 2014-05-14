#ifndef STPLOTVIEW_H
#define STPLOTVIEW_H

#include <QObject>

#include "STModel.h"
#include "MPlot/MPlot.h"

class STPlotView : public QObject
{
    Q_OBJECT

public:
    /// Constructor.
    explicit STPlotView(STModel *model, QObject *parent = 0);
    /// Destructor.
    ~STPlotView();

    /// Returns the current model.
    STModel* model() const;

signals:
    /// Emitted when the model is set.
    void modelChanged();

public slots:
    void setModel(STModel *newModel);

protected:
    STModel *model_;

};

#endif // STPLOTVIEW_H
