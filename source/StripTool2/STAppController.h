#ifndef STAPPCONTROLLER_H
#define STAPPCONTROLLER_H

#include <QObject>

#include "STModel.h"
#include "STPlotView.h"

class STAppController : public QObject
{
    Q_OBJECT
public:
    /// Constructor.
    explicit STAppController(QObject *parent = 0);
    /// Destructor.
    ~STAppController();

    /// Create and setup StripTool widgets, connections, and data objects. Returns true on success.
    bool startup();

    /// Returns true when the application is running normally.
    bool isRunning();

    /// Destroy all StripTool widgets, connections, and data objects.
    void shutdown();

protected:
    /// Sets up the user interface.
    void setupUserInterface();

protected:
    bool isStarting_;
    bool isShuttingDown_;

    STModel *model_;

//    STPlotView *plotView_;

};

#endif // STAPPCONTROLLER_H
