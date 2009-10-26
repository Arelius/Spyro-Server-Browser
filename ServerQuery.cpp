#include <QUdpSocket>

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

class ServeryQuery
{
private:
    Qt::QUdpSocket* UdpSocket;
public:
    ServerQuery();
    ~ServerQuery();
    //TODO: Read these from MasterServers.vdf is possible.
    SteamServer GetMasterServerHost();
    void SendMasterServerQuery(RegionCode Region, const char* IP_Port, const char* Filter);
    QBool CheckMasterServerRetCode(const char* Packet, const char** inc = NULL);
    // Returns false if there are no more servers incoming.
    QBool ParseMasterServerRet(const char* PacketData,
                               size_t Length,
                               QList<SteamServer>& RetServers);
};


ServerQuery::ServerQuery()
{
    UdpSocket = new QUdpSocket();
    UdpSocket->bind();
}

ServerQuery::~ServerQuery()
{
    delete UdpSocket();
}

QHostAddress GetMasterServerHost()
{
    //Warning: We assume that QHostInfo::fromName is reentrant if it's used in another thread.
    return SteamServer(
        QHostInfo::fromName(QString("hl2master.steampowered.com")).addresses().first(),
        27012);
}

void ServerQuery::SendMasterServerQuery(RegionCode Region,
                                        const char* IP_Port,
                                        const char* Filter)
{
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

    memcpy(Curr, &PackHead, sizeof(PackHead));  Curr += PackHead;
    memcpy(Curr, IP_Port, strlen(IP_Port) + 1); Curr += strlen(IP_Port) + 1;
    memcpy(Curr, Filter, strlen(Filter) + 1);   Curr += strlen(Filter) + 1;

    SteamServer MasterServer = GetMasterServerHost();

    UdpSocket->writeDatagram(Buffer, PacketLength, MasterServer.first(), MasterServer.second());

    delete Buffer;
}

QBool CheckMasterServerRetCode(const char* Packet, const char** inc)
{
#define CHECK_INC_BYTE(PTR, BYT) if(*(PTR++) != (char)(BYT)) return false;

    const char* Curr = Packet;
    for(int i=0, i<4, i++)
        CHECK_INC_BYTE(Curr, 0xFF);
    CHECK_INC_BYTE(Curr, 0x66);
    CHECK_INC_BYTE(Curr, 0x0A);
    
    if(inc) *inc = Curr;
    return true;

#undef CHECK_INC_BYTE
}

QBool ParseMasterServerRet(const char* PacketData,
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
        }

        //NETASSERT: Shouldn't assert on packet errors.
        assert(Port && "Valid IP but port is 0.");

        // Generate a SteamServer add it to the list.
        RetServers.push_back(SteamServer(
                                 QHostAddress(RetAddress->iAddress),
                                 RetAddress->Port));

        Curr += sizeof(MS_RetAddress);
    }
}
