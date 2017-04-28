#include "RealTimeProcessor.h"

void RealTimeProcessor_MDP::Run()
{
  for (;;)
  {
    //--------------------------------------------------
    // System shutdown
    //--------------------------------------------------
    if (m_ShrObj->ThreadShouldExit())
    {
      m_Logger->Write(Logger::NOTICE,"RealTimeProcessor: ChannelID:%u. Stopping thread.", m_ChannelID);
      return;
    }

    //--------------------------------------------------
    // Report Health
    //--------------------------------------------------
    m_ThreadHealthMon->ReportThatIAmHealthy(ThreadHealthMonitor::REALTIMEPROCESSOR, m_ChannelID);

    //--------------------------------------------------
    // Must stop RealTimeProcessor if Refresh mode is activated
    // Remember that we can trigger refresh manually from our command terminal
    //--------------------------------------------------
    if (!m_ShrObj->CheckCapTestStatus() && m_ShrObj->CheckRefreshActivatnStatus(m_ChannelID))
    {
      boost::this_thread::sleep(boost::posix_time::seconds(1)); // force sleep 1 sleep
      m_Logger->Write(Logger::NOTICE,"RealTimeProcessor: ChannelID:%u. Refresh mode is on. RealTimeProcessor will halt processing until refresh is done.", m_ChannelID);

      //--------------------------------------------------
      // Warn when cir buffer is too long
      //--------------------------------------------------
      if (m_MsgCirBuf->Size() > 10000)
        m_Logger->Write(Logger::WARNING,"RealTimeProcessor: ChannelID:%u. Detected abnormally long message circular buffer with size %u. Please investigate.", m_ChannelID, m_MsgCirBuf->Size());

      continue;
    }

    //--------------------------------------------------
    // Try get data from queue
    //--------------------------------------------------
    BYTE *pbPktMut = NULL;
    uint32_t uiPktSeqNo = 0;
    uint64_t ulTS = 0;
    StateOfNextSeqNo snsn = m_MsgCirBuf->GetMsgSeqNoTStamp(pbPktMut, &uiPktSeqNo, &ulTS);
    if (snsn == ALL_RETRIEVED)
    {
      m_MsgCirBuf->WaitForData();
      continue;
    }
    else if (snsn == SEQNO_MISSING)
    {
      if (m_ShrObj->CheckCapTestStatus()) m_MsgCirBuf->PopFront();
      else                                m_MsgCirBuf->WaitForData();
      continue;
    }
    const BYTE *pbPkt = pbPktMut;

    //--------------------------------------------------
    // Output Debug info
    //--------------------------------------------------
    m_Logger->Write(Logger::DEBUG,"RealTimeProcessor: ChannelID:%u. m_MsgCirBuf.Size()          %u", m_ChannelID, m_MsgCirBuf->Size());
    m_Logger->Write(Logger::DEBUG,"RealTimeProcessor: ChannelID:%u. m_MsgCirBuf.AllocatedSize() %u", m_ChannelID, m_MsgCirBuf->AllocatedSize());
    m_Logger->Write(Logger::DEBUG,"RealTimeProcessor: ChannelID:%u. Pkt Seq No:                 %u", m_ChannelID, uiPktSeqNo);
    m_Logger->Write(Logger::DEBUG,"RealTimeProcessor: ChannelID:%u. Message Header: Local Timestamp of Pkt: %s", m_ChannelID, SDateTime::fromUnixTimeToString(ulTS,SDateTime::MILLISEC).c_str());


    //--------------------------------------------------
    // Record canned data
    //--------------------------------------------------
    m_BinaryRecorder.WriteHKExUnixTime(pbPkt);

    //--------------------------------------------------
    Modified_MDP_Packet_Header * mmph = (Modified_MDP_Packet_Header*)(pbPkt);
    mktdata::MessageHeader mdpMsgHdr;
    int iOffset = sizeof(Modified_MDP_Packet_Header);

    while (iOffset < mmph->PktSize)
    {
      int16_t iMsgSize = READ_INT16(&pbPkt[iOffset]);
      const size_t iWrap2 = iOffset + sizeof(int16_t);
      const size_t iWrap3 = iMsgSize - sizeof(int16_t);
      const size_t iWrap4 = iOffset + iMsgSize;

      //--------------------------------------------------
      m_Logger->Write(Logger::DEBUG,"RealTimeProcessor: ChannelID:%u. Message Header: Msg size: %d", m_ChannelID, iMsgSize);
      if (iMsgSize <= 0) break;

      //--------------------------------------------------
      m_Logger->Write(Logger::DEBUG,"RealTimeProcessor: ChannelID:%u. Message Header: iWrap2    %d", m_ChannelID, iWrap2);
      m_Logger->Write(Logger::DEBUG,"RealTimeProcessor: ChannelID:%u. Message Header: iWrap3    %d", m_ChannelID, iWrap3);
      m_Logger->Write(Logger::DEBUG,"RealTimeProcessor: ChannelID:%u. Message Header: iWrap4    %d", m_ChannelID, iWrap4);

      mdpMsgHdr.wrap((char*)pbPkt, iWrap2, MDP_VERSION, iWrap4);

      //--------------------------------------------------
      m_Logger->Write(Logger::DEBUG,"RealTimeProcessor: ChannelID:%u. Message Header: Msg type:        %u", m_ChannelID, mdpMsgHdr.templateId());

      switch (mdpMsgHdr.templateId())
      {
        case MDP_HEARTBEAT                           : m_DataProcFunc->OnHeartBeat                        ();                                                           break;
        case MDP_CHANNEL_RESET                       : m_DataProcFunc->OnChannelReset                     (mdpMsgHdr, (char*)pbPkt + iWrap2, iWrap3);                   break;
        case MDP_REFRESH_SECURITY_DEFINITION_FUTURE  : m_DataProcFunc->OnRefreshSecurityDefinitionFuture  (mdpMsgHdr, (char*)pbPkt + iWrap2, iWrap3);                   break;
        case MDP_REFRESH_SECURITY_DEFINITION_SPREAD  : m_DataProcFunc->OnRefreshSecurityDefinitionSpread  (mdpMsgHdr, (char*)pbPkt + iWrap2, iWrap3);                   break;
        case MDP_SECURITY_STATUS                     : m_DataProcFunc->OnSecurityStatus                   (mdpMsgHdr, (char*)pbPkt + iWrap2, iWrap3);                   break;
        case MDP_REFRESH_BOOK                        : m_DataProcFunc->OnRefreshBook                      (mdpMsgHdr, (char*)pbPkt + iWrap2, iWrap3);                   break;
        case MDP_REFRESH_DAILY_STATISTICS            : m_DataProcFunc->OnRefreshDailyStatistics           (mdpMsgHdr, (char*)pbPkt + iWrap2, iWrap3);                   break;
        case MDP_REFRESH_LIMITS_BANDING              : m_DataProcFunc->OnRefreshLimitsBanding             (mdpMsgHdr, (char*)pbPkt + iWrap2, iWrap3);                   break;
        case MDP_REFRESH_SESSION_STATISTICS          : m_DataProcFunc->OnRefreshSessionStatistics         (mdpMsgHdr, (char*)pbPkt + iWrap2, iWrap3);                   break;
        case MDP_REFRESH_TRADE                       : m_DataProcFunc->OnRefreshTrade                     (mdpMsgHdr, (char*)pbPkt + iWrap2, iWrap3);                   break;
        case MDP_REFRESH_VOLUME                      : m_DataProcFunc->OnRefreshVolume                    (mdpMsgHdr, (char*)pbPkt + iWrap2, iWrap3);                   break;
        case MDP_SNAPSHOT_FULL_REFRESH               : m_DataProcFunc->OnSnapshotFullRefresh              (mdpMsgHdr, (char*)pbPkt + iWrap2, iWrap3);                   break;
        case MDP_QUOTE_REQUEST                       : m_DataProcFunc->OnQuoteRequest                     (mdpMsgHdr, (char*)pbPkt + iWrap2, iWrap3);                   break;
        case MDP_INSTRUMENT_DEFINITION_OPTION        : m_DataProcFunc->OnInstrumentDefinitionOption       (mdpMsgHdr, (char*)pbPkt + iWrap2, iWrap3);                   break;
        case MDP_REFRESH_TRADE_SUMMARY               : m_DataProcFunc->OnRefreshTradeSummary              (mdpMsgHdr, (char*)pbPkt + iWrap2, iWrap3);                   break;
        default                                      : m_Logger->Write(Logger::DEBUG,"Unprocessed message. templateId: %d", mdpMsgHdr.templateId());                    break;
      }

      //--------------------------------------------------
      // Process message for omd_mdi
      //--------------------------------------------------
      m_DataProcFunc->ProcessMessageForTransmission(pbPkt, mdpMsgHdr.templateId());
      //--------------------------------------------------

      //--------------------------------------------------
      // Special handling for order book 
      //--------------------------------------------------
      if (mdpMsgHdr.templateId() == OMDC_AGGREGATE_ORDER_BOOK_UPDATE ||
          mdpMsgHdr.templateId() == OMDD_AGGREGATE_ORDER_BOOK_UPDATE)
      {
        m_DataProcFunc->ProcessOrderBookInstruction(__FILE__,m_Logger,pbPkt,m_ShrObj,m_PrintOrderBookAsInfo);
      }

      iOffset += iMsgSize;
    }

    m_MsgCirBuf->PopFront();
  }
}
