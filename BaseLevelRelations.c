/****************************************************************
 *
 * $Source: /pv/CvsTree/pv/gen/src/prg/methods/EPI/BaseLevelRelations.c,v $
 *
 * Copyright (c) 2001 - 2009
 * Bruker BioSpin MRI GmbH
 * D-76275 Ettlingen, Germany
 *
 * All Rights Reserved
 *
 * $Id: BaseLevelRelations.c,v 1.46 2012/06/04 09:12:56 sako Exp $
 *
 ****************************************************************/

static const char resid[] = "$Id: BaseLevelRelations.c,v 1.46 2012/06/04 09:12:56 sako Exp $ (C) 2001 - 2009 Bruker BioSpin MRI GmbH";

#define DEBUG		0
#define DB_MODULE	1
#define DB_LINE_NR	1

extern double MinTE_left, MinTE_right;

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



void SetBaseLevelParam( void )
{

  DB_MSG(("Entering SetBaseLevelParam()"));


  SetBasicParameters();

  SetFrequencyParameters();

  SetPpgParameters();

  SetGradientParameters();

  SetInfoParameters();

  SetMachineParameters();

  /* multiple receivers */

  (void)ATB_SetMultiRec();


  /* setting baselevel parameters used by modules */
  ATB_SetFatSupBaselevel();
  ATB_SetMagTransBaseLevel();
  ATB_SetFovSatBaseLevel();
  ATB_EpiSetBaseLevel();
  ATB_SetTaggingBaseLevel();

#if DEBUG
  printTiming();
#endif

  DB_MSG(("Exiting SetBaseLevelParam"));

}


/* Toolboxes referenced in this file: ATB, GTB, PTB, STB, UT */


void SetBasicParameters( void )
{
  int spatDim, specDim;
  int nSlices;
  int dim;

  DB_MSG(("Entering SetBasicParameters()"));
    
  /* ACQ_dim */

  spatDim = PTB_GetSpatDim();

  specDim = PTB_GetSpecDim();

  ACQ_dim = spatDim + specDim;
  ParxRelsParRelations("ACQ_dim",Yes);

  /* ACQ_dim_desc */

  ATB_SetAcqDimDesc( specDim, spatDim, NULL );

  /* ACQ_size */

  /*
  ATB_SetAcqSize( Spatial, spatDim, PVM_EncMatrix, NULL, No );
  */
  ACQ_size[0] = PVM_EpiNSamplesPerScan*2;
  ACQ_size[1] = PVM_EpiNShots;
  if(spatDim>2)
    ACQ_size[2] = PVM_EncMatrix[2];

  /* NSLICES */

  nSlices = GTB_NumberOfSlices( PVM_NSPacks, PVM_SPackArrNSlices );

  ATB_SetNSlices( nSlices );

  /* NR */

  ATB_SetNR( PVM_NRepetitions );

  /* NI */

  ATB_SetNI( nSlices * PVM_NEchoImages );

  /* NA */

  ATB_SetNA( PVM_NAverages );

  /* NAE */

  ATB_SetNAE( 1 );

  /* ACQ_ns */

  ACQ_ns_list_size = PVM_NEchoImages;
  
  dim = PARX_get_dim("ACQ_ns_list",1);
  if( dim != 1 )
  {
    PARX_change_dims( "ACQ_ns_list",1 );
  }

  NS = 1;
  ACQ_ns = NS;
  ACQ_ns_list[0] = ACQ_ns;

  ParxRelsParRelations("ACQ_ns",Yes);


  /* NECHOES */

  NECHOES = PVM_NEchoImages;

  /* ACQ_obj_order */

  PARX_change_dims("ACQ_obj_order",NI);

  ATB_SetAcqObjOrder( nSlices, PVM_ObjOrderList, PVM_NEchoImages, 1 );


  /* DS */

  DS = 0; /* L[0] = PVM_DummyScans is directly used in ppg */
  ACQ_DS_enabled = No;

    
  ATB_DisableAcqUserFilter();

  ATB_SetAcqScanSize( One_scan );

  DB_MSG(("Exiting SetBasicParameters()"));
}

