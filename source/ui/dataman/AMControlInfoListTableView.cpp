#include "AMControlInfoListTableView.h"

#include <QTableWidget>
#include <QBoxLayout>

#include "dataman/info/AMControlInfoList.h"

AMControlInfoListTableView::AMControlInfoListTableView(const AMControlInfoList* list, QWidget *parent) :
    QWidget(parent)
{
    // setup UI:
    QVBoxLayout* vl = new QVBoxLayout(this);
    table_ = new QTableWidget();
    table_->setColumnCount(3);
    table_->setRowCount(0);
    table_->setHorizontalHeaderLabels(QStringList() << "Control" << "Position" << "Units");
    vl->addWidget(table_);

    // Populate the table
    setFromInfoList(list);
}

void AMControlInfoListTableView::setFromInfoList(const AMControlInfoList* list)
{
    table_->setRowCount(0);

    if(list) {
        table_->setRowCount(list->count());

        for(int i=0,cc=list->count(); i<cc; i++) {
            const AMControlInfo& info = list->at(i);

            QTableWidgetItem* item = new QTableWidgetItem(info.description().isEmpty() ? info.name() : info.description());
            table_->setItem(i, 0, item);

            item = new QTableWidgetItem(QString::number(info.value()));
            table_->setItem(i, 1, item);

            item = new QTableWidgetItem(info.units());
            table_->setItem(i, 2, item);
        }
    }
}
