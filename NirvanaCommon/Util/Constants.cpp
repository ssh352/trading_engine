#include "Constants.h"

string GetStrategyName(const StrategyID id)
{
  switch (id)
  {
    case STY_NIR:      { return "NIR";     }
    case STY_B1_HKF:   { return "B1_HKF";  }
    case STY_B2_USSTK: { return "B2_USSTK";}
    case STY_B2_HK:    { return "B2_HK";   }
    case STY_ETFR:     { return "ETFR";    }
    case STY_S11A:     { return "S11A";    }
    case STY_A1:       { return "A1";      }
    case STY_A6:       { return "A6";      }
    case STY_R1:       { return "R1";      }
    case STY_R3:       { return "R3";      }
    case STY_R7:       { return "R7";      }
    case STY_R8:       { return "R8";      }
    case STY_R9:       { return "R9";      }
    case STY_TEST:     { return "TEST";    }
    default:           { return "_UNKWN_"; }
  }
}

void GetStrategyNameCStr(const StrategyID id, char outputStrategyName[HEALTHKEYLEN+1])
{
  switch (id)
  {
    case STY_NIR:      { strcpy(outputStrategyName,"NIR\0");     break; }
    case STY_B1_HKF:   { strcpy(outputStrategyName,"B1_HKF\0");  break; }
    case STY_B2_USSTK: { strcpy(outputStrategyName,"B2_USSTK\0");break; }
    case STY_B2_HK:    { strcpy(outputStrategyName,"B2_HK\0");   break; }
    case STY_ETFR:     { strcpy(outputStrategyName,"ETFR\0");    break; }
    case STY_S11A:     { strcpy(outputStrategyName,"S11A\0");    break; }
    case STY_A1:       { strcpy(outputStrategyName,"A1\0");      break; }
    case STY_A6:       { strcpy(outputStrategyName,"A6\0");      break; }
    case STY_R1:       { strcpy(outputStrategyName,"R1\0");      break; }
    case STY_R3:       { strcpy(outputStrategyName,"R3\0");      break; }
    case STY_R7:       { strcpy(outputStrategyName,"R7\0");      break; }
    case STY_R8:       { strcpy(outputStrategyName,"R8\0");      break; }
    case STY_R9:       { strcpy(outputStrategyName,"R9\0");      break; }
    case STY_TEST:     { strcpy(outputStrategyName,"TEST\0");    break; }
    default:           { strcpy(outputStrategyName,"_UNKWN_\0"); break; }
  }
}