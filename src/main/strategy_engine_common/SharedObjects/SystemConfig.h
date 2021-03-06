#ifndef SYSTEMCONFIG_HPP_
#define SYSTEMCONFIG_HPP_

#include "PCH.h"
#include "StrategyConstants.h"
#include "Constants.h"
#include "SFunctional.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/time_duration.hpp>
#include <boost/date_time/date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/algorithm/string.hpp>

#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <string>
#include <map>
#include <set>
#include "Logger.h"
#include "SDateTime.h"

using namespace std;
using namespace boost;

class OTIMDIConfig
{
  public:
    OTIMDIConfig();

    string                 m_MDI_File;
    string                 m_MDI_IP;
    string                 m_MDI_Port;
    string                 m_OTI_IP;
    string                 m_OTI_Port;
    vector<string>         m_MDISubscriptionSymbols;
    string                 m_MDISubscriptionStartDate;
    string                 m_MDISubscriptionEndDate;
    string                 m_MDISubscriptionReplaySpeed;
};

class TradingStrategyConfig
{
  public:
    enum TrainingFreq {Auto=0,Daily,Weekly,Monthly,Yearly,OnceAtInitWarmup};
    TradingStrategyConfig();

    bool                      m_OnOff;
    vector<double>            m_ParamVector;
    string                    m_SignalLogPath;
    int                       m_StartTimeBufferInSec;
    int                       m_EndTimeBufferInSec;
    int                       m_BreakTimeBufferInSec;
    set<HHMMSS>               m_SpecialTime;
    HHMMSS                    m_MTMTime;
    vector<string>            m_TradedSymbols;
    map<string,set<string> >  m_SynchronizedSymbols;
    int                       m_SynchronizedSymbolsSec;
    bool                      m_TreatRegularMiniAsOne;
    int                       m_MonitoringIntervalInSec;
    bool                      m_WhetherToRetrain;
    TrainingFreq              m_TrainingFreq;
    bool                      m_CPnLHistPersistenceIsEnabled;
    string                    m_CPnLHistPersistenceFile;
};

class SystemConfig : boost::noncopyable
{
  public:
    enum MktDataTradeVolumeMode {PER_TRADE_VOLUME_MODE=0, ACCUMULATED_VOLUME_MODE=1};
    enum MDIMode {MDI_READFILE=0, MDI_TCPWITHACK=1, MDI_TCPWITHOUTACK=2};
    enum OTIMode {OTI_RECORD=0, OTI_TCP=1};

    static boost::shared_ptr<SystemConfig> Instance();
    void SetConfigPath(const string &);
    ~SystemConfig(){};
    void Reset();
    void ReadConfig(const string &);
    void SetProgramStartTime();
    boost::posix_time::ptime GetProgramStartTime() const;

    void SetItrdHighLowFromIB(const string &);

    SDateTime::TIMEZONE    GetDefaultTimeZone()                  const;
    const unsigned short   GetSystemServicePort()                const;
    string                 Get_Main_Log_Path()                   const;
    string                 Get_MTM_Log_Path()                    const;
    string                 Get_MTMBySym_Log_Path()               const;
    string                 Get_Exec_Log_Path()                   const;
    string                 Get_Holdings_Log_Path()               const;
    string                 Get_SupplementaryBarD1Path()          const;
    string                 Get_SupplementaryBarM1Path()          const;
    bool                   Get_SupplementaryBarLeadingZeroAdj()  const;
    bool                   Get_HKIntradaySeasonalityOnOff()      const;
    string                 Get_HKIntradaySeasonalityPath()       const;
    int                    Get_HKIntradaySeasonalityWindow()     const;
    string                 Get_PositionPersistenceFile()         const;
    string                 Get_LoadItrdHighLowFromIB()           const;
    bool                   PositionPersistenceIsEnabled()        const;
    string                 Get_S11AParamFileHistSDCorrel()       const;
    string                 Get_S11AParamFileHSIHHIIV()           const;
    string                 Get_S11AParamFileT1GumbelCauchy()     const;
    string                 Get_S11AParamFileT1GumbelGauss()      const;
    string                 Get_S11AParamFileT2Copula()           const;
    bool                   SendResetOnConnectionToCAPI()         const;
    int                    GetNumOfMDI()                         const;
    int                    GetNumOfOTI()                         const;

    HHMMSS                 Get_HKFEContFutRollFwdTime()       const;
    HHMMSS                 Get_CMEContFutRollFwdTime()        const;

