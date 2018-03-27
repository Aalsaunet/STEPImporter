//! Created on: 2016-06-03
//! Created by: NIKOLAI BUKHALOV
//! Copyright (c) 2016 OPEN CASCADE SAS
//!
//! This file is part of Open CASCADE Technology software library.
//!
//! This library is free software; you can redistribute it and/or modify it under
//! the terms of the GNU Lesser General Public License version 2.1 as published
//! by the Free Software Foundation, with special exception defined in the file
//! OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
//! distribution for complete text of the license and disclaimer of any warranty.
//!
//! Alternatively, this file may be used under the terms of Open CASCADE
//! commercial license or contractual agreement.

#ifndef _IntPatch_SpecialPoints_HeaderFile
#define _IntPatch_SpecialPoints_HeaderFile

//! Contains methods to add some special points
//! (such as apex of cone, pole of sphere,
//! point on surface boundary etc.) in the intersection line.

#include <IntPatch_SpecPntType.hxx>
#include <Standard_Handle.hxx>

class Adaptor3d_HSurface;
class IntPatch_Point;
class IntSurf_PntOn2S;
class math_Vector;

class IntPatch_SpecialPoints
{
public:
  //! Adds the point defined as intersection
  //! of two isolines (U = 0 and V = 0) on theQSurf in theLine.
  //! theRefPt is used to correct adjusting parameters.
  //! If theIsReversed is TRUE then theQSurf correspond to the 
  //! second (otherwise, the first) surface while forming
  //! intersection point IntSurf_PntOn2S.
  Standard_EXPORT static Standard_Boolean 
                      AddCrossUVIsoPoint(const Handle(Adaptor3d_HSurface)& theQSurf,
                                         const Handle(Adaptor3d_HSurface)& thePSurf,
                                         const IntSurf_PntOn2S& theRefPt,
                                         const Standard_Real theTol3d,
                                         IntSurf_PntOn2S& theAddedPoint,
                                         const Standard_Boolean theIsReversed
                                                                = Standard_False);

  //! Adds the point lain strictly in the isoline U = 0 or V = 0 of theQSurf,
  //! in theLine.
  //! theRefPt is used to correct adjusting parameters.
  //! If theIsReversed is TRUE then theQSurf corresponds to the 
  //! second (otherwise, the first) surface while forming
  //! intersection point IntSurf_PntOn2S.  
  Standard_EXPORT static Standard_Boolean
                      AddPointOnUorVIso(const Handle(Adaptor3d_HSurface)& theQSurf,
                                        const Handle(Adaptor3d_HSurface)& thePSurf,
                                        const IntSurf_PntOn2S& theRefPt,
                                        const Standard_Boolean theIsU,
                                        const math_Vector& theToler,
                                        const math_Vector& theInitPoint,
                                        const math_Vector& theInfBound,
                                        const math_Vector& theSupBound,
                                        IntSurf_PntOn2S& theAddedPoint,
                                        const Standard_Boolean theIsReversed
                                                                = Standard_False);

  //! Computes the pole of sphere to add it in the intersection line.
  //! Stores the result in theAddedPoint variable (does not add in the line).
  //! At that, cone and sphere (with singularity) must be set in theQSurf parameter.
  //! By default (if theIsReversed == FALSE), theQSurf is the first surface of the
  //! Walking line. If it is not, theIsReversed parameter must be set to TRUE.
  //! theIsReqRefCheck is TRUE if and only if 3D-point of theRefPt must be pole or apex
  //! for check (e.g. if it is vertex).
  //! thePtIso is the reference point for obtaining isoline where must be placed the Apex/Pole.
  //!
  //! ATTENTION!!!
  //!   theVertex must be initialized before calling the method .
  Standard_EXPORT static Standard_Boolean
                      AddSingularPole(const Handle(Adaptor3d_HSurface)& theQSurf,
                                      const Handle(Adaptor3d_HSurface)& thePSurf,
                                      const IntSurf_PntOn2S& thePtIso,
                                      const Standard_Real theTol3d,
                                      IntPatch_Point& theVertex,
                                      IntSurf_PntOn2S& theAddedPoint,                                      
                                      const Standard_Boolean theIsReversed =
                                                                Standard_False,
                                      const Standard_Boolean theIsReqRefCheck = 
                                                                Standard_False);

  //! Special point has already been added in the line. Now, we need in correct
  //! prolongation of the line or in start new line. This function returns new point.
  //!
  //! ATTENTION!!!
  //!   theNewPoint is not only Output parameter. It is Input/Output one. I.e. 
  //! theNewPoint is reference point together with theRefPt.
  Standard_EXPORT static Standard_Boolean
                    ContinueAfterSpecialPoint(const Handle(Adaptor3d_HSurface)& theQSurf,
                                              const Handle(Adaptor3d_HSurface)& thePSurf,
                                              const IntSurf_PntOn2S& theRefPt,
                                              const IntPatch_SpecPntType theSPType,
                                              const Standard_Real theTol2D,
                                              IntSurf_PntOn2S& theNewPoint,
                                              const Standard_Boolean theIsReversed
                                                                = Standard_False);

  //! Sets theNewPoint parameters in 2D-space the closest to
  //! theRefPoint with help of adding/subtracting corresponding periods.
  //! theArrPeriods must be filled as follows:
  //! {<U-period of 1st surface>, <V-period of 1st surface>,
  //!  <U-period of 2nd surface>, <V-period of 2nd surface>}.
  //! If theVertex != 0 then its parameters will be filled as
  //! corresponding parameters of theNewPoint.
  //!
  //! ATTENTION!!!
  //!   theNewPoint is not only Output parameter. It is Input/Output one. I.e. 
  //! theNewPoint is reference point together with theRefPt.
  Standard_EXPORT static void 
                      AdjustPointAndVertex(const IntSurf_PntOn2S &theRefPoint,
                                           const Standard_Real theArrPeriods[4],
                                           IntSurf_PntOn2S &theNewPoint,
                                           IntPatch_Point* const theVertex = 0);
};

#endif // _IntPatch_AddSpecialPoints_HeaderFile
