/****************************************************************
 *
 * $Source: /pv/CvsTree/pv/gen/src/prg/methods/EPI/backbone.c,v $
 *
 * Copyright (c) 1999-2011
 * Bruker BioSpin MRI GmbH
 * D-76275 Ettlingen, Germany
 *
 * All Rights Reserved
 *
 *
 * $Id: backbone.c,v 1.30 2013/06/06 12:51:58 sako Exp $
 *
 ****************************************************************/

static const char resid[] = "$Id: backbone.c,v 1.30 2013/06/06 12:51:58 sako Exp $ (C) 2003 Bruker BioSpin MRI GmbH";

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

double  MinTE_left, MinTE_right;

/****************************************************************/
/*	I M P L E M E N T A T I O N   S E C T I O N		*/
/****************************************************************/

double  LimExSliceGradient;
double  LimExSliceRephaseGradient;
double  SliceGradRatio;
double  Phase2dInteg;
double  ReadGradient;

/****************************************************************/
/*		G L O B A L   F U N C T I O N S			*/
/****************************************************************/


/* ------------------------------------------------------------ 
  backbone 
  The main part of method code. The consitency of all parameters is checked
  chere, relations between them are resolved and, finally, functions setting
  the base level parameters are called.
  --------------------------------------------------------------*/
