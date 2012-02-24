#ifndef AMCONTROLINFOLISTTABLEVIEW_H
#define AMCONTROLINFOLISTTABLEVIEW_H

#include <QWidget>

class AMControlInfoList;
class QTableWidget;

/// This widget provides a simple table to display the name/description, value, and units of all controls in an AMControlInfoList. Because AMControlInfoList is a static, unchanging list, there is no event-driven updating of the view when the values change; simply call setFromInfoList() to update the view with the contents of any AMControlInfoList.
class AMControlInfoListTableView : public QWidget
{
    Q_OBJECT
public:
    /// Constructor.  The widget does not retain the reference to \c list, but simply uses it to initialize the table. If \c list is 0, the table is left empty.
    AMControlInfoListTableView(const AMControlInfoList* list = 0, QWidget *parent = 0);

    /// Update the view based on the contents of \c list. If \c list is 0, the table is cleared.
    void setFromInfoList(const AMControlInfoList* list);

signals:

public slots:

protected:
    QTableWidget* table_;


};

#endif // AMCONTROLINFOLISTTABLEVIEW_H