    int                    Get_B1_HKF_SmplgIntvlInSec()       const;
    long                   Get_VolSurfCalcIntervalInSec()     const;
    long                   Get_ProbDistrnCalcIntervalInSec()  const;
    long                   Get_TechIndUpdateIntervalInSec()   const;
    string                 Get_TradingHoursPath()             const;
    string                 Get_HKFE_CalendarPath()            const;
    string                 Get_HKMA_ExchgFundBillPath()       const;
    string                 Get_HKSE_HSIConstituentsPath()     const;
    string                 Get_CorrelMatricesPath()           const;
    long                   Get_GKYZ_Window_Size()             const;
    MktDataTradeVolumeMode Get_MktDataTradeVolumeMode()       const;
    string                 Get_VolSurfParamFile1FM()          const;
    string                 Get_VolSurfParamFile2FM()          const;
    string                 Get_ProbDistrFileFSMC1D()          const;

    MDIMode                Get_MDIMode()                      const;
    OTIMode                Get_OTIMode()                      const;
    bool                   NextTierZMQIsOn()                  const;
    string                 Get_NextTier_ZMQ_MD_IP_Port()      const;
    string                 Get_NextTier_ZMQ_TF_IP_Port()      const;

    bool                   Get_OnOff_S11A_T1GBM()             const;
    bool                   Get_OnOff_S11A_T1GUMBELCAUCHY()    const;
    bool                   Get_OnOff_S11A_T1GUMBELGAUSS()     const;
    bool                   Get_OnOff_S11A_T2GBM()             const;
    bool                   Get_OnOff_S11A_T2PCTL()            const;
    bool                   Get_OnOff_S11A_T2COPU()            const;

