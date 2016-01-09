#include <MarketData.h>

boost::weak_ptr<MarketData> MarketData::m_pInstance;

boost::shared_ptr<MarketData> MarketData::Instance() {
  boost::shared_ptr<MarketData> instance = m_pInstance.lock();
  if (!instance) {
    instance.reset(new MarketData());
    m_pInstance = instance;
  }
  return instance;
}

MarketData::MarketData()
{
  m_Logger = Logger::Instance();
  m_SysCfg = SystemConfig::Instance();
  m_Exchg  = Exchange::Instance();
  m_HKFE   = HKFE::Instance();
  m_HKSE   = HKSE::Instance();
  m_NYSE   = NYSE::Instance();
  m_CME    = CME::Instance();
  m_MDIAck = MDI_Acknowledgement::Instance();
  m_ContFut.reset(new ContFut());
}

MarketData::~MarketData()
{

  {
    map<string,ATU_MDI_marketfeed_struct*>::iterator it;
    for (it = m_LatestSnapshots.begin(); it != m_LatestSnapshots.end(); ++it)
    {
      delete it->second;
      it->second = NULL;
    }
  }

  {
    map<string,YYYYMMDDHHMMSS*>::iterator it;
    for (it = m_LatestSnapshotsUpdateTime.begin(); it != m_LatestSnapshotsUpdateTime.end(); ++it)
    {
      delete it->second;
      it->second = NULL;
    }
  }

  {
    map<string,shared_mutex*>::iterator it;
    for (it = m_LatestSnapshotsMutex.begin(); it != m_LatestSnapshotsMutex.end(); ++it)
    {
      delete it->second;
      it->second = NULL;
    }
  }

  {
    map<string,shared_mutex*>::iterator it;
    for (it = m_LastAccumTradeVolumeMutex.begin(); it != m_LastAccumTradeVolumeMutex.end(); ++it)
    {
      delete it->second;
      it->second = NULL;
    }
  }
  {
    map<string,BarAggregator*>::iterator it;
    for (it = m_1DayBarAgg.begin(); it != m_1DayBarAgg.end(); ++it)
    {
      delete it->second;
      it->second = NULL;
    }
  }
  {
    map<string,BarProvider*>::iterator it;
    for (it = m_1DayBarProv.begin(); it != m_1DayBarProv.end(); ++it)
    {
      delete it->second;
      it->second = NULL;
    }
  }

  {
    map<string,shared_mutex*>::iterator it;
    for (it = m_1DayBarsMutex.begin(); it != m_1DayBarsMutex.end(); ++it)
    {
      delete it->second;
      it->second = NULL;
    }
  }

  {
    map<string,BarAggregator*>::iterator it;
    for (it = m_1MinBarAgg.begin(); it != m_1MinBarAgg.end(); ++it)
    {
      delete it->second;
      it->second = NULL;
    }
  }
  {
    map<string,BarProvider*>::iterator it;
    for (it = m_1MinBarProv.begin(); it != m_1MinBarProv.end(); ++it)
    {
      delete it->second;
      it->second = NULL;
    }
  }
  {
    map<string,shared_mutex*>::iterator it;
    for (it = m_1MinBarsMutex.begin(); it != m_1MinBarsMutex.end(); ++it)
    {
      delete it->second;
      it->second = NULL;
    }
  }

  {
    map<string,BarProvider*>::iterator it;
    for (it = m_SupplementaryDayBar.begin(); it != m_SupplementaryDayBar.end(); ++it)
    {
      delete it->second;
      it->second = NULL;
    }
  }

  {
    map<string,BarProvider*>::iterator it;
    for (it = m_SupplementaryM1Bar.begin(); it != m_SupplementaryM1Bar.end(); ++it)
    {
      delete it->second;
      it->second = NULL;
    }
  }

}

bool MarketData::ParseMDIString(const string & sMD, vector<string> & vMD, YYYYMMDD & yyyymmdd, HHMMSS & hhmmss)
{
  vMD.clear();
  boost::split(vMD, sMD, boost::is_any_of(","));

  //--------------------------------------------------
  // 1 for time stamp
  // 1 for feed code
  // 2 for trade price and volume
  // 11 for bid queue
  // 11 for ask queue
  //--------------------------------------------------
  if (vMD.size() != 26 &&
      vMD.size() != 4)
  {
    m_Logger->Write(Logger::DEBUG,"MarketData: Discarded marketdata with incorrect length. Received MD: %s",sMD.c_str());
    return false;
  }

  if (vMD[1] == "acknowledgement")
  {
    m_Logger->Write(Logger::NOTICE,"MarketData: MDI gave us an acknowledgement market feed?!? Received MD: %s",sMD.c_str());
    return false;
  }

  SDateTime::FromCashTSToYMDHMS(vMD[0],yyyymmdd,hhmmss);

  return true;
}

void MarketData::UpdateInternalDataWithParsedMDIString(const vector<string> & vMD, const YYYYMMDD & yyyymmdd, const HHMMSS & hhmmss)
{

  ATU_MDI_marketfeed_struct sMD;

  sMD.m_timestamp     =                             vMD[ 0] ;
  sMD.m_feedcode      =                             vMD[ 1] ;
  sMD.m_traded_price  = boost::lexical_cast<double>(vMD[ 2]);
  sMD.m_traded_volume = boost::lexical_cast<double>(vMD[ 3]);
  sMD.m_bid_price_1   = boost::lexical_cast<double>(vMD[ 5]);
  sMD.m_bid_volume_1  = boost::lexical_cast<double>(vMD[ 6]);
  sMD.m_ask_price_1   = boost::lexical_cast<double>(vMD[16]);
  sMD.m_ask_volume_1  = boost::lexical_cast<double>(vMD[17]);

  UpdateInternalDataWithParsedMDIString(sMD, yyyymmdd, hhmmss);
  return;
}


void MarketData::UpdateInternalDataWithParsedMDIString(const ATU_MDI_marketfeed_struct & sMDa, const YYYYMMDD & yyyymmdd, const HHMMSS & hhmmss)
{

  //--------------------------------------------------
  UpdateSystemTime(yyyymmdd,hhmmss);
  //--------------------------------------------------

  //--------------------------------------------------
  // Clear Snapshots if needed
  //--------------------------------------------------
  if (
    (m_MDITimeHKT_YYYYMMDDHHMMSS - m_LastMonitorTime > 12*60*60) ||
    (m_LastMonitorTime.GetHHMMSS() < HHMMSS(500) && m_MDITimeHKT_YYYYMMDDHHMMSS.GetHHMMSS() >= HHMMSS(500))
    )
  {
    CleanUpOldSnapshots();
  }
  m_LastMonitorTime.Set(yyyymmdd,hhmmss);

  //--------------------------------------------------
  // Corp action adjustment
  //--------------------------------------------------
  ATU_MDI_marketfeed_struct sMDb;
  const ATU_MDI_marketfeed_struct * p_sMD = NULL;
  {
    double price_delta = 0;
    double price_ratio = 1;
    bool bCorpAction = false;

    const string & symbol = sMDa.m_feedcode;
    bool bHKT = (m_Exchg->GetPrimaryExchange(symbol) == EX_HKFE || m_Exchg->GetPrimaryExchange(symbol) == EX_HKSE);
    bool bUST = (m_Exchg->GetPrimaryExchange(symbol) == EX_NYSE || m_Exchg->GetPrimaryExchange(symbol) == EX_CME);

    if      (bHKT) bCorpAction = m_SysCfg->GetCorpActionAdj(symbol,m_MDITimeHKT_YYYYMMDDHHMMSS.GetYYYYMMDD(),price_delta,price_ratio);
    else if (bUST) bCorpAction = m_SysCfg->GetCorpActionAdj(symbol,m_MDITimeEST_YYYYMMDDHHMMSS.GetYYYYMMDD(),price_delta,price_ratio);

    if (bCorpAction)
    {
      sMDb = sMDa;

      if (price_delta == 0 && price_ratio != 1)
      {
        //--------------------------------------------------
        // Adjust by price ratio
        //--------------------------------------------------
        sMDb.m_traded_price *= price_ratio;
        sMDb.m_bid_price_1  *= price_ratio;
        sMDb.m_ask_price_1  *= price_ratio;
      }
      else if (price_delta != 0 && price_ratio == 1)
      {
        //--------------------------------------------------
        // Adjust by price delta
        //--------------------------------------------------
        sMDb.m_traded_price += price_delta;
        sMDb.m_bid_price_1  += price_delta;
        sMDb.m_ask_price_1  += price_delta;
      }
      p_sMD = &sMDb;
    }
    else
    {
      p_sMD = &sMDa;
    }
  }
  //--------------------------------------------------

  //--------------------------------------------------
  // Try to filter out erroneous ticks
  // This has to be done after adjusting for corporate actions because our internal snapshot is after corporate adjustment
  //--------------------------------------------------
  if (m_SysCfg->GetErroneousTickPxChg() > 0)
  {
    const string & symbol    = p_sMD->m_feedcode;
    const double price       = p_sMD->m_traded_price;
    const double dTickReturn = ComputeTickReturn(symbol,price);

    if (
      (std::isnan(price))
      ||
      (std::isnan(dTickReturn))
      ||
      (!std::isnan(dTickReturn) && fabs(dTickReturn) > m_SysCfg->GetErroneousTickPxChg())
      )
    {
      if (m_SysCfg->Get_TCPOrEmbeddedMode() == SystemConfig::TCPWITHACK || m_SysCfg->Get_TCPOrEmbeddedMode() == SystemConfig::EMBEDDED)
      {
        m_MDIAck->ReportNotMktData();
      }
      m_Logger->Write(Logger::INFO,"MarketData: Discarded bad tick: %s %s %s Traded Price = %f Tick Return = %f",
                      yyyymmdd.ToStr_().c_str(),hhmmss.ToStr_().c_str(),
                      symbol.c_str(), price, dTickReturn);
      return;
    }
  }
  //--------------------------------------------------


  //--------------------------------------------------
  // Update Bars
  //--------------------------------------------------
  if (m_SysCfg->Get_MktDataTradeVolumeMode() == SystemConfig::ACCUMULATED_VOLUME_MODE)
  {
    double dCurAccVol = p_sMD->m_traded_volume;
    double dLastAccVol = GetLastAccumTradeVolume(p_sMD->m_feedcode);
    if (dCurAccVol < dLastAccVol)
    {
      UpdateBarAggregators(yyyymmdd,hhmmss,p_sMD->m_feedcode,p_sMD->m_traded_price,dCurAccVol);
    }
    else
    {
      UpdateBarAggregators(yyyymmdd,hhmmss,p_sMD->m_feedcode,p_sMD->m_traded_price,dCurAccVol-dLastAccVol);
    }
    UpdateLastAccumTradeVolume(p_sMD->m_feedcode,dCurAccVol);
  }
  else
  {
    UpdateBarAggregators(yyyymmdd,hhmmss,p_sMD->m_feedcode,p_sMD->m_traded_price,p_sMD->m_traded_volume);
  }

  //--------------------------------------------------
  // Update order book snapshots
  //--------------------------------------------------
  UpdateLatestSnapshot(yyyymmdd,hhmmss,*p_sMD);

  //--------------------------------------------------
  // Construction of continuous futures for HSI and HHI
  //--------------------------------------------------
  UpdateBarAggregatorsAndLatestSnapshotContFut(YYYYMMDDHHMMSS(yyyymmdd,hhmmss),*p_sMD);

  //--------------------------------------------------
  const string & sym = p_sMD->m_feedcode;
  AddUpdatedSymbol(sym);
  if (m_ContFut->DoesSymHaveContFut(sym)) AddUpdatedSymbol(ContFut::GetContFutFrRglrSym(sym,1));

  //--------------------------------------------------

  return;
}


