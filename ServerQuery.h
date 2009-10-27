#ifndef SERVERQUERY_H
#define SERVERQUERY_H

#include <QtNetwork/QHostAddress>
#include <QtCore/QMetaType>

enum RegionCode
{
    US_East = 0,
    US_West = 1,
    South_America = 2,
    Europe = 3,
    Asia = 4,
    Australia = 5,
    Middle_East = 6,
    Africa = 7,
    Rest = 0xff
};

typedef QPair<QHostAddress, quint16> SteamServer;

struct SteamServerInfo
{
    QString ServerName;
    QString Map;
    QString GameDir;
    QString GameDescription;
    short AppID;
    char NumPlayers;
    char MaxPlayers;
    char NumBots;
    char Dedicated;
    char OS;
    bool Password;
    bool Secure;
    QString GameVersion;
    QString ConnectAddress;
    SteamServerInfo();
    SteamServerInfo(const SteamServer& Address);
    SteamServerInfo(const SteamServerInfo&);
    ~SteamServerInfo();
};

class AsyncServerQuery : public QObject
{
    Q_OBJECT
private:
    friend class ServerQuery;
    QThread* SQThread;
public:
    AsyncServerQuery();
    ~AsyncServerQuery();
    void QueryServerList();
signals:
    void ServerInfoReceived(SteamServerInfo Info);
    void ServerQueryFinished();
    void ServerQueryTimeout();
};

class ServerQuery : public QObject
{
    Q_OBJECT
private:
    class QUdpSocket* UdpSocket;
    QList<SteamServer> PendingQueries;
public:
    ServerQuery();
    ~ServerQuery();
    //TODO: Read these from MasterServers.vdf is possible.
    SteamServer GetMasterServerHost();
    void SendMasterServerQuery(RegionCode Region, SteamServer LastServer, const char* Filter);
    bool CheckMasterServerRetCode(const char* Packet, const char** inc = NULL);
    // Returns false if there are no more servers incoming.
    bool ParseMasterServerRet(const char* PacketData,
                               size_t Length,
                               QList<SteamServer>& RetServers);
    void GetServerList();
    static SteamServer StartServer()
    {
        return SteamServer(QHostAddress((quint32)0), 0);
    }
signals:
    void ServerInfoReceived(SteamServerInfo Info);
    void ServerQueryFinished();
    void ServerQueryTimeout();
};

Q_DECLARE_METATYPE(SteamServerInfo)

#endif // SERVERQUERY_H
