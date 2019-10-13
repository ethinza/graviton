///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2018-2019 Ethinza Inc, All rights reserved
//
// This file contains TRADE SECRETs of ETHINZA INC and is CONFIDENTIAL.
// Without written consent from ETHINZA INC, it is prohibited to disclose or
// reproduce its contents, or to manufacture, use or sell it in whole or part.
// Any reproduction of this file without written consent of ETHINZA INC is a 
// violation of the copyright laws and is subject to civil liability and 
// criminal prosecution.
//
///////////////////////////////////////////////////////////////////////////////

#include "protocol.h"
#include <utility/memoryutility.h>

/**
 * @brief Populates most header fields
 * @param header
 */
void Protocol::PrepareHeader(struct Header& header,
                            PacketId packetId,
                            PacketType packetType,
                            u16_t sequence,
                            u8_t retry,
                            u16_t packetLength)
{
    header.signature = MemoryUtility::CheckAndSwap(static_cast<u16_t>(Constant::Signature));
    header.version = MemoryUtility::CheckAndSwap(static_cast<u8_t>(Constant::Version));
    header.length = MemoryUtility::CheckAndSwap(packetLength);
    header.node_id = MemoryUtility::CheckAndSwap(m_nodeId);
    header.node_type = MemoryUtility::CheckAndSwap(static_cast<u8_t>(m_nodeType));
    header.sequence = MemoryUtility::CheckAndSwap(sequence);
    header.retry_count = MemoryUtility::CheckAndSwap(retry);
    header.packet_id = MemoryUtility::CheckAndSwap(static_cast<u8_t>(packetId));
    header.packet_type = MemoryUtility::CheckAndSwap(static_cast<u8_t>(packetType));
    header.reserved = 0;
    header.reserved2 = 0;
}
