#include "mainwindow.h"

#include <QIcon>
#include <QLabel>
#include <QToolBar>
#include <QBoxLayout>
#include <QMessageBox>

#include <QImageReader>
#include <QImageWriter>
#include <QStandardPaths>

#include <QDebug>
#include <chrono>
#include <thread>
#include <iostream>

#include "node.h"

enum EColor { Yellow, Red, Blue, Green, Gray, DarkCyan };

CMainWindow::CMainWindow(QWidget *pParent)
    : QMainWindow(pParent)
{
    SetupUi();
}

void CMainWindow::onDroneSelected(const CNode::SNodeInfo &oNodeInfo)
{
   qDebug() << "Drone Selected";
   m_pwGraph->setDroneColor(oNodeInfo.nX, oNodeInfo.nY, QColor(Qt::red));
   m_oClient.sendMessage("select, " + QString::number(oNodeInfo.nX) + "," + QString::number(oNodeInfo.nY) + ',');
}

void CMainWindow::onFirst()
{
    m_isFirst = true;
}

void CMainWindow::onUpdateNode(int nRow, int nCol, int nColor, int nDirection)
{   //Left, Up, Right, Down
    switch (nDirection)
    {
    case 0:
        m_pwGraph->setDroneArrowDir(nRow, nCol, CNode::EArrow::Left);
    break;
    case 1:
        m_pwGraph->setDroneArrowDir(nRow, nCol, CNode::EArrow::Up);
    break;
    case 2:
        m_pwGraph->setDroneArrowDir(nRow, nCol, CNode::EArrow::Right);
    break;
    case 3:
        m_pwGraph->setDroneArrowDir(nRow, nCol, CNode::EArrow::Down);
    break;
    default:
        // m_pwGraph->setDroneArrowDir(nRow, nCol, CNode::EArrow::NoDir);
        break;
    }

    switch (nColor)
    {
    case 0:
        m_pwGraph->setDroneColor(nRow, nCol, QColor(Qt::yellow));
    break;
    case 1:
        m_pwGraph->setDroneColor(nRow, nCol, QColor(Qt::red));
    break;
    case 2:
        m_pwGraph->setDroneColor(nRow, nCol, QColor(Qt::blue));
    break;
    case 3:
        m_pwGraph->setDroneColor(nRow, nCol, QColor(Qt::green));
    break;
    case 4:
        m_pwGraph->setDroneColor(nRow, nCol, QColor(Qt::gray));
    break;
    case 5:
        m_pwGraph->setDroneColor(nRow, nCol, QColor(Qt::darkCyan));
    break;
    default:
        break;
    }
}

void CMainWindow::onMsg(const QString &sMsg, bool bEnableStart)
{
    (void)QMessageBox::information(nullptr, "Drone View", sMsg, QMessageBox::Close);
    m_pactStart->setEnabled(bEnableStart);
}

void CMainWindow::SetupUi()
{
    setToolTip("SandPile");
    setWindowTitle("SandPile");
    setWindowIcon(QIcon("://Icons/sandPileLogo.png"));

    m_pwGraph = new CGraphWidget();
    connect(m_pwGraph, &CGraphWidget::sigDroneSelected, this, &CMainWindow::onDroneSelected);

    setCentralWidget(m_pwGraph);
    m_pwGraph->init("");

    QToolBar* pToolBar = addToolBar("Main ToolBar");

    // Clear cycles
    m_pactClearCycles = new QAction(QIcon(QPixmap("://Icons/keys.svg")), tr("Clear cycles"));
    // m_pactGenerateKey->setDisabled(true);
    pToolBar->addAction(m_pactClearCycles);
    connect(m_pactClearCycles, &QAction::triggered, [=]() {
        m_oClient.sendMessage("clear,");
    });

    // Start
    m_pactStart = new QAction(QIcon(QPixmap("://Icons/encode.svg")), tr("Start"));
    pToolBar->addAction(m_pactStart);
    m_pactStart->setDisabled(true);
    connect(m_pactStart, &QAction::triggered, [=]() {
        m_oClient.sendMessage("start,");
    });

    pToolBar->addSeparator();

    // Message Box
    m_pwMessageBox = new QLabel();
    m_pwMessageBox->setAlignment(Qt::AlignCenter);
    m_pwMessageBox->setStyleSheet("color:red; background-color:yellow; border: 2px solid black; padding: 3px; font-weight:bold");
    m_pwMessageBox->setText(tr("Ready."));
    pToolBar->addWidget(m_pwMessageBox);

    connect(&m_oClient, &CClient::sigFirst, this, &CMainWindow::onFirst);
    connect(&m_oClient, &CClient::sigMsg, this, &CMainWindow::onMsg);
    connect(&m_oClient, &CClient::sigUpdateNode, this, &CMainWindow::onUpdateNode);

    connectToserver();
}

void CMainWindow::connectToserver()
{
    m_oClient.connectToServer();
    std::this_thread::sleep_for(std::chrono::microseconds(1000));
}
