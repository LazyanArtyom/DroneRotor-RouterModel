#include "logic.h"
#include <QDebug>

#include <QRandomGenerator>

#include <assert.h>
#include <iostream>
#include <math.h>

CLogic::CLogic(size_t nColNum, size_t nRowNum)
    : m_nColNum(nColNum), m_nRowNum(nRowNum)
{
    m_vecNodes.resize(nColNum);
    for (size_t i = 0; i < nColNum; ++i)
    {
        m_vecNodes[i].resize(nRowNum);
        for (size_t j = 0; j < nRowNum; ++j)
        {
            m_vecNodes[i][j] = new CNode();
            CNode::SNodeInfo oNodeInfo = m_vecNodes[i][j]->getNodeInfo();
            oNodeInfo.nX = i;
            oNodeInfo.nY = j;
            oNodeInfo.nID = j * m_vecNodes[i].size() + i;
            m_vecNodes[i][j]->setNodeInfo(oNodeInfo);
        }
    }

    //////////////init//////////////////
    for (size_t i = 0; i < m_vecNodes.size(); ++i)
    {
        for (size_t j = 0; j < m_vecNodes[i].size(); ++j)
        {
            if(i != 0)
                m_vecNodes[i][j]->m_vecPosibleDirs.push_back(CNode::EArrow::Left);
            if(j != 0)
                m_vecNodes[i][j]->m_vecPosibleDirs.push_back(CNode::EArrow::Up);
            if(i != (m_vecNodes.size() - 1))
                m_vecNodes[i][j]->m_vecPosibleDirs.push_back(CNode::EArrow::Right);
            if(j != (m_vecNodes[i].size() - 1))
                m_vecNodes[i][j]->m_vecPosibleDirs.push_back(CNode::EArrow::Down);

            if(i == 0 && j != 0)
            {
                m_vecNodes[i][j]->setArrowDir(CNode::EArrow::Up);
                m_vecNodes[i][j]->m_nCurrDirId = 0;
            }
            else if(j == 0 && i != (m_vecNodes.size() - 1))
            {
                m_vecNodes[i][j]->setArrowDir(CNode::EArrow::Right);
                if(i == 0)
                {
                    m_vecNodes[i][j]->m_nCurrDirId = 0;
                }
                else
                {
                    m_vecNodes[i][j]->m_nCurrDirId = 1;
                }
            }
            else if(i == (m_vecNodes.size() - 1) && j != (m_vecNodes[i].size() - 1))
            {
                m_vecNodes[i][j]->setArrowDir(CNode::EArrow::Down);
                if(j == 0)
                {
                    m_vecNodes[i][j]->m_nCurrDirId = 1;
                }
                else
                {
                    m_vecNodes[i][j]->m_nCurrDirId = 2;
                }
            }
            else if(j == (m_vecNodes[i].size() - 1))
            {
                m_vecNodes[i][j]->setArrowDir(CNode::EArrow::Left);
                m_vecNodes[i][j]->m_nCurrDirId = 0;
            }
            else
            {
                auto dir = QRandomGenerator::global()->bounded(0, m_vecNodes[i][j]->m_vecPosibleDirs.size());
                m_vecNodes[i][j]->m_nCurrDirId = dir;
                m_vecNodes[i][j]->setArrowDir(m_vecNodes[i][j]->m_vecPosibleDirs[dir]);
            }

            m_vecNodes[i][j]->setColor(CNode::EColor::Yellow);
            //m_vecNodes[i][j]->m_nCurrDir = dir;
        }
    }
    m_vecNodes[4][0]->setColor(CNode::EColor::Red);

}

CLogic::~CLogic()
{
    for (size_t i = 0; i < m_nColNum; ++i)
    {
        for (size_t j = 0; j < m_nRowNum; ++j)
        {
            if(m_vecNodes[i][j])
                delete m_vecNodes[i][j];
        }
    }
}

