/*
 * OrderBookCache.cpp
 *
 *  Created on: Jul 16, 2014
 *      Author: sunny
 */

#include "OrderBookCache.h"


OrderBookCache::OrderBookCache(const dma::Identity id) : m_Identity(id)
{
  return;
}

void OrderBookCache::ResetAllOrderBooks()
{
  for (map<uint32_t, OrderBook*>::iterator it = m_OrderBook.begin(); it != m_OrderBook.end(); ++it)
  {
    it->second->Reset();
  }
  return;
}

void OrderBookCache::DeleteAllOrderBooks()
{
  for (map<uint32_t,OrderBook*>::iterator it = m_OrderBook.begin(); it != m_OrderBook.end(); ++it)
  {
    delete it->second;
    it->second = NULL;
  }
  m_OrderBook.clear();

  return;
}

OrderBook* OrderBookCache::GetOrderBook(uint32_t uiCode)
{
  map<uint32_t,OrderBook*>::iterator it = m_OrderBook.find(uiCode);
  if (it != m_OrderBook.end())
  {
    return it->second;
  }
  else
  {
    OrderBook* ob = NULL;
    
    if      (m_Identity == dma::OMDC) ob = new OrderBook();
    else if (m_Identity == dma::OMDD) ob = new OrderBookOMDDSP();
    else if (m_Identity == dma::MDP ) ob = new OrderBook();
    
    m_OrderBook[uiCode] = ob;
    return ob;
  }
}
