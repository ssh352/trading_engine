
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include <quickfix/FileStore.h>
#include <quickfix/SocketInitiator.h>
#include <quickfix/SessionSettings.h>
#include <quickfix/Log.h>

#include "fix_trader.h"
#include "order.h"

using namespace std;

char ParseToOrderType(string order_type) {
  char ret = kOrderTypeLimit;
  if (strcasecmp(order_type.c_str(), "limit") == 0) {
    ret = kOrderTypeLimit;
  } else if (strcasecmp(order_type.c_str(), "market") == 0) {
    ret = kOrderTypeMarket;
  } else if (strcasecmp(order_type.c_str(), "market_limit") == 0) {
    ret = kOrderTypeMarketLimit;
  } else if (strcasecmp(order_type.c_str(), "stop") == 0) {
    ret = kOrderTypeStop;
  } else if (strcasecmp(order_type.c_str(), "stop_limit") == 0) {
    ret = kOrderTypeStopLimit;
  }
  return ret;
}

char ParseToDirection(string direction) {
  char ret = kDirectionBuy;
  if (strcasecmp(direction.c_str(), "buy") == 0) {
    ret = kDirectionBuy;
  } else if (strcasecmp(direction.c_str(), "sell") == 0){
    ret = kDirectionSell;
  }
  return ret;
}

char ParseToTimeInForce(string time_in_force) {
  char ret = kTimeInForceDay;
  if (strcasecmp(time_in_force.c_str(), "day") == 0) {
    ret = kTimeInForceDay;
  } else if (strcasecmp(time_in_force.c_str(), "gtc") == 0) {
    ret = kTimeInForceGTC;
  } else if (strcasecmp(time_in_force.c_str(), "fak") == 0) {
    ret = kTimeInForceFAK;
  } else if (strcasecmp(time_in_force.c_str(), "gtd") == 0) {
    ret = kTimeInForceGTD;
  }
  return ret;
}