bool MarketData::UpdateMarketData(const string & sMD)
{
  YYYYMMDD yyyymmdd;
  HHMMSS hhmmss;
  vector<string> vMD;

  if (!ParseMDIString(sMD,vMD,yyyymmdd,hhmmss))
  {
    if (m_SysCfg->Get_TCPOrEmbeddedMode() == SystemConfig::TCPWITHACK || m_SysCfg->Get_TCPOrEmbeddedMode() == SystemConfig::EMBEDDED)
    {
      m_MDIAck->ReportNotMktData();
    }
    m_Logger->Write(Logger::INFO,"MarketData: Failed to parse string from MDI: %s", sMD.c_str());
    return false;
  }

  UpdateInternalDataWithParsedMDIString(vMD,yyyymmdd,hhmmss);

  NotifyConsumers();
  return true;
}


bool MarketData::UpdateMarketData(const ATU_MDI_marketfeed_struct & sMD)
{
  YYYYMMDD yyyymmdd;
  HHMMSS hhmmss;

  try
  {
    //--------------------------------------------------
    // Determine the format of timestamp
    //--------------------------------------------------
    vector<string> vTS;
    boost::split(vTS, sMD.m_timestamp, boost::is_any_of("_"));

    if (vTS.size() >= 4)
    {
      return false;
    }
    else if (vTS.size() == 3)
    {
      yyyymmdd.Set(boost::lexical_cast<int>(vTS[0]));
      hhmmss.Set(boost::lexical_cast<int>(vTS[1]));
    }
    else if (vTS.size() == 2)
    {
      yyyymmdd.Set(boost::lexical_cast<int>(vTS[0]));
      hhmmss.Set(boost::lexical_cast<int>(vTS[1]));
    }
    else // size must be 1
    {
      time_t t = time(0);
      struct tm * now = localtime(&t);
      yyyymmdd.Set((now->tm_year+1900)*10000 + (now->tm_mon+1)*100 + (now->tm_mday));
      hhmmss.Set(boost::lexical_cast<int>(vTS[0]));
    }
  }
  catch (std::exception e)
  {
    m_Logger->Write(Logger::ERROR,"MarketData: Exception: %s. Error parsing market data.",e.what());
    return false;
  }

  UpdateInternalDataWithParsedMDIString(sMD,yyyymmdd,hhmmss);

  NotifyConsumers();
  return true;
}



shared_mutex * MarketData::Get1MinBarsMutex(const string & symbol)
{
  map<string,shared_mutex *>::iterator it = m_1MinBarsMutex.find(symbol);
  if (it != m_1MinBarsMutex.end())
  {
    return it->second;
  }
  else
  {
    shared_mutex * p = new shared_mutex();
    m_1MinBarsMutex[symbol] = p;
    return p;
  }
}

shared_mutex * MarketData::Get1DayBarsMutex(const string & symbol)
{
  map<string,shared_mutex *>::iterator it = m_1DayBarsMutex.find(symbol);
  if (it != m_1DayBarsMutex.end())
  {
    return it->second;
  }
  else
  {
    shared_mutex * p = new shared_mutex();
    m_1DayBarsMutex[symbol] = p;
    return p;
  }
}

shared_mutex * MarketData::GetLatestSnapshotsMutex(const string & symbol)
{
  map<string,shared_mutex *>::iterator it = m_LatestSnapshotsMutex.find(symbol);
  if (it != m_LatestSnapshotsMutex.end())
  {
    return it->second;
  }
  else
  {
    shared_mutex * p = new shared_mutex();
    m_LatestSnapshotsMutex[symbol] = p;
    return p;
  }
}

