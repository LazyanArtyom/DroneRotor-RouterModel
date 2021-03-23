#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>
#include <QVector>

class CGraphWidget;

class CNode : public QGraphicsItem
{
public:
    CNode(CGraphWidget *graphWidget);

    enum { Type = UserType + 1 };
    enum EArrow { Left, Up, Right, Down, NoDir };

    struct SNodeInfo
    {
        int nID = 0;
        int nX  = 0;
        int nY  = 0;
        bool bIsSelected = false;
        EArrow eDir = EArrow::NoDir;
    };

    int type() const override { return Type; }
    SNodeInfo getNodeInfo() const;
    void setNodeInfo(SNodeInfo oNodeInfo);

    bool advancePosition();

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    int getOffset() const;

    QColor getColor();
    EArrow getArrowDir() const;
    void setColor(QColor);
    void setArrowDir(EArrow eDir);

    void setDronTraversed(bool bTraversed);

    size_t m_nCount = 0;
    bool finishNode = false;
    QVector<EArrow> m_vecPosibleDirs;
    size_t          m_nCurrDirId = 0;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QPointF newPos;
    CGraphWidget *m_pwGraph = nullptr;
    QColor m_oColor = QColor(Qt::yellow);
    int m_nNodeOffset = 200;
    int m_nArrowSize  = 10;
    int m_nNodeSize   = 30;
    bool m_bIsRed     = false;
    bool m_bIsBlue    = false;
    EArrow m_eCurrDirection = EArrow::NoDir;
    SNodeInfo m_oNodeInfo;
};

#endif // NODE_H
