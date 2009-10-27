#include "ServerQuery.h"
#include <QtCore/QString>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostInfo>
#include <QtCore/QThread>
#include <assert.h>

#define PACKED __attribute__((packed))

QString SteamServerToQString(const SteamServer& Address)
{
    return Address.first.toString() + ":" + QString::number(Address.second);
}

SteamServerInfo::SteamServerInfo()
{
    AppID = 0;
    NumPlayers = 0;
    MaxPlayers = 0;
    NumBots = 0;
    Dedicated = 0;
    OS = 0;
    Password = false;
    Secure = false;
}

SteamServerInfo::SteamServerInfo(const SteamServer& Address)
{
    AppID = 0;
    NumPlayers = 0;
    MaxPlayers = 0;
    NumBots = 0;
    Dedicated = 0;
    OS = 0;
    Password = false;
    Secure = false;
    ConnectAddress = SteamServerToQString(Address);
    ServerName = ConnectAddress;
}

SteamServerInfo::SteamServerInfo(const SteamServerInfo& O)
{
    ServerName = O.ServerName;
    Map = O.Map;
    GameDir = O.GameDir;
    GameDescription = O.GameDescription;
    AppID = O.AppID;
    NumPlayers = O.NumPlayers;
    MaxPlayers = O.MaxPlayers;
    NumBots = O.NumBots;
    Dedicated = O.Dedicated;
    OS = O.OS;
    Password = O.Password;
    Secure = O.Secure;
    GameVersion = O.GameVersion;
    ConnectAddress = O.ConnectAddress;
}

SteamServerInfo::~SteamServerInfo()
{}


ServerQuery::ServerQuery()
{
    UdpSocket = new QUdpSocket();
    UdpSocket->bind();
}

ServerQuery::~ServerQuery()
{
    delete UdpSocket;
}

SteamServer ServerQuery::GetMasterServerHost()
{
    //Warning: We assume that QHostInfo::fromName is reentrant if it's used in another thread.
    // return SteamServer(
    //     QHostInfo::fromName(QString("hl2master.steampowered.com")).addresses().first(),
    //     27011);

    //hl2master doesn't seem to respond.
    return SteamServer(
        QHostAddress(QString("72.165.61.136")), 27014);
}

void ServerQuery::SendMasterServerQuery(RegionCode Region,
                                        SteamServer LastServer,
                                        const char* Filter)
{
    QString Address = LastServer.first.toString() + ":" + QString::number(LastServer.second);
    const char* IP_Port = Address.toAscii().data();
    struct PACKED MS_Packet_Header
    {
        char MessageType:8;
        char RegionCode:8;
    } PackHead;

    size_t PacketLength = sizeof(MS_Packet_Header) + strlen(IP_Port) + strlen(Filter) + 2;
    
    PackHead.MessageType = 0x31;
    
    PackHead.RegionCode = (char)Region;

    char* Buffer = new char[PacketLength];
    char* Curr = Buffer;

    memcpy(Curr, &PackHead, sizeof(PackHead));  Curr += sizeof(PackHead);
    memcpy(Curr, IP_Port, strlen(IP_Port) + 1); Curr += strlen(IP_Port) + 1;
    memcpy(Curr, Filter, strlen(Filter) + 1);   Curr += strlen(Filter) + 1;

    SteamServer MasterServer = GetMasterServerHost();

    UdpSocket->writeDatagram(Buffer, PacketLength, MasterServer.first, MasterServer.second);

    delete Buffer;
}

bool ServerQuery::CheckMasterServerRetCode(const char* Packet, const char** inc)
{
#define CHECK_INC_BYTE(PTR, BYT) if(*(PTR++) != (char)(BYT)) return false;

    const char* Curr = Packet;
    for(int i=0; i<4; i++)
        CHECK_INC_BYTE(Curr, 0xFF);
    CHECK_INC_BYTE(Curr, 0x66);
    CHECK_INC_BYTE(Curr, 0x0A);
    
    if(inc) *inc = Curr;
    return true;

#undef CHECK_INC_BYTE
}