shared_mutex * MarketData::GetLastAccumTradeVolumeMutex(const string & symbol)
{
  map<string,shared_mutex *>::iterator it = m_LastAccumTradeVolumeMutex.find(symbol);
  if (it != m_LastAccumTradeVolumeMutex.end())
  {
    return it->second;
  }
  else
  {
    shared_mutex * p = new shared_mutex();
    m_LastAccumTradeVolumeMutex[symbol] = p;
    return p;
  }
}

YYYYMMDDHHMMSS MarketData::GetSystemTimeHKT()
{
  boost::shared_lock<boost::shared_mutex> lock(m_MDITime_Mutex);
  return m_MDITimeHKT_YYYYMMDDHHMMSS;
}
YYYYMMDDHHMMSS MarketData::GetSystemTimeEST()
{
  boost::shared_lock<boost::shared_mutex> lock(m_MDITime_Mutex);
  return m_MDITimeEST_YYYYMMDDHHMMSS;
}

bool MarketData::GetLatestBestBidAskMid(const string & sSymbol, double & dBestBid,double & dBestAsk,double & dMidQuote,YYYYMMDDHHMMSS & ymdhms)
{
  //--------------------------------------------------
  // Continuous Futures
  //--------------------------------------------------
  if (ContFut::IsSymContFut(sSymbol))
  {
    ATU_MDI_marketfeed_struct mdi_struct;
    if (m_ContFut->GetContFutLatestSnapshot(sSymbol,mdi_struct,ymdhms))
    {
      dBestBid = mdi_struct.m_bid_price_1;
      dBestAsk = mdi_struct.m_ask_price_1;
      //--------------------------------------------------
      // Weighted mid-quote
      //--------------------------------------------------
      dMidQuote = (mdi_struct.m_bid_price_1 * mdi_struct.m_ask_volume_1 + mdi_struct.m_ask_price_1 * mdi_struct.m_bid_volume_1) / (mdi_struct.m_bid_volume_1 + mdi_struct.m_ask_volume_1);
      return true;
    }
    else return false;
  }


  boost::shared_lock<boost::shared_mutex> lock(*(GetLatestSnapshotsMutex(sSymbol)));

  map<string,ATU_MDI_marketfeed_struct*>::iterator it1 = m_LatestSnapshots.find(sSymbol);
  map<string,YYYYMMDDHHMMSS*>::iterator it2 = m_LatestSnapshotsUpdateTime.find(sSymbol);

  if (it1 == m_LatestSnapshots.end() || it2 == m_LatestSnapshotsUpdateTime.end())
  {
    return false;
  }
  else if (!it1->second || !it2->second)
  {
    return false;
  }
  else if (!STool::IsValidPrice(it1->second->m_bid_price_1) || !STool::IsValidPrice(it1->second->m_ask_price_1))
  {
    return false;
  }
  else
  {
    ymdhms.Set(*(it2->second));
    dBestBid = it1->second->m_bid_price_1;
    dBestAsk = it1->second->m_ask_price_1;
    dMidQuote = (it1->second->m_bid_price_1 * it1->second->m_ask_volume_1 + it1->second->m_ask_price_1 * it1->second->m_bid_volume_1) / (it1->second->m_bid_volume_1 + it1->second->m_ask_volume_1);
    return true;
  }
}

bool MarketData::GetLatestMidQuote(const string & sSymbol, double & dMidQuote)
{
  YYYYMMDDHHMMSS ymdhms;
  return GetLatestMidQuote(sSymbol, dMidQuote, ymdhms);
}
bool MarketData::GetLatestMidQuote(const string & sSymbol, double & dMidQuote, YYYYMMDDHHMMSS & ymdhms)
{
  //--------------------------------------------------
  // Continuous Future
  //--------------------------------------------------
  if (ContFut::IsSymContFut(sSymbol))
  {
    ATU_MDI_marketfeed_struct mdi_struct;
    if (m_ContFut->GetContFutLatestSnapshot(sSymbol,mdi_struct,ymdhms))
    {
      dMidQuote = (mdi_struct.m_bid_price_1 * mdi_struct.m_ask_volume_1 + mdi_struct.m_ask_price_1 * mdi_struct.m_bid_volume_1) / (mdi_struct.m_bid_volume_1 + mdi_struct.m_ask_volume_1);
      return true;
    }
    else return false;
  }
  //--------------------------------------------------

  boost::shared_lock<boost::shared_mutex> lock(*(GetLatestSnapshotsMutex(sSymbol)));

  map<string,ATU_MDI_marketfeed_struct*>::iterator it1 = m_LatestSnapshots.find(sSymbol);
  map<string,YYYYMMDDHHMMSS*>::iterator it2 = m_LatestSnapshotsUpdateTime.find(sSymbol);

  if (it1 == m_LatestSnapshots.end() || it2 == m_LatestSnapshotsUpdateTime.end())
  {
    return false;
  }
  else if (!it1->second || !it2->second)
  {
    return false;
  }
  else
  {
    //--------------------------------------------------
    // Index
    //--------------------------------------------------
    if (sSymbol == UNDERLYING_HSI || sSymbol == UNDERLYING_HHI)
    {
      dMidQuote = it1->second->m_traded_price;
      ymdhms.Set(*(it2->second));
      return true;
    }
    else
    {
      if (!STool::IsValidPrice(it1->second->m_bid_price_1) || !STool::IsValidPrice(it1->second->m_ask_price_1))
      {
        return false;
      }
      else
      {
        dMidQuote = (it1->second->m_bid_price_1 * it1->second->m_ask_volume_1 + it1->second->m_ask_price_1 * it1->second->m_bid_volume_1) / (it1->second->m_bid_volume_1 + it1->second->m_ask_volume_1);
        ymdhms.Set(*(it2->second));
        return true;
      }
    }
  }
}

