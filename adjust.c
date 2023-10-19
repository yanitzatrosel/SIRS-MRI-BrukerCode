/* ***************************************************************
 *
 * $Source: /pv/CvsTree/pv/gen/src/prg/methods/EPI/adjust.c,v $
 *
 * Copyright (c) 2006 - 2010
 * Bruker BioSpin MRI GmbH
 * D-76275 Ettlingen, Germany
 *
 * All Rights Reserved
 *
 *
 * $Id: adjust.c,v 1.32 2011/10/28 14:59:28 fhen Exp $
 *
 * ***************************************************************/
const char * methods_EPI_adjust = "$Id: adjust.c,v 1.32 2011/10/28 14:59:28 fhen Exp $ (C) 2006-2010 Bruker BioSpin MRI GmbH ";

#define DEBUG		0
#define DB_MODULE	0
#define DB_LINE_NR	0

#include "method.h"
#include "ovl_toolbox/AdjTools.h"



/* -------------------------------------------------------------------
  Relations of PVM_AdjHandler, called when an adjustment is starting.
  Parameter changes made her will be used for the adjustment and then 
  disarded (the current scan will not be affected).
  -------------------------------------------------------------------*/
void HandleAdjustmentRequests(void)
{
  YesNo adjPossible=No,TrajAdjRequired=Yes;
  int virtualAccel,nSubAdj;
  char  adjSequence[100];

  DB_MSG(("-->HandleAdjustmentRequests"));

  if(No == PTB_AdjMethSpec())
  {
    DB_MSG(("<--HandleAdjustmentRequests: no method specific adjustment required"));
    ParxRelsParRelations("PVM_AdjHandler",Yes);
    return;
  }
  if(No == PTB_AdjustmentStarting()) 
  {
    DB_MSG(("<--HandleAdjustmentRequests: method should not be rebuilt in adjustment platform"));
    return;
  }

  if((PVM_NSPacks==1)&&(PVM_EpiTrajAdjMeasured==No)&&(PVM_EpiTrajAdjYesNo==Yes)&&(PVM_EpiTrajAdjAutomatic==Yes)) 
    TrajAdjRequired=Yes;
  else 
    TrajAdjRequired=No;

  // Sequence of RG sub-adjustments:
  nSubAdj = 0;
  if(TrajAdjRequired==Yes)
  {
   strcpy(adjSequence,"Traj,");
   nSubAdj++;
  }
  else
    adjSequence[0] = '\0';

  if(PVM_EpiAutoGhost==No || PVM_EpiCombine==Yes)
  {
    strcat(adjSequence,"Rg");
    nSubAdj++;

  }
  else
  {
    if(PVM_SignalType == SignalType_Fid)
    {
      strcat(adjSequence,"SeGhost,Rg");
      nSubAdj+=2;
    }
    else
    {
      strcat(adjSequence,"RgGhost");
      nSubAdj++;
    }
  }
  
  DB_MSG(("RG sub-adjustment sequence: %s", adjSequence));

  switch(PTB_GetAdjCategory())
  {
    default:
      DB_MSG(("<--HandleAdjustmentRequests (Category unkown)"));
      return;
    
    case RCVR:
      if(Yes==PTB_AdjMethSpec())
      {
        DB_MSG(("setting up RG adjustment"));

        DB_MSG(("Subprocess: %s",PVM_AdjHandler.subprocess));
              
        if(PTB_CheckSubProcess(0)==Yes) 
        {
          if(nSubAdj != PTB_InitSubProcess(adjSequence)) {
            DB_MSG(("Could not initialize subprocesses")); 
          }
          DB_MSG(("<--HandleAdjustmentRequests (init subprocesses)"));
          return;
        }
        else if(PTB_CheckSubProcess("Traj")==Yes) 
        { // adjust trajectory
          DB_MSG(("RG-Subadjustment: Traj"));
          setTrajAdj();
          adjPossible=Yes;     
        }
        else if(PTB_CheckSubProcess("SeGhost")==Yes) 
        { // adapt to SE, and adjust RG and ghost
          DB_MSG(("RG-Subadjustment: SeGhost"));
          setSpinEcho();              
          setRGGhostAdj(Yes);
          adjPossible=Yes;     
        }
        else if(PTB_CheckSubProcess("RgGhost")==Yes)
        { // adjust RG and ghost
          DB_MSG(("RG-Subadjustment: RgGhost"));
          setRGGhostAdj(Yes);         
          adjPossible=Yes;        
        }       
        else if(PTB_CheckSubProcess("Rg")==Yes)
        { // adjust RG only
          DB_MSG(("RG-Subadjustment: Rg"));
          PVM_EpiAutoGhost=No;          
          setRGGhostAdj(No);
          adjPossible=Yes;        
        }       
        else 
          DB_MSG(("Unknown sub-adjustment"));
      }
      break;
 
    case USERADJ1:  
      if(Yes==PTB_AdjMethSpec() && !strcmp(PTB_GetAdjName(),"EpiTraj"))
      {
	       DB_MSG(("setting up epi trajectory adjustment"));

         setTrajAdj();
         adjPossible = Yes;
 
      }
      break;

    case USERADJ2:
      if(Yes==PTB_AdjMethSpec() &&
         !strcmp(PTB_GetAdjName(),"EpiGrappa"))
      {
        /* Set up a multi-shot, non-accelerated scan to adjust
           the GRAPPA parameters */

        DB_MSG(("setting up EpiGrappa adjustment"));

        ATB_EpiSetGrappaAdj();

        virtualAccel = PVM_EncPpi[1];
        if(PVM_EpiGrappaSegAdj==Yes)
        {
          NSegments *= PVM_EncPpi[1];
          /* Ds for at least 3 sec */
          PVM_DummyScans = (int) (3000.0/PVM_RepetitionTime);
        }
        else
          PVM_EncZf[1] *= PVM_EncPpi[1];

        PVM_EncPpi[1] = 1;
        PVM_NRepetitions = 1;
        PVM_NAverages = 1;

        adjPossible = Yes;
        backbone(); 

        /* derive reco, with acceleration */
        SetRecoParam(virtualAccel);

      }
      break;
  }
  
  if(adjPossible == No)
  {
    PARX_sprintf("Unknown adjustment required");
    /* make illegal adjustment request fail by setting GS_auto_name to empty string */
    GS_auto_name[0]='\0';
  }

  DB_MSG(("<--HandleAdjustmentRequests"));
  return;
}