void backbone( void )
{
  int dim, ret, nImagesPerRep;
  double minFov[3] = {1e-3, 1e-3, 1e-3},
         minThickness;

  
  
  DB_MSG(("Entering EPI/backbone.c:backbone"));


  /* update of preemphasis; must take place before nuclei update */
  STB_UpdatePreemphasis();

  /* Nuclei and  PVM_GradCalConst  are handled by this funtion: */
  STB_UpdateNuclei(Yes);
 
 
  /* do not allow a-aliasing */
  PVM_AntiAlias[1] = 1.0;

  /* Encoding
   * note: Grappa reference lines are disabled. Grappa coeeficients will be set
   * in a special adjustment. */
  STB_UpdateEncodingForEpi(
    &NSegments,        /* segment size */
    Yes,               /* ppi in 2nd dim allowed */
    No,                /* ppi ref lines in 2nd dim allowed */
    Yes);              /* partial ft in 2nd dim allowed */ 

  dim = PTB_GetSpatDim();
  
  /* handle RF pulse */   
  
  STB_UpdateRFPulse("ExcPul",1,PVM_DeriveGains,Conventional);
  STB_UpdateRFPulse("Exc2Pul",1,PVM_DeriveGains,Conventional); /* added by YT */
  STB_UpdateRFPulse("RefPul",1,PVM_DeriveGains,Conventional);
  
  if(PVM_DeriveGains==Yes)
  {
    ParxRelsHideInEditor("ExcPulseAmpl");
  }
  else
  {
    ParxRelsShowInEditor("ExcPulseAmpl");
  }
  
  if (PVM_SignalType == SignalType_Fid)
  {
    ParxRelsHideInEditor("RefPul, RefPulseEnum, RefPulseAmpl");
  }
  else
  {
    ParxRelsShowInEditor("RefPul, RefPulseEnum");
    if (PVM_DeriveGains == Yes)
    {
      ParxRelsHideInEditor("RefPulseAmpl");
    }
    else
    {
      ParxRelsShowInEditor("RefPulseAmpl");
    }
  }
    
  /* sequence variables */
  LimExSliceGradient = 100;
  LimExSliceRephaseGradient = 50;

  /*** Update Geometry **/
  
  LocalGeometryMinimaRels(&minThickness);

  /* do not allow isotropic geometry */
  PVM_IsotropicFovRes = Isot_None;

  // only one package with one slice if 3D
  int maxPackages = dim>2? 1:0;
  int maxPerPackage = dim>2? 1:0;
  
  STB_UpdateImageGeometry(dim, 
                          PVM_Matrix,
                          minFov,
                          0, // total slices (no restr)
                          maxPackages,
                          maxPerPackage,
                          minThickness,
                          1.0); // sliceFovRatio in 3D
                            
  /** Update EPI parameter group */

  PVM_NRepetitions = MAX_OF(1,PVM_NRepetitions);
  PVM_NEchoImages = 1;
  nImagesPerRep = PVM_NEchoImages * GTB_NumberOfSlices( PVM_NSPacks, PVM_SPackArrNSlices );

  ret = STB_EpiUpdate(No,
		      dim, 
		      PVM_EncMatrix, 
		      PVM_AntiAlias,
		      PVM_Fov, 
		      minFov, 
		      &PVM_EffSWh, 
		      PVM_GradCalConst, 
		      nImagesPerRep,
		      NSegments,
		      PVM_EncCentralStep1,
		      PVM_EncPpi[1],
		      PVM_EncNReceivers);
  
  if(ret <0)
    DB_MSG(("--!!!!!!!!!!!!!!! Illegal arguments for STB_UpdateEPI: EPI Module not ready !"));
 
  /* Update geometry again (minFov is known) */
  
  ConstrainReadOffset(); /* special for EPI */

  STB_UpdateImageGeometry(dim, 
                          PVM_Matrix,
                          minFov,
                          0, // total slices (no restr)
                          maxPackages,
                          maxPerPackage,
                          minThickness,
                          1.0); // sliceFovRatio in 3D
                                 
  /*** end Update Geometry **/

  LocalGradientStrengthRels();  
  LocalFrequencyOffsetRels();

  if (ParxRelsParHasValue("PVM_NAverages") == 0)
     PVM_NAverages = 1;

  /* update slice spoiler */
  double mindurSlice = MAX_OF(2.0, 2.0*CFG_GradientRiseTime());
  double spoilerThick = dim>2 ? PVM_SpatResol[2]*PVM_EncZf[2] : PVM_SliceThick;
  MRT_UpdateSpoiler("SliceSpoiler",5.0,LimExSliceGradient,mindurSlice,PVM_GradCalConst,spoilerThick);

  /* handling of modules */
  STB_UpdateFatSupModule(PVM_Nucleus1, PVM_DeriveGains, spoilerThick);
  STB_UpdateMagTransModule(PVM_DeriveGains);
  STB_UpdateFovSatModule(PVM_Nucleus1, PVM_DeriveGains);
  STB_UpdateTriggerModule();
  STB_UpdateTriggerOutModule();
  STB_UpdateTaggingModule(PVM_Fov,PVM_Matrix,PVM_SpatResol[0]*PVM_EncZf[0],PVM_DeriveGains); 

  rfcSpoilerUpdate();

  echoTimeRels();

  repetitionTimeRels();

  PTB_ClearAdjustments();

  PTB_AppendConfiguredAdjustment(per_scan,RCVR);

  PTB_AppendAdjustment("EpiTraj",
                       "Adjust Traj.",
                       "Adjust EPI Trajectory",
                       on_demand,USERADJ1);

  if(PVM_EncPpi[1] > 1)
  {
    PTB_AppendAdjustment("EpiGrappa",
                         "Adjust GRAPPA Coeff.",
                         "Adjustment of GRAPPA Coefficients",
                         per_scan,USERADJ2);
  }

  STB_UpdateDummyScans(PVM_RepetitionTime);
  
  /* update mapshim parameter class */
  STB_UpdateMapShim(PVM_Nucleus1,"PVM_SliceGeoObj");

  /* set baselevel acquisition parameter */
  SetBaseLevelParam();
  
  /* set baselevel reconstruction parameter */
  SetRecoParam(PVM_EncPpi[1]);


  /* adapt size of trajectory arrays if necessary and set 
     PVM_EpiTrajAdj to No if a trajectory relevant parameter has changed */

  STB_EpiCheckTrajectory(PVM_Fov[0],PVM_EffSWh,PVM_SPackArrGradOrient[0][0]);
 
  DB_MSG(("Exiting EPI/backbone.c:backbone"));
}


void rfcSpoilerUpdate(void)
{
  RfcSpoilerStrength = 2*ExSliceGradient;
  RfcSpoilerStrength = MIN_OF(RfcSpoilerStrength,80);
  RfcSpoilerStrength = MAX_OF(RfcSpoilerStrength,30);

  RfcSpoilerDuration = MAX_OF(RefPul.Length, 2*PVM_RiseTime);
}

/****************************************************************/
/*	         L O C A L   F U N C T I O N S			*/
/****************************************************************/