    vector<long>           Get_S11AT1GBMDeltaTPeriods()                                        const;
    vector<long>           Get_S11AT2MAPeriods()                                               const;
    vector<long>           Get_S11AT2GBMDeltaTPeriods()                                        const;
    double                 Get_S11AT1GBMTriggerThreshold()                                     const;
    double                 Get_S11AT2GBMTriggerThreshold()                                     const;
    double                 Get_S11AT1GumbelCauchyTriggerThreshold()                            const;
    double                 Get_S11AT1GumbelGaussTriggerThreshold()                             const;
    double                 Get_S11AT2CopuTriggerThreshold()                                    const;
    double                 Get_S11AT1GBMTakeProfitThreshold()                                  const;
    double                 Get_S11AT2GBMTakeProfitThreshold()                                  const;
    double                 Get_S11AT1GumbelCauchyTakeProfitThreshold()                         const;
    double                 Get_S11AT1GumbelGaussTakeProfitThreshold()                          const;
    double                 Get_S11AT2CopuTakeProfitThreshold()                                 const;
    double                 Get_S11AT1GBMStopLossThreshold()                                    const;
    double                 Get_S11AT2GBMStopLossThreshold()                                    const;
    double                 Get_S11AT1GumbelCauchyStopLossThreshold()                           const;
    double                 Get_S11AT1GumbelGaussStopLossThreshold()                            const;
    double                 Get_S11AT2CopuStopLossThreshold()                                   const;
    double                 Get_S11AT1GumbelCauchyEOLPeriod()                                   const;
    double                 Get_S11AT1GumbelGaussEOLPeriod()                                    const;
    double                 Get_S11ANotionalValuePerLeg()                                       const;
    double                 Get_S11AT1GumbelCauchyPeriod()                                      const;
    double                 Get_S11AT1GumbelGaussPeriod()                                       const;
    double                 Get_S11AT2PctlPeriod()                                              const;
    double                 Get_S11AT2CopuPeriod()                                              const;
    double                 Get_S11AT1GumbelCauchyThetaMean()                                   const;
    double                 Get_S11AT1GumbelGaussThetaMean()                                    const;
    double                 Get_S11AT1GumbelCauchyThetaMeanReversion()                          const;
    double                 Get_S11AT1GumbelGaussThetaMeanReversion()                           const;
    double                 Get_S11AT1GumbelCauchyMarginalSDScaling()                           const;
    double                 Get_S11AT1GumbelGaussMarginalSDScaling()                            const;
    double                 Get_S11AMonitoringIntervalInSec()                                   const;
    double                 Get_S11AT1GumbelGaussSignalLimit()                                  const;
    bool                   B2_HasEnabledMinCommissionCheck(const StrategyID)                   const;
    double                 B2_CommissionRateThreshold(const StrategyID)                        const;
    int                    B2_HasEnabledRotationMode(const StrategyID)                         const;
    bool                   B2_RotationModeTradeHighestReturn(const StrategyID)                 const;
    bool                   B2_RotationModeUseVolyAdjdReturn(const StrategyID)                  const;
    int                    B2_RotationModeUseNDayReturn(const StrategyID)                      const;
    boost::optional<double>B2_LongOnlyWhenClosePriceBelowAvgPrice(const StrategyID)            const;
    boost::optional<double>B2_ShortOnlyWhenClosePriceAboveAvgPrice(const StrategyID)           const;
    boost::optional<double>B2_LongOnlyWhenAvgPriceReturnAbove(const StrategyID)                const;
    boost::optional<double>B2_LongOnlyWhenGrpAvgReturnAbove(const StrategyID)                  const;
    boost::optional<double>B2_ShortOnlyWhenAvgPriceReturnBelow(const StrategyID)               const;
    string                 B2_TheoCPnLHistFolder(const StrategyID)                             const;
    string                 B2_TheoPosFolder(const StrategyID)                                  const;
    bool                   B2_PersistTheoPosCPnL(const StrategyID)                             const;
    string                 Get_B2_RotationGroupFile(const StrategyID)                          const;
    string                 Get_B2_Arima_ZMQ_IP_Port(const StrategyID)                          const;
    bool                   Get_B2_PerformCppTaylor(const StrategyID)                           const;
    bool                   Get_B2_SymbolStickiness(const StrategyID)                           const;
    double                 Get_B2_TaylorWeight(const StrategyID)                               const;
    double                 Get_B2_ArimaWeight(const StrategyID)                                const;
    vector<int>            Get_B2_RoleOfSym(const StrategyID)                                  const;
    double                 Get_B2_ChooseBestNRotationGroupProp(const StrategyID)               const;
    int                    Get_B2_ActionTimeBefCloseInSec(const StrategyID)                    const;
    int                    Get_B2_ActionTimeAftOpenInSec(const StrategyID)                     const;
    vector<int>            Get_B2_FilterSMAPeriod(const StrategyID)                            const;
    bool                   Get_B2_MoveNextBestGroupUpIfNoSignal(const StrategyID)              const;
    bool                   Get_B2_MoveNextBestStkInGrpUpIfNoSignal(const StrategyID)           const;
    double                 Get_B2_AvgAggSgndNotnlThresh(const StrategyID)                      const;
    double                 Get_B2_MinSharpeThreshFilter(const StrategyID)                      const;
    set<string>            Get_B2_TradedSymTradeAtOpen(const StrategyID)                       const;
    bool                   B3_HasEnabledMinCommissionCheck(const StrategyID)                   const;
    double                 B3_CommissionRateThreshold(const StrategyID)                        const;
    int                    B3_HasEnabledRotationMode(const StrategyID)                         const;
    bool                   B3_RotationModeTradeHighestReturn(const StrategyID)                 const;
    bool                   B3_RotationModeUseVolyAdjdReturn(const StrategyID)                  const;
    int                    B3_RotationModeUseNDayReturn(const StrategyID)                      const;
    boost::optional<double>B3_LongOnlyWhenClosePriceBelowAvgPrice(const StrategyID)            const;
    boost::optional<double>B3_ShortOnlyWhenClosePriceAboveAvgPrice(const StrategyID)           const;
    boost::optional<double>B3_LongOnlyWhenAvgPriceReturnAbove(const StrategyID)                const;
    boost::optional<double>B3_LongOnlyWhenGrpAvgReturnAbove(const StrategyID)                  const;
    boost::optional<double>B3_ShortOnlyWhenAvgPriceReturnBelow(const StrategyID)               const;
    string                 B3_TheoCPnLHistFolder(const StrategyID)                             const;
    string                 B3_TheoPosFolder(const StrategyID)                                  const;
    bool                   B3_PersistTheoPosCPnL(const StrategyID)                             const;
    string                 Get_B3_RotationGroupFile(const StrategyID)                          const;
    string                 Get_B3_Arima_ZMQ_IP_Port(const StrategyID)                          const;
    bool                   Get_B3_PerformCppTaylor(const StrategyID)                           const;
    bool                   Get_B3_SymbolStickiness(const StrategyID)                           const;
    double                 Get_B3_TaylorWeight(const StrategyID)                               const;
    double                 Get_B3_ArimaWeight(const StrategyID)                                const;
    vector<int>            Get_B3_RoleOfSym(const StrategyID)                                  const;
    double                 Get_B3_ChooseBestNRotationGroupProp(const StrategyID)               const;
    int                    Get_B3_ActionTimeBefCloseInSec(const StrategyID)                    const;
    int                    Get_B3_ActionTimeAftOpenInSec(const StrategyID)                     const;
    vector<int>            Get_B3_FilterSMAPeriod(const StrategyID)                            const;
    bool                   Get_B3_MoveNextBestGroupUpIfNoSignal(const StrategyID)              const;
    bool                   Get_B3_MoveNextBestStkInGrpUpIfNoSignal(const StrategyID)           const;
    double                 Get_B3_AvgAggSgndNotnlThresh(const StrategyID)                      const;
    set<string>            Get_B3_TradedSymTradeAtOpen(const StrategyID)                       const;
    bool                   CheckIfHaltTrading(const string &,const YYYYMMDD &)                 const;
    bool                   CheckIfHalfDayTrading(const string &,const YYYYMMDD &)              const;
    bool                   GetCorpActionAdj(const string &,const YYYYMMDD &,double &,double &) const;
    int                    GetLotSize(const string &)                                          const;
    long                   RoundDownLotSize(const string &,const long)                         const;
    bool                   IsPriceFwdrToNextTierOn()                                           const;
    int                    GetPriceFwdrToNextTierIntervalInSec()                               const;

