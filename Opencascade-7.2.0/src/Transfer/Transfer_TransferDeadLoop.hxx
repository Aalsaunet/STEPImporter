// Created on: 1992-02-03
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _Transfer_TransferDeadLoop_HeaderFile
#define _Transfer_TransferDeadLoop_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Transfer_TransferFailure.hxx>

class Transfer_TransferDeadLoop;
DEFINE_STANDARD_HANDLE(Transfer_TransferDeadLoop, Transfer_TransferFailure)

#if !defined No_Exception && !defined No_Transfer_TransferDeadLoop
  #define Transfer_TransferDeadLoop_Raise_if(CONDITION, MESSAGE) \
  if (CONDITION) throw Transfer_TransferDeadLoop(MESSAGE);
#else
  #define Transfer_TransferDeadLoop_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(Transfer_TransferDeadLoop, Transfer_TransferFailure)

#endif // _Transfer_TransferDeadLoop_HeaderFile