void MarketData::UpdateSystemTime(const YYYYMMDD & yyyymmdd, const HHMMSS & hhmmss)
{
  boost::unique_lock<boost::shared_mutex> lock(m_MDITime_Mutex);

  m_MDITimeHKT_YYYYMMDDHHMMSS.Set(yyyymmdd,hhmmss);
  m_MDITimeEST_YYYYMMDDHHMMSS.Set(SDateTime::ChangeTimeZone(m_MDITimeHKT_YYYYMMDDHHMMSS,m_SysCfg->GetDefaultTimeZone(),SDateTime::EST));

  if (m_LastPrintedMDITime_YYYYMMDDHHMMSS.GetYYYYMMDD() == YYYYMMDD::BAD_DATE)
  {
    m_LastPrintedMDITime_YYYYMMDDHHMMSS.Set(yyyymmdd,hhmmss);
  }
  else
  {
    if (m_MDITimeHKT_YYYYMMDDHHMMSS - m_LastPrintedMDITime_YYYYMMDDHHMMSS > 60)
    {
      m_Logger->Write(Logger::INFO,"MarketData: System time HKT: %s",m_MDITimeHKT_YYYYMMDDHHMMSS.ToStr().c_str());
      m_LastPrintedMDITime_YYYYMMDDHHMMSS.Set(yyyymmdd,hhmmss);
    }
  }

  if (m_LastMonitorTime.GetYYYYMMDD() == YYYYMMDD::BAD_DATE)
  {
    m_LastMonitorTime.Set(yyyymmdd,hhmmss);
  }

}


void MarketData::UpdateBarAggregators(const YYYYMMDD & yyyymmdd, const HHMMSS & hhmmss, const string & symbol, const double dPrice,const long lVolume)
{
  if (!STool::IsValidPrice(dPrice)) return;

  if (m_SysCfg->CheckIfBarAggregationM1Symbol(symbol))
  {
    //--------------------------------------------------
    // Update 1-min BarAggregator
    //--------------------------------------------------
    boost::unique_lock<boost::shared_mutex> lock(*(Get1MinBarsMutex(symbol)));
    map<string,BarAggregator*>::iterator it = m_1MinBarAgg.find(symbol);
    if (it == m_1MinBarAgg.end())
    {
      BarAggregator *ba = new BarAggregator();
      ba->SetBarInterval(BarAggregator::MINUTE);
      m_1MinBarAgg[symbol] = ba;
      it = m_1MinBarAgg.find(symbol);
    }

    it->second->AddTick(yyyymmdd,hhmmss,dPrice,lVolume);
  }
  if (m_SysCfg->CheckIfBarAggregationD1Symbol(symbol))
  {
    //--------------------------------------------------
    // Update 1-day BarAggregator
    //--------------------------------------------------
    boost::unique_lock<boost::shared_mutex> lock(*(Get1DayBarsMutex(symbol)));
    map<string,BarAggregator*>::iterator it = m_1DayBarAgg.find(symbol);
    if (it == m_1DayBarAgg.end())
    {
      BarAggregator *ba = new BarAggregator();
      ba->SetBarInterval(BarAggregator::DAY);
      m_1DayBarAgg[symbol] = ba;
      it = m_1DayBarAgg.find(symbol);
    }

    it->second->AddTick(yyyymmdd,hhmmss,dPrice,lVolume);
  }
}

void MarketData::UpdateLatestSnapshot(const YYYYMMDD & yyyymmdd, const HHMMSS & hhmmss, const ATU_MDI_marketfeed_struct & sMD)
{
  UpdateLatestSnapshot(yyyymmdd, hhmmss, sMD, 0);
  return;
}

