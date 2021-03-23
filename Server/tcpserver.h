#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

class CLogic;
/////////////////////////////////////////////////////////////////////
/// \brief The CGameServer class
//
class CGameServer : public QTcpServer
{
    Q_OBJECT
public:
    //
    /// \brief CGameServer constructor
    /// \param parent
    //
    CGameServer(QObject* parent = nullptr);
    //
    /// \brief sendMessage, Sending Message to client
    /// \param msg
    /// \param user
    //
    void sendMessage(QString msg, int user);

private slots:
    //
    /// \brief onNewConnection, New pending connection handler
    //
    void onNewConnection();
    //
    /// \brief onReadyRead, Client data handler
    //
    void onReadyRead();
    //
    /// \brief onFlushTimerTick, Flush client data stream to std::out
    //
    void onFlushTimerTick();
    //
    /// \brief onDisconnected, Client disconnection handler
    //
    void onDisconnected();

    void onClearTimerTick();

    void onAlgoTimerTick();

private:
    int getNextClientIndex(QTcpSocket* pSocket);
    //
    ///
    //
    int getCurrentUserID(QTcpSocket* pSocket);
    //
    ///
    //
    void cleanup();

private:
    int                           m_nUserCount = 0;
    size_t                        m_nCount = 0;
    bool                          m_bSettingsInicialized = false;
    QMap<int, bool>               m_oServerClients;
    QMap<int, QTcpSocket*>        m_users;
    QMap<QTcpSocket*, QByteArray> m_oClientMessageBuffer;
    QTimer*                       m_pFlushTimer = nullptr;
    QTimer*                       m_pClearTimer = nullptr;
    QTimer*                       m_pAlgoTimer = nullptr;
    QMap<int, QString>            m_oClientNamesMap;
    int                  	      m_nMaxClients = 4;
    CLogic*                       m_pLogic = nullptr;
};

#endif // !GAMESERVER_H
