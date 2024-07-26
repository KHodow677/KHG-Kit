#include "khg_net/net.h"
#include <string.h>

ENetPacket *enet_packet_create(const void *data, size_t dataLength, enet_uint32 flags) {
  ENetPacket *packet = (ENetPacket *)net_malloc(sizeof(ENetPacket));
  if (packet == NULL) {
    return NULL;
  }
  if (flags & ENET_PACKET_FLAG_NO_ALLOCATE) {
    packet->data = (enet_uint8 *)data;
  }
  else if (dataLength <= 0) {
    packet->data = NULL;
  }
  else {
    packet->data = (enet_uint8 *)net_malloc(dataLength);
    if (packet->data == NULL) {
      net_free(packet);
      return NULL;
    }
    if (data != NULL) {
      memcpy(packet->data, data, dataLength);
    }
  }
  packet->referenceCount = 0;
  packet->flags = flags;
  packet->dataLength = dataLength;
  packet->freeCallback = NULL;
  packet->userData = NULL;
  return packet;
}
