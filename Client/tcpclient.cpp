#include "tcpclient.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <QMessageBox>

#define HOST_ADDRESS "127.0.0.1" // "185.127.66.104"
#define HOST_PORT 40004

std::map<QString, int> sRequestTypeMap = {
    std::make_pair(QString("FIRST"), 0),
    std::make_pair(QString("DATAC"), 1),
    std::make_pair(QString("DATAA"), 2),
    std::make_pair(QString("DONEC"), 3),
    std::make_pair(QString("DONEA"), 4),
    std::make_pair(QString("DATA"), 5),
    std::make_pair(QString("DRONE"), 6)
};


CClient::CClient()
{
    //
    /// Init socket and timer
    //
    m_pSocket = new QTcpSocket(this);
    m_pFlushTimer = new QTimer(this);

    //
    /// Make connections
    //
    connect(m_pSocket, &QTcpSocket::connected, this, &CClient::onSocketConnected);
    connect(m_pSocket, &QTcpSocket::readyRead, this, &CClient::onReadyRead);
    connect(m_pSocket, &QTcpSocket::disconnected, this, &CClient::onSocketDisconnected);
    connect(m_pFlushTimer, &QTimer::timeout, this, &CClient::onFlushTimerTick);

    //
    /// Flush data to std::out
    //
    m_pFlushTimer->setInterval(1000);
    m_pFlushTimer->start();
}

bool CClient::isConnected()
{
    return m_pSocket->state() == QAbstractSocket::ConnectedState;
}

void CClient::connectToServer()
{
    if (m_pSocket->state() == QAbstractSocket::UnconnectedState)
    {
        m_pSocket->connectToHost(HOST_ADDRESS, HOST_PORT);
        bool bConnected = m_pSocket->waitForConnected(2000);
        if (!bConnected)
        {
           (void)QMessageBox::critical(nullptr, tr("Drone View"), tr(m_pSocket->errorString().toStdString().c_str()), QMessageBox::Close);
           exit(0);
        }
    }
}

void CClient::sendMessage(QString msg)
{
    qDebug() << msg;
    QString msgToSend = msg;
    if (m_pSocket->state() != QAbstractSocket::ConnectedState)
    {
        return;
    }

    QByteArray data = msgToSend.toLatin1();
    int bytesRateCount = data.size() / 1000;
    int bytesRateRem = data.size() % 1000;

    int i = 0;
    for (; 0 != bytesRateCount; --bytesRateCount)
    {
        m_pSocket->write(data.mid(i, 1000));
        m_pSocket->waitForBytesWritten(2000);
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        i += 1000;
    }

    QByteArray sData = "";
    if (0 != bytesRateRem)
    {
        sData = data.mid(i, bytesRateRem);
    }

    sData = sData.append(QByteArray("END"));
    m_pSocket->write(sData);
    m_pSocket->waitForBytesWritten(2000);
    std::this_thread::sleep_for(std::chrono::microseconds(200));
}

void CClient::disconnectFromServer()
{
    qDebug() << "state: " << m_pSocket->state();
    if (m_pSocket->state() != QAbstractSocket::ConnectedState)
    {
        return;
    }

    qDebug() << "Client disconnecting";
    m_pSocket->disconnectFromHost();
}

void CClient::onSocketConnected()
{
    onReadyRead();
}

void CClient::onSocketDisconnected()
{
}

void CClient::onReadyRead()
{
    qDebug() << "INFO  Reading...";
    std::this_thread::sleep_for(std::chrono::microseconds(100));
    QByteArray data = m_pSocket->readAll();
    m_pSocket->waitForBytesWritten(50000);
    if (data.isEmpty())
    {
        return;
    }
    QByteArray sEndMessage = "END";
    if (!data.endsWith(sEndMessage))
    {
        m_oMessageBuffer += data;
        return;
    }
    else
    {
       m_oMessageBuffer += data.remove(data.size() - 3, 3);
    }

    QList<QString> sDataList = QString::fromStdString(m_oMessageBuffer.toStdString()).split(",");
    QString sCommand = sDataList[0];
    sDataList.removeFirst();
    switch (sRequestTypeMap[sCommand])
    {
        case 0:
        {
            emit sigFirst();
            m_oMessageBuffer.clear();
            qDebug() << "FIRSTFIRSTFIRST";
            eval(sDataList);
            return;
        }
        case 1:
        {
            eval(sDataList);
         //   sendMessage("clear,");
        }
        break;
        case 2:
        {
            eval(sDataList);
           // sendMessage("start,");
        }
        break;
        case 3:
            emit sigMsg(sDataList.join(','), true);
            break;
        case 4:
            emit sigMsg(sDataList.join(','), false);
            break;
        case 5:
            eval(sDataList);
            qDebug() << "Data first time  : size - " << sDataList.size();
            break;
        case 6:
            emit sigUpdateNode(sDataList[0].toInt(), sDataList[1].toInt(), 1, -1);
        break;
        default:
            break;
    }
    m_oMessageBuffer.clear();
}

void CClient::onFlushTimerTick()
{
    std::flush(std::cout);
}

void CClient::eval(QList<QString> sDataList)
{
    for (int i = 0; i <= sDataList.size() - 4; i += 4)
    {
        int nRow = sDataList[i].toInt();
        int nCol = sDataList[i + 1].toInt();
        int nColor = sDataList[i + 2].toInt();
        int nDirection = sDataList[i + 3].toInt();

        emit sigUpdateNode(nRow, nCol, nColor, nDirection);
    }

}