void MarketData::UpdateLatestSnapshot(const YYYYMMDD & yyyymmdd, const HHMMSS & hhmmss, const ATU_MDI_marketfeed_struct & sMD, const double price_offset)
{
  //--------------------------------------------------
  // Update Latest Snapshot
  //--------------------------------------------------
  boost::unique_lock<boost::shared_mutex> lock(*(GetLatestSnapshotsMutex(sMD.m_feedcode)));

  map<string,ATU_MDI_marketfeed_struct*>::iterator it = m_LatestSnapshots.find(sMD.m_feedcode);

  if (it == m_LatestSnapshots.end() || !it->second)
  {
    ATU_MDI_marketfeed_struct* sMDI = new ATU_MDI_marketfeed_struct();
    m_LatestSnapshots[sMD.m_feedcode] = sMDI;
    it = m_LatestSnapshots.find(sMD.m_feedcode);
  }

  {
    if (STool::IsValidPrice(sMD.m_traded_price)) it->second->m_traded_price = sMD.m_traded_price + price_offset;
    else                                         it->second->m_traded_price = NAN;

    it->second->m_traded_volume  = sMD.m_traded_volume;
  }

  if (STool::IsValidPrice(sMD.m_bid_price_1)) it->second->m_bid_price_1  = sMD.m_bid_price_1 + price_offset;
  else                                        it->second->m_bid_price_1  = NAN;


  if (STool::IsValidPrice(sMD.m_ask_price_1)) it->second->m_ask_price_1  = sMD.m_ask_price_1 + price_offset;
  else                                        it->second->m_ask_price_1  = NAN;

  it->second->m_bid_volume_1 = boost::lexical_cast<double> (sMD.m_bid_volume_1);
  it->second->m_ask_volume_1 = boost::lexical_cast<double> (sMD.m_ask_volume_1);

  //--------------------------------------------------
  // record update time
  //--------------------------------------------------
  map<string,YYYYMMDDHHMMSS*>::iterator it2 = m_LatestSnapshotsUpdateTime.find(sMD.m_feedcode);
  if (it2 == m_LatestSnapshotsUpdateTime.end() || !it2->second)
  {
    m_LatestSnapshotsUpdateTime[sMD.m_feedcode] = new YYYYMMDDHHMMSS();
    it2 = m_LatestSnapshotsUpdateTime.find(sMD.m_feedcode);
  }
  it2->second->Set(yyyymmdd,hhmmss);
}



void MarketData::UpdateLastAccumTradeVolume(const string & symbol, const long accumvolume)
{
  boost::unique_lock<boost::shared_mutex> lock(*(GetLastAccumTradeVolumeMutex(symbol)));
  m_LastAccumTradeVolume[symbol] = accumvolume;
}


long MarketData::GetLastAccumTradeVolume(const string & symbol)
{
  boost::shared_lock<boost::shared_mutex> lock(*(GetLastAccumTradeVolumeMutex(symbol)));
  map<string,long>::iterator it = m_LastAccumTradeVolume.find(symbol);
  if (it == m_LastAccumTradeVolume.end()) return 0;

  return it->second;
}

bool MarketData::Get1MinOHLCBar(const string & symbol, YYYYMMDD & yyyymmdd, HHMMSS & hhmmss, double & open, double & high, double & low, double & close, long & volume)
{
  boost::unique_lock<boost::shared_mutex> lock(*(Get1MinBarsMutex(symbol)));

  map<string,BarAggregator *>::iterator it = m_1MinBarAgg.find(symbol);
  if (it == m_1MinBarAgg.end()) return false;

  return it->second->GetOHLCBar(yyyymmdd, hhmmss, open, high, low, close, volume);
}
bool MarketData::Peek1MinOHLCBar(const string & symbol, YYYYMMDD & yyyymmdd, HHMMSS & hhmmss, double & open, double & high, double & low, double & close, long & volume)
{
  boost::shared_lock<boost::shared_mutex> lock(*(Get1MinBarsMutex(symbol)));

  map<string,BarAggregator *>::iterator it = m_1MinBarAgg.find(symbol);
  if (it == m_1MinBarAgg.end()) return false;

  return it->second->PeekOHLCBar(yyyymmdd, hhmmss, open, high, low, close, volume);
}
bool MarketData::Get1DayOHLCBar(const string & symbol, YYYYMMDD & yyyymmdd, HHMMSS & hhmmss, double & open, double & high, double & low, double & close, long & volume)
{
  boost::unique_lock<boost::shared_mutex> lock(*(Get1DayBarsMutex(symbol)));

  map<string,BarAggregator *>::iterator it = m_1DayBarAgg.find(symbol);
  if (it == m_1DayBarAgg.end()) return false;

  return it->second->GetOHLCBar(yyyymmdd, hhmmss, open, high, low, close, volume);
}
bool MarketData::Peek1DayOHLCBar(const string & symbol, YYYYMMDD & yyyymmdd, HHMMSS & hhmmss, double & open, double & high, double & low, double & close, long & volume)
{
  boost::shared_lock<boost::shared_mutex> lock(*(Get1DayBarsMutex(symbol)));

  map<string,BarAggregator *>::iterator it = m_1DayBarAgg.find(symbol);
  if (it == m_1DayBarAgg.end()) return false;

  return it->second->PeekOHLCBar(yyyymmdd, hhmmss, open, high, low, close, volume);
}

void MarketData::UpdateBarAggregatorsAndLatestSnapshotContFut(const YYYYMMDDHHMMSS & ymdhms, const ATU_MDI_marketfeed_struct & sMD)
{
  if (!STool::IsValidPrice(sMD.m_traded_price)) return;

  m_ContFut->Add(ymdhms, sMD);
  ATU_MDI_marketfeed_struct mdi_struct;

  set<string> setContFutList = m_ContFut->GetAvbContFut();

  for (set<string>::iterator it = setContFutList.begin(); it != setContFutList.end(); ++it)
  {
    const string contsym = ContFut::GetContFutFrUndly(*it,1);
    if (m_ContFut->GetContFutLatestSnapshot(contsym,mdi_struct))
    {
      UpdateBarAggregators(ymdhms.GetYYYYMMDD(),ymdhms.GetHHMMSS(),contsym,mdi_struct.m_traded_price,1);
    }
  }

  return;
}


