/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "graphwidget.h"
//#include "edge.h"
#include "node.h"

#include <set>
#include <math.h>
#include <thread>
#include <chrono>

#include <QDebug>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QGraphicsLineItem>
#include <QPen>
#include <QCoreApplication>


CGraphWidget::CGraphWidget(QWidget *pParent)
    : QGraphicsView(pParent)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    //scene->setSceneRect(-200, -200, 400, 400);
    setScene(scene);
    setCacheMode(CacheBackground);
    // setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setMinimumSize(400, 400);
    setWindowTitle(tr("Elastic Nodes"));

    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setCacheMode(QGraphicsView::CacheNone);
    setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing | QGraphicsView::IndirectPainting);
}

QColor CGraphWidget::getDroneColor(int i, int j)
{
   return m_vecNodes[i][j]->getColor();
}

CNode::EArrow CGraphWidget::getDroneArrowDir(int i, int j) const
{
   return m_vecNodes[i][j]->getArrowDir();
}

void CGraphWidget::setDroneColor(int i, int j, QColor oColor)
{
   return m_vecNodes[i][j]->setColor(oColor);
}

void CGraphWidget::setDroneArrowDir(int i, int j, CNode::EArrow eDir)
{
    return m_vecNodes[i][j]->setArrowDir(eDir);
}

void CGraphWidget::init(QString sData)
{
    int nSize = 10;
    int nX = 0, nY = 0;
    int nOffset;

    m_vecNodes.resize(nSize);
    for (int i = 0; i < nSize; ++i)
    {
        nY = 0;
        m_vecNodes[i].resize(nSize);
        for (int j = 0; j < nSize; ++j)
        {
            m_vecNodes[i][j] = new CNode(this);
            CNode::SNodeInfo oNodeInfo;// = m_vecNodes[i][j]->getNodeInfo();
            oNodeInfo.nX = i;
            oNodeInfo.nY = j;
            oNodeInfo.nID = j * m_vecNodes[i].size() + i;
            m_vecNodes[i][j]->setNodeInfo(oNodeInfo);
            scene()->addItem(m_vecNodes[i][j]);

            m_vecNodes[i][j]->setPos(nX, nY);
            nOffset = m_vecNodes[i][j]->boundingRect().size().width();

            nY += nOffset;
        }
        nX += nOffset;
    }

}

void CGraphWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        for (auto vecNode : m_vecNodes)
        {
            for (auto node : vecNode)
            {
                node->setArrowDir(CNode::EArrow::Up);
                node->setColor(QColor(Qt::yellow));
            }
        }
        break;
    case Qt::Key_Down:
        for (auto vecNode : m_vecNodes)
        {
            for (auto node : vecNode)
            {
                node->setArrowDir(CNode::EArrow::Down);
                node->setColor(QColor(Qt::yellow));
            }
        }
        break;
    case Qt::Key_Left:
        for (auto vecNode : m_vecNodes)
        {
            for (auto node : vecNode)
            {
                node->setArrowDir(CNode::EArrow::Left);
                node->setColor(QColor(Qt::yellow));
            }
        }
        break;
    case Qt::Key_Right:
        for (auto vecNode : m_vecNodes)
        {
            for (auto node : vecNode)
            {
                node->setArrowDir(CNode::EArrow::Right);
                node->setColor(QColor(Qt::yellow));
            }
        }
        break;
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}

#if QT_CONFIG(wheelevent)
void CGraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow(2., -event->angleDelta().y() / 240.0));
}
#endif

void CGraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
        painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
        painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);
}

void CGraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

void CGraphWidget::shuffle()
{
    const QList<QGraphicsItem *> items = scene()->items();
    for (QGraphicsItem *item : items)
    {
        if (qgraphicsitem_cast<CNode *>(item))
            item->setPos(-150 + QRandomGenerator::global()->bounded(300), -150 + QRandomGenerator::global()->bounded(300));
    }
}

void CGraphWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void CGraphWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}

void CGraphWidget::gosip(CNode* pNode1, CNode* pNode2, QColor oLineColor)
{
    QPen* pPen = new QPen();
    pPen->setStyle(Qt::DashDotLine);
    pPen->setWidth(15);
    pPen->setBrush(oLineColor);
    pPen->setCapStyle(Qt::RoundCap);
    pPen->setJoinStyle(Qt::RoundJoin);
    QGraphicsLineItem* pLineItem = new QGraphicsLineItem(QLineF(pNode1->mapToScene(pNode1->boundingRect().center()), pNode2->mapToScene(pNode2->boundingRect().center())));
    pLineItem->setPen(*pPen);
    scene()->addItem(pLineItem);
    m_oGosipLinesVec.push_back(pLineItem);
}

void CGraphWidget::clearGosipLines()
{
    for(auto& pItem : m_oGosipLinesVec)
    {
        scene()->removeItem(pItem);
        delete pItem;
        pItem = nullptr;
    }
    m_oGosipLinesVec.clear();
}
