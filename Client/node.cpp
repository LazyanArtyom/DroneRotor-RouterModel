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

//#include "edge.h"
#include "node.h"
#include "graphwidget.h"

#include <cmath>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>

#include <chrono>
#include <thread>

CNode::CNode(CGraphWidget *graphWidget)
    : m_pwGraph(graphWidget)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
}

bool CNode::advancePosition()
{
    if (newPos == pos())
        return false;

    setPos(newPos);
    return true;
}


CNode::SNodeInfo CNode::getNodeInfo() const
{
    return m_oNodeInfo;
}

void CNode::setNodeInfo(CNode::SNodeInfo oNodeInfo)
{
    m_oNodeInfo = oNodeInfo;
}


QRectF CNode::boundingRect() const
{
    return QRectF( 0, 0, m_nNodeSize + 2 * m_nNodeOffset, m_nNodeSize + 2 * m_nNodeOffset);
}


QPainterPath CNode::shape() const
{
    QPainterPath path;
    path.addEllipse(0, 0, m_nNodeSize + 2 * m_nNodeOffset, m_nNodeSize + 2 * m_nNodeOffset);
    return path;
}


void CNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    QRadialGradient gradient(-3, -3, 10);
    if (option->state & QStyle::State_Sunken) {
        gradient.setCenter(3, 3);
        gradient.setFocalPoint(3, 3);
        gradient.setColorAt(0, m_oColor.lighter(120));
    } else {
        gradient.setColorAt(0, m_oColor);
    }
    painter->setBrush(gradient);

    int radius = (boundingRect().width() / 8) + 5;
    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(boundingRect().center(), radius, radius);

    QPointF center = boundingRect().center();
    QLineF line;

    switch (m_eCurrDirection)
    {
        case EArrow::Up:
            line.setP1(QPointF(center.x(), center.y() - radius));
            line.setP2(QPointF(center.x(), boundingRect().top()));
        break;
        case EArrow::Left:
            line.setP1(QPointF(center.x() - radius, center.y()));
            line.setP2(QPointF(boundingRect().left(), center.y()));
        break;
        case EArrow::Right:
            line.setP1(QPointF(center.x() + radius, center.y()));
            line.setP2(QPointF(boundingRect().right(), center.y()));
        break;
        case EArrow::Down:
            line.setP1(QPointF(center.x(), center.y() + radius));
            line.setP2(QPointF(center.x(), boundingRect().bottom()));
        break;

        default:
            return;
    }

    Qt::GlobalColor oCurrColor = Qt::black;
    if (m_bIsRed) oCurrColor = Qt::red;

    painter->setPen(QPen(oCurrColor, 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line);

    // Draw the arrows
    qreal arrowSize = 50;
    painter->setBrush(oCurrColor);
    double angle = std::atan2(-line.dy(), line.dx());

    QPointF destArrowP1 = line.p2() + QPointF(sin(angle - M_PI / 3) * arrowSize, cos(angle - M_PI / 3) * arrowSize);
    QPointF destArrowP2 = line.p2() + QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize, cos(angle - M_PI + M_PI / 3) * arrowSize);
    painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
}

int CNode::getOffset() const
{
    return m_nNodeOffset;
}

void CNode::setDronTraversed(bool bTraversed)
{
    m_bIsRed = bTraversed;
    update();
}

void CNode::setColor(QColor c)
{
    if(finishNode)
        return;
    if(c == QColor(Qt::blue))
    {
        if(m_nCount == 1)
        {
            m_oColor = QColor(Qt::green);
            update();
            m_nCount++;
            return;
        }
        if(m_nCount == 2)
        {
            m_oColor = QColor(Qt::gray);
            update();
            m_nCount++;
            return;
        }
        if(m_nCount > 2)
        {
            m_oColor = QColor(Qt::darkCyan);
            update();
            m_nCount++;
            return;
        }
        m_nCount++;
    }
    m_oColor = c;
    update();
}

QColor CNode::getColor()
{
    return m_oColor;
}

CNode::EArrow CNode::getArrowDir() const
{
    return m_eCurrDirection;
}

void CNode::setArrowDir(CNode::EArrow eDir)
{
    m_eCurrDirection = eDir;
    update();
}

QVariant CNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionHasChanged:
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void CNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

void CNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (0 == m_nCount && !m_oNodeInfo.bIsSelected)
    {
        m_oColor = Qt::red;
        m_oNodeInfo.bIsSelected = true;
        m_oNodeInfo.eDir = getArrowDir();
        m_pwGraph->sigDroneSelected(m_oNodeInfo);
        qDebug() << "drone selected from release event";
    }
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
