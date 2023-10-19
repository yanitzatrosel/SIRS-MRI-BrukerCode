/****************************************************************
 *
 * $Source: /pv/CvsTree/pv/gen/src/prg/methods/EPI/callbackDefs.h,v $
 *
 * Copyright (c) 1999-2003
 * Bruker BioSpin MRI GmbH
 * D-76275 Ettlingen, Germany
 *
 * All Rights Reserved
 *
 *
 * $Id: callbackDefs.h,v 1.19.2.1 2014/08/19 14:12:13 sako Exp $
 *
 ****************************************************************/


/* react on changes of system configuration                  */
relations PVM_SysConfigHandler backbone;

/* digitizer group */
relations PVM_DigHandler backbone;

/* image geometry */
relations PVM_ImageGeometryHandler  backbone;

/* preemphasis */
relations PVM_PreemphasisHandler backbone;

/* bandwidth */
relations PVM_EffSWh            LocalSWhRels;

/* modules */
relations PVM_FatSupHandler     backbone;
relations PVM_MagTransHandler   backbone;
relations PVM_FovSatHandler     backbone;
relations PVM_FlowSatHandler    backbone;
relations PVM_TriggerHandler    backbone;
relations PVM_EpiHandler        backbone;
relations PVM_TriggerOutHandler backbone;
relations PVM_TaggingHandler    backbone;
relations PVM_DummyScansHandler backbone;

/* other parameters */
relations PVM_NucleiHandler     backbone;
relations PVM_DeriveGains       backbone;
relations PVM_RepetitionTime    backbone;
relations PVM_EchoTime          backbone;
relations PVM_NAverages         Local_NAveragesHandler;
relations PVM_NRepetitions      backbone;
relations PVM_SignalType        backbone;

/* reconstruction */
relations RecoUserUpdate        RecoDerive;
relations VisuDerivePars        deriveVisu;

/* encoding group */
relations PVM_EncodingHandler   backbone;

/* Adjustment related redirections */
relations PVM_AdjResultHandler   HandleAdjustmentResults;
relations PVM_AdjHandler         HandleAdjustmentRequests;

/* relations for mapshim parameter group*/
relations PVM_MapShimHandler backbone;
/****************************************************************/
/*	E N D   O F   F I L E					*/
/****************************************************************/







