#include "CtpMd.h"

CtpMd::CtpMd(boost::shared_ptr<ExpandableCirBuffer> p) : m_pCThostFtdcMdApi(NULL),m_p_ctp_lib_handle(NULL),m_connection_string(""),m_ecbMD(p)
{
  m_Logger = StdStreamLogger::Instance();
  m_subscribedSymbols.clear();
}

CtpMd::~CtpMd()
{
  if (m_pCThostFtdcMdApi == NULL) return;
  m_pCThostFtdcMdApi->RegisterSpi(NULL);
  m_Logger->Write(StdStreamLogger::INFO,"%s: Called RegisterSpi(NULL)", __FILENAME__);
  m_pCThostFtdcMdApi->Release();
  m_Logger->Write(StdStreamLogger::INFO,"%s: Called Release()", __FILENAME__);
  m_pCThostFtdcMdApi = NULL;
}

//--------------------------------------------------
// setters
//--------------------------------------------------
void CtpMd::setConnectString(const string & connStr)
{
  m_connection_string = connStr;
}
void CtpMd::setBrokerID(const string & bid)
{
  strcpy(m_broker_id,(char*)bid.c_str());
}
void CtpMd::setInvestorID(const string & iid)
{
  strcpy(m_user_id,(char*)iid.c_str());
}
void CtpMd::setPassword(const string & pwd)
{
  strcpy(m_password,(char*)pwd.c_str());
}
//--------------------------------------------------
// thread run
//--------------------------------------------------
void CtpMd::run()
{
  m_p_ctp_lib_handle = dlopen("thostmduserapi.so",RTLD_NOW);
  if (m_p_ctp_lib_handle == NULL)
  {
    m_Logger->Write(StdStreamLogger::ERROR,"%s: CTP libthostmduserapi.so NOT loaded. %s", __FILENAME__, dlerror());
    return;
  }
  else
  {
    m_Logger->Write(StdStreamLogger::INFO,"%s: CTP libthostmduserapi.so loaded", __FILENAME__);
  }
  typedef CThostFtdcMdApi* (*CreateFtdcMdApiPtr)(const char *, bool, bool);
  CreateFtdcMdApiPtr CreateFtdcMdApi = (CreateFtdcMdApiPtr)dlsym(m_p_ctp_lib_handle,"_ZN15CThostFtdcMdApi15CreateFtdcMdApiEPKcbb");

  m_pCThostFtdcMdApi = CreateFtdcMdApi("",true,true);
  m_Logger->Write(StdStreamLogger::INFO,"%s: Called CreateFtdcMdApi", __FILENAME__);
  m_pCThostFtdcMdApi->RegisterSpi((CThostFtdcMdSpi*) this);
  m_Logger->Write(StdStreamLogger::INFO,"%s: Called RegisterSpi()", __FILENAME__);
  m_pCThostFtdcMdApi->RegisterFront((char*)m_connection_string.c_str());
  m_Logger->Write(StdStreamLogger::INFO,"%s: Called RegisterFront()", __FILENAME__);
  m_pCThostFtdcMdApi->Init();
  m_Logger->Write(StdStreamLogger::INFO,"%s: Called Init()", __FILENAME__);
  if (m_pCThostFtdcMdApi!=NULL) m_pCThostFtdcMdApi->Join();
  m_Logger->Write(StdStreamLogger::INFO,"%s: After Join()", __FILENAME__);
}

//--------------------------------------------------
// callback
//--------------------------------------------------
void CtpMd::OnFrontConnected()
{
  m_Logger->Write(StdStreamLogger::INFO,"%s: Front is connected", __FILENAME__);
  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, m_broker_id);
  strcpy(req.UserID, m_user_id);
  strcpy(req.Password, m_password);
  int iResult = m_pCThostFtdcMdApi->ReqUserLogin(&req, ++m_iRequestID);
  m_Logger->Write(StdStreamLogger::INFO,"%s: Called ReqUserLogin()", __FILENAME__);
  m_Logger->Write(StdStreamLogger::NOTICE,"%s: User Login is %s", __FILENAME__,(iResult == 0) ? "OK" : "Fail");
  SubscribeSymbols(m_subscribedSymbols);
}

void CtpMd::OnRspError(CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
  if (pRspInfo && (pRspInfo->ErrorID != 0))
    m_Logger->Write(StdStreamLogger::ERROR,"%s: ErrorID=%d, ErrorMsg=%s ErrorMsg=%s", __FILENAME__,pRspInfo->ErrorID,pRspInfo->ErrorMsg,ctp::GetCTPErrorMsg(pRspInfo->ErrorID).c_str());
}

void CtpMd::OnFrontDisconnected(int nReason)
{
  m_Logger->Write(StdStreamLogger::NOTICE,"%s: Front is disconnected!", __FILENAME__);
}

void CtpMd::OnHeartBeatWarning(int nTimeLapse)
{
}

