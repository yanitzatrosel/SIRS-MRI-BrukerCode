/****************************************************************
 *
 * $Source: /pv/CvsTree/pv/gen/src/prg/methods/EPI/parsRelations.c,v $
 *
 * Copyright (c) 1999-2003
 * Bruker BioSpin MRI GmbH
 * D-76275 Ettlingen, Germany
 *
 * All Rights Reserved
 *
 *
 * $Id: parsRelations.c,v 1.51 2012/11/16 11:10:01 fhen Exp $
 *
 ****************************************************************/

static const char resid[] = "$Id: parsRelations.c,v 1.51 2012/11/16 11:10:01 fhen Exp $ (C) 2003 Bruker BioSpin MRI GmbH";

#define DEBUG		0
#define DB_MODULE	0
#define DB_LINE_NR	0

/*:=INFO=:*******************************************************
 *
 * Description :
 *   it contains functions necessary for responding to a 'load'
 *	of this method. This function will be automatically
 *	called in response to a method file for this method
 *	having been read.
 *
 *::=info=:******************************************************/

/****************************************************************/
/****************************************************************/
/*		I N T E R F A C E   S E C T I O N		*/
/****************************************************************/
/****************************************************************/

/****************************************************************/
/*		I N C L U D E   F I L E S			*/
/****************************************************************/

#include "method.h"


/****************************************************************/
/*	I M P L E M E N T A T I O N   S E C T I O N		*/
/****************************************************************/

/* -----------------------------------------------------------
   Relations of ExcPulse
 
   -----------------------------------------------------------*/
   
void ExcPulseEnumRelation(void)
{
  DB_MSG(("-->ExcPulseEnumRelation"));

  UT_SetRequest("ExcPulseEnum");
  backbone();
  
  DB_MSG(("<--ExcPulseEnumRelation"));                                       
}

void ExcPulRelation(void)
{
  DB_MSG(("-->ExcPulRelation"));
  
  UT_SetRequest("ExcPul");
  ExcPulRange();
  backbone();
  
  DB_MSG(("<--ExcPulRelation"));
}

void ExcPulRange(void)
{
  DB_MSG(("-->ExcPulse1Range"));
    
 ExcPul.Flipangle = MIN_OF(90.0,MAX_OF(ExcPul.Flipangle,1.0));

 DB_MSG(("<--ExcPulseRange")); 
}

void ExcPulseAmplRel(void)
{
  DB_MSG(("-->ExcPulseAmplRel"));
  
  UT_SetRequest("ExcPulseAmpl");
  HandleRFPulseAmplitudes();
  
  DB_MSG(("-->ExcPulseAmplRel"));
}

/* -----------------------------------------------------------
   Relations of ExcPulse Changes done YT
 
   -----------------------------------------------------------*/
   
void Exc2PulseEnumRelation(void)
{
  DB_MSG(("-->Exc2PulseEnumRelation"));

  UT_SetRequest("Exc2PulseEnum");
  backbone();
  
  DB_MSG(("<--Exc2PulseEnumRelation"));                                       
}

void Exc2PulRelation(void)
{
  DB_MSG(("-->Exc2PulRelation"));
  
  UT_SetRequest("Exc2Pul");
  Exc2PulRange();
  backbone();
  
  DB_MSG(("<--Exc2PulRelation"));
}

void Exc2PulRange(void)
{
  DB_MSG(("-->Exc2Pulse1Range"));
    
 Exc2Pul.Flipangle = MIN_OF(90.0,MAX_OF(Exc2Pul.Flipangle,1.0)); /*the angle will be between 1 and 90*/

 DB_MSG(("<--Exc2PulseRange")); 
}

void Exc2PulseAmplRel(void)
{
  DB_MSG(("-->Exc2PulseAmplRel"));
  
  UT_SetRequest("Exc2PulseAmpl");
  STB_UpdateRFShapeAmplitude("Exc2PulseAmpl",Yes); /*changed*/
  ATB_SetRFPulse("Exc2Pul","ACQ_RfShapes[6]"); /*changed*/
  
  DB_MSG(("-->Exc2PulseAmplRel"));
}

/* -----------------------------------------------------------
 Changes done by YT end here
   -----------------------------------------------------------*/

/* --------------------------------------------------------------
   Relations of RefPul

   --------------------------------------------------------------*/