void MarketData::CleanUpOldSnapshots()
{
  map<string,YYYYMMDDHHMMSS*>::iterator it;
  for (it = m_LatestSnapshotsUpdateTime.begin(); it != m_LatestSnapshotsUpdateTime.end(); ++it)
  {
    if (!it->second) continue;

    boost::unique_lock<boost::shared_mutex> lock(*(GetLatestSnapshotsMutex(it->first)));

    m_Logger->Write(Logger::DEBUG,"MarketData: CleanUpOldSnapshots. %s", it->first.c_str());
    map<string,ATU_MDI_marketfeed_struct*>::iterator it2 = m_LatestSnapshots.find(it->first);

    delete it2->second;
    it2->second = NULL;
    m_LatestSnapshots.erase(it2);

    delete it->second;
    it->second = NULL;
    m_LatestSnapshotsUpdateTime.erase(it);

  }
  return;
}

void MarketData::DebugPrintBarAgg(const string & symbol)
{
  boost::shared_lock<boost::shared_mutex> lock(*(Get1MinBarsMutex(symbol)));

  map<string,BarAggregator *>::iterator it = m_1MinBarAgg.find(symbol);
  if (it == m_1MinBarAgg.end())
  {
    cout << __FILE__ << "::" << __FUNCTION__ << " (" << __LINE__ << ") BarAggregator for " << symbol << " does not exist." << endl << flush;
    return;
  }

  it->second->DebugPrintContents();

  return;
}

void MarketData::WaitForData()
{
  boost::mutex::scoped_lock lock(m_ObserverMutex);
  m_cvDataAvb.wait(lock); //blocks and returns mutex
  return;
}

void MarketData::NotifyConsumers()
{
  m_cvDataAvb.notify_all();
}


bool MarketData::GetLatestNominalPrice(const string & feedcode, double & nominalprice)
{
  boost::shared_lock<boost::shared_mutex> lock(*(GetLatestSnapshotsMutex(feedcode)));

  map<string,ATU_MDI_marketfeed_struct*>::iterator it = m_LatestSnapshots.find(feedcode);

  if (it == m_LatestSnapshots.end()) return false;

  if (
    !STool::IsValidPrice(it->second->m_bid_price_1) ||
    !STool::IsValidPrice(it->second->m_ask_price_1)
    )
  {
    nominalprice = it->second->m_traded_price;
  }
  else if (it->second->m_traded_price >= it->second->m_bid_price_1 &&
           it->second->m_traded_price <= it->second->m_ask_price_1)
  {
    nominalprice = it->second->m_traded_price;
  }
  else if (it->second->m_traded_price < it->second->m_bid_price_1)
  {
    nominalprice = it->second->m_bid_price_1;
  }
  else if (it->second->m_traded_price > it->second->m_ask_price_1)
  {
    nominalprice = it->second->m_ask_price_1;
  }

  return true;
}

bool MarketData::GetLatestTradePrice(const string & sSymbol, double & tradeprice)
{
  YYYYMMDDHHMMSS ymdhms;
  return GetLatestTradePrice(sSymbol, tradeprice, ymdhms);
}
bool MarketData::GetLatestTradePrice(const string & sSymbol, double & tradeprice, YYYYMMDDHHMMSS & ymdhms)
{
  boost::shared_lock<boost::shared_mutex> lock(*(GetLatestSnapshotsMutex(sSymbol)));

  //--------------------------------------------------
  // Continuous Future
  //--------------------------------------------------
  if (ContFut::IsSymContFut(sSymbol))
  {
    ATU_MDI_marketfeed_struct mdi_struct;
    if (m_ContFut->GetContFutLatestSnapshot(sSymbol,mdi_struct,ymdhms))
    {
      tradeprice = mdi_struct.m_traded_price;
      return true;
    }
    else return true;
  }
  //--------------------------------------------------

  map<string,ATU_MDI_marketfeed_struct*>::iterator it = m_LatestSnapshots.find(sSymbol);

  if (it == m_LatestSnapshots.end()) return false;
  if (!STool::IsValidPrice(it->second->m_traded_price)) return false;

  tradeprice = it->second->m_traded_price;

  return true;
}


bool MarketData::ChkAvbyOfSuppD1BarOHLCV(const string & symbol)
{
  string sFile = m_SysCfg->Get_SupplementaryBarD1Path()+"/"+symbol+".csv";
  return STool::ChkIfFileExists(sFile);
}

