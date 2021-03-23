#ifndef CLOGIC_H
#define CLOGIC_H

#include <map>

#include <vector>
#include <set>
#include <QTimer>
#include <tuple>
#include "node.h"

class CLogic
{
public:
    CLogic(size_t nColNum, size_t nRowNum);
    ~CLogic();
    void setDrone(size_t nI, size_t nJ);
    bool clearCicles();
    bool activateAlgo();
    QString getStr();
    size_t nDronesCount = 0;

    ////////only for cleareCicles
    size_t startI = 4, startJ = 0;
    size_t currI = 4, currJ = 0;
    std::set<size_t> forCount;
private:

    std::vector<std::tuple<size_t, size_t, CNode::EArrow>> m_arrStartPts;
    std::vector<std::tuple<size_t, size_t, CNode::EArrow>> m_arrCurrPts;
    std::vector<std::pair<int, int>> m_arrChangedNodes;
    std::vector<bool> m_arrFreePoints;
    std::vector<bool> m_arrFinishedDrones;
    size_t m_nColNum;
    size_t m_nRowNum;
    std::vector<std::vector<CNode*>> m_vecNodes;
    QTimer* m_pSystemTimer = nullptr;
    bool cleared = false;
    bool first = false;
    size_t m_nDronesCount = 0;
};

#endif // CLOGIC_H