int main(int argc, char **argv) {
  int arg_pos = 1;
  string config_file_name = "./test.cfg";
  string cmd_file_name = "";
  string account = "W80004";
  while (arg_pos < argc) {
    if (strcmp(argv[arg_pos], "-f") == 0) {
      config_file_name = argv[++arg_pos];
    }
    if (strcmp(argv[arg_pos], "-c") == 0) {
      cmd_file_name = argv[++arg_pos];
    }
    if (strcmp(argv[arg_pos], "-a") == 0) {
      account = argv[++arg_pos];
    }
    ++arg_pos;
  }

  FixTrader fix_trader;
  // cout << "initialize setting" << endl;
  // // FIX::SessionSettings settings(config_file_name);
  // FIX::SessionSettings *settings = new FIX::SessionSettings(config_file_name);
  // cout << "initialize store factory" << endl;
  // // FIX::FileStoreFactory store_factory(settings);
  // FIX::FileStoreFactory *store_factory = new FIX::FileStoreFactory(*settings);
  // cout << "initialize log factory" << endl;
  // FIX::ScreenLogFactory *log_factory = new FIX::ScreenLogFactory(*settings);
  // cout << "initialize initiator" << endl;
  // FIX::SocketInitiator initiator(fix_trader, *store_factory, *settings, 
  //                                *log_factory);
  // cout << "initiator start" << endl;
  // initiator.start();
  fix_trader.Init(config_file_name);
  sleep(10);
  // cout << "initiator run" << endl;
  // fix_trader.run();
  fstream cmd_file;
  cmd_file.open(cmd_file_name, fstream::in | fstream::app);
  while(!cmd_file.eof()) {
    char line[512];
    cmd_file.getline(line, 512);
    if (line[0] == '#') {
      cout << "\033[32mComment:[" << line << "]\033[0m" << endl;
      continue;
    } else if (strcmp(line, "") == 0) {
      cout << "Passing blank line..." << endl;
      continue;
    }
    cout << "\033[31mCommand:[" << line << "]\033[0m" << endl;
    char cmd;
    while(true) {
      cout << "\033[33mInput command-[E-Execute P-Pass Q-Quit] :\033[0m";
      scanf("%c", &cmd);
      getchar();
      if (cmd == 'E' || cmd == 'e' || cmd == 'P' || cmd == 'p' ||
          cmd == 'Q' || cmd == 'q') {
        break;
      } else {
        cout << "Invalid command : " << cmd << endl;
      }
    }
    if (cmd == 'E' || cmd == 'e') {
      istringstream line_stream(line);
      string request;
      line_stream >> request;
      if (strcasecmp(request.c_str(), "insert") == 0) {
        string symbol, instrument, price, stop_price, volume, direction, 
               order_type, time_in_force;
        line_stream >> symbol >> instrument >> price >> stop_price >> volume 
                    >> direction >> order_type >> time_in_force;
        Order *order = new Order();
        snprintf(order->account, sizeof(order->account), "%s", account.c_str());
        snprintf(order->symbol, sizeof(order->symbol), "%s", symbol.c_str());
        snprintf(order->instrument_id, sizeof(order->instrument_id), "%s", instrument.c_str());
        order->limit_price = atof(price.c_str());
        order->stop_price = atof(stop_price.c_str());
        order->volume = atoi(volume.c_str());
        order->direction = ParseToDirection(direction);
        order->order_type = ParseToOrderType(order_type);
        order->time_in_force = ParseToTimeInForce(time_in_force);
        fix_trader.ReqOrderInsert(order);
      } else if (strcasecmp(request.c_str(), "cancel") == 0) {
        string order_id;
        line_stream >> order_id;
        if (order_id == "-1") {
          string symbol, instrument, side, local_id, sys_id;
          cout << "\033[33mInput order info to cancel:\033[0m" << endl;
          cin >> symbol >> instrument >> side >> local_id >> sys_id;
          getchar();
          fix_trader.ReqOrderAction(symbol, instrument, side, local_id, sys_id,
                                    account);
        } else {
          Order *order = new Order();
          order->orig_order_id = atoi(order_id.c_str());
          fix_trader.ReqOrderAction(order);
        }
      }  else if (strcasecmp(request.c_str(), "cancelall") == 0) {
        string instrument_id;
        line_stream >> instrument_id;
        if (instrument_id == "all") {
        } else {
          Order *order = new Order();
          snprintf(order->instrument_id, sizeof(order->instrument_id),
                   "%s", instrument_id.c_str());
          fix_trader.ReqMassOrderAction(order);
        }
      } else if (strcasecmp(request.c_str(), "modify") == 0) {
        string order_id, volume, price;
        line_stream >> order_id >> price >> volume;
        Order *order = new Order();
        order->orig_order_id = atoi(order_id.c_str());
        order->volume = atoi(volume.c_str());
        order->limit_price = atof(price.c_str());
        fix_trader.ReqOrderReplace(order);
      } else if (strcasecmp(request.c_str(), "heartbeat") == 0) {
        fix_trader.SendHeartbeat();
      } else if (strcasecmp(request.c_str(), "logon") == 0) {
        fix_trader.ReqUserLogon();
      } else if (strcasecmp(request.c_str(), "logout") == 0) {
        fix_trader.ReqUserLogout();
      } else if (strcasecmp(request.c_str(), "resend") == 0) {
        fix_trader.SendResendRequest();
      } else if (strcasecmp(request.c_str(), "testrequest") == 0) {
        fix_trader.SendTestRequest();
      } else if (strcasecmp(request.c_str(), "reset") == 0) {
        fix_trader.SendResetSequence();
      }
      sleep(3);
    } else if (cmd == 'P' || cmd == 'p') {
      continue;
    } else if (cmd == 'Q' || cmd == 'q') {
      cout << "Quit ..." << endl;
      break;
    }
  }

  char q;
  cout << "Confirm to quit ? y/n :";
  scanf("%c", &q);
  getchar();
  // initiator.stop();
  fix_trader.Logout();
  sleep(3);

  return 0;
}
