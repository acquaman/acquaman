#ifndef GRAPHICSTEXTITEM_H
#define GRAPHICSTEXTITEM_H
#include <QGraphicsTextItem>

class QTextDocument;

class GraphicsTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    GraphicsTextItem(QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);

    void setShapeIndex(int index);
    int shapeIndex();

signals:
    void textChanged(int);

protected slots:
    void changingText();


protected:
    QTextDocument* document_;

    int shapeIndex_;
};

#endif // GRAPHICSTEXTITEM_H
