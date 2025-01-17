/* Aravis - Digital camera library
 *
 * Copyright © 2009-2022 Emmanuel Pacaud
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Author: Emmanuel Pacaud <emmanuel.pacaud@free.fr>
 */

#ifndef ARV_GVSP_PRIVATE_H
#define ARV_GVSP_PRIVATE_H

#include <arvtypes.h>
#include <arvbuffer.h>
#include <arvdebugprivate.h>

G_BEGIN_DECLS

#define ARV_GVSP_PACKET_EXTENDED_ID_MODE_MASK	0x80
#define ARV_GVSP_PACKET_ID_MASK			0x00ffffff
#define ARV_GVSP_PACKET_INFOS_CONTENT_TYPE_MASK	0x7f000000
#define ARV_GVSP_PACKET_INFOS_CONTENT_TYPE_POS	24

/**
 * ArvGvspPacketType:
 * @ARV_GVSP_PACKET_TYPE_OK: valid packet
 * @ARV_GVSP_PACKET_TYPE_RESEND: resent packet (BlackFly PointGrey camera support)
 * @ARV_GVSP_PACKET_TYPE_PACKET_UNAVAILABLE: error packet, indicating invalid resend request
 */

typedef enum {
	ARV_GVSP_PACKET_TYPE_OK =			0x0000,
	ARV_GVSP_PACKET_TYPE_RESEND =			0x0100,
	ARV_GVSP_PACKET_TYPE_PACKET_UNAVAILABLE =	0x800c
} ArvGvspPacketType;

/**
 * ArvGvspContentType:
 * @ARV_GVSP_CONTENT_TYPE_DATA_LEADER: leader packet
 * @ARV_GVSP_CONTENT_TYPE_DATA_TRAILER: trailer packet
 * @ARV_GVSP_CONTENT_TYPE_DATA_BLOCK: data packet
 */

typedef enum {
	ARV_GVSP_CONTENT_TYPE_DATA_LEADER = 	0x01,
	ARV_GVSP_CONTENT_TYPE_DATA_TRAILER = 	0x02,
	ARV_GVSP_CONTENT_TYPE_DATA_BLOCK =	0x03,
	ARV_GVSP_CONTENT_TYPE_ALL_IN =		0x04
} ArvGvspContentType;

/**
 * ArvGvspPayloadType:
 * @ARV_GVSP_PAYLOAD_TYPE_IMAGE: image data
 * @ARV_GVSP_PAYLOAD_TYPE_RAWDATA: raw data
 * @ARV_GVSP_PAYLOAD_TYPE_FILE: file
 * @ARV_GVSP_PAYLOAD_TYPE_CHUNK_DATA: chunk data
 * @ARV_GVSP_PAYLOAD_TYPE_EXTENDED_CHUNK_DATA: extended chunk data
 * @ARV_GVSP_PAYLOAD_TYPE_JPEG: JPEG data
 * @ARV_GVSP_PAYLOAD_TYPE_JPEG2000: JPEG2000 data
 * @ARV_GVSP_PAYLOAD_TYPE_H264: h264 data
 * @ARV_GVSP_PAYLOAD_TYPE_MULTIZONE_IMAGE: multizone image
*/

typedef enum {
	ARV_GVSP_PAYLOAD_TYPE_IMAGE =			0x0001,
	ARV_GVSP_PAYLOAD_TYPE_RAWDATA = 		0x0002,
	ARV_GVSP_PAYLOAD_TYPE_FILE = 			0x0003,
	ARV_GVSP_PAYLOAD_TYPE_CHUNK_DATA = 		0x0004,
	ARV_GVSP_PAYLOAD_TYPE_EXTENDED_CHUNK_DATA = 	0x0005, /* Deprecated */
	ARV_GVSP_PAYLOAD_TYPE_JPEG = 			0x0006,
	ARV_GVSP_PAYLOAD_TYPE_JPEG2000 = 		0x0007,
	ARV_GVSP_PAYLOAD_TYPE_H264 = 			0x0008,
	ARV_GVSP_PAYLOAD_TYPE_MULTIZONE_IMAGE = 	0x0009,
	ARV_GVSP_PAYLOAD_TYPE_IMAGE_EXTENDED_CHUNK = 	0x4001,

} ArvGvspPayloadType;

#pragma pack(push,1)

/**
 * ArvGvspHeader:
 * @frame_id: frame identifier
 * @packet_infos: #ArvGvspContentType and packet identifier in a 32 bit value
 * @data: data byte array
 *
 * GVSP packet header structure.
 */

typedef struct {
	guint16 frame_id;
	guint32 packet_infos;
	guint8 data[];
} ArvGvspHeader;

typedef struct {
	guint16 flags;
	guint32 packet_infos;
	guint64 frame_id;
	guint32 packet_id;
	guint8 data[];
} ArvGvspExtendedHeader;

