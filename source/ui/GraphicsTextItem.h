#ifndef GRAPHICSTEXTITEM_H
#define GRAPHICSTEXTITEM_H
#include <QGraphicsTextItem>

class QTextDocument;

class GraphicsTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    enum {Type = UserType + 1};
    GraphicsTextItem(QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);

    void setShapeIndex(int index);
    int shapeIndex();

    int type() const;

signals:
    void textChanged(int);

protected slots:
    void changingText();


protected:
    QTextDocument* document_;

    int shapeIndex_;
};

#endif // GRAPHICSTEXTITEM_H