/*
 *  This function is called each time an adjustment is finished. Changes made here
 *  will stay for the active scan. This routine is performed in the parameter space
 *  of the active scan and NOT in the parameter space of the adjustment.
 */

void HandleAdjustmentResults(void)
{
  DB_MSG(("-->HandleAdjustmentResults"));

  AdjCategoryEnum cat;

  cat = PTB_GetAdjResultCategory();
  DB_MSG(("category %d",(int)cat));

  switch(cat)
  {
    default:
      
      backbone();
      break;

    case RCVR:
      DB_MSG(("RCVR adjustment result"));

      DB_MSG(("Subadj: %s",PTB_GetAdjResultSubProcess()));

      if(!strcmp(PTB_GetAdjResultSubProcess(),"Traj")) 
        STB_EpiHandleTrajAdjResults(PVM_Fov[0],PVM_EffSWh,PVM_SPackArrGradOrient[0][0]); 
      /* Mirror parameters: 
       * PVM_EpiTrajAdjFov0, PVM_EpiTrajAdjMatrix0, PVM_EpiTrajAdjBw, PVM_EpiTrajAdjComp, 
       * PVM_EpiTrajAdjRampmode, PVM_EpiTrajAdjRampform, PVM_EpiTrajAdjRamptime, PVM_EpiTrajAdjReadvec
       */

      DB_MSG(("Calling backbone"));
      backbone();
      break;
    
    case USERADJ1:
      DB_MSG(("useradj1"));

      STB_EpiHandleTrajAdjResults(PVM_Fov[0],PVM_EffSWh,PVM_SPackArrGradOrient[0][0]);
 
      DB_MSG(("Calling backbone"));
      backbone();

      break;
  }
  DB_MSG(("<--HandleAdjustmentResults"));

}