    double                 GetMaxAllowedTradeNotional()                                        const;
    long                   GetMaxAllowedTradeQty()                                             const;

    //--------------------------------------------------
    // Convenience for Trading System config
    //--------------------------------------------------
    string                 GetSignalLogPath(const StrategyID);
    HHMMSS                 GetStrategyMTMTime(const StrategyID);
    vector<double>         GetParamVector(const StrategyID);
    vector<string>         GetTradedSymbols(const StrategyID);
    set<string>            GetSynchronizedSymbols(const StrategyID,const string &);
    int                    GetSynchronizedSymbolsSec(const StrategyID);
    bool                   ChkIfTreatRegularMiniAsOne(const StrategyID);
    int                    GetStrategyStartTimeBufferInSec(const StrategyID);
    int                    GetStrategyEndTimeBufferInSec(const StrategyID);
    int                    GetStrategyBreakTimeBufferInSec(const StrategyID);
    set<HHMMSS>            GetStrategySpecialTime(const StrategyID);
    bool                   IsStrategyOn(const StrategyID);
    int                    GetMonitoringIntervalInSec(const StrategyID);
    bool                   GetWhetherToRetrain(const StrategyID);
    TradingStrategyConfig::TrainingFreq GetTrainingFreq(const StrategyID);
    string                 Get_CPnLHistPersistenceFile(const StrategyID);
    bool                   IsCPnLHistPersistenceEnabled(const StrategyID);

    //--------------------------------------------------
    // Convenience for Trading System config
    //--------------------------------------------------
    string                 Get_MDI_File(const int);
    string                 Get_MDI_IP(const int);
    string                 Get_MDI_Port(const int);
    string                 Get_OTI_IP(const int);
    string                 Get_OTI_Port(const int);
    const vector<string>   GetMDISubscriptionSymbols(const int);
    const string           GetMDISubscriptionStartDate(const int);
    const string           GetMDISubscriptionEndDate(const int);
    const string           GetMDISubscriptionReplaySpeed(const int);

    bool CheckIfBarAggregationM1Symbol(const string &) const;
    bool CheckIfBarAggregationD1Symbol(const string &) const;
    double GetErroneousTickPxChg() const;
    bool CheckIfSymToBeWarmedUpWithDayBars(const string &) const;
    bool ChkIfProceedStyForceExcnEvenIfNoMD(const string &) const;

  private:
    //--------------------------------------------------
    // Singleton
    //--------------------------------------------------
    SystemConfig();
    SystemConfig(SystemConfig const&);
    SystemConfig& operator=(SystemConfig const&){};
    static boost::weak_ptr<SystemConfig> m_pInstance;

    //--------------------------------------------------
    boost::shared_ptr<Logger> m_Logger;
    string m_ConfigPath;

    //--------------------------------------------------
    typedef struct {
      double    m_price_delta;
      double    m_price_ratio;
    } tuple_corp_action_adj;