void SetFrequencyParameters( void )
{
  int nslices;

  DB_MSG(("Entering SetFrequencyParameters()"));

  ATB_SetNuc1(PVM_Nucleus1);
 
  sprintf(NUC2,"off");
  sprintf(NUC3,"off");
  sprintf(NUC4,"off");
  sprintf(NUC5,"off");
  sprintf(NUC6,"off");
  sprintf(NUC7,"off");
  sprintf(NUC8,"off");

  ATB_SetNucleus(PVM_Nucleus1);

  /* setting of SW_h, DIGMOD, DSPFIRM and AQ_mod */

  ATB_SetDigPars();
  
  ACQ_O1_mode = BF_plus_Offset_list;
  ParxRelsParRelations("ACQ_O1_mode",Yes);

  ACQ_O2_mode = BF_plus_Offset_list;
  ParxRelsParRelations("ACQ_O2_mode",Yes);

  ACQ_O3_mode = BF_plus_Offset_list;
  ParxRelsParRelations("ACQ_O3_mode",Yes);

  O1 = 0.0;
  O2 = 0.0;
  O3 = 0.0;
  O4 = 0.0;
  O5 = 0.0;
  O6 = 0.0;
  O7 = 0.0;
  O8 = 0.0;

  /* Set BF's to working frequencies on used channels */
  ACQ_BF_enable = No;
  BF1 = PVM_FrqWork[0];
  BF2 = PVM_FrqWork[1];
  /* call relations of BF1 (no need for other BF's) */
  ParxRelsParRelations("BF1", Yes); 

  nslices = GTB_NumberOfSlices( PVM_NSPacks, PVM_SPackArrNSlices );

  ATB_SetAcqO1List( nslices,
                    PVM_ObjOrderList,
                    PVM_SliceOffsetHz );

  ATB_SetAcqO1BList( nslices,
                     PVM_ObjOrderList,
                     PVM_ReadOffsetHz);

  DB_MSG(("Exiting SetFrequencyParameters()"));
}

void SetGradientParameters( void )
{
  int spatDim, dim, i;

  DB_MSG(("Entering SetGradientParameters()"));
  

  ATB_SetAcqPhaseFactor( 1 );

  spatDim = PTB_GetSpatDim();

  dim = PARX_get_dim("ACQ_phase_encoding_mode", 1 );

  if( dim != spatDim )
    {
      PARX_change_dims("ACQ_phase_encoding_mode", spatDim );
    }

  ACQ_phase_encoding_mode[0] = Read;
  ACQ_phase_encoding_mode[1] = Linear;
  if(spatDim==3)
  {
    ACQ_phase_encoding_mode[2] = User_Defined_Encoding;
    ACQ_spatial_size_2 = PVM_EncMatrix[2];
    ParxRelsCopyPar("ACQ_spatial_phase_2","PVM_EncValues2");
  }

  ParxRelsParRelations("ACQ_phase_encoding_mode",Yes);

  dim = PARX_get_dim("ACQ_phase_enc_start", 1 );

  if( dim != spatDim )
  {
    PARX_change_dims("ACQ_phase_enc_start", spatDim );
  }

  for( i=0; i<spatDim; i++ )
  {
    ACQ_phase_enc_start[i] = -1;
  }
  
   
  ATB_SetAcqGradMatrix( PVM_NSPacks, PVM_SPackArrNSlices,
			PtrType3x3 PVM_SPackArrGradOrient[0],
			PVM_ObjOrderList );

  ACQ_scaling_read  = 1.0;
  ACQ_scaling_phase = 1.0;
  ACQ_scaling_slice = 1.0;

  ACQ_rare_factor = 1;

  ACQ_grad_str_X = 0.0;
  ACQ_grad_str_Y = 0.0;
  ACQ_grad_str_Z = 0.0;


  strcpy(GRDPROG, "");
   
  /* gradient amplitudes */
  ACQ_gradient_amplitude[0] = ExSliceGradient;         /* g0 */
  ACQ_gradient_amplitude[1] = -ExSliceRephaseGradient; /* g1 */
  ACQ_gradient_amplitude[2] = SliceSpoiler.ampl;       /* g2 */
  ACQ_gradient_amplitude[3] = RfcSpoilerStrength;      /* g3 */

  DB_MSG(("Exiting SetGradientParameters()"));
}

