#ifndef NODE_H
#define NODE_H

#include <QVector>
class CNode
{
public:
    CNode();
    enum EColor { Yellow, Red, Blue, Green, Gray, DarkCyan };
    enum EArrow { Left, Up, Right, Down, NoDir };

    struct SNodeInfo
    {
        int nID = 0;
        int nX  = 0;
        int nY  = 0;
        bool bIsSelected = false;
        EArrow eDir = EArrow::NoDir;
    };

    SNodeInfo getNodeInfo() const;
    void setNodeInfo(SNodeInfo oNodeInfo);

    int getOffset() const;
    EArrow getArrowDir() const;
    void setArrowDir(EArrow eDir);
    void setDronTraversed(bool bTraversed);
    void setColor(int c);
    int getColor();
    size_t m_nCount = 0;
    bool finishNode = false;
    QVector<EArrow> m_vecPosibleDirs;
    size_t          m_nCurrDirId = 0;

private:
    int m_oColor   = EColor::Yellow;
    bool m_bIsRed  = false;
    bool m_bIsBlue = false;
    EArrow m_eCurrDirection = EArrow::NoDir;
    SNodeInfo m_oNodeInfo;
};

#endif // NODE_H
