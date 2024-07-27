#pragma once 

#include <stdlib.h>

#if defined(_WIN32)
#include "khg_net/win32.h"
#elif defined(__linux__)
#include "khg_net/unix.h"
#endif

#include "khg_net/types.h"
#include "khg_net/protocol.h"
#include "khg_net/list.h"
#include "khg_net/callbacks.h"

#define ENET_VERSION_MAJOR 1
#define ENET_VERSION_MINOR 3
#define ENET_VERSION_PATCH 18
#define ENET_VERSION_CREATE(major, minor, patch) (((major)<<16) | ((minor)<<8) | (patch))
#define ENET_VERSION_GET_MAJOR(version) (((version)>>16)&0xFF)
#define ENET_VERSION_GET_MINOR(version) (((version)>>8)&0xFF)
#define ENET_VERSION_GET_PATCH(version) ((version)&0xFF)
#define ENET_VERSION ENET_VERSION_CREATE(ENET_VERSION_MAJOR, ENET_VERSION_MINOR, ENET_VERSION_PATCH)

typedef enet_uint32 ENetVersion;

struct _ENetHost;
struct _ENetEvent;
struct _ENetPacket;

typedef enum _ENetSocketType {
  ENET_SOCKET_TYPE_STREAM = 1,
  ENET_SOCKET_TYPE_DATAGRAM = 2
} ENetSocketType;

typedef enum _ENetSocketWait {
   ENET_SOCKET_WAIT_NONE = 0,
   ENET_SOCKET_WAIT_SEND = (1 << 0),
   ENET_SOCKET_WAIT_RECEIVE = (1 << 1),
   ENET_SOCKET_WAIT_INTERRUPT = (1 << 2)
} ENetSocketWait;

typedef enum _ENetSocketOption {
   ENET_SOCKOPT_NONBLOCK = 1,
   ENET_SOCKOPT_BROADCAST = 2,
   ENET_SOCKOPT_RCVBUF = 3,
   ENET_SOCKOPT_SNDBUF = 4,
   ENET_SOCKOPT_REUSEADDR = 5,
   ENET_SOCKOPT_RCVTIMEO = 6,
   ENET_SOCKOPT_SNDTIMEO = 7,
   ENET_SOCKOPT_ERROR = 8,
   ENET_SOCKOPT_NODELAY = 9,
   ENET_SOCKOPT_TTL = 10
} ENetSocketOption;

typedef enum _ENetSocketShutdown {
    ENET_SOCKET_SHUTDOWN_READ = 0,
    ENET_SOCKET_SHUTDOWN_WRITE = 1,
    ENET_SOCKET_SHUTDOWN_READ_WRITE = 2
} ENetSocketShutdown;

#define ENET_HOST_ANY 0
#define ENET_HOST_BROADCAST 0xFFFFFFFFU
#define ENET_PORT_ANY 0

typedef struct _ENetAddress {
   enet_uint32 host;
   enet_uint16 port;
} ENetAddress;

typedef enum _ENetPacketFlag {
   ENET_PACKET_FLAG_RELIABLE = (1 << 0),
   ENET_PACKET_FLAG_UNSEQUENCED = (1 << 1),
   ENET_PACKET_FLAG_NO_ALLOCATE = (1 << 2),
   ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT = (1 << 3),
   ENET_PACKET_FLAG_SENT = (1<<8)
} ENetPacketFlag;

typedef void (NET_CALLBACK * ENetPacketFreeCallback) (struct _ENetPacket *);

typedef struct _ENetPacket {
  size_t referenceCount;
  enet_uint32 flags;
  enet_uint8 *data;
  size_t dataLength;
  ENetPacketFreeCallback freeCallback;
  void *userData;
} ENetPacket;

typedef struct _ENetAcknowledgement {
  net_list_node acknowledgementList;
  enet_uint32 sentTime;
  ENetProtocol command;
} ENetAcknowledgement;