void echoTimeRels( void )
{
  DB_MSG(("Entering EPI/backbone.c:echoTimeRels()"));

  if(PVM_SignalType == SignalType_Fid)
  {
    PVM_MinEchoTime =      /* min gradient echo time */
      ExcPul.Length * ExcPul.Rpfac/100 +
      PVM_RiseTime               +
      PVM_InterGradientWaitTime  +
      RephaseTime       +
      PVM_InterGradientWaitTime  +
      PVM_EpiEchoDelay;
    MinTE_right  = MinTE_left = 0.0; /* not used */
  }
  else
  {
    MinTE_left =  /* min half spinecho-time given by left hand side of pi */
    ExcPul.Length * ExcPul.Rpfac/100  +
    PVM_RiseTime               +
    PVM_InterGradientWaitTime  + 
    RephaseTime       +
    PVM_InterGradientWaitTime  +
    RfcSpoilerDuration + 
    RefPul.Length/2.0;

    MinTE_right = /* min half spinecho-time given by right hand side of pi */
    RefPul.Length/2.0 +
    RfcSpoilerDuration +
    PVM_InterGradientWaitTime  +
    PVM_EpiEchoDelay;

    PVM_MinEchoTime = 2 * MAX_OF(MinTE_left, MinTE_right);
  }

  EchoTime = EchoTime < PVM_MinEchoTime ?
    PVM_MinEchoTime : EchoTime;

  
  /* Set Echo Parameters for Scan Editor   */
  PVM_EchoTime = PVM_Matrix[0]*PVM_DigDw;  /* echo spacing */
  
  PVM_EchoPosition = 50.0;
 
  DB_MSG(("Exiting EPI/backbone.c:echoTimeRels"));
}

void repetitionTimeRels( void )
{
  int nSlices,dim;
  double TotalTime,trigger, trigger_v, trigOutSlice, trigOutVol;
  double amplifierenable;

  DB_MSG(("--> minRepetitionTimeRels"));

  trigger = STB_UpdateTriggerModule();
  if(PVM_TriggerMode == per_PhaseStep) /* per volume */
  {
    trigger_v=trigger;
    trigger=0.0; 
  }
  else trigger_v=0.0;

  amplifierenable = CFG_AmplifierEnable();
  TotalTime = 0.0;
  nSlices = GTB_NumberOfSlices( PVM_NSPacks, PVM_SPackArrNSlices );

  if(PVM_TriggerOutOnOff == On)
  {
    switch(PVM_TriggerOutMode)
    {
    case PER_SLICE:   
      trigOutSlice = PVM_TriggerOutModuleTime;
      trigOutVol = 0.0;
      break;

    case PER_VOLUME: 
      trigOutSlice = 0.0;
      trigOutVol = PVM_TriggerOutModuleTime;
      break;

    case AT_START:
    default:
      trigOutSlice = 0.0;
      trigOutVol = 0.0;
      
    }
  }
  else
    trigOutSlice = trigOutVol = 0.0;

  dim = PTB_GetSpatDim();
  
  if(dim>2) /* disable inter-volume delay in 3d */
  {
    PackDel=0;
    ParxRelsMakeNonEditable("PackDel");
  }
  else
  {
    ParxRelsMakeEditable("PackDel");
  }


  PVM_MinRepetitionTime =
    nSlices * ( 0.01 +
               0.01 + /* added 10u YT */
               Exc2Pul.Length + /* my p6 pulse YT */
               0.005 + /* added delay 5u YT */
               PVM_FatSupModuleTime +
               PVM_MagTransModuleTime +
               PVM_FovSatModuleTime +
               trigger +
               trigOutSlice +
               SliceSpoiler.dur +
	       CFG_GradientRiseTime() +
               amplifierenable +
               ExcPul.Length/2 +
               EchoTime +
               PVM_EpiModuleTime - PVM_EpiEchoDelay +
               0.01 + /* 10u */
               0.1 /* 100u: min d0, includes islice */
    ) + 
    PVM_TaggingModuleTime +
    trigOutVol +
    trigger_v +
    PackDel +
    0.010;

  if(PackDel==0.0) PVM_MinRepetitionTime+=0.001; 

  PVM_RepetitionTime = ( PVM_RepetitionTime < PVM_MinRepetitionTime ? 
			 PVM_MinRepetitionTime : PVM_RepetitionTime );
  
  /** Calculate Total Scan Time and Set for Scan Editor **/ 

  if( dim >1 )
    TotalTime = PVM_RepetitionTime*PVM_EpiNShots*PVM_NAverages*PVM_NRepetitions;
  if( dim >2 )
    TotalTime *= PVM_EncMatrix[2];

  /* time for one repetition */
  OneRepTime = TotalTime/(PVM_NRepetitions*1000.0);

  PVM_ScanTime = TotalTime;
  UT_ScanTimeStr(PVM_ScanTimeStr,TotalTime);
 

  ParxRelsShowInEditor("PVM_ScanTimeStr");
  ParxRelsMakeNonEditable("PVM_ScanTimeStr");

  DB_MSG(("<-- repetitionTimeRels"));
}

