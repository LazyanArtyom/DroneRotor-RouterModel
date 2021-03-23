#include "tcpserver.h"
#include "logic.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <map>

#define HOST_ADDRESS "127.0.0.1" // "185.127.66.104"
#define HOST_PORT 40004
#define MAX_SOCKETS (4)


std::map<QString, int> sRequestTypeMap = {
    std::make_pair(QString("clear"), 0),
    std::make_pair(QString("start"), 1),
    std::make_pair(QString("select"), 2),
};

CGameServer::CGameServer(QObject* parent):
    QTcpServer(parent)
{
    /// Setup timer and connect to host
    //
    m_pFlushTimer = new QTimer(this);
    m_pClearTimer = new QTimer(this);
    m_pAlgoTimer  = new QTimer(this);
    if (!listen(QHostAddress::Any, HOST_PORT))
    {
        std::cout << "Failed to listen" << std::endl;
        return;
    }
    std::cout << "Listening on " << serverAddress().toString().toStdString() << ":" << HOST_PORT << std::endl;
    m_pLogic = new CLogic(10,10);
    connect(this, &QTcpServer::newConnection, this, &CGameServer::onNewConnection);
    connect(m_pFlushTimer, &QTimer::timeout, this, &CGameServer::onFlushTimerTick);
    connect(m_pClearTimer, &QTimer::timeout, this, &CGameServer::onClearTimerTick);
    connect(m_pAlgoTimer, &QTimer::timeout, this, &CGameServer::onAlgoTimerTick);

    m_pFlushTimer->setInterval(1000);
    m_pFlushTimer->start();

    m_pClearTimer->setInterval(20);
    m_pAlgoTimer->setInterval(20);
}

void CGameServer::sendMessage(QString sMessage, int user)
{
    qDebug() << sMessage;

    QByteArray data = sMessage.toLatin1();
    int bytesRateCount = data.size() / 1000;
    int bytesRateRem = data.size() % 1000;

    int i = 0;
    for (; 0 != bytesRateCount; --bytesRateCount)
    {
        m_users[user]->write(data.mid(i, 1000));
        m_users[user]->waitForBytesWritten(5000);
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
        i += 1000;
    }

    QByteArray sData = "";
    if (0 != bytesRateRem)
    {
        sData = data.mid(i, bytesRateRem);
    }

    sData = sData.append(QByteArray("END"));
    m_users[user]->write(sData);
    m_users[user]->waitForBytesWritten(5000);
    std::this_thread::sleep_for(std::chrono::microseconds(2000));
}

void CGameServer::onNewConnection()
{
    if (m_oServerClients.size() >= m_nMaxClients)
    {
        std::cout << "Cannot form new connection. Server busy";

        QTcpSocket* pCurSocket = nextPendingConnection();
        pCurSocket->close();
        return;
    }

    QTcpSocket* pCurSocket = nextPendingConnection();
    connect(pCurSocket, &QTcpSocket::readyRead, this, &CGameServer::onReadyRead);
    connect(pCurSocket, &QTcpSocket::disconnected, this, &CGameServer::onDisconnected);

    qDebug() << "Client " << pCurSocket->socketDescriptor();
    m_oServerClients.insert(m_nUserCount, !bool(m_oServerClients.size()));
    m_users.insert(m_nUserCount, pCurSocket);
    m_oClientMessageBuffer[pCurSocket] = QByteArray();

    if (0 == m_nUserCount)
    {
        /// Send data to main clinet
        //
        sendMessage(QString("FIRST,")+ m_pLogic->getStr() + ",", m_nUserCount);
    }
    else
        sendMessage("DATA," + m_pLogic->getStr() + ",", m_nUserCount);
    ++m_nUserCount;

    return;
}