void CtpMd::OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
  if (pRspInfo && (pRspInfo->ErrorID != 0))
    m_Logger->Write(StdStreamLogger::ERROR,"%s: ErrorID=%d, ErrorMsg=%s ErrorMsg=%s", __FILENAME__,pRspInfo->ErrorID,pRspInfo->ErrorMsg,ctp::GetCTPErrorMsg(pRspInfo->ErrorID).c_str());
}

void CtpMd::OnRspSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void CtpMd::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData)
{
  ATU_MDI_marketfeed_struct* mfs = (ATU_MDI_marketfeed_struct *)(m_ecbMD->GetWritingPtr());

  //--------------------------------------------------
  // string tradingDay = pDepthMarketData->TradingDay;
  // string updateTime = pDepthMarketData->UpdateTime;
  // string updateMillisec = boost::lexical_cast<string>(pDepthMarketData->UpdateMillisec);
  // boost::replace_all(updateTime, ":", "");
  // char sMicrosec[10];
  // sprintf(sMicrosec,"%06d",pDepthMarketData->UpdateMillisec*1000);
  // mfs.m_timestamp     = tradingDay+"_"+updateTime+"_"+string(sMicrosec);
  //--------------------------------------------------
  mfs->m_microsec_since_epoch_gmt = SDateTime::GetCurrentUnixTimeInMicrosecGMT();
  memset(mfs->m_instrument,'\0',strlen(mfs->m_instrument)); // just to enable better compression ratio in the output binary data file.
  strcpy(mfs->m_instrument,pDepthMarketData->InstrumentID);

  mfs->m_traded_price  = (pDepthMarketData->LastPrice  == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->LastPrice;
  mfs->m_bid_price_1   = (pDepthMarketData->BidPrice1  == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->BidPrice1;
  mfs->m_bid_price_2   = (pDepthMarketData->BidPrice2  == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->BidPrice2;
  mfs->m_bid_price_3   = (pDepthMarketData->BidPrice3  == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->BidPrice3;
  mfs->m_bid_price_4   = (pDepthMarketData->BidPrice4  == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->BidPrice4;
  mfs->m_bid_price_5   = (pDepthMarketData->BidPrice5  == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->BidPrice5;
  mfs->m_ask_price_1   = (pDepthMarketData->AskPrice1  == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->AskPrice1;
  mfs->m_ask_price_2   = (pDepthMarketData->AskPrice2  == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->AskPrice2;
  mfs->m_ask_price_3   = (pDepthMarketData->AskPrice3  == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->AskPrice3;
  mfs->m_ask_price_4   = (pDepthMarketData->AskPrice4  == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->AskPrice4;
  mfs->m_ask_price_5   = (pDepthMarketData->AskPrice5  == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->AskPrice5;
  mfs->m_traded_volume = (pDepthMarketData->Volume     == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->Volume;
  mfs->m_bid_volume_1  = (pDepthMarketData->BidVolume1 == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->BidVolume1;
  mfs->m_bid_volume_2  = (pDepthMarketData->BidVolume2 == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->BidVolume2;
  mfs->m_bid_volume_3  = (pDepthMarketData->BidVolume3 == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->BidVolume3;
  mfs->m_bid_volume_4  = (pDepthMarketData->BidVolume4 == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->BidVolume4;
  mfs->m_bid_volume_5  = (pDepthMarketData->BidVolume5 == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->BidVolume5;
  mfs->m_ask_volume_1  = (pDepthMarketData->AskVolume1 == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->AskVolume1;
  mfs->m_ask_volume_2  = (pDepthMarketData->AskVolume2 == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->AskVolume2;
  mfs->m_ask_volume_3  = (pDepthMarketData->AskVolume3 == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->AskVolume3;
  mfs->m_ask_volume_4  = (pDepthMarketData->AskVolume4 == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->AskVolume4;
  mfs->m_ask_volume_5  = (pDepthMarketData->AskVolume5 == DBL_MAX) ? ATU_INVALID_PRICE : pDepthMarketData->AskVolume5;

  m_ecbMD->PushBack(sizeof(ATU_MDI_marketfeed_struct));
}

//--------------------------------------------------
// receive from strategy engine
//--------------------------------------------------
void CtpMd::on_process_subscription(const ATU_MDI_subscription_struct & ssub)
{
  SubscribeSymbol(ssub.m_instrument);
  m_subscribedSymbols.insert(ssub.m_instrument);
}

void CtpMd::SubscribeSymbol(const string & sym)
{
  char * _ppInstrumentID[1];
  char csSym[64];
  strcpy(csSym, sym.c_str());
  _ppInstrumentID[0] = csSym;
  m_pCThostFtdcMdApi->SubscribeMarketData(_ppInstrumentID,1);
  m_Logger->Write(StdStreamLogger::INFO,"%s: Subscribed %s.", __FILENAME__,sym.c_str());
}