void LocalGeometryMinimaRels(double *min_thickness )
{
  /*
    This function calculates the minima for the minimum  slice thickness.
    It is always assumed that all slices have the same thickness
    (WE DO NOT set min_fov[0 and 1] here, this will be done by the epi module) 
 */

  double sliceRampInteg; /* normalised integral falling slice gradient ramp */
  double sliceRephInteg; /* normalised integral slice rephase gradient      */
    
  /* min slice thickness: */
  /*  Calculate the normalised integral of the descending gradient ramp after
      the RF pulse */
  sliceRampInteg = 0.5 *  PVM_RiseTime;
  /* Calculate the normalised integral of the slice selection rephasing
     gradient */
  sliceRephInteg = RephaseTime - PVM_RiseTime;;
  
  /*
	Calculate the ratio of the strength of the slice selection gradient to
	the strength of the slice selection rephase gradient

  */
  SliceGradRatio = MRT_SliceGradRatio( ExcPul.Length,
                                       ExcPul.Rpfac,
                                       0.0,
                                       sliceRampInteg,
                                       sliceRephInteg );
  /*
    Calculate the minimum slice thickness
  */
      
  *min_thickness = MRT_MinSliceThickness( ExcPul.Bandwidth,
					  SliceGradRatio,
					  LimExSliceGradient,
					  LimExSliceRephaseGradient,
					  PVM_GradCalConst );
     
    
} /* end of localGeometryMinima() */



void LocalGradientStrengthRels( void )
{
  /*
    This function calculates all the gradient strengths 
  */

  switch( PTB_GetSpatDim() )
    {
    case 3: /* PHASE ENCODING GRADIENT - 3nd DIM */
     
     /* falls through */
    case 1:
      ReadGradient = (PVM_EpiReadEvenGrad+PVM_EpiReadOddGrad)/2.0; /* used in LocFreqOff */
     /* falls through */
    default: /* SLICE SELECTION GRADIENT */
      ExSliceGradient = MRT_SliceGrad( ExcPul.Bandwidth,
				       PVM_SliceThick,
				       PVM_GradCalConst );
      ExSliceRephaseGradient = MRT_SliceRephaseGrad( SliceGradRatio,
							 ExSliceGradient );
      break;
    }
}

void LocalFrequencyOffsetRels( void )
{
  int spatDim;
  int i,nslices;

  spatDim = PTB_GetSpatDim();

  nslices = GTB_NumberOfSlices(PVM_NSPacks,PVM_SPackArrNSlices);

  /* set ReadOffsetHz to zero. In EPI the fov offset is made by reco, not by detection freq. offsets */
  MRT_FrequencyOffsetList(nslices,
			  PVM_EffReadOffset,
			  ReadGradient,
			  0.0, /* instead PVM_GradCalConst; this sets offHz to zero */
			  PVM_ReadOffsetHz );

  MRT_FrequencyOffsetList(nslices,
			  PVM_EffSliceOffset,
			  ExSliceGradient,
			  PVM_GradCalConst,
			  PVM_SliceOffsetHz );

  if(spatDim == 3)
  {
    for(i=0;i<nslices;i++)
      PVM_EffPhase2Offset[i] = -PVM_SliceOffset[i];
  }


}


/*
 *  local function to constrain the read offset for slice packages
 */

void ConstrainReadOffset(void)
{
  int dim=0,i=0;
  double *offs, max;
  DB_MSG(("-->ConstrainReadOffset"));

  dim = PARX_get_dim("PVM_SPackArrReadOffset",1);
  offs = PVM_SPackArrReadOffset;

  max = (PVM_Fov[0]/2)*(PVM_AntiAlias[0]-1.0);
  max = MAX_OF(max, 0);

  for(i=0;i<dim;i++)
  {
    offs[i]=MAX_OF(offs[i], -max);
    offs[i]=MIN_OF(offs[i], max);
  }


  DB_MSG(("<--ConstrainReadOffset"));
  return;
}


/****************************************************************/
/*		E N D   O F   F I L E				*/
/****************************************************************/