    //--------------------------------------------------
    // Config.ini fields
    //--------------------------------------------------
    SDateTime::TIMEZONE                                m_DefaultTimeZone;
    boost::posix_time::ptime                           m_ProgramStartTime;
    unsigned short                                     m_SystemServicePort;
    string                                             m_MainLogPath;
    string                                             m_MTMLogPath;
    string                                             m_MTMBySymLogPath;
    string                                             m_ExecLogPath;
    string                                             m_MTMHoldingsLogPath;
    string                                             m_SupplementaryBarD1Path;
    string                                             m_SupplementaryBarM1Path;
    bool                                               m_SupplementaryBarLeadingZeroAdj;
    bool                                               m_HKIntradaySeasonalityOnOff;
    string                                             m_HKIntradaySeasonalityPath;
    int                                                m_HKIntradaySeasonalityWindow;
    string                                             m_PositionPersistenceFile;
    string                                             m_LoadItrdHighLowFromIB;
    bool                                               m_PositionPersistenceIsEnabled;
    string                                             m_S11A_ParamFileHistSDCorrel;
    string                                             m_S11A_ParamFileHSIHHIIV;
    string                                             m_S11A_ParamFileT1GumbelCauchy;
    string                                             m_S11A_ParamFileT1GumbelGauss;
    string                                             m_S11A_ParamFileT2Copula;
    long                                               m_VolSurfCalcIntervalInSec;
    long                                               m_ProbDistrnCalcIntervalInSec;
    long                                               m_TechIndUpdateIntervalInSec;
    string                                             m_TradingHoursPath;
    string                                             m_HKFE_CalendarPath;
    string                                             m_HKSE_HSIConstituentsPath;
    string                                             m_HKMA_ExchgFundBillPath;
    string                                             m_CorrelMatricesPath;
    string                                             m_HaltTradingPath;
    map<string,set<YYYYMMDD> >                         m_HaltTradingSymDate;
    string                                             m_HalfDayTradingPath;
    map<string,set<YYYYMMDD> >                         m_HalfDayTradingSymDate;
    string                                             m_CorpActionAdjPath;
    map<string,map<YYYYMMDD,tuple_corp_action_adj> >   m_CorpActionAdjDetails;
    string                                             m_LotSizePath;
    map<string,int>                                    m_map_LotSize;
    bool                                               m_PriceFwdrIsOn;
    int                                                m_PriceFwdrIntervalInSec;
    set<string>                                        m_BarAggregationM1Symbols;
    set<string>                                        m_BarAggregationD1Symbols;
    double                                             m_ErroneousTickPxChg;
    set<string>                                        m_SymToBeWarmedUpWithDayBars;
    long                                               m_GKYZ_Window_Size;
    MktDataTradeVolumeMode                             m_MktData_TradeVolumeMode;
    string                                             m_VolSurfParamFile1FM;
    string                                             m_VolSurfParamFile2FM;
    string                                             m_ProbDistrFileFSMC1D;
    MDIMode                                            m_MDIMode;
    OTIMode                                            m_OTIMode;
    bool                                               m_NextTierZMQIsOn;
    string                                             m_NextTier_ZMQ_MD_IP_Port;
    string                                             m_NextTier_ZMQ_TF_IP_Port;
    int                                                m_B1_HKF_SamplingIntervalInSec;
    map<int,OTIMDIConfig>                              m_map_OTIMDIConfig;
    map<StrategyID,TradingStrategyConfig>              m_map_TradingStrategyConfig;
    bool                                               m_SendResetOnConnectionToCAPI;
    int                                                m_NumOfMDI;
    int                                                m_NumOfOTI;
    HHMMSS                                             m_HKFEContFutRollFwdTime;
    HHMMSS                                             m_CMEContFutRollFwdTime;
    set<string>                                        m_ProceedStyForceExcnEvenIfNoMD;

    bool                                               m_On_S11A_T1GBM          ;
    bool                                               m_On_S11A_T1GUMBELCAUCHY ;
    bool                                               m_On_S11A_T1GUMBELGAUSS  ;
    bool                                               m_On_S11A_T2GBM          ;
    bool                                               m_On_S11A_T2PCTL         ;
    bool                                               m_On_S11A_T2COPU         ;