typedef struct _ENetOutgoingCommand {
  net_list_node outgoingCommandList;
  enet_uint16 reliableSequenceNumber;
  enet_uint16 unreliableSequenceNumber;
  enet_uint32 sentTime;
  enet_uint32 roundTripTimeout;
  enet_uint32 queueTime;
  enet_uint32 fragmentOffset;
  enet_uint16 fragmentLength;
  enet_uint16 sendAttempts;
  ENetProtocol command;
  ENetPacket *packet;
} ENetOutgoingCommand;

typedef struct _ENetIncomingCommand {
  net_list_node incomingCommandList;
  enet_uint16 reliableSequenceNumber;
  enet_uint16 unreliableSequenceNumber;
  ENetProtocol command;
  enet_uint32 fragmentCount;
  enet_uint32 fragmentsRemaining;
  enet_uint32 *fragments;
  ENetPacket *packet;
} ENetIncomingCommand;

typedef enum _ENetPeerState {
  ENET_PEER_STATE_DISCONNECTED = 0,
  ENET_PEER_STATE_CONNECTING = 1,
  ENET_PEER_STATE_ACKNOWLEDGING_CONNECT = 2,
  ENET_PEER_STATE_CONNECTION_PENDING = 3,
  ENET_PEER_STATE_CONNECTION_SUCCEEDED = 4,
  ENET_PEER_STATE_CONNECTED = 5,
  ENET_PEER_STATE_DISCONNECT_LATER = 6,
  ENET_PEER_STATE_DISCONNECTING = 7,
  ENET_PEER_STATE_ACKNOWLEDGING_DISCONNECT = 8,
  ENET_PEER_STATE_ZOMBIE = 9 
} ENetPeerState;

#ifndef ENET_BUFFER_MAXIMUM
#define ENET_BUFFER_MAXIMUM (1 + 2 * net_protocol_maximum_packet_commands)
#endif

enum {
   ENET_HOST_RECEIVE_BUFFER_SIZE = 256 * 1024,
   ENET_HOST_SEND_BUFFER_SIZE = 256 * 1024,
   ENET_HOST_BANDWIDTH_THROTTLE_INTERVAL = 1000,
   ENET_HOST_DEFAULT_MTU = 1392,
   ENET_HOST_DEFAULT_MAXIMUM_PACKET_SIZE = 32 * 1024 * 1024,
   ENET_HOST_DEFAULT_MAXIMUM_WAITING_DATA = 32 * 1024 * 1024,
   ENET_PEER_DEFAULT_ROUND_TRIP_TIME = 500,
   ENET_PEER_DEFAULT_PACKET_THROTTLE = 32,
   ENET_PEER_PACKET_THROTTLE_SCALE = 32,
   ENET_PEER_PACKET_THROTTLE_COUNTER = 7, 
   ENET_PEER_PACKET_THROTTLE_ACCELERATION = 2,
   ENET_PEER_PACKET_THROTTLE_DECELERATION = 2,
   ENET_PEER_PACKET_THROTTLE_INTERVAL = 5000,
   ENET_PEER_PACKET_LOSS_SCALE = (1 << 16),
   ENET_PEER_PACKET_LOSS_INTERVAL = 10000,
   ENET_PEER_WINDOW_SIZE_SCALE = 64 * 1024,
   ENET_PEER_TIMEOUT_LIMIT = 32,
   ENET_PEER_TIMEOUT_MINIMUM = 5000,
   ENET_PEER_TIMEOUT_MAXIMUM = 30000,
   ENET_PEER_PING_INTERVAL = 500,
   ENET_PEER_UNSEQUENCED_WINDOWS = 64,
   ENET_PEER_UNSEQUENCED_WINDOW_SIZE = 1024,
   ENET_PEER_FREE_UNSEQUENCED_WINDOWS = 32,
   ENET_PEER_RELIABLE_WINDOWS = 16,
   ENET_PEER_RELIABLE_WINDOW_SIZE = 0x1000,
   ENET_PEER_FREE_RELIABLE_WINDOWS = 8
};

