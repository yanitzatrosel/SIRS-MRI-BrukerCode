/****************************************************************
 *
 * $Source: /pv/CvsTree/pv/gen/src/prg/methods/EPI/initMeth.c,v $
 *
 * Copyright (c) 1999-2003
 * Bruker BioSpin MRI GmbH
 * D-76275 Ettlingen, Germany
 *
 * All Rights Reserved
 *
 *
 * $Id: initMeth.c,v 1.33 2013/05/22 11:02:08 sako Exp $
 *
 ****************************************************************/

static const char resid[] = "$Id: initMeth.c,v 1.33 2013/05/22 11:02:08 sako Exp $ (C) Bruker BioSpin MRI GmbH";

#define DEBUG		0
#define DB_MODULE	1
#define DB_LINE_NR	1

/*:=INFO=:*******************************************************
 *
 * Description :
 *   it contains functions necessary for initialisation of the
 *	onepulse method.
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


/****************************************************************/
/*		G L O B A L   F U N C T I O N S			*/
/****************************************************************/


/*:=MPB=:=======================================================*
 *
 * Global Function: initMeth
 *
 * Description: This procedure is implicitly called when this
 *	method is selected.
 *
 * Error History: 
 *
 * Interface:							*/

void initMeth()
/*:=MPE=:=======================================================*/
{
  int dimRange[2] = { 2,3 };
  int lowMat[3] = { 16, 16, 8 };
  int upMat[3]  = { 512, 512, 256 };
  int defaultMat[3] = {96, 64, 32};

  DB_MSG(( "Entering epi2:initMeth()" ));

  /* which version of toolboxes should be active */
  PTB_VersionRequirement( Yes,20100101,"");
  
  /* initialize the Preemphasis group */
  STB_InitPreemphasis();
  
  /*  Initialize NA ( see code in parsRelations ) */
  Local_NAveragesRange();

  RephaseTimeRange();

  /* segments */
  NSegmentsRange();

  /* Encoding */
  STB_InitEncoding( 2, dimRange, lowMat, upMat, defaultMat);  

  /*Initialisation of repetitions time*/
  if(ParxRelsParHasValue("PVM_RepetitionTime") == No)
      PVM_RepetitionTime = 1000;

  /*Initialisation of automatic derive RF gains*/
  if(ParxRelsParHasValue("PVM_DeriveGains") == No)
      PVM_DeriveGains = Yes;

  /* Initialisation of signal type */
  if(ParxRelsParHasValue("PVM_SignalType") == No)
      PVM_SignalType = SignalType_Echo;


  /* Initialization of rf pulse parameters */
  
  STB_InitRFPulse("ExcPul",        // name of pulse structure
                  "ExcPulseEnum",  // name of pulse list parameter
                  "ExcPulseAmpl",  // name of pulse amplitude parameter
                  "ExcPulseShape", // name of pulse shape (for calc. pulses)
                  0,               // used for excitation
                  "Calculated", // default shape
                  2000.0,          // default bandwidth
                  90.0);           // default pulse angle
  ExcPulRange();
  
  /* Initialization of rf pulse 2 parameters YT */
  if(!ParxRelsParHasValue("Exc2Pul"))
  {
  STB_InitRFPulse("Exc2Pul",        // name of pulse structure
                  "Exc2PulseEnum",  // name of pulse list parameter
                  "Exc2PulseAmpl",  // name of pulse amplitude parameter
                  "Exc2PulseShape", // name of pulse shape (for calc. pulses)
                  0,               // used for excitation
                  "Calculated", // default shape
                  2000.0,          // default bandwidth
                  90.0);           // default pulse angle
  }
  Exc2PulRange();

   /* end of YT changes */
  
  STB_InitRFPulse("RefPul",        // name of pulse structure
                  "RefPulseEnum",  // name of pulse list parameter
                  "RefPulseAmpl",  // name of pulse amplitude parameter
                  "RefPulseShape", // name of pulse shape (for calc. pulses)
                  1,               // used for excitation
                  "Calculated", // default shape
                  2000.0,          // default bandwidth
                  180.0);          // default pulse angle
  RefPulRange();

  /* Initialisation of nucleus and frequency */  
  STB_InitNuclei(1);


  /* Initialisation of the delay between the slices packages */  
  PackDelRange();
 
  /* Initialisation of spoilers */
  MRT_InitSpoiler("SliceSpoiler");
  
  /* Initialisation of geometry parameters */ 
  STB_InitImageGeometry();
    
  /* initializtion of bandwidth */
  LocalSWhRange();
  STB_InitDigPars();

  
  /* not a csi experiment */
  PTB_SetSpectrocopyDims( 0, 0 );

  /* Initialisation of modules */
  STB_InitEpi(UserSlope);
  STB_InitFatSupModule();
  STB_InitMagTransModule();
  STB_InitFovSatModule();
  STB_InitTriggerModule();
  STB_InitTriggerOutModule();
  STB_InitTaggingModule();
  STB_InitDummyScans(1000.0);

  /* initialize mapshim parameter class */
  STB_InitMapShim();

  /* Visibility of Scan Editor parameters */
  ParxRelsShowInEditor("PVM_EchoTime,PVM_NEchoImages");
  ParxRelsShowInFile("PVM_EchoTime,PVM_NEchoImages");
  ParxRelsMakeNonEditable("PVM_EchoTime,PVM_MinEchoTime");
  ParxRelsMakeNonEditable("PVM_NEchoImages,PVM_EchoPosition");


  /* Once all parameters have initial values, the backbone is called
     to assure they are consistent */
  backbone();
 

  DB_MSG(( "Exiting epi2:initMeth()" ));

}



/****************************************************************/
/*		E N D   O F   F I L E				*/
/****************************************************************/









