#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

///////////////////////////////////////////////////////////////////
/// \brief The CClient class
//
class CClient : public QObject
{
    Q_OBJECT
public:
    CClient();

    bool isConnected();
    void connectToServer();
    void disconnectFromServer();
    void sendMessage(QString msg);

signals:
    void sigUpdateNode(int, int, int, int);
    void sigFirst();
    void sigMsg(const QString&, bool);

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onReadyRead();
    void onFlushTimerTick();

private:
    void eval(QList<QString> sDataList);

private:
    QTcpSocket* m_pSocket     = nullptr;
    QTimer*     m_pFlushTimer = nullptr;
    QByteArray  m_oMessageBuffer;
};

#endif // !CLIENT_H