/**
 * ArvGvspDataLeader:
 * @flags: generic flags
 * @payload_type: ID of the payload type
 * @timestamp_high: most significant bits of frame timestamp
 * @timestamp_low: least significant bits of frame timestamp_low
 * @pixel_format: a #ArvPixelFormat identifier
 * @width: frame width, in pixels
 * @height: frame height, in pixels
 * @x_offset: frame x offset, in pixels
 * @y_offset: frame y offset, in pixels
 *
 * GVSP data leader packet data area.
 */

typedef struct {
	guint16 flags;
	guint16 payload_type;
	guint32 timestamp_high;
	guint32 timestamp_low;
	guint32 pixel_format;
	guint32 width;
	guint32 height;
	guint32	x_offset;
	guint32	y_offset;
} ArvGvspDataLeader;

/**
 * ArvGvspDataTrailer:
 * @payload_type: ID of the payload type
 * @data0: unused
 *
 * GVSP data trailer packet data area.
 */

typedef struct {
	guint32 payload_type;
	guint32 data0;
} ArvGvspDataTrailer;


/**
 * ArvGvspPacket:
 * @packet_type: packet type, also known as status in wireshark dissector
 * @header: common GVSP packet header
 *
 * GVSP packet structure.
 */

typedef struct {
	guint16 packet_type;
	guint8 header[];
} ArvGvspPacket;

 /* IP + UDP + GVSP headers */
#define ARV_GVSP_PACKET_PROTOCOL_OVERHEAD		(20 + 8 + sizeof (ArvGvspPacket) + sizeof (ArvGvspHeader))
 /* IP + UDP + GVSP extended headers */
#define ARV_GVSP_PACKET_EXTENDED_PROTOCOL_OVERHEAD	(20 + 8 + sizeof (ArvGvspPacket) + sizeof (ArvGvspExtendedHeader))

#pragma pack(pop)

ArvGvspPacket *		arv_gvsp_packet_new_data_leader		(guint16 frame_id, guint32 packet_id,
								 guint64 timestamp, ArvPixelFormat pixel_format,
								 guint32 width, guint32 height,
								 guint32 x_offset, guint32 y_offset,
								 void *buffer, size_t *buffer_size);
ArvGvspPacket *		arv_gvsp_packet_new_data_trailer	(guint16 frame_id, guint32 packet_id,
								 void *buffer, size_t *buffer_size);
ArvGvspPacket *		arv_gvsp_packet_new_data_block		(guint16 frame_id, guint32 packet_id,
								 size_t size, void *data,
								 void *buffer, size_t *buffer_size);
char * 			arv_gvsp_packet_to_string 		(const ArvGvspPacket *packet, size_t packet_size);
void 			arv_gvsp_packet_debug 			(const ArvGvspPacket *packet, size_t packet_size,
								 ArvDebugLevel level);
static inline ArvGvspPacketType
arv_gvsp_packet_get_packet_type (const ArvGvspPacket *packet)
{
	return (ArvGvspPacketType) g_ntohs (packet->packet_type);
}

static inline gboolean
arv_gvsp_packet_type_is_error (const ArvGvspPacketType packet_type)
{
	return (packet_type & 0x8000) != 0;
}

static inline gboolean
arv_gvsp_packet_has_extended_ids (const ArvGvspPacket *packet)
{
	return (packet->header[2] & ARV_GVSP_PACKET_EXTENDED_ID_MODE_MASK) != 0;
}

static inline ArvGvspContentType
arv_gvsp_packet_get_content_type (const ArvGvspPacket *packet)
{
	if (arv_gvsp_packet_has_extended_ids (packet)) {
		ArvGvspExtendedHeader *header = (void *) &packet->header;

		return (ArvGvspContentType) ((g_ntohl (header->packet_infos) & ARV_GVSP_PACKET_INFOS_CONTENT_TYPE_MASK) >>
					     ARV_GVSP_PACKET_INFOS_CONTENT_TYPE_POS);
	} else {
		ArvGvspHeader *header = (void *) &packet->header;

		return (ArvGvspContentType) ((g_ntohl (header->packet_infos) & ARV_GVSP_PACKET_INFOS_CONTENT_TYPE_MASK) >>
					     ARV_GVSP_PACKET_INFOS_CONTENT_TYPE_POS);
	}
}

static inline guint32
arv_gvsp_packet_get_packet_id (const ArvGvspPacket *packet)
{
	if (arv_gvsp_packet_has_extended_ids (packet)) {
		ArvGvspExtendedHeader *header = (void *) &packet->header;

		return g_ntohl (header->packet_id);
	} else {
		ArvGvspHeader *header = (void *) &packet->header;

		return g_ntohl (header->packet_infos) & ARV_GVSP_PACKET_ID_MASK;
	}
}

static inline guint64
arv_gvsp_packet_get_frame_id (const ArvGvspPacket *packet)
{
	if (arv_gvsp_packet_has_extended_ids (packet)) {
		ArvGvspExtendedHeader *header = (void *) &packet->header;

		return GUINT64_FROM_BE(header->frame_id);
	} else {
		ArvGvspHeader *header = (void *) &packet->header;

		return g_ntohs (header->frame_id);
	}
}