bool ServerQuery::ParseMasterServerRet(const char* PacketData,
                           size_t Length,
                           QList<SteamServer>& RetServers)
{
    const char* Curr;
    bool verify = CheckMasterServerRetCode(PacketData, &Curr);
    // Only checking IP before hand, change this to an error messsage!
    //NETASSERT: Shouldn't assert on packet errors.
    assert(verify && "Received wrong packet from Master Server.");
    
    struct PACKED MS_RetAddress
    {
        quint32 iAddress:32;
        char Port:8;
    };
    
    MS_RetAddress* RetAddress;
    
    while(Curr - PacketData + sizeof(MS_RetAddress) <= Length)
    {
        RetAddress = (MS_RetAddress*)Curr;
        if(RetAddress->iAddress == 0)
        {
            //NETASSERT: Shouldn't assert on packet errors.
            assert(RetAddress->Port == 0 && "IP Address is Null but Port isn't.");
            assert(Curr - PacketData + sizeof(MS_RetAddress) == Length &&
                   "Extra packet data after list end address.");

            return false; // Done receiving all servers.
        }

        //NETASSERT: Shouldn't assert on packet errors.
        assert(RetAddress->Port && "Valid IP but port is 0.");

        // Generate a SteamServer add it to the list.
        RetServers.push_back(SteamServer(
                                 QHostAddress(RetAddress->iAddress),
                                 RetAddress->Port));

        Curr += sizeof(MS_RetAddress);
    }

    return false; // Still have more servers to query.
}

void ServerQuery::GetServerList()
{
    bool MSReceivedAll = false;
    // Test query
    SendMasterServerQuery(Rest, ServerQuery::StartServer(), "\\gamedir\\tf\\napp\\500");
    
    while(true)
    {
        if(UdpSocket->hasPendingDatagrams())
        {
            size_t PacketSize = UdpSocket->pendingDatagramSize();
            char* PacketData = new char[PacketSize];
            
            QHostAddress SourceAddress;
            quint16 SourcePort;
            size_t Err =
                UdpSocket->readDatagram(PacketData,
                                        PacketSize,
                                        &SourceAddress,
                                        &SourcePort);

            if((int)Err == -1)
            {
                assert(false && "Problem reading datagram");
                delete PacketData;
                continue;
            }
            assert(Err == PacketSize && "pending and read datagram sizes differ");
            //TODO: Verify SourceAddress and SourcePort.

            MSReceivedAll = ParseMasterServerRet(PacketData, PacketSize, PendingQueries);
            
            delete PacketData;
        }
        else if(!PendingQueries.empty())
        {
            // Send query to the server.
            SteamServerInfo Info = SteamServerInfo(PendingQueries.front());
            printf("Server Info:%s\n", Info.ConnectAddress.toAscii().data());
            ServerInfoReceived(Info);
            PendingQueries.pop_front();
        }
        else
        {
            if(MSReceivedAll)
                return;
            // There was Nothing to do! sleep!
            //TODO: Check timeout!
            sleep(0.1);
        }
    }
    ServerQueryFinished();
}

class QueryThread : public QThread
{
    AsyncServerQuery* QueryProxy;
public:
    QueryThread(AsyncServerQuery* Receiving) : QueryProxy(Receiving)
    {}
    void run()
    {
        ServerQuery Query;

        // They say that Signals and slots work between threads.
        // so, wire up the signals from this thread into the
        // async object in the other.
        Query.connect(&Query, SIGNAL(ServerInfoReceived(SteamServerInfo)),
                      QueryProxy, SIGNAL(ServerInfoReceived(SteamServerInfo)),
                      Qt::QueuedConnection);
        Query.connect(&Query, SIGNAL(ServerQueryFinished()),
                      QueryProxy, SIGNAL(ServerQueryFinished()),
                      Qt::QueuedConnection);
        Query.connect(&Query, SIGNAL(ServerQueryTimeout()),
                      QueryProxy, SIGNAL(ServerQueryTimeout()),
                      Qt::QueuedConnection);

        Query.GetServerList();
    }
};

AsyncServerQuery::AsyncServerQuery()
{
    SQThread = new QueryThread(this);
}

AsyncServerQuery::~AsyncServerQuery()
{
    SQThread->wait();
    delete SQThread;
}

void AsyncServerQuery::QueryServerList()
{
    SQThread->start();
}