typedef struct _ENetChannel {
   enet_uint16 outgoingReliableSequenceNumber;
   enet_uint16 outgoingUnreliableSequenceNumber;
   enet_uint16 usedReliableWindows;
   enet_uint16 reliableWindows [ENET_PEER_RELIABLE_WINDOWS];
   enet_uint16 incomingReliableSequenceNumber;
   enet_uint16 incomingUnreliableSequenceNumber;
   net_list incomingReliableCommands;
   net_list incomingUnreliableCommands;
} ENetChannel;

typedef enum _ENetPeerFlag {
  ENET_PEER_FLAG_NEEDS_DISPATCH = (1 << 0),
  ENET_PEER_FLAG_CONTINUE_SENDING = (1 << 1)
} ENetPeerFlag;

typedef struct _ENetPeer { 
  net_list_node dispatchList;
  struct _ENetHost *host;
  enet_uint16 outgoingPeerID;
  enet_uint16 incomingPeerID;
  enet_uint32 connectID;
  enet_uint8 outgoingSessionID;
  enet_uint8 incomingSessionID;
  ENetAddress address;
  void *data;
  ENetPeerState state;
  ENetChannel *channels;
  size_t channelCount;
  enet_uint32 incomingBandwidth;
  enet_uint32 outgoingBandwidth;
  enet_uint32 incomingBandwidthThrottleEpoch;
  enet_uint32 outgoingBandwidthThrottleEpoch;
  enet_uint32 incomingDataTotal;
  enet_uint32 outgoingDataTotal;
  enet_uint32 lastSendTime;
  enet_uint32 lastReceiveTime;
  enet_uint32 nextTimeout;
  enet_uint32 earliestTimeout;
  enet_uint32 packetLossEpoch;
  enet_uint32 packetsSent;
  enet_uint32 packetsLost;
  enet_uint32 packetLoss;
  enet_uint32 packetLossVariance;
  enet_uint32 packetThrottle;
  enet_uint32 packetThrottleLimit;
  enet_uint32 packetThrottleCounter;
  enet_uint32 packetThrottleEpoch;
  enet_uint32 packetThrottleAcceleration;
  enet_uint32 packetThrottleDeceleration;
  enet_uint32 packetThrottleInterval;
  enet_uint32 pingInterval;
  enet_uint32 timeoutLimit;
  enet_uint32 timeoutMinimum;
  enet_uint32 timeoutMaximum;
  enet_uint32 lastRoundTripTime;
  enet_uint32 lowestRoundTripTime;
  enet_uint32 lastRoundTripTimeVariance;
  enet_uint32 highestRoundTripTimeVariance;
  enet_uint32 roundTripTime;
  enet_uint32 roundTripTimeVariance;
  enet_uint32 mtu;
  enet_uint32 windowSize;
  enet_uint32 reliableDataInTransit;
  enet_uint16 outgoingReliableSequenceNumber;
  net_list acknowledgements;
  net_list sentReliableCommands;
  net_list outgoingSendReliableCommands;
  net_list outgoingCommands;
  net_list dispatchedCommands;
  enet_uint16 flags;
  enet_uint16 reserved;
  enet_uint16 incomingUnsequencedGroup;
  enet_uint16 outgoingUnsequencedGroup;
  enet_uint32 unsequencedWindow[ENET_PEER_UNSEQUENCED_WINDOW_SIZE / 32]; 
  enet_uint32 eventData;
  size_t totalWaitingData;
} ENetPeer;

typedef struct _ENetCompressor {
  void *context;
  size_t (NET_CALLBACK * compress) (void * context, const net_buffer * inBuffers, size_t inBufferCount, size_t inLimit, enet_uint8 * outData, size_t outLimit);
  size_t (NET_CALLBACK * decompress) (void * context, const enet_uint8 * inData, size_t inLimit, enet_uint8 * outData, size_t outLimit);
  void (NET_CALLBACK * destroy) (void * context);
} ENetCompressor;