    double                                             m_S11A_NotionalValuePerLeg;
    double                                             m_S11A_T1GBM_TriggerThreshold;
    double                                             m_S11A_T2GBM_TriggerThreshold;
    double                                             m_S11A_T1GumbelCauchy_TriggerThreshold;
    double                                             m_S11A_T1GumbelGauss_TriggerThreshold;
    double                                             m_S11A_T2Copu_TriggerThreshold;
    double                                             m_S11A_T1GBM_TakeProfitThreshold;
    double                                             m_S11A_T2GBM_TakeProfitThreshold;
    double                                             m_S11A_T1GumbelCauchy_TakeProfitThreshold;
    double                                             m_S11A_T1GumbelGauss_TakeProfitThreshold;
    double                                             m_S11A_T2Copu_TakeProfitThreshold;
    double                                             m_S11A_T1GBM_StopLossThreshold;
    double                                             m_S11A_T2GBM_StopLossThreshold;
    double                                             m_S11A_T1GumbelCauchy_StopLossThreshold;
    double                                             m_S11A_T1GumbelGauss_StopLossThreshold;
    double                                             m_S11A_T1GumbelCauchy_EOLPeriod;
    double                                             m_S11A_T1GumbelGauss_EOLPeriod;
    double                                             m_S11A_T2Copu_StopLossThreshold;
    vector<long>                                       m_S11A_T1GBM_DeltaTPeriods;
    vector<long>                                       m_S11A_T2_MAPeriods;
    vector<long>                                       m_S11A_T2GBM_DeltaTPeriods;
    double                                             m_S11A_T1GumbelCauchyPeriod;
    double                                             m_S11A_T1GumbelGaussPeriod;
    double                                             m_S11A_T2CopuPeriod;
    double                                             m_S11A_T2PctlPeriod;
    double                                             m_S11A_T1GumbelCauchyThetaMean;
    double                                             m_S11A_T1GumbelGaussThetaMean;
    double                                             m_S11A_T1GumbelCauchyThetaMeanReversion;
    double                                             m_S11A_T1GumbelGaussThetaMeanReversion;
    double                                             m_S11A_T1GumbelCauchyMarginalSDScaling;
    double                                             m_S11A_T1GumbelGaussMarginalSDScaling;
    double                                             m_S11A_MonitoringIntervalInSec;
    double                                             m_S11A_T1GumbelGaussSigalLimit;
    bool                                               m_B2_US1_HasEnabledMinCommissionCheck;
    double                                             m_B2_US1_CommissionRateThreshold;
    int                                                m_B2_US1_HasEnabledRotationMode;
    bool                                               m_B2_US1_RotationModeTradeHighestReturn;
    bool                                               m_B2_US1_RotationModeUseVolyAdjdReturn;
    int                                                m_B2_US1_RotationModeUseNDayReturn;
    boost::optional<double>                            m_B2_US1_LongOnlyWhenClosePriceBelowAvgPrice;
    boost::optional<double>                            m_B2_US1_ShortOnlyWhenClosePriceAboveAvgPrice;
    boost::optional<double>                            m_B2_US1_LongOnlyWhenAvgPriceReturnAbove;
    boost::optional<double>                            m_B2_US1_LongOnlyWhenGrpAvgReturnAbove;
    boost::optional<double>                            m_B2_US1_ShortOnlyWhenAvgPriceReturnBelow;
    string                                             m_B2_US1_TheoCPnLHistFolder;
    string                                             m_B2_US1_TheoPosFolder;
    bool                                               m_B2_US1_PersistTheoPosCPnL;
    string                                             m_B2_US1_RotationGroupFile;
    string                                             m_B2_US1_Arima_ZMQ_IP_Port;
    bool                                               m_B2_US1_PerformCppTaylor;
    bool                                               m_B2_US1_SymbolStickiness;
    double                                             m_B2_US1_TaylorWeight;
    double                                             m_B2_US1_ArimaWeight;
    vector<int>                                        m_B2_US1_RoleOfSym;
    double                                             m_B2_US1_ChooseBestNRotationGroupProp;
    int                                                m_B2_US1_ActionTimeBefCloseInSec;
    int                                                m_B2_US1_ActionTimeAftOpenInSec;
    vector<int>                                        m_B2_US1_FilterSMAPeriod;
    bool                                               m_B2_US1_MoveNextBestGroupUpIfNoSignal;
    bool                                               m_B2_US1_MoveNextBestStkInGrpUpIfNoSignal;
    double                                             m_B2_US1_AvgAggSgndNotnlThresh;
    double                                             m_B2_US1_MinSharpeThreshFilter;
    set<string>                                        m_B2_US1_TradedSymbolsTradeAtOpen;
    bool                                               m_B2_US2_HasEnabledMinCommissionCheck;
    double                                             m_B2_US2_CommissionRateThreshold;
    int                                                m_B2_US2_HasEnabledRotationMode;
    bool                                               m_B2_US2_RotationModeTradeHighestReturn;
    bool                                               m_B2_US2_RotationModeUseVolyAdjdReturn;
    int                                                m_B2_US2_RotationModeUseNDayReturn;
    boost::optional<double>                            m_B2_US2_LongOnlyWhenClosePriceBelowAvgPrice;
    boost::optional<double>                            m_B2_US2_ShortOnlyWhenClosePriceAboveAvgPrice;
    boost::optional<double>                            m_B2_US2_LongOnlyWhenAvgPriceReturnAbove;
    boost::optional<double>                            m_B2_US2_LongOnlyWhenGrpAvgReturnAbove;
    boost::optional<double>                            m_B2_US2_ShortOnlyWhenAvgPriceReturnBelow;
    string                                             m_B2_US2_TheoCPnLHistFolder;
    string                                             m_B2_US2_TheoPosFolder;
    bool                                               m_B2_US2_PersistTheoPosCPnL;
    string                                             m_B2_US2_RotationGroupFile;
    string                                             m_B2_US2_Arima_ZMQ_IP_Port;
    bool                                               m_B2_US2_PerformCppTaylor;
    bool                                               m_B2_US2_SymbolStickiness;
    double                                             m_B2_US2_TaylorWeight;
    double                                             m_B2_US2_ArimaWeight;
    vector<int>                                        m_B2_US2_RoleOfSym;
    double                                             m_B2_US2_ChooseBestNRotationGroupProp;
    int                                                m_B2_US2_ActionTimeBefCloseInSec;
    int                                                m_B2_US2_ActionTimeAftOpenInSec;
    vector<int>                                        m_B2_US2_FilterSMAPeriod;
    bool                                               m_B2_US2_MoveNextBestGroupUpIfNoSignal;
    bool                                               m_B2_US2_MoveNextBestStkInGrpUpIfNoSignal;
    double                                             m_B2_US2_AvgAggSgndNotnlThresh;
    double                                             m_B2_US2_MinSharpeThreshFilter;
    set<string>                                        m_B2_US2_TradedSymbolsTradeAtOpen;
    bool                                               m_B2_US3_HasEnabledMinCommissionCheck;
    double                                             m_B2_US3_CommissionRateThreshold;
    int                                                m_B2_US3_HasEnabledRotationMode;
    bool                                               m_B2_US3_RotationModeTradeHighestReturn;
    bool                                               m_B2_US3_RotationModeUseVolyAdjdReturn;
    int                                                m_B2_US3_RotationModeUseNDayReturn;
    boost::optional<double>                            m_B2_US3_LongOnlyWhenClosePriceBelowAvgPrice;
    boost::optional<double>                            m_B2_US3_ShortOnlyWhenClosePriceAboveAvgPrice;
    boost::optional<double>                            m_B2_US3_LongOnlyWhenAvgPriceReturnAbove;
    boost::optional<double>                            m_B2_US3_LongOnlyWhenGrpAvgReturnAbove;
    boost::optional<double>                            m_B2_US3_ShortOnlyWhenAvgPriceReturnBelow;
    string                                             m_B2_US3_TheoCPnLHistFolder;
    string                                             m_B2_US3_TheoPosFolder;
    bool                                               m_B2_US3_PersistTheoPosCPnL;
    string                                             m_B2_US3_RotationGroupFile;
    string                                             m_B2_US3_Arima_ZMQ_IP_Port;
    bool                                               m_B2_US3_PerformCppTaylor;
    bool                                               m_B2_US3_SymbolStickiness;
    double                                             m_B2_US3_TaylorWeight;
    double                                             m_B2_US3_ArimaWeight;
    vector<int>                                        m_B2_US3_RoleOfSym;
    double                                             m_B2_US3_ChooseBestNRotationGroupProp;
    int                                                m_B2_US3_ActionTimeBefCloseInSec;
    int                                                m_B2_US3_ActionTimeAftOpenInSec;
    vector<int>                                        m_B2_US3_FilterSMAPeriod;
    bool                                               m_B2_US3_MoveNextBestGroupUpIfNoSignal;
    bool                                               m_B2_US3_MoveNextBestStkInGrpUpIfNoSignal;
    double                                             m_B2_US3_AvgAggSgndNotnlThresh;
    double                                             m_B2_US3_MinSharpeThreshFilter;
    set<string>                                        m_B2_US3_TradedSymbolsTradeAtOpen;
    bool                                               m_B2_HK_HasEnabledMinCommissionCheck;
    double                                             m_B2_HK_CommissionRateThreshold;
    int                                                m_B2_HK_HasEnabledRotationMode;
    bool                                               m_B2_HK_RotationModeTradeHighestReturn;
    bool                                               m_B2_HK_RotationModeUseVolyAdjdReturn;
    int                                                m_B2_HK_RotationModeUseNDayReturn;
    boost::optional<double>                            m_B2_HK_LongOnlyWhenClosePriceBelowAvgPrice;
    boost::optional<double>                            m_B2_HK_ShortOnlyWhenClosePriceAboveAvgPrice;
    boost::optional<double>                            m_B2_HK_LongOnlyWhenAvgPriceReturnAbove;
    boost::optional<double>                            m_B2_HK_LongOnlyWhenGrpAvgReturnAbove;
    boost::optional<double>                            m_B2_HK_ShortOnlyWhenAvgPriceReturnBelow;
    string                                             m_B2_HK_TheoCPnLHistFolder;
    string                                             m_B2_HK_TheoPosFolder;
    bool                                               m_B2_HK_PersistTheoPosCPnL;
    string                                             m_B2_HK_RotationGroupFile;
    string                                             m_B2_HK_Arima_ZMQ_IP_Port;
    bool                                               m_B2_HK_PerformCppTaylor;
    bool                                               m_B2_HK_SymbolStickiness;
    double                                             m_B2_HK_TaylorWeight;
    double                                             m_B2_HK_ArimaWeight;
    vector<int>                                        m_B2_HK_RoleOfSym;
    double                                             m_B2_HK_ChooseBestNRotationGroupProp;
    int                                                m_B2_HK_ActionTimeBefCloseInSec;
    int                                                m_B2_HK_ActionTimeAftOpenInSec;
    vector<int>                                        m_B2_HK_FilterSMAPeriod;
    bool                                               m_B2_HK_MoveNextBestGroupUpIfNoSignal;
    bool                                               m_B2_HK_MoveNextBestStkInGrpUpIfNoSignal;
    double                                             m_B2_HK_AvgAggSgndNotnlThresh;
    double                                             m_B2_HK_MinSharpeThreshFilter;
    set<string>                                        m_B2_HK_TradedSymbolsTradeAtOpen;

