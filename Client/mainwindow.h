#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QFileDialog>
#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsScene>

#include "graphwidget.h"

#include "tcpclient.h"

class CMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    CMainWindow(QWidget *pParent = nullptr);
    ~CMainWindow() = default;

public:

public slots:
    // slots from client
    void onDroneSelected(const CNode::SNodeInfo& oNodeInfo);
    void onFirst();
    void onUpdateNode(int, int, int, int);
    void onMsg(const QString& sMsg, bool bEnableStart);

private:
    void SetupUi();
    void connectToserver();

private:
    CClient         m_oClient;
    QLabel*         m_pwMessageBox      = nullptr;

    bool     m_isFirst         = false;
    QAction* m_pactStart       = nullptr;
    QAction* m_pactClearCycles = nullptr;
    CGraphWidget* m_pwGraph    = nullptr;
};
#endif // MAINWINDOW_H