typedef enet_uint32 (NET_CALLBACK * ENetChecksumCallback) (const net_buffer * buffers, size_t bufferCount);

typedef int (NET_CALLBACK * ENetInterceptCallback) (struct _ENetHost * host, struct _ENetEvent * event);
 
typedef struct _ENetHost {
  net_socket socket;
  ENetAddress address;
  enet_uint32 incomingBandwidth;
  enet_uint32 outgoingBandwidth;
  enet_uint32 bandwidthThrottleEpoch;
  enet_uint32 mtu;
  enet_uint32 randomSeed;
  int recalculateBandwidthLimits;
  ENetPeer *peers;
  size_t peerCount;
  size_t channelLimit;
  enet_uint32 serviceTime;
  net_list dispatchQueue;
  enet_uint32 totalQueued;
  size_t packetSize;
  enet_uint16 headerFlags;
  ENetProtocol commands[net_protocol_maximum_packet_commands];
  size_t commandCount;
  net_buffer buffers[ENET_BUFFER_MAXIMUM];
  size_t bufferCount;
  ENetChecksumCallback checksum;
  ENetCompressor compressor;
  enet_uint8 packetData[2][net_protocol_maximum_mtu];
  ENetAddress receivedAddress;
  enet_uint8 *receivedData;
  size_t receivedDataLength;
  enet_uint32 totalSentData;
  enet_uint32 totalSentPackets;
  enet_uint32 totalReceivedData;
  enet_uint32 totalReceivedPackets;
  ENetInterceptCallback intercept;
  size_t connectedPeers;
  size_t bandwidthLimitedPeers;
  size_t duplicatePeers;
  size_t maximumPacketSize;
  size_t maximumWaitingData;
} ENetHost;

typedef enum _ENetEventType {
  ENET_EVENT_TYPE_NONE = 0,  
  ENET_EVENT_TYPE_CONNECT = 1,  
  ENET_EVENT_TYPE_DISCONNECT = 2,  
  ENET_EVENT_TYPE_RECEIVE = 3
} ENetEventType;

typedef struct _ENetEvent {
  ENetEventType type;
  ENetPeer *peer;
  enet_uint8 channelID;
  enet_uint32 data;
  ENetPacket *packet;
} ENetEvent;

extern int enet_initialize(void);
extern int enet_initialize_with_callbacks(ENetVersion version, const net_callbacks *inits);
extern void enet_deinitialize(void);

extern ENetVersion enet_linked_version(void);

extern enet_uint32 enet_time_get(void);
extern void enet_time_set(enet_uint32);

extern net_socket enet_socket_create(ENetSocketType);
extern int enet_socket_bind(net_socket, const ENetAddress *);
extern int enet_socket_get_address(net_socket, ENetAddress *);
extern int enet_socket_listen(net_socket, int);
extern net_socket enet_socket_accept(net_socket, ENetAddress *);
extern int enet_socket_connect(net_socket, const ENetAddress *);
extern int enet_socket_send(net_socket, const ENetAddress *, const net_buffer *, size_t);
extern int enet_socket_receive(net_socket, ENetAddress *, net_buffer *, size_t);
extern int enet_socket_wait(net_socket, enet_uint32 *, enet_uint32);
extern int enet_socket_set_option(net_socket, ENetSocketOption, int);
extern int enet_socket_get_option(net_socket, ENetSocketOption, int *);
extern int enet_socket_shutdown(net_socket, ENetSocketShutdown);
extern void enet_socket_destroy(net_socket);
extern int enet_socketset_select(net_socket, net_socket_set *, net_socket_set *, enet_uint32);

extern int enet_address_set_host_ip(ENetAddress *address, const char *hostName);