void CGameServer::onReadyRead()
{
    QTcpSocket* pSocket = (QTcpSocket *)sender();
    //
    /// IF client is inactive skip data processing
    /// Do not chagne this line
    //
    QByteArray sEndMessage = "END";
    int oCurrentClient = getCurrentUserID(pSocket);
    std::this_thread::sleep_for(std::chrono::microseconds(1000));
    QByteArray data = pSocket->readAll();
    pSocket->waitForBytesWritten(5000);
    if (!data.endsWith(sEndMessage))
    {
        m_oClientMessageBuffer[pSocket] += data;
        return;
    }
    else
    {
       m_oClientMessageBuffer[pSocket] += data.remove(data.size() - 3, 3);
       qDebug() << m_oClientMessageBuffer[pSocket];
    }

    QStringList sValueList = QString::fromStdString(m_oClientMessageBuffer[pSocket].toStdString()).split(",");
    m_oClientMessageBuffer[pSocket].clear();
    QString sRequestType = sValueList[0];
    sValueList.removeAll(sRequestType);
    qDebug() << sValueList;

    switch (sRequestTypeMap[sRequestType])
    {
    case 0:
        {
            m_pClearTimer->start();
        }
        break;
    case 1:
        {
            m_pAlgoTimer->start();
        }
        break;
    case 2:
        {   
            m_pLogic->setDrone(sValueList[0].toInt(), sValueList[1].toInt());
            foreach (int user, m_users.keys())
                sendMessage(QString("DRONE," + sValueList[0] + "," + sValueList[1] + ","), user);
            qDebug() << "selected drone";
        }
        break;
    default:
        break;
    }
}

void CGameServer::onFlushTimerTick()
{
    std::flush(std::cout);
}

void CGameServer::onClearTimerTick()
{
    bool bOut = m_pLogic->clearCicles();
    foreach (int user, m_users.keys())
        sendMessage("DATAC," + m_pLogic->getStr().toLatin1(), user);
    if (bOut)
    {
        foreach (int user, m_users.keys())
            sendMessage(QString("DONEC,Cycles are cleard."), user);
    }
    if(bOut) m_pClearTimer->stop();
}

void CGameServer::onAlgoTimerTick()
{
    bool bOut = m_pLogic->activateAlgo();
    foreach (int user, m_users.keys())
        sendMessage("DATAA," + m_pLogic->getStr().toLatin1(), user);
    if (bOut)
    {
        foreach (int user, m_users.keys())
            sendMessage("DONEA,Dorne's traversing process are done.", user);
    }
    if(bOut) m_pAlgoTimer->stop();
}

void CGameServer::onDisconnected()
{
    QTcpSocket* pSocket = (QTcpSocket *) sender();
    int removeUser = getCurrentUserID(pSocket);
    if (removeUser != -1)
    {
        m_oServerClients.remove(removeUser);
        m_users.remove(removeUser);
        qDebug() << "Client" << removeUser << "disconnected from game";
    }

    if (0 == m_users.size())
        cleanup();
}


int CGameServer::getNextClientIndex(QTcpSocket* pSocket)
{
    int oCurrentUser = getCurrentUserID(pSocket);
    QList<int> keys = m_users.keys();
    for(size_t i = 0; i < keys.size(); ++i)
    {
        if (keys[i] == oCurrentUser)
        {
            if (i == keys.size() - 1)
                return 0;
            else
                return i + 1;
        }
    }
}

int CGameServer::getCurrentUserID(QTcpSocket* pSocket)
{
    foreach (int user, m_users.keys())
    {
        if (pSocket == m_users[user]) return user;
    }
    return -1;
}

void CGameServer::cleanup()
{
    m_nUserCount = 0;
    m_nCount = 0;
    m_oServerClients.clear();
    m_oClientNamesMap.clear();

    if (0 != m_users.size())
    {
        foreach(int user, m_users.keys())
        {
            m_users[user]->close();
            delete m_users[user];
            m_users[user] = nullptr;
        }
        m_users.clear();
    }
}

