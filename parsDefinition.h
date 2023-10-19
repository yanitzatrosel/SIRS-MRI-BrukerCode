/****************************************************************
 *
 * $Source: /pv/CvsTree/pv/gen/src/prg/methods/EPI/parsDefinition.h,v $
 *
 * Copyright (c) 1999-2003
 * Bruker BioSpin MRI GmbH
 * D-76275 Ettlingen, Germany
 *
 * All Rights Reserved
 *
 * $Id: parsDefinition.h,v 1.19 2013/05/27 05:58:00 fhen Exp $
 *
 ****************************************************************/



/****************************************************************/
/* INCLUDE FILES						*/
/****************************************************************/




double parameter ExSliceGradient;
double parameter ExSliceRephaseGradient;
double parameter RfcSpoilerStrength;
double parameter RfcSpoilerDuration;
double parameter OneRepTime;

double parameter 
{
  display_name "Rephasing/encoding time";
  relations RephaseTimeRels;
  units "ms";
  format "%.2f";
} RephaseTime;


int parameter 
{
  display_name "Segments";
  short_description "Number of Segments.";
  relations NSegmentsRels;
} NSegments;

double parameter
{
  display_name "Echo Time";
  units "ms";
  format "%.3f";
  relations backbone;
} EchoTime;

double parameter
{
  display_name "Inter-Volume Delay";
  short_description "Break after every volume acquisition.";
  format "%.2f";
  units "ms";
  relations PackDelRelation;
} PackDel;

PVM_SPOILER_TYPE parameter 
{
  display_name "Slice Spoiler";
  relations SliceSpoilerRel;
}SliceSpoiler;

PV_PULSE_LIST parameter
{
  display_name "Excitation Pulse Shape";
  relations    ExcPulseEnumRelation;
}ExcPulseEnum;

PV_PULSE_LIST parameter
{
  display_name "Excitation Pulse Shape 2";
  relations    Exc2PulseEnumRelation;
}Exc2PulseEnum;

PV_PULSE_LIST parameter
{
  display_name "Refocusing Pulse Shape";
  relations    RefPulseEnumRelation;
}RefPulseEnum;

PVM_RF_PULSE parameter
{
  display_name "Excitation Pulse";
  relations    ExcPulRelation;
}ExcPul;

PVM_RF_PULSE parameter
{
  display_name "Excitation Pulse 2";
  relations    Exc2PulRelation;
}Exc2Pul;

PVM_RF_PULSE parameter
{
  display_name "Refocusing Pulse";
  relations    RefPulRelation;
}RefPul;

PVM_RF_PULSE_AMP_TYPE parameter
{
  display_name "Excitation Pulse Amplitude";
  relations ExcPulseAmplRel;
}ExcPulseAmpl;

PVM_RF_PULSE_AMP_TYPE parameter
{
  display_name "Excitation Pulse Amplitude 2";
  relations Exc2PulseAmplRel;
}Exc2PulseAmpl;

PVM_RF_PULSE_AMP_TYPE parameter
{
  display_name "Refocusing Pulse Amplitude";
  relations RefPulseAmplRel;
}RefPulseAmpl;

double parameter
{
  editable false;
}ExcPulseShape[];

double parameter
{
  relations backbone;
}Exc2PulseShape[];

double parameter
{
  editable false;
}RefPulseShape[];

/****************************************************************/
/*	E N D   O F   F I L E					*/
/****************************************************************/