void setRGGhostAdj(YesNo DoubleShot)
{
  ATB_EpiSetRgAndGhostAdj();

  /* change to to 1-shot with the same echo train length */

  if((DoubleShot==Yes)&&((PVM_EpiDoubleShotAdj==In_All)||((PVM_EpiDoubleShotAdj==In_Multishot)&&(PVM_EpiNShots>1)))) 
  {
    PVM_EncZf[1] *= (NSegments/2.0);
    NSegments = 2;
    PVM_EpiEchoTimeShifting=Yes;
  }
  else
  {
    PVM_EncZf[1] *= NSegments;
    NSegments = 1;
  }
  if(PTB_GetSpatDim() >2) /* limit dimensions to 2 */
  {
    int dimRange[2] = {2,2};
    int lowMat[3] = { 16, 16, 8 };
    int upMat[3]  = { 512, 512, 256 };
    int defaultMat[3] = {64,64,64};
    STB_InitEncoding( 2, dimRange, lowMat, upMat, defaultMat);
  }
  backbone();   
}

void setSpinEcho(void)
{
  PVM_SignalType=SignalType_Echo;  /* adapt to SE */
  if(PVM_RepetitionTime<3000.0) PVM_RepetitionTime=3000.0;
  RefPul.Flipangle=180.0;
  ExcPul.Flipangle=90.0;
}

void setTrajAdj(void)
{ 

  static const ADJ_RESULT_PAR epiAdjPars[] =
  {
                { adj_type, "PVM_EpiTrajAdjkx" },
                { adj_type, "PVM_EpiTrajAdjb0" },
                { adj_type, "PVM_EpiTrajAdjMeasured" }
  };
  PvAdjManStorePar();
  PvAdjManSetAdjParameterList(epiAdjPars, ARRAY_LEN(epiAdjPars));

  PVM_EncZf[1] *= PVM_EncPpi[1];
  PVM_EncPpi[1]=1;

  PVM_EpiAdjustMode = 3; 

  strcpy(GS_auto_name,"PVM_EpiTrajCounter"); 

  /* change geometry to 2D, single package, 2 slices: */
  { 
    int dimRange[2]={2,2}, lowMat[3]={16,16,8}, upMat[3]={512,512,256};
    int defaultMat[3] = {64,64,64};
    STB_InitEncoding( 2, dimRange, lowMat, upMat, defaultMat);
  }

  PVM_SliceThick = PVM_SpatResol[0];
  if(PVM_SliceThick<1.333333*PVM_MinSliceThick) 
  {
    /* longer excitation pulse necessary to achieve slice thickness */
    ExcPul.Length*=(PVM_MinSliceThick/PVM_SliceThick)*1.333333;
  }      

  PVM_SPackArrNSlices[0] = 2;
  PVM_SPackArrSliceGapMode[0] = non_contiguous;
  PVM_ObjOrderScheme = Sequential;

  PVM_SPackArrSliceOffset[0] = 0.0;
  STB_UpdateSliceGeoPars(0,1,0,0.0);  /* only one package */
  PVM_SPackArrSliceDistance[0] = PVM_Fov[0]*PVM_AntiAlias[0]*PVM_EpiTrajAdjDistRatio; 

  /* change to to 1-shot with the same echo train length */
  PVM_EncZf[1] *= NSegments;
  NSegments = 1;
  PVM_DummyScans=0;
  setSpinEcho(); /* adapt to SE */

  ParxRelsParRelations("PVM_SPackArrSliceDistance[0]",No); /* ->backbone(); */

  /* Orient slices orthogonal to read direction  */         

  ACQ_grad_matrix[0][2][0]= ACQ_grad_matrix[0][0][0];
  ACQ_grad_matrix[0][2][1]= ACQ_grad_matrix[0][0][1];
  ACQ_grad_matrix[0][2][2]= ACQ_grad_matrix[0][0][2];

  ACQ_grad_matrix[0][1][0]=0.0;
  ACQ_grad_matrix[0][1][1]=0.0;
  ACQ_grad_matrix[0][1][2]=0.0;

  ACQ_grad_matrix[1][2][0]= ACQ_grad_matrix[1][0][0];
  ACQ_grad_matrix[1][2][1]= ACQ_grad_matrix[1][0][1];
  ACQ_grad_matrix[1][2][2]= ACQ_grad_matrix[1][0][2];

  ACQ_grad_matrix[1][1][0]=0.0;
  ACQ_grad_matrix[1][1][1]=0.0;
  ACQ_grad_matrix[1][1][2]=0.0;
}