extern int enet_address_set_host(ENetAddress *address, const char *hostName);
extern int enet_address_get_host_ip(const ENetAddress *address, char *hostName, size_t nameLength);
extern int enet_address_get_host(const ENetAddress *address, char *hostName, size_t nameLength);

extern ENetPacket *enet_packet_create(const void *, size_t, enet_uint32);
extern void enet_packet_destroy(ENetPacket *);
extern int enet_packet_resize(ENetPacket *, size_t);
extern enet_uint32 enet_crc32(const net_buffer *, size_t);
                
extern ENetHost *enet_host_create(const ENetAddress *, size_t, size_t, enet_uint32, enet_uint32);
extern void enet_host_destroy(ENetHost *);
extern ENetPeer *enet_host_connect(ENetHost *, const ENetAddress *, size_t, enet_uint32);
extern int enet_host_check_events(ENetHost *, ENetEvent *);
extern int enet_host_service(ENetHost *, ENetEvent *, enet_uint32);
extern void enet_host_flush(ENetHost *);
extern void enet_host_broadcast(ENetHost *, enet_uint8, ENetPacket *);
extern void enet_host_compress(ENetHost *, const ENetCompressor *);
extern int enet_host_compress_with_range_coder(ENetHost *host);
extern void enet_host_channel_limit(ENetHost *, size_t);
extern void enet_host_bandwidth_limit(ENetHost *, enet_uint32, enet_uint32);
extern void enet_host_bandwidth_throttle(ENetHost *);
extern enet_uint32 enet_host_random_seed(void);
extern enet_uint32 enet_host_random(ENetHost *);

extern int enet_peer_send(ENetPeer *, enet_uint8, ENetPacket *);
extern ENetPacket *enet_peer_receive(ENetPeer *, enet_uint8 *channelID);
extern void enet_peer_ping(ENetPeer *);
extern void enet_peer_ping_interval(ENetPeer *, enet_uint32);
extern void enet_peer_timeout(ENetPeer *, enet_uint32, enet_uint32, enet_uint32);
extern void enet_peer_reset(ENetPeer *);
extern void enet_peer_disconnect(ENetPeer *, enet_uint32);
extern void enet_peer_disconnect_now(ENetPeer *, enet_uint32);
extern void enet_peer_disconnect_later(ENetPeer *, enet_uint32);
extern void enet_peer_throttle_configure(ENetPeer *, enet_uint32, enet_uint32, enet_uint32);
extern int enet_peer_throttle(ENetPeer *, enet_uint32);
extern void enet_peer_reset_queues(ENetPeer *);
extern int enet_peer_has_outgoing_commands(ENetPeer *);
extern void enet_peer_setup_outgoing_command(ENetPeer *, ENetOutgoingCommand *);
extern ENetOutgoingCommand *enet_peer_queue_outgoing_command(ENetPeer *, const ENetProtocol *, ENetPacket *, enet_uint32, enet_uint16);
extern ENetIncomingCommand *enet_peer_queue_incoming_command(ENetPeer *, const ENetProtocol *, const void *, size_t, enet_uint32, enet_uint32);
extern ENetAcknowledgement *enet_peer_queue_acknowledgement(ENetPeer *, const ENetProtocol *, enet_uint16);
extern void enet_peer_dispatch_incoming_unreliable_commands(ENetPeer *, ENetChannel *, ENetIncomingCommand *);
extern void enet_peer_dispatch_incoming_reliable_commands(ENetPeer *, ENetChannel *, ENetIncomingCommand *);
extern void enet_peer_on_connect(ENetPeer *);
extern void enet_peer_on_disconnect(ENetPeer *);

extern void *enet_range_coder_create(void);
extern void enet_range_coder_destroy(void *);
extern size_t enet_range_coder_compress(void *, const net_buffer *, size_t, size_t, enet_uint8 *, size_t);
extern size_t enet_range_coder_decompress(void *, const enet_uint8 *, size_t, enet_uint8 *, size_t);
   
extern size_t enet_protocol_command_size(enet_uint8);
