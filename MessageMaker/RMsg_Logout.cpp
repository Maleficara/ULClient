// RMsg_Logout.cpp  -*- C++ -*-
// $Id: RMsg_Logout.cpp,v 1.8 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_Logout.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock2.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#endif /* WIN32 */
#include <stdio.h>
#include <string.h>

#include "RMsg_Logout.h"
#include "RMsg.h"
#include "LyraDefs.h"

#ifndef USE_INLINE
#include "RMsg_Logout.i"
#endif

////
// constructor
////

RMsg_Logout::RMsg_Logout()
  : LmMesg(RMsg::LOGOUT, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(RMsg_Logout::UNKNOWN, 0);
}

////
// destructor
////

RMsg_Logout::~RMsg_Logout()
{
  // empty
}

////
// Init
////

void RMsg_Logout::Init(int status, int num_items)
{
  SetStatus(status);
  SetNumItems(num_items);
}

////
// hton
////

void RMsg_Logout::hton()
{
  HTONL(data_.status);
  for (int i = 0; i < NumItems(); ++i) {
    HTONL(data_.items[i].serial);
    HTONS(data_.items[i].pos);
    HTONS(data_.items[i].flags);
  }
  HTONS(data_.num_items);
}

////
// ntoh
////

void RMsg_Logout::ntoh()
{
  NTOHL(data_.status);
  NTOHS(data_.num_items);
  for (int i = 0; i < NumItems(); ++i) {
    NTOHL(data_.items[i].serial);
    NTOHS(data_.items[i].pos);
    NTOHS(data_.items[i].flags);
  }
  calc_size(); // variable-length message
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_Logout::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_Logout[%p,%d]: "), this, sizeof(RMsg_Logout));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("status=%c items=%d>\n"), data_.status, data_.num_items);
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetNumItems
////

void RMsg_Logout::SetNumItems(int num_items)
{
  // prevent overflow
  data_.num_items = MIN(num_items, Lyra::INVENTORY_MAX);
  calc_size();
}

////
// calc_size
////

void RMsg_Logout::calc_size()
{
  // initial size: entire struct, minus variable-length inventory
  int size = sizeof(data_t) - sizeof(data_.items);
  // add inventory length
  size += (data_.num_items * sizeof(item_state_t));
  SetMessageSize(size);
}