void SetInfoParameters( void )
{
  int slices, i, spatDim, nrep;

  DB_MSG(("Entering SetInfoParameters()"));

  // initialize ACQ_n_echo_images ACQ_echo_descr
  //            ACQ_n_movie_frames ACQ_movie_descr
  ATB_ResetEchoDescr();
  ATB_ResetMovieDescr();

  spatDim = PTB_GetSpatDim();
  nrep = PVM_NRepetitions;

  ATB_SetAcqMethod();

  ATB_SetAcqFov( Spatial, spatDim, PVM_Fov, PVM_AntiAlias );

  ACQ_flip_angle = ExcPul.Flipangle;

  PARX_change_dims("ACQ_echo_time",1);
  ACQ_echo_time[0] = EchoTime;

  PARX_change_dims("ACQ_inter_echo_time",1);
  ACQ_inter_echo_time[0] = PVM_EchoTime;

  PARX_change_dims("ACQ_repetition_time",1);
  ACQ_repetition_time[0] = PVM_RepetitionTime;

  PARX_change_dims("ACQ_recov_time",1);
  ACQ_recov_time[0] =  PVM_RepetitionTime - ExcPul.Length;

  /* calculation of ACQ_time_points */
  PARX_change_dims("ACQ_time_points",nrep);
  ACQ_time_points[0] = 0;
  for(i=1; i<nrep; i++)
    ACQ_time_points[i] = OneRepTime * i; 
  
  PARX_change_dims("ACQ_inversion_time",1);
  ACQ_inversion_time[0] = PVM_InversionTime;

  ATB_SetAcqSliceAngle( PtrType3x3 PVM_SPackArrGradOrient[0],
			PVM_NSPacks );

  ACQ_slice_orient = Arbitrary_Oblique;

  ACQ_slice_thick = PVM_SliceThick;

  slices = GTB_NumberOfSlices( PVM_NSPacks, PVM_SPackArrNSlices );

  PARX_change_dims("ACQ_slice_offset",slices);
  PARX_change_dims("ACQ_read_offset",slices);
  PARX_change_dims("ACQ_phase1_offset",slices);
  PARX_change_dims("ACQ_phase2_offset",slices);

  for(i=0;i<slices;i++)
  {
    ACQ_slice_offset[i]  = PVM_SliceOffset[i];
    ACQ_read_offset[i]   = PVM_ReadOffset[i];
    ACQ_phase1_offset[i] = PVM_Phase1Offset[i];
    ACQ_phase2_offset[i] = PVM_Phase2Offset[i];
  }


  ACQ_read_ext = (int)PVM_AntiAlias[0];

  PARX_change_dims("ACQ_slice_sepn", slices==1 ? 1 : slices-1);

  if( slices == 1 )
  {
    ACQ_slice_sepn[0] = 0.0;
  }
  else
  {
    for( i=1; i<slices;i++ )
	  {
	    ACQ_slice_sepn[i-1]=PVM_SliceOffset[i]-PVM_SliceOffset[i-1];
	  }
  }

  ATB_SetAcqSliceSepn( PVM_SPackArrSliceDistance,
                       PVM_NSPacks );

  ATB_SetAcqPatientPosition();

  ACQ_n_t1_points = 1;

  if( ParxRelsParHasValue("ACQ_transmitter_coil") == No )
  {
    ACQ_transmitter_coil[0] = '\0';
  }

  if( ParxRelsParHasValue("ACQ_contrast_agent") == No )
  {
      ACQ_contrast_agent[0] = '\0';
  }

  if( ParxRelsParHasValue("ACQ_contrast") == No )
  {
      ACQ_contrast.volume = 0.0;
      ACQ_contrast.dose = 0.0;
      ACQ_contrast.route[0] = '\0';
      ACQ_contrast.start_time[0] = '\0';
      ACQ_contrast.stop_time[0] = '\0';
  }

  ParxRelsParRelations("ACQ_contrast_agent",Yes);

  ACQ_position_X = 0.0;
  ACQ_position_Y = 0.0;
  ACQ_position_Z = 0.0;

  PARX_change_dims("ACQ_temporal_delay",1);
  ACQ_temporal_delay[0] = 0.0;

  ACQ_RF_power = 0;

  ACQ_flipback = No;

  ACQ_n_echo_images = PVM_NEchoImages;
  ACQ_n_movie_frames = 1;
 
  DB_MSG(("Exiting SetInfoParameters()"));

}

void SetMachineParameters( void )
{
  DB_MSG(("Entering SetMachineParameters()"));

  /* setting of DIGMOD,DSPFIRM,AQ_mod done in frequency par setting routine */
 
  if( ParxRelsParHasValue("ACQ_word_size") == No )
  {
    ACQ_word_size = _32_BIT;
  }

  DE = DE < 6.0 ? 6.0: DE;
 
  PAPS = QP;

  DB_MSG(("Exiting SetMachineParameters"));
}

