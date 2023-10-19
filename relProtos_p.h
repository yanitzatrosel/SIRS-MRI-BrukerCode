/* /opt/PV6.0.1/prog/curdir/mri/ParaVision/methods/src/ytEPI/initMeth.c */
void initMeth(void);
/* /opt/PV6.0.1/prog/curdir/mri/ParaVision/methods/src/ytEPI/loadMeth.c */
void loadMeth(const char *);
/* /opt/PV6.0.1/prog/curdir/mri/ParaVision/methods/src/ytEPI/parsRelations.c */
void ExcPulseEnumRelation(void);
void ExcPulRelation(void);
void ExcPulRange(void);
void ExcPulseAmplRel(void);
void Exc2PulseEnumRelation(void);
void Exc2PulRelation(void);
void Exc2PulRange(void);
void Exc2PulseAmplRel(void);
void RefPulseEnumRelation(void);
void RefPulRelation(void);
void RefPulRange(void);
void RefPulseAmplRel(void);
void HandleRFPulseAmplitudes(void);
void PackDelRange(void);
void PackDelRelation(void);
void RephaseTimeRels(void);
void RephaseTimeRange(void);
void Local_NAveragesRange(void);
void Local_NAveragesHandler(void);
void LocalSWhRange(void);
void LocalSWhRels(void);
void NSegmentsRels(void);
void NSegmentsRange(void);
void SliceSpoilerRel(void);
/* /opt/PV6.0.1/prog/curdir/mri/ParaVision/methods/src/ytEPI/BaseLevelRelations.c */
void SetBaseLevelParam(void);
void SetBasicParameters(void);
void SetFrequencyParameters(void);
void SetGradientParameters(void);
void SetInfoParameters(void);
void SetMachineParameters(void);
void SetPpgParameters(void);
/* /opt/PV6.0.1/prog/curdir/mri/ParaVision/methods/src/ytEPI/RecoRelations.c */
void SetRecoParam(int);
void RecoDerive(void);
/* /opt/PV6.0.1/prog/curdir/mri/ParaVision/methods/src/ytEPI/adjust.c */
void HandleAdjustmentRequests(void);
void HandleAdjustmentResults(void);
void setRGGhostAdj(YesNo);
void setSpinEcho(void);
void setTrajAdj(void);
/* /opt/PV6.0.1/prog/curdir/mri/ParaVision/methods/src/ytEPI/backbone.c */
void backbone(void);
void rfcSpoilerUpdate(void);
void echoTimeRels(void);
void repetitionTimeRels(void);
void LocalGeometryMinimaRels(double *);
void LocalGradientStrengthRels(void);
void LocalFrequencyOffsetRels(void);
void ConstrainReadOffset(void);
/* /opt/PV6.0.1/prog/curdir/mri/ParaVision/methods/src/ytEPI/deriveVisu.c */
void deriveVisu(void);