void MarketData::GetSuppD1BarOHLCVInDateRange(
  const string & sSymbol,
  const YYYYMMDD & ymdStart,
  const YYYYMMDD & ymdEnd,
  vector<YYYYMMDD> & vYMD,
  vector<double> & vOpen,
  vector<double> & vHigh,
  vector<double> & vLow,
  vector<double> & vClose,
  vector<long> & vVol)
{
  boost::unique_lock<boost::shared_mutex> lock(m_SupplementaryDayBarMutex);

  //--------------------------------------------------
  // Lazy initialization
  //--------------------------------------------------
  map<string,BarProvider*>::iterator it = m_SupplementaryDayBar.find(sSymbol);
  if (it == m_SupplementaryDayBar.end())
  {
    string sFile = m_SysCfg->Get_SupplementaryBarD1Path()+"/"+sSymbol+".csv";
    m_Logger->Write(Logger::INFO,"MarketData: Supplementary Day bar path: %s",sFile.c_str());

    string sFormat = "DxOHLC";
    if (sSymbol == UNDERLYING_VHSI) sFormat = "DxC";

    if (!STool::ChkIfFileExists(sFile))
    {
      vYMD.clear();
      vOpen.clear();
      vHigh.clear();
      vLow.clear();
      vClose.clear();
      vVol.clear();
      return;
    }

    BarProvider * bp = new BarProvider(
      sFile.c_str(),
      sFormat.c_str(),
      5,
      'F',
      'N',
      'P');
    m_SupplementaryDayBar[sSymbol] = bp;
    it = m_SupplementaryDayBar.find(sSymbol);
  }

  it->second->GetOHLCVInDateRange(ymdStart, ymdEnd, vYMD, vOpen, vHigh, vLow, vClose, vVol);

  return;
}

void MarketData::GetSuppM1BarOHLCVInDateTimeRange(
  const string     & sSymbol,
  const YYYYMMDD   & ymdStart,
  const HHMMSS     & hmsStart,
  const YYYYMMDD   & ymdEnd,
  const HHMMSS     & hmsEnd,
  vector<YYYYMMDD> & vYMD,
  vector<HHMMSS>   & vHMS,
  vector<double>   & vOpen,
  vector<double>   & vHigh,
  vector<double>   & vLow,
  vector<double>   & vClose,
  vector<long>     & vVol)
{
  boost::unique_lock<boost::shared_mutex> lock(m_SupplementaryM1BarMutex);

  //--------------------------------------------------
  // Lazy initialization
  //--------------------------------------------------
  map<string,BarProvider*>::iterator it = m_SupplementaryM1Bar.find(sSymbol);
  if (it == m_SupplementaryM1Bar.end())
  {
    string sFile = m_SysCfg->Get_SupplementaryBarM1Path()+"/"+sSymbol+".csv";

    BarProvider * bp = new BarProvider(
      sFile.c_str(),
      "DTOHLC",
      5,
      'F',
      'N',
      'P');
    m_SupplementaryM1Bar[sSymbol] = bp;
    it = m_SupplementaryM1Bar.find(sSymbol);
  }

  it->second->GetOHLCVInDateTimeRange(ymdStart, hmsStart, ymdEnd, hmsEnd, vYMD, vHMS, vOpen, vHigh, vLow, vClose, vVol);

  return;
}



void MarketData::GetUpdatedSymbols(set<string> & s)
{
  boost::unique_lock<boost::shared_mutex> lock(m_UpdatedSymbolsMutex);
  s.clear();
  s.insert(m_UpdatedSymbols.begin(),m_UpdatedSymbols.end());
}

void MarketData::ClearUpdatedSymbols()
{
  boost::unique_lock<boost::shared_mutex> lock(m_UpdatedSymbolsMutex);
  m_UpdatedSymbols.clear();
}

void MarketData::AddUpdatedSymbol(const string & symbol)
{
  boost::unique_lock<boost::shared_mutex> lock(m_UpdatedSymbolsMutex);
  m_UpdatedSymbols.insert(symbol);
}

double MarketData::ComputeTickReturn(const string & symbol, const double price)
{
  //--------------------------------------------------
  // If incoming price is invalid, return NAN to indicate error
  //--------------------------------------------------
  if (!STool::IsValidPrice(price)) return NAN;

  //--------------------------------------------------
  // If m_LatestSnapshots does not contain valid data,
  // e.g. right after starting our engine, we can't compute tick return,
  // just return 0 to let the data flows through the pipe
  //--------------------------------------------------
  boost::shared_lock<boost::shared_mutex> lock(*(GetLatestSnapshotsMutex(symbol)));
  map<string,ATU_MDI_marketfeed_struct*>::iterator it = m_LatestSnapshots.find(symbol);
  if (it == m_LatestSnapshots.end()) return 0;

  //--------------------------------------------------
  // same here, we want the new data to replace the old invalid data, so return 0
  //--------------------------------------------------
  ATU_MDI_marketfeed_struct & s = *(it->second);
  if (!STool::IsValidPrice(s.m_traded_price)) return 0;

  return (price/s.m_traded_price)-1;
}

bool MarketData::CheckIfSymbolIsUpdated(const string & symbol)
{
  boost::shared_lock<boost::shared_mutex> lock(m_UpdatedSymbolsMutex);
  set<string>::iterator it = m_UpdatedSymbols.find(symbol);
  if (it == m_UpdatedSymbols.end()) return false;
  return true;
}