void RefPulseEnumRelation(void)
{
  DB_MSG(("-->RefPulseEnumRelation"));

  UT_SetRequest("RefPulseEnum");
  backbone();
  
  DB_MSG(("<--RefPulseEnumRelation"));                                       
}

void RefPulRelation(void)
{
  DB_MSG(("-->RefPulRelation"));
  
  UT_SetRequest("RefPul");
  RefPulRange();
  backbone();
  
  DB_MSG(("<--RefPulRelation"));
}

void RefPulRange(void)
{
  DB_MSG(("-->RefPulse1Range"));
    
 RefPul.Flipangle = MIN_OF(180.0,MAX_OF(RefPul.Flipangle,1.0));

 DB_MSG(("<--RefPulseRange")); 
}

void RefPulseAmplRel(void)
{
  DB_MSG(("-->RefPulseAmplRel"));
  
  UT_SetRequest("RefPulseAmpl");
  HandleRFPulseAmplitudes();
  
  DB_MSG(("-->RefPulseAmplRel"));
}

void HandleRFPulseAmplitudes(void)
{
  DB_MSG(("-->HandleRFPulseAmplitude"));

  STB_UpdateRFShapeAmplitude("ExcPulseAmpl,RefPulseAmpl",No);
  ATB_SetRFPulse("ExcPul","ACQ_RfShapes[0]");
  ATB_SetRFPulse("RefPul","ACQ_RfShapes[1]");

  DB_MSG(("<--HandleRFPulseAmplitude"));
}

/* --------------------------------------------------------------
   Other Relations

   --------------------------------------------------------------*/

void PackDelRange( void )
{
  if(ParxRelsParHasValue("PackDel") == No)
    PackDel = 0.0;
  PackDel = MAX_OF(PackDel,0.0);
}

void PackDelRelation( void )
{
  PackDelRange();
  backbone();
}

void RephaseTimeRels(void)
{
  RephaseTimeRange();
  backbone();
}

void RephaseTimeRange(void)
{
  if(!ParxRelsParHasValue("RephaseTime"))
    RephaseTime = 2.0;

  RephaseTime = MAX_OF(RephaseTime, 2*CFG_GradientRiseTime());
}

void Local_NAveragesRange(void)
{
  int ival;
  DB_MSG(("Entering Local_NAveragesRange"));
  
  /* 
   *  Range check of PVM_NAverages: prevent it to be negative or 0
   */

  if(ParxRelsParHasValue("PVM_NAverages") == No)
    {
      PVM_NAverages = 1;
    }

  ival = PVM_NAverages;
  PVM_NAverages = MAX_OF(ival,1);
  

  DB_MSG(("Exiting Local_NAveragesRange"));


}



void Local_NAveragesHandler(void)
{

  DB_MSG(("Exiting Local_NAveragesHandler with value %d",PVM_NAverages));

  Local_NAveragesRange();

  /*
   *   Averages range check is finished, handle the request by
   *   the method:
   */

  
  backbone();


  DB_MSG(("Exiting Local_NAveragesHandler with value %d",PVM_NAverages));
  return;
}
void LocalSWhRange(void)
{
  DB_MSG(("-->LocalSWhRange"));

  if(!ParxRelsParHasValue("PVM_EffSWh"))
  {
    PVM_EffSWh = 200000.0;
  }
  else
  {
    PVM_EffSWh = MAX_OF(50000.0,PVM_EffSWh);
    PVM_EffSWh = MIN_OF(1000000.0,PVM_EffSWh);
  }

  DB_MSG(("<--LocalSWhRange"));
}

void LocalSWhRels(void)
{
  DB_MSG(("-->LocalSWhRel"));

  UT_SetRequest("PVM_EffSWh");
  LocalSWhRange();
  backbone();

  DB_MSG(("<--LocalSWhRel"));
  return;
}


/* relations of NSegments */

void NSegmentsRels(void)
{
  NSegmentsRange();
  backbone();
}

void NSegmentsRange(void)
{
  if(!ParxRelsParHasValue("NSegments"))
    NSegments = 1;
  else
    NSegments = MAX_OF(1,NSegments);
}

/* relations of slice spoiler */
void SliceSpoilerRel(void)
{
  DB_MSG(("-->SliceSpoilerRel"));
  UT_SetRequest("SliceSpoiler");
  backbone();
  DB_MSG(("<--SliceSpoilerRel"));
}

/****************************************************************/
/*		E N D   O F   F I L E				*/
/****************************************************************/