bool CLogic::clearCicles()
{
    if(cleared) return false;

    m_vecNodes[currI][currJ]->m_nCurrDirId = (m_vecNodes[currI][currJ]->m_nCurrDirId + 1) % m_vecNodes[currI][currJ]->m_vecPosibleDirs.size();
    m_vecNodes[currI][currJ]->setColor(CNode::EColor::Yellow);
    m_vecNodes[currI][currJ]->setArrowDir(m_vecNodes[currI][currJ]->m_vecPosibleDirs[m_vecNodes[currI][currJ]->m_nCurrDirId]);
    switch (m_vecNodes[currI][currJ]->m_vecPosibleDirs[m_vecNodes[currI][currJ]->m_nCurrDirId])
    {
        case CNode::EArrow::Left:
            currI--;
            break;
        case CNode::EArrow::Up:
            currJ--;
            break;
        case CNode::EArrow::Right:
            currI++;
            break;
        case CNode::EArrow::Down:
            currJ++;
            break;
        default:
            break;
    }
    m_vecNodes[currI][currJ]->setColor(CNode::EColor::Red);
    forCount.emplace(currJ * m_vecNodes[currI].size() + currI);

    if (startI == currI &&
        startJ == currJ &&
        m_vecNodes[currI][currJ]->getArrowDir() == CNode::EArrow::Right &&
        (int)m_vecNodes.size() * (int)m_vecNodes[0].size() == (int)forCount.size())
    {
        m_vecNodes[currI][currJ]->setColor(CNode::EColor::Yellow);
        cleared = true;
        first = true;
        return true;
    }
    return false;
}

bool CLogic::activateAlgo()
{
    for(size_t i = 0; i < m_nDronesCount; ++i)
    {
        if(!m_arrFinishedDrones[i])
        {
            size_t& currI1 = std::get<0>(m_arrCurrPts[i]);
            size_t& currJ1 = std::get<1>(m_arrCurrPts[i]);
            m_vecNodes[currI1][currJ1]->m_nCurrDirId = (m_vecNodes[currI1][currJ1]->m_nCurrDirId + 1) % m_vecNodes[currI1][currJ1]->m_vecPosibleDirs.size();
            m_vecNodes[currI1][currJ1]->setColor(CNode::EColor::Blue);
            m_vecNodes[currI1][currJ1]->setArrowDir(m_vecNodes[currI1][currJ1]->m_vecPosibleDirs[m_vecNodes[currI1][currJ1]->m_nCurrDirId]);
            switch (m_vecNodes[currI1][currJ1]->m_vecPosibleDirs[m_vecNodes[currI1][currJ1]->m_nCurrDirId])
            {
                case CNode::EArrow::Left:
                    currI1--;
                    break;
                case CNode::EArrow::Up:
                    currJ1--;
                    break;
                case CNode::EArrow::Right:
                    currI1++;
                    break;
                case CNode::EArrow::Down:
                    currJ1++;
                    break;
                default:
                    break;
            }
            m_vecNodes[currI1][currJ1]->setColor(CNode::EColor::Red);
            forCount.emplace(currJ1 * m_vecNodes[currI1].size() + currI1);
            for(size_t j = 0; j < m_nDronesCount; ++j)
            {
                m_arrFinishedDrones[i] = m_arrFinishedDrones[i] || (currI1 == std::get<0>(m_arrStartPts[j]) && currJ1 == std::get<1>(m_arrStartPts[j])
                                                                    && m_vecNodes[currI1][currJ1]->getArrowDir() == std::get<2>(m_arrStartPts[j])
                                                                    && !(m_vecNodes[currI1][currJ1]->finishNode));
                if(m_arrFinishedDrones[i])
                {
                    m_vecNodes[currI1][currJ1]->finishNode = true;
                    break;
                }
            }

        }
    }
    bool bOK = true;
    for (size_t i = 0; i < m_arrFinishedDrones.size(); ++i)
    {
        if (!m_arrFinishedDrones[i])
        {
            bOK = false;
            break;
        }
    }
    return bOK;
}

void CLogic::setDrone(size_t nI, size_t nJ)
{
    m_arrStartPts.push_back(std::make_tuple(nI, nJ, m_vecNodes[nI][nJ]->getArrowDir()));
    m_arrCurrPts.push_back(std::make_tuple(nI, nJ, m_vecNodes[nI][nJ]->getArrowDir()));
    m_vecNodes[nI][nJ] -> setColor(CNode::EColor::Red);
    m_arrFinishedDrones.push_back(false);
    m_arrFreePoints.push_back(true);
    m_nDronesCount++;
}


QString CLogic::getStr()
{
    QStringList strMsg;
    for (size_t i = 0; i < m_vecNodes.size() ; ++i)
    {
        for(size_t j = 0; j < m_vecNodes[i].size(); ++j)
        {
            strMsg << QString::number(i)
                   << QString::number(j)
                   << QString::number(m_vecNodes[i][j]->getColor())
                   << QString::number(m_vecNodes[i][j]->getArrowDir());
        }
    }
    return strMsg.join(",");
}