void SetPpgParameters( void )
{
  DB_MSG(("Entering SetPpgParameters()"));

  ACQ_vd_list_size=1;
  PARX_change_dims("ACQ_vd_list",1);
  ACQ_vd_list[0] = 1e-6;
  ParxRelsParRelations("ACQ_vd_list",Yes);

  ACQ_vp_list_size=1;
  PARX_change_dims("ACQ_vp_list",1);
  ACQ_vp_list[0] = 1e-6;
  ParxRelsParRelations("ACQ_vp_list",Yes);


  ATB_SetPulprog("ytEPI.ppg");

  double riseTime = CFG_GradientRiseTime();

  D[0]  = ((PVM_RepetitionTime - PVM_MinRepetitionTime)/NSLICES 
	   + 0.1) / 1000.0;
  D[1] = (RephaseTime - riseTime) / 1000.0;
  if (PVM_SignalType == SignalType_Echo)
    D[2]  = (EchoTime/2 - MinTE_right + PVM_InterGradientWaitTime) / 1000.0;
  else
    D[2] = (EchoTime - PVM_MinEchoTime + PVM_InterGradientWaitTime) / 1000.0;
  D[4]  = riseTime / 1000.0;
  D[3]  = (riseTime + PVM_InterGradientWaitTime) / 1000.0;
  D[5] = SliceSpoiler.dur * 1e-3;
  D[6] = PackDel*1e-3;
  if(PackDel == 0.0) D[6] = 1*1e-6;
  D[7]  = (EchoTime/2 - MinTE_left + PVM_InterGradientWaitTime) / 1000.0;
  D[8] = CFG_AmplifierEnable()/1000.0;
  D[9] = (RfcSpoilerDuration - riseTime)*1e-3;

  /* set shaped pulses */

  ATB_SetRFPulse("ExcPul","ACQ_RfShapes[0]","P[0]");
  ATB_SetRFPulse("RefPul","ACQ_RfShapes[1]","P[1]");
  ATB_SetRFPulse("Exc2Pul","ACQ_RfShapes[6]","P[6]");  /* added YT */

  /* loop counters */
  L[0] = PVM_DummyScans;
  L[2] = ACQ_dim>2? ACQ_size[2]:1;
  DB_MSG(("Exiting SetPpgParameters"));
}

#if DEBUG
#define d(n) D[n]*1e3
#define p(n) P[n]*1e-3

void printTiming(void)
{
  double prep, te,left_te,part1,part2,part3,tr;
  double trigger, trigOutSlice, trigOutVol;
  int nsl;

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

  DB_MSG(("-->printTiming"));

  prep=0.010+PVM_FovSatModuleTime + PVM_MagTransModuleTime + PVM_FatSupModuleTime; /* PVM_SatPulseModuleTime */ 
  part1=d(5)+d(4)+d(8)+p(0)+d(3)+d(1)+d(4);
  part2=d(7)+d(9)+d(4)+p(1)+d(9)+d(4);
  part3=d(2)+(PVM_EpiModuleTime)+0.01+d(0);

  nsl = GTB_NumberOfSlices( PVM_NSPacks, PVM_SPackArrNSlices );

  left_te=ExcPul.Rpfac/100*p(0)+d(3)+d(1)+d(4)+d(7)+d(9)+d(4)+p(1)/2.0;
  te=ExcPul.Rpfac/100*p(0)+d(3)+d(1)+d(4)+d(2)+PVM_EpiEchoDelay;
  tr=prep+part1+part3;

  if (PVM_SignalType == SignalType_Echo) {
    tr=tr+part2; 
    te=te+part2;  
    DB_MSG(("left TE: %fms, left TE (should be): %fms ",left_te, EchoTime/2.0));
  }

  tr=nsl*(trigger+trigOutSlice+tr)+d(6)+ PVM_TaggingModuleTime + trigOutVol +0.010;

  DB_MSG(("TR: %fms, TR (should be): %fms, diff: %fms",tr, PVM_RepetitionTime, (tr-PVM_RepetitionTime))); 
  DB_MSG(("TE: %fms, TE (should be): %fms, diff: %fms",te, EchoTime, (te-EchoTime)));


  DB_MSG(("Signal Type: %d,  ExcPulseLength %f, pvmrise %f, rephase %f, spoilerdur %f, refpulse %f  l %f r %f",
          PVM_SignalType, ExcPul.Length, PVM_RiseTime, RephaseTime, RfcSpoilerDuration, RefPul.Length, MinTE_left, MinTE_right ));  
  DB_MSG(("<--printTiming"));  
}

#undef d
#undef p
#endif