    bool                                               m_B3_US_HasEnabledMinCommissionCheck;
    double                                             m_B3_US_CommissionRateThreshold;
    int                                                m_B3_US_HasEnabledRotationMode;
    bool                                               m_B3_US_RotationModeTradeHighestReturn;
    bool                                               m_B3_US_RotationModeUseVolyAdjdReturn;
    int                                                m_B3_US_RotationModeUseNDayReturn;
    boost::optional<double>                            m_B3_US_LongOnlyWhenClosePriceBelowAvgPrice;
    boost::optional<double>                            m_B3_US_ShortOnlyWhenClosePriceAboveAvgPrice;
    boost::optional<double>                            m_B3_US_LongOnlyWhenAvgPriceReturnAbove;
    boost::optional<double>                            m_B3_US_LongOnlyWhenGrpAvgReturnAbove;
    boost::optional<double>                            m_B3_US_ShortOnlyWhenAvgPriceReturnBelow;
    string                                             m_B3_US_TheoCPnLHistFolder;
    string                                             m_B3_US_TheoPosFolder;
    bool                                               m_B3_US_PersistTheoPosCPnL;
    string                                             m_B3_US_RotationGroupFile;
    string                                             m_B3_US_Arima_ZMQ_IP_Port;
    bool                                               m_B3_US_PerformCppTaylor;
    bool                                               m_B3_US_SymbolStickiness;
    double                                             m_B3_US_TaylorWeight;
    double                                             m_B3_US_ArimaWeight;
    vector<int>                                        m_B3_US_RoleOfSym;
    double                                             m_B3_US_ChooseBestNRotationGroupProp;
    int                                                m_B3_US_ActionTimeBefCloseInSec;
    int                                                m_B3_US_ActionTimeAftOpenInSec;
    vector<int>                                        m_B3_US_FilterSMAPeriod;
    bool                                               m_B3_US_MoveNextBestGroupUpIfNoSignal;
    bool                                               m_B3_US_MoveNextBestStkInGrpUpIfNoSignal;
    double                                             m_B3_US_AvgAggSgndNotnlThresh;
    set<string>                                        m_B3_US_TradedSymbolsTradeAtOpen;

    string                                             m_SymbolsToBeUsedInAllSections;



    //--------------------------------------------------
    // Global risk management for placing orders
    //--------------------------------------------------
    double   m_MaxAllowedTradeNotional;
    long     m_MaxAllowedTradeQty;
};

#endif