static inline void *
arv_gvsp_packet_get_data (const ArvGvspPacket *packet)
{
	if (arv_gvsp_packet_has_extended_ids (packet)) {
		ArvGvspExtendedHeader *header = (void *) &packet->header;

		return &header->data;
	} else {
		ArvGvspHeader *header = (void *) &packet->header;

		return &header->data;
	}
}

static inline ArvBufferPayloadType
arv_gvsp_packet_get_buffer_payload_type (const ArvGvspPacket *packet)
{
	ArvGvspDataLeader *leader;
	ArvGvspPayloadType gvsp_payload_type;

	leader = arv_gvsp_packet_get_data (packet);

	gvsp_payload_type = (ArvGvspPayloadType) g_ntohs (leader->payload_type);

	switch (gvsp_payload_type) {
		case ARV_GVSP_PAYLOAD_TYPE_IMAGE:
			return ARV_BUFFER_PAYLOAD_TYPE_IMAGE;
		case ARV_GVSP_PAYLOAD_TYPE_RAWDATA:
			return ARV_BUFFER_PAYLOAD_TYPE_RAWDATA;
		case ARV_GVSP_PAYLOAD_TYPE_FILE:
			return ARV_BUFFER_PAYLOAD_TYPE_FILE;
		case ARV_GVSP_PAYLOAD_TYPE_CHUNK_DATA:
			return ARV_BUFFER_PAYLOAD_TYPE_CHUNK_DATA;
		case ARV_GVSP_PAYLOAD_TYPE_EXTENDED_CHUNK_DATA:
			return ARV_BUFFER_PAYLOAD_TYPE_EXTENDED_CHUNK_DATA;
		case ARV_GVSP_PAYLOAD_TYPE_JPEG:
			return ARV_BUFFER_PAYLOAD_TYPE_JPEG;
		case ARV_GVSP_PAYLOAD_TYPE_JPEG2000:
			return ARV_BUFFER_PAYLOAD_TYPE_JPEG2000;
		case ARV_GVSP_PAYLOAD_TYPE_H264:
			return ARV_BUFFER_PAYLOAD_TYPE_H264;
		case ARV_GVSP_PAYLOAD_TYPE_MULTIZONE_IMAGE:
			return ARV_BUFFER_PAYLOAD_TYPE_MULTIZONE_IMAGE;
		case ARV_GVSP_PAYLOAD_TYPE_IMAGE_EXTENDED_CHUNK:
			return ARV_BUFFER_PAYLOAD_TYPE_IMAGE_EXTENDED_CHUNK;
	}

	return ARV_BUFFER_PAYLOAD_TYPE_UNKNOWN;
}

static inline guint32
arv_gvsp_packet_get_x_offset (const ArvGvspPacket *packet)
{
	ArvGvspDataLeader *leader;

	leader = arv_gvsp_packet_get_data (packet);
	return g_ntohl (leader->x_offset);
}

static inline guint32
arv_gvsp_packet_get_y_offset (const ArvGvspPacket *packet)
{
	ArvGvspDataLeader *leader;

	leader = arv_gvsp_packet_get_data (packet);
	return g_ntohl (leader->y_offset);
}

static inline guint32
arv_gvsp_packet_get_width (const ArvGvspPacket *packet)
{
	ArvGvspDataLeader *leader;

	leader = arv_gvsp_packet_get_data (packet);
	return g_ntohl (leader->width);
}

static inline guint32
arv_gvsp_packet_get_height (const ArvGvspPacket *packet)
{
	ArvGvspDataLeader *leader;

	leader = arv_gvsp_packet_get_data (packet);
	return g_ntohl (leader->height);
}

static inline ArvPixelFormat
arv_gvsp_packet_get_pixel_format (const ArvGvspPacket *packet)
{
	ArvGvspDataLeader *leader;

	leader = arv_gvsp_packet_get_data (packet);
	return (ArvPixelFormat) g_ntohl (leader->pixel_format);
}

static inline guint64
arv_gvsp_packet_get_timestamp (const ArvGvspPacket *packet, guint64 timestamp_tick_frequency)
{
	ArvGvspDataLeader *leader;
	guint64 timestamp_s;
	guint64 timestamp_ns;
	guint64 timestamp;

	if (timestamp_tick_frequency < 1)
		return 0;

	leader = arv_gvsp_packet_get_data (packet);

	timestamp = ( (guint64) g_ntohl (leader->timestamp_high) << 32) | g_ntohl (leader->timestamp_low);

	timestamp_s = timestamp / timestamp_tick_frequency;
	timestamp_ns = ((timestamp % timestamp_tick_frequency) * 1000000000) / timestamp_tick_frequency;

	timestamp_ns += timestamp_s * 1000000000;

	return timestamp_ns;
}

static inline size_t
arv_gvsp_packet_get_data_size (const ArvGvspPacket *packet, size_t packet_size)
{
	if (arv_gvsp_packet_has_extended_ids (packet))
		return packet_size - sizeof (ArvGvspPacket) - sizeof (ArvGvspExtendedHeader);
	else
		return packet_size - sizeof (ArvGvspPacket) - sizeof (ArvGvspHeader);
}

G_END_DECLS

#endif
