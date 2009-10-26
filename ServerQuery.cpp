#include <QtCore/QString>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostInfo>
#include <assert.h>

#define PACKED __attribute__((packed))

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
    SteamServer ConnectAddress;
};

class ServerQuery
{
private:
    QUdpSocket* UdpSocket;
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
};


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
    return SteamServer(
        QHostInfo::fromName(QString("hl2master.steampowered.com")).addresses().first(),
        27012);
}

void ServerQuery::SendMasterServerQuery(RegionCode Region,
                                        SteamServer LastServer,
                                        const char* Filter)
{
    QString Address = LastServer.first.toString() + ":" + QString(LastServer.second);
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
    SendMasterServerQuery(US_West, ServerQuery::StartServer(), "");
    
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

            if(Err == -1)
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
        }
        else
        {
            // There was Nothing to do! sleep!
            //TODO: Check timeout!
            sleep(0.1);
        }
    }
}
