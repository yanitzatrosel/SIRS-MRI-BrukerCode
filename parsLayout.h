/****************************************************************
 *
 * $Source: /pv/CvsTree/pv/gen/src/prg/methods/EPI/parsLayout.h,v $
 *
 * Copyright (c) 1999-2003
 * Bruker BioSpin MRI GmbH
 * D-76275 Ettlingen, Germany
 *
 * All Rights Reserved
 *
 * $Id: parsLayout.h,v 1.25 2013/05/22 11:02:08 sako Exp $
 *
 ****************************************************************/

/****************************************************************/
/*	PARAMETER CLASSES				       	*/
/****************************************************************/


/*--------------------------------------------------------------*
 * Definition of the PV class...
 *--------------------------------------------------------------*/

parclass
{
  PVM_EffSWh; 
  RephaseTime;
  PVM_MinFov;
  PVM_MinSliceThick;
  SliceSpoiler;
  DigitizerPars;
}
attributes
{
  display_name "Sequence Details";
} Sequence_Details;

parclass
{
  ExcPulseEnum;
  ExcPul;
  ExcPulseAmpl;
  ExcPulseShape;
  Exc2PulseEnum;  /* added by YT */
  Exc2Pul;      /* added by YT */
  Exc2PulseAmpl;    /* added by YT */
  Exc2PulseShape;   /* added by YT */
  RefPulseEnum;
  RefPul;
  RefPulseAmpl;
  RefPulseShape;
} 
attributes
{
  display_name "RF Pulses";
} RF_Pulses;

parclass
{
  DummyScans_Parameters;

  PVM_TriggerModule;
  Trigger_Parameters;

  PVM_TaggingOnOff;
  Tagging_Parameters;

  PVM_FatSupOnOff;
  Fat_Sup_Parameters;

  PVM_MagTransOnOff;
  Magn_Transfer_Parameters;

  PVM_FovSatOnOff;
  Fov_Sat_Parameters;

  PVM_TriggerOutOnOff;
  TriggerOut_Parameters;

} Preparation;

parclass
{
  Method;
  PVM_EffSWh;
  EchoTime;
  PVM_MinEchoTime;
  PVM_EchoTime;
  NSegments;
  PVM_RepetitionTime;
  PackDel;
  PVM_NEchoImages;
  PVM_NAverages;
  PVM_NRepetitions;
  PVM_ScanTimeStr;
  PVM_ScanTime;
  PVM_SignalType;
  PVM_DeriveGains;
  Encoding;
  EPI_Parameters;
  RF_Pulses;
  Nuclei;
  Sequence_Details;
  ImageGeometry;
  Preparation;
  MapShim;
  Preemphasis;
} MethodClass;

// parameters that should be tested after any editing
conflicts
{
  EchoTime;
  PVM_RepetitionTime;
  PVM_Fov;
  PVM_SliceThick;
};

/****************************************************************/
/*	E N D   O F   F I L E					*/
/****************************************************************/



