#include "node.h"

CNode::CNode()
{
}


CNode::SNodeInfo CNode::getNodeInfo() const
{
    return m_oNodeInfo;
}

void CNode::setNodeInfo(CNode::SNodeInfo oNodeInfo)
{
    m_oNodeInfo = oNodeInfo;
}


void CNode::setDronTraversed(bool bTraversed)
{
    m_bIsRed = bTraversed;
}

void CNode::setColor(int c)
{
    if(finishNode) return;
    if(c == EColor::Blue)
    {
        if(m_nCount == 1)
        {
            m_oColor =EColor::Green;
            m_nCount++;
            return;
        }
        if(m_nCount == 2)
        {
            m_oColor = EColor::Gray;
            m_nCount++;
            return;
        }
        if(m_nCount > 2)
        {
            m_oColor = EColor::DarkCyan;
            m_nCount++;
            return;
        }
        m_nCount++;
    }
    m_oColor = c;
}

int CNode::getColor()
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
}
