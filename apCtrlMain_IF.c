/*""FILE COMMENT""***********************************************************
* System Name	: ���������R��
* File Name		: apCtrlMain_IF.c
* Contents		: ��ʕ\��
* Customer		: �����i�C�������
* Model			: MC-300VC
* Note			: �Ȃ�
*****************************************************************************
* Copyright, RB Controls Co.,Ltd.
*****************************************************************************
*""FILE COMMENT END""*******************************************************/
//==========================================================================================
//	Preprocessor
//------------------------------------------------------------------------------------------
//==============================================================
//	Header File
//-------------------------------------------------------------
#include "_common.h"
#include "apCtrlDispMakeScreen.h"

#include "mdRemoconCommu.h"
#include "apYukaDan.h"
#include "apOnsuiDanbou.h"
#include "apHuroYoyaku.h"
#include "apVariationKirikae.h"
#include "apEcoKinou.h"
#include "apEneLookSettei.h"
#include "apClock.h"
#include "apDousaTbl.h"
#include "eneLook.h"
#include "apSiunten.h"
#include "apTeidenMode.h"
#include "apCtrlMain_IF.h"
//==============================================================
//	Definition
//--------------------------------------------------------------

//==============================================================
//	Type Definition
//--------------------------------------------------------------

//==============================================================
//	Variable Definition
//==============================================================

//==============================================================
//	Function Prototype
//--------------------------------------------------------------

//==========================================================================================
//	Function
//------------------------------------------------------------------------------------------
//----------------------
//	���^�]
//----------------------
static TrialRunState_e getTrialRunTermState(SiuntenState_t state_)
{
	switch (state_) {
	case Z_SiuntenState_00:			return (ZM_eTrialRunState_00);
	case Z_SiuntenState_10:			return (ZM_eTrialRunState_10);
	case Z_SiuntenState_11:			return (ZM_eTrialRunState_11);
	case Z_SiuntenState_20:			return (ZM_eTrialRunState_20);
	case Z_SiuntenState_21:			return (ZM_eTrialRunState_21);
	case Z_SiuntenState_22:			return (ZM_eTrialRunState_22);
	case Z_SiuntenState_MAX:
	case Z_SiuntenState_Bar:
	case Z_SiuntenState_Batu:
	default:
		break;
	}

	return (ZM_eTrialRunState_00);
}

static TrialRunResult_e getTrialRunTermResult(SiuntenDisp_t result_)
{
	switch (result_) {
	case Z_SiuntenDisp_Batu:		return (Z_eTrialRunResult_Nothing);
	case Z_SiuntenDisp_Wait:		return (Z_eTrialRunResult_Waiting);
	case Z_SiuntenDisp_Mizuhari:	return (Z_eTrialRunResult_PutWater);
	case Z_SiuntenDisp_Danbou:		return (Z_eTrialRunResult_Heating);
	case Z_SiuntenDisp_PASS:		return (Z_eTrialRunResult_Pass);
	case Z_SiuntenDisp_BAD:			return (Z_eTrialRunResult_Bad);
	default:
		break;
	}

	return (Z_eTrialRunResult_Nothing);
}

const TrialRunInfo_t* apTrialRunTerm_GetInfo( void )
{
	static TrialRunInfo_t 	info;

	info.term1.state = getTrialRunTermState(apSiunten_GetState1());
	info.term2.state = getTrialRunTermState(apSiunten_GetState2());
	info.term4.state = getTrialRunTermState(apSiunten_GetState4());

	info.term1.disp  = getTrialRunTermResult(apSiunten_GetDisp1());
	info.term2.disp  = getTrialRunTermResult(apSiunten_GetDisp2());
	info.term4.disp  = getTrialRunTermResult(apSiunten_GetDisp4());

	return &info;
}

//----------------------
//	�����R���D��
//----------------------
RemoconPrio_e apRemoconPrio_GetState( void )
{
	RemoconPrio_e ret;
	RemoconPrio_t prio = mdRemoconCommu_GetRemoconYuusen();

	switch(prio){
	case Z_RemoconPrio_BC:
		ret = ZM_eRemoconPrio_BC;
		break;
	case Z_RemoconPrio_MC:
		ret = ZM_eRemoconPrio_MC;
		break;
	default:
		ret = ZM_eRemoconPrio_MC;
		break;
	}

	return ret;
}

//----------------------
//	�g��2��
//----------------------
#define Z_NoDispExDispNum2keta			(0xDD)												//	�g��2����\��
u8t apExDisp_GetExDispNumber2keta( void )
{
	u8t ret;
	KyuutouCode_t ex2Keta = mdRemoconCommu_GetKakutyou2Keta();

	if ((ex2Keta.higher > Z_Character_F)
	 || (ex2Keta.lower  > Z_Character_F)) {
		return Z_NoDispExDispNum2keta;
	}

	ret = ((u8t)ex2Keta.higher) * 16 + ((u8t)ex2Keta.lower);

	return ret;
}
//----------------------
//	�g��3��
//----------------------
#define Z_NoDispExDispNum3keta			(0x0DDD)											//	�g��3����\��
u16t apExDisp_GetExDispNumber3keta( void )
{
	u16t ret;
	TokeiCode_t ex3Keta = mdRemoconCommu_GetKakutyou3Keta();

	if ((ex3Keta.higher > Z_Character_F)
	 || (ex3Keta.medium > Z_Character_F)
	 || (ex3Keta.lower  > Z_Character_F)) {
	 	return Z_NoDispExDispNum3keta;
	}

	ret = ((u16t)ex3Keta.higher) * 256 + ((u16t)ex3Keta.medium) * 16 + ((u16t)ex3Keta.lower);

	return ret;
}

//----------------------
//	�ӂ� �\���� �擾
//----------------------
const BathRsvInfo_t* apBathRsv_GetRsvInfo( void )
{
	static BathRsvInfo_t bathRsv;
	HuroYoyakuJikokuDisp_t onDisp = apHuroYoyaku_Get_YoyakuJikokuDisp();
	u8t hour = apHuroYoyaku_Get_YoyakuJikoku().hour;
	u8t minute = apHuroYoyaku_Get_YoyakuJikoku().minute;

	bathRsv.sw = apHuroYoyaku_Get_HuroYoyakuSiji();
	bathRsv.rsvTime.hour = hour;
	bathRsv.rsvTime.minute = minute;

	switch(onDisp){
	case Z_HuroYoyakuJikokuDisp_On:
		bathRsv.onDisp = Z_eTimerDisp_Tentou;
		break;
	case Z_HuroYoyakuJikokuDisp_Tenmetu:
		bathRsv.onDisp = Z_eTimerDisp_Tenmetu;
		break;
	case Z_HuroYoyakuJikokuDisp_Off:
		bathRsv.onDisp = Z_eTimerDisp_Syoutou;
		break;
	default:
		bathRsv.onDisp = Z_eTimerDisp_Syoutou;
		break;
	}

	return(&bathRsv);
}

//----------------------
//	���g�[1 ������ �擾
//----------------------
YukaDanRunType_e apYukaDan1_GetRunType( void )
{
	YukaDanRunType_e ret;
	YukaDanState_t state = apYukaDan_GetState(Z_YukaDanbou_Num1);

	switch(state){
	case Z_YukaDanState_Stop:
		ret = Z_eYukaDan_Idle;
		break;
	case Z_YukaDanState_Unten:
		ret = Z_eYukaDan_Running;
		break;
	case Z_YukaDanState_TimerWait:
		ret = Z_eYukaDan_TimerIdle;
		break;
	case Z_YukaDanState_TimerUnten:
		ret = Z_eYukaDan_TimerRunning;
		break;
	default:
		ret = Z_eYukaDan_Idle;
		break;
	}
	return ret;
}

//----------------------
//	���g�[2 ������ �擾
//----------------------
YukaDanRunType_e apYukaDan2_GetRunType( void )
{
	YukaDanRunType_e ret;
	YukaDanState_t state = apYukaDan_GetState(Z_YukaDanbou_Num2);

	switch(state){
	case Z_YukaDanState_Stop:
		ret = Z_eYukaDan_Idle;
		break;
	case Z_YukaDanState_Unten:
		ret = Z_eYukaDan_Running;
		break;
	case Z_YukaDanState_TimerWait:
		ret = Z_eYukaDan_TimerIdle;
		break;
	case Z_YukaDanState_TimerUnten:
		ret = Z_eYukaDan_TimerRunning;
		break;
	default:
		ret = Z_eYukaDan_Idle;
		break;
	}
	return ret;
}

//----------------------
//	���g�[3 ������ �擾
//----------------------
YukaDanRunType_e apYukaDan3_GetRunType( void )
{
	YukaDanRunType_e ret;
	YukaDanState_t state = apYukaDan_GetState(Z_YukaDanbou_Num3);

	switch(state){
	case Z_YukaDanState_Stop:
		ret = Z_eYukaDan_Idle;
		break;
	case Z_YukaDanState_Unten:
		ret = Z_eYukaDan_Running;
		break;
	case Z_YukaDanState_TimerWait:
		ret = Z_eYukaDan_TimerIdle;
		break;
	case Z_YukaDanState_TimerUnten:
		ret = Z_eYukaDan_TimerRunning;
		break;
	default:
		ret = Z_eYukaDan_Idle;
		break;
	}
	return ret;
}

//----------------------
//	���g�[1 �w�� �擾
//----------------------
YukaDanShiji_e apYukaDan1_GetYukaDanShiji( void )
{
	YukaDanShiji_e ret;
	YukaDanSiji_t yukaSiji = apYukaDan_GetYukaDanSiji(Z_YukaDanbou_Num1);

	switch(yukaSiji){
	case Z_YukaDanSiji_Idle:
		ret = Z_eYukaDanShiji_Idle;
		break;
	case Z_YukaDanSiji_Disable:
		ret = Z_eYukaDanShiji_Disable;
		break;
	case Z_YukaDanSiji_Enable:
		ret = Z_eYukaDanShiji_Enable;
		break;
	case Z_YukaDanSiji_Teiden:
		ret = Z_eYukaDanShiji_Idle;
		break;
	default:
		ret = Z_eYukaDanShiji_Idle;
		break;
	}
	return ret;
}

//----------------------
//	���g�[2 �w�� �擾
//----------------------
YukaDanShiji_e apYukaDan2_GetYukaDanShiji( void )
{
	YukaDanShiji_e ret;
	YukaDanSiji_t yukaSiji = apYukaDan_GetYukaDanSiji(Z_YukaDanbou_Num2);

	switch(yukaSiji){
	case Z_YukaDanSiji_Idle:
		ret = Z_eYukaDanShiji_Idle;
		break;
	case Z_YukaDanSiji_Disable:
		ret = Z_eYukaDanShiji_Disable;
		break;
	case Z_YukaDanSiji_Enable:
		ret = Z_eYukaDanShiji_Enable;
		break;
	case Z_YukaDanSiji_Teiden:
		ret = Z_eYukaDanShiji_Idle;
		break;
	default:
		ret = Z_eYukaDanShiji_Idle;
		break;
	}
	return ret;
}

//----------------------
//	���g�[3 �w�� �擾
//----------------------
YukaDanShiji_e apYukaDan3_GetYukaDanShiji( void )
{
	YukaDanShiji_e ret;
	YukaDanSiji_t yukaSiji = apYukaDan_GetYukaDanSiji(Z_YukaDanbou_Num3);

	switch(yukaSiji){
	case Z_YukaDanSiji_Idle:
		ret = Z_eYukaDanShiji_Idle;
		break;
	case Z_YukaDanSiji_Disable:
		ret = Z_eYukaDanShiji_Disable;
		break;
	case Z_YukaDanSiji_Enable:
		ret = Z_eYukaDanShiji_Enable;
		break;
	case Z_YukaDanSiji_Teiden:
		ret = Z_eYukaDanShiji_Idle;
		break;
	default:
		ret = Z_eYukaDanShiji_Idle;
		break;
	}
	return ret;
}

//----------------------
//	���g�[1 ���� �擾
//----------------------
YukaDanFloorType_e apYukaDan1_GetYukaDanFloorType( void )
{
	YukaDanFloorType_e ret;
	YukaDanFloorType_t yuka1Name = apYukaDan_GetYukaDanFloorType(Z_YukaDanbou_Num1);

	switch(yuka1Name){
	case Z_YukaDanFloorType_Misettei:					// ���ݒ�
		ret = Z_eYukaDanFloorType_Nothing;
		break;
	case Z_YukaDanFloorType_Kitchen:					// �L�b�`��
		ret = Z_eYukaDanFloorType_Kitchen;
		break;
	case Z_YukaDanFloorType_Living:						// ���r���O
		ret = Z_eYukaDanFloorType_Living;
		break;
	case Z_YukaDanFloorType_Dining:						// �_�C�j���O
		ret = Z_eYukaDanFloorType_Dining;
		break;
	case Z_YukaDanFloorType_Childroom:					// �q������
		ret = Z_eYukaDanFloorType_KidsRoom;
		break;
	case Z_YukaDanFloorType_Wasitu:						// �a��
		ret = Z_eYukaDanFloorType_JapaneseStyleRoom;
		break;
	case Z_YukaDanFloorType_Yositu:						// �m��
		ret = Z_eYukaDanFloorType_WesternStyleRoom;
		break;
	case Z_YukaDanFloorType_Sinsitu:					// �Q��
		ret = Z_eYukaDanFloorType_BedRoom;
		break;
	default:
		ret = Z_eYukaDanFloorType_Nothing;
		break;
	}

	return ret;
}

//----------------------
//	���g�[2 ���� �擾
//----------------------
YukaDanFloorType_e apYukaDan2_GetYukaDanFloorType( void )
{
	YukaDanFloorType_e ret;
	YukaDanFloorType_t yuka2Name = apYukaDan_GetYukaDanFloorType(Z_YukaDanbou_Num2);

	switch(yuka2Name){
	case Z_YukaDanFloorType_Misettei:					// ���ݒ�
		ret = Z_eYukaDanFloorType_Nothing;
		break;
	case Z_YukaDanFloorType_Kitchen:					// �L�b�`��
		ret = Z_eYukaDanFloorType_Kitchen;
		break;
	case Z_YukaDanFloorType_Living:						// ���r���O
		ret = Z_eYukaDanFloorType_Living;
		break;
	case Z_YukaDanFloorType_Dining:						// �_�C�j���O
		ret = Z_eYukaDanFloorType_Dining;
		break;
	case Z_YukaDanFloorType_Childroom:					// �q������
		ret = Z_eYukaDanFloorType_KidsRoom;
		break;
	case Z_YukaDanFloorType_Wasitu:						// �a��
		ret = Z_eYukaDanFloorType_JapaneseStyleRoom;
		break;
	case Z_YukaDanFloorType_Yositu:						// �m��
		ret = Z_eYukaDanFloorType_WesternStyleRoom;
		break;
	case Z_YukaDanFloorType_Sinsitu:					// �Q��
		ret = Z_eYukaDanFloorType_BedRoom;
		break;
	default:
		ret = Z_eYukaDanFloorType_Nothing;
		break;
	}

	return ret;
}

//----------------------
//	���g�[3 ���� �擾
//----------------------
YukaDanFloorType_e apYukaDan3_GetYukaDanFloorType( void )
{
	YukaDanFloorType_e ret;
	YukaDanFloorType_t yuka3Name = apYukaDan_GetYukaDanFloorType(Z_YukaDanbou_Num3);

	switch(yuka3Name){
	case Z_YukaDanFloorType_Misettei:					// ���ݒ�
		ret = Z_eYukaDanFloorType_Nothing;
		break;
	case Z_YukaDanFloorType_Kitchen:					// �L�b�`��
		ret = Z_eYukaDanFloorType_Kitchen;
		break;
	case Z_YukaDanFloorType_Living:						// ���r���O
		ret = Z_eYukaDanFloorType_Living;
		break;
	case Z_YukaDanFloorType_Dining:						// �_�C�j���O
		ret = Z_eYukaDanFloorType_Dining;
		break;
	case Z_YukaDanFloorType_Childroom:					// �q������
		ret = Z_eYukaDanFloorType_KidsRoom;
		break;
	case Z_YukaDanFloorType_Wasitu:						// �a��
		ret = Z_eYukaDanFloorType_JapaneseStyleRoom;
		break;
	case Z_YukaDanFloorType_Yositu:						// �m��
		ret = Z_eYukaDanFloorType_WesternStyleRoom;
		break;
	case Z_YukaDanFloorType_Sinsitu:					// �Q��
		ret = Z_eYukaDanFloorType_BedRoom;
		break;
	default:
		ret = Z_eYukaDanFloorType_Nothing;
		break;
	}

	return ret;
}

//----------------------
//	���g�[1 �^�C�}�[��� �擾
//----------------------
const YukaDanTimerInfo_t* apYukaDan1_GetTimerInfo( void )
{
	static YukaDanTimerInfo_t ret;
	const YukaDanTimerOutput_t* yukaTimer = apYukaDan_pGetTimerInfo(Z_YukaDanbou_Num1);
	YukaDanTimer_Hour_t   onHourTimer1 = yukaTimer->timer1.onTime.hour;
	YukaDanTimer_Minute_t onMinuteTimer1 = yukaTimer->timer1.onTime.minute;
	YukaDanTimer_Hour_t   offHourTimer1 = yukaTimer->timer1.offTime.hour;
	YukaDanTimer_Minute_t offMinuteTimer1 = yukaTimer->timer1.offTime.minute;
	YukaDanTimer_Hour_t   onHourTimer2 = yukaTimer->timer2.onTime.hour;
	YukaDanTimer_Minute_t onMinuteTimer2 = yukaTimer->timer2.onTime.minute;
	YukaDanTimer_Hour_t   offHourTimer2 = yukaTimer->timer2.offTime.hour;
	YukaDanTimer_Minute_t offMinuteTimer2 = yukaTimer->timer2.offTime.minute;

	ret.timer1.onTime.hour = (onHourTimer1 != Z_YukaDanTimer_Hour_Bar) ? (u8t)onHourTimer1 : (u8t)0xFF;
	ret.timer1.onTime.minute = (onMinuteTimer1 != Z_YukaDanTimer_Minute_Bar) ? (u8t)(onMinuteTimer1*10) : (u8t)0xFF;
	ret.timer1.offTime.hour = (offHourTimer1 != Z_YukaDanTimer_Hour_Bar) ? (u8t)offHourTimer1 : (u8t)0xFF;
	ret.timer1.offTime.minute = (offMinuteTimer1 != Z_YukaDanTimer_Minute_Bar) ? (u8t)(offMinuteTimer1*10) : (u8t)0xFF;
	ret.timer2.onTime.hour = (onHourTimer2 != Z_YukaDanTimer_Hour_Bar) ? (u8t)onHourTimer2 : (u8t)0xFF;
	ret.timer2.onTime.minute = (onMinuteTimer2 != Z_YukaDanTimer_Minute_Bar) ? (u8t)(onMinuteTimer2*10) : (u8t)0xFF;
	ret.timer2.offTime.hour = (offHourTimer2 != Z_YukaDanTimer_Hour_Bar) ? (u8t)offHourTimer2 : (u8t)0xFF;
	ret.timer2.offTime.minute = (offMinuteTimer2 != Z_YukaDanTimer_Minute_Bar) ? (u8t)(offMinuteTimer2*10) : (u8t)0xFF;

	ret.timer1.sw = apYukaDan_GetTimer1_OnOff(Z_YukaDanbou_Num1);
	ret.timer1.onDisp = Z_eTimerDisp_Tentou;
	ret.timer1.offDisp = Z_eTimerDisp_Tentou;
	ret.timer2.sw = apYukaDan_GetTimer2_OnOff(Z_YukaDanbou_Num1);
	ret.timer2.onDisp = Z_eTimerDisp_Tentou;
	ret.timer2.offDisp = Z_eTimerDisp_Tentou;

	return &ret;
}

//----------------------
//	���g�[2 �^�C�}�[��� �擾
//----------------------
const YukaDanTimerInfo_t* apYukaDan2_GetTimerInfo( void )
{
	static YukaDanTimerInfo_t ret;
	const YukaDanTimerOutput_t* yukaTimer = apYukaDan_pGetTimerInfo(Z_YukaDanbou_Num2);
	YukaDanTimer_Hour_t   onHourTimer1 = yukaTimer->timer1.onTime.hour;
	YukaDanTimer_Minute_t onMinuteTimer1 = yukaTimer->timer1.onTime.minute;
	YukaDanTimer_Hour_t   offHourTimer1 = yukaTimer->timer1.offTime.hour;
	YukaDanTimer_Minute_t offMinuteTimer1 = yukaTimer->timer1.offTime.minute;
	YukaDanTimer_Hour_t   onHourTimer2 = yukaTimer->timer2.onTime.hour;
	YukaDanTimer_Minute_t onMinuteTimer2 = yukaTimer->timer2.onTime.minute;
	YukaDanTimer_Hour_t   offHourTimer2 = yukaTimer->timer2.offTime.hour;
	YukaDanTimer_Minute_t offMinuteTimer2 = yukaTimer->timer2.offTime.minute;

	ret.timer1.onTime.hour = (onHourTimer1 != Z_YukaDanTimer_Hour_Bar) ? (u8t)onHourTimer1 : (u8t)0xFF;
	ret.timer1.onTime.minute = (onMinuteTimer1 != Z_YukaDanTimer_Minute_Bar) ? (u8t)(onMinuteTimer1*10) : (u8t)0xFF;
	ret.timer1.offTime.hour = (offHourTimer1 != Z_YukaDanTimer_Hour_Bar) ? (u8t)offHourTimer1 : (u8t)0xFF;
	ret.timer1.offTime.minute = (offMinuteTimer1 != Z_YukaDanTimer_Minute_Bar) ? (u8t)(offMinuteTimer1*10) : (u8t)0xFF;
	ret.timer2.onTime.hour = (onHourTimer2 != Z_YukaDanTimer_Hour_Bar) ? (u8t)onHourTimer2 : (u8t)0xFF;
	ret.timer2.onTime.minute = (onMinuteTimer2 != Z_YukaDanTimer_Minute_Bar) ? (u8t)(onMinuteTimer2*10) : (u8t)0xFF;
	ret.timer2.offTime.hour = (offHourTimer2 != Z_YukaDanTimer_Hour_Bar) ? (u8t)offHourTimer2 : (u8t)0xFF;
	ret.timer2.offTime.minute = (offMinuteTimer2 != Z_YukaDanTimer_Minute_Bar) ? (u8t)(offMinuteTimer2*10) : (u8t)0xFF;

	ret.timer1.sw = apYukaDan_GetTimer1_OnOff(Z_YukaDanbou_Num2);
	ret.timer1.onDisp = Z_eTimerDisp_Tentou;
	ret.timer1.offDisp = Z_eTimerDisp_Tentou;
	ret.timer2.sw = apYukaDan_GetTimer2_OnOff(Z_YukaDanbou_Num2);
	ret.timer2.onDisp = Z_eTimerDisp_Tentou;
	ret.timer2.offDisp = Z_eTimerDisp_Tentou;

	return &ret;
}

//----------------------
//	���g�[3 �^�C�}�[��� �擾
//----------------------
const YukaDanTimerInfo_t* apYukaDan3_GetTimerInfo( void )
{
	static YukaDanTimerInfo_t ret;
	const YukaDanTimerOutput_t* yukaTimer = apYukaDan_pGetTimerInfo(Z_YukaDanbou_Num3);
	YukaDanTimer_Hour_t   onHourTimer1 = yukaTimer->timer1.onTime.hour;
	YukaDanTimer_Minute_t onMinuteTimer1 = yukaTimer->timer1.onTime.minute;
	YukaDanTimer_Hour_t   offHourTimer1 = yukaTimer->timer1.offTime.hour;
	YukaDanTimer_Minute_t offMinuteTimer1 = yukaTimer->timer1.offTime.minute;
	YukaDanTimer_Hour_t   onHourTimer2 = yukaTimer->timer2.onTime.hour;
	YukaDanTimer_Minute_t onMinuteTimer2 = yukaTimer->timer2.onTime.minute;
	YukaDanTimer_Hour_t   offHourTimer2 = yukaTimer->timer2.offTime.hour;
	YukaDanTimer_Minute_t offMinuteTimer2 = yukaTimer->timer2.offTime.minute;

	ret.timer1.onTime.hour = (onHourTimer1 != Z_YukaDanTimer_Hour_Bar) ? (u8t)onHourTimer1 : (u8t)0xFF;
	ret.timer1.onTime.minute = (onMinuteTimer1 != Z_YukaDanTimer_Minute_Bar) ? (u8t)(onMinuteTimer1*10) : (u8t)0xFF;
	ret.timer1.offTime.hour = (offHourTimer1 != Z_YukaDanTimer_Hour_Bar) ? (u8t)offHourTimer1 : (u8t)0xFF;
	ret.timer1.offTime.minute = (offMinuteTimer1 != Z_YukaDanTimer_Minute_Bar) ? (u8t)(offMinuteTimer1*10) : (u8t)0xFF;
	ret.timer2.onTime.hour = (onHourTimer2 != Z_YukaDanTimer_Hour_Bar) ? (u8t)onHourTimer2 : (u8t)0xFF;
	ret.timer2.onTime.minute = (onMinuteTimer2 != Z_YukaDanTimer_Minute_Bar) ? (u8t)(onMinuteTimer2*10) : (u8t)0xFF;
	ret.timer2.offTime.hour = (offHourTimer2 != Z_YukaDanTimer_Hour_Bar) ? (u8t)offHourTimer2 : (u8t)0xFF;
	ret.timer2.offTime.minute = (offMinuteTimer2 != Z_YukaDanTimer_Minute_Bar) ? (u8t)(offMinuteTimer2*10) : (u8t)0xFF;

	ret.timer1.sw = apYukaDan_GetTimer1_OnOff(Z_YukaDanbou_Num3);
	ret.timer1.onDisp = Z_eTimerDisp_Tentou;
	ret.timer1.offDisp = Z_eTimerDisp_Tentou;
	ret.timer2.sw = apYukaDan_GetTimer2_OnOff(Z_YukaDanbou_Num3);
	ret.timer2.onDisp = Z_eTimerDisp_Tentou;
	ret.timer2.offDisp = Z_eTimerDisp_Tentou;

	return &ret;
}

//----------------------
//	���g�[1 ���x���x�� �擾
//----------------------
u8t apYukaDan1_GetOndoLevel( void )
{
	u8t ret;
	YukaOndoLevel_t yuka1Level = apYukaDan_GetOndoLevel(Z_YukaDanbou_Num1);

	switch(yuka1Level){
	case Z_YukaOndoLevel_1:
		ret = 1;
		break;
	case Z_YukaOndoLevel_2:
		ret = 2;
		break;
	case Z_YukaOndoLevel_3:
		ret = 3;
		break;
	case Z_YukaOndoLevel_4:
		ret = 4;
		break;
	case Z_YukaOndoLevel_5:
		ret = 5;
		break;
	case Z_YukaOndoLevel_6:
		ret = 6;
		break;
	case Z_YukaOndoLevel_7:
		ret = 7;
		break;
	case Z_YukaOndoLevel_8:
		ret = 8;
		break;
	case Z_YukaOndoLevel_9:
		ret = 9;
		break;
	default:
		ret = 1;
		break;
	}

	return ret;
}

//----------------------
//	���g�[2 ���x���x�� �擾
//----------------------
u8t apYukaDan2_GetOndoLevel( void )
{
	u8t ret;
	YukaOndoLevel_t yuka2Level = apYukaDan_GetOndoLevel(Z_YukaDanbou_Num2);

	switch(yuka2Level){
	case Z_YukaOndoLevel_1:
		ret = 1;
		break;
	case Z_YukaOndoLevel_2:
		ret = 2;
		break;
	case Z_YukaOndoLevel_3:
		ret = 3;
		break;
	case Z_YukaOndoLevel_4:
		ret = 4;
		break;
	case Z_YukaOndoLevel_5:
		ret = 5;
		break;
	case Z_YukaOndoLevel_6:
		ret = 6;
		break;
	case Z_YukaOndoLevel_7:
		ret = 7;
		break;
	case Z_YukaOndoLevel_8:
		ret = 8;
		break;
	case Z_YukaOndoLevel_9:
		ret = 9;
		break;
	default:
		ret = 1;
		break;
	}

	return ret;
}

//----------------------
//	���g�[3 ���x���x�� �擾
//----------------------
u8t apYukaDan3_GetOndoLevel( void )
{
	u8t ret;
	YukaOndoLevel_t yuka3Level = apYukaDan_GetOndoLevel(Z_YukaDanbou_Num3);

	switch(yuka3Level){
	case Z_YukaOndoLevel_1:
		ret = 1;
		break;
	case Z_YukaOndoLevel_2:
		ret = 2;
		break;
	case Z_YukaOndoLevel_3:
		ret = 3;
		break;
	case Z_YukaOndoLevel_4:
		ret = 4;
		break;
	case Z_YukaOndoLevel_5:
		ret = 5;
		break;
	case Z_YukaOndoLevel_6:
		ret = 6;
		break;
	case Z_YukaOndoLevel_7:
		ret = 7;
		break;
	case Z_YukaOndoLevel_8:
		ret = 8;
		break;
	case Z_YukaOndoLevel_9:
		ret = 9;
		break;
	default:
		ret = 1;
		break;
	}

	return ret;
}

//----------------------
//	�����g�[ ������ �擾
//----------------------
OnsuiDanbouRunType_e apOnsuiDan_GetRunType( void )
{
	OnsuiDanbouRunType_e ret;
	OnsuiDanbouState_t state = apOnsuiDanbou_GetState();

	switch(state){
	case Z_OnsuiDanbouState_Stop:
		ret = Z_eOnsuiDanbou_Idle;
		break;
	case Z_OnsuiDanbouState_Unten:
		ret = Z_eOnsuiDanbou_Running;
		break;
	case Z_OnsuiDanbouState_TimerWait:
		ret = Z_eOnsuiDanbou_TimerIdle;
		break;
	case Z_OnsuiDanbouState_TimerUnten:
		ret = Z_eOnsuiDanbou_TimerRunning;
		break;
	default:
		ret = Z_eOnsuiDanbou_Idle;
		break;
	}

	return ret;
}

//----------------------
//	�����g�[ �w�� �擾
//----------------------
OnsuiDanShiji_e apOnsuiDan_GetOnsuiDanShiji( void )
{
	OnsuiDanShiji_e ret;
	OnsuiDanbouSiji_t siji = apOnsuiDanbou_GetOnsuiDanbouSiji();

	switch(siji){
	case Z_OnsuiDanbouSiji_Idle:
		ret = Z_eOnsuiDanShiji_Idle;
		break;
	case Z_OnsuiDanbouSiji_Disable:
		ret = Z_eOnsuiDanShiji_Disable;
		break;
	case Z_OnsuiDanbouSiji_Enable:
		ret = Z_eOnsuiDanShiji_Enable;
		break;
	case Z_OnsuiDanbouSiji_Teiden:
		ret = Z_eOnsuiDanShiji_Idle;
		break;
	default:
		ret = Z_eOnsuiDanShiji_Idle;
		break;
	}

	return ret;
}

//----------------------
//	�g�[�^�C�}�[1�I������ �擾
//----------------------
const OnsuiDanbouClock_t* apOnsuiDan_GetOnsuiDanTimer1( void )
{
	static OnsuiDanbouClock_t clock;

	clock = apOnsuiDanbou_GetDanbouTimer1();

	return(&clock);
}

//----------------------
//	�g�[�^�C�}�[2�I������ �擾
//----------------------
const OnsuiDanbouClock_t* apOnsuiDan_GetOnsuiDanTimer2( void )
{
	static OnsuiDanbouClock_t clock;

	clock = apOnsuiDanbou_GetDanbouTimer2();

	return(&clock);
}

//----------------------
//	�����g�[ ���x���x�� �擾
//----------------------
u8t apOnsuiDan_GetOndoLevel( void )
{
	u8t ret;
	OnsuiDanbouOndoLevel_t level = apOnsuiDanbou_GetOnsuiDanbouOndoLevel();

	switch(level){
	case Z_OnsuiDanbouOndoLevel_1:
		ret = 1;
		break;
	case Z_OnsuiDanbouOndoLevel_2:
		ret = 2;
		break;
	case Z_OnsuiDanbouOndoLevel_3:
		ret = 3;
		break;
	case Z_OnsuiDanbouOndoLevel_4:
		ret = 4;
		break;
	case Z_OnsuiDanbouOndoLevel_5:
		ret = 5;
		break;
	case Z_OnsuiDanbouOndoLevel_6:
		ret = 6;
		break;
	case Z_OnsuiDanbouOndoLevel_7:
		ret = 7;
		break;
	default:
		ret = 1;
		break;
	}
	return ret;
}

//----------------------
//	�ȃG�l�g�[�^�C�}�[�I������ �擾
//----------------------
const OnsuiDanbouClock_t* apSaveEnergyDanbou_GetTimer( void )
{
	static OnsuiDanbouClock_t clock;

	clock = apOnsuiDanbou_GetSyouEneDanbouTimer();

	return(&clock);
}

//----------------------
//	�����g�[�^�]�ώZ���� �擾
//----------------------
u32t apOnsuiDan_GetSekisan( void )
{
	u32t ret = apOnsuiDanbou_GetUntenSekisanTime();
	return ret;
}

//----------------------
//	�����R���g�[ ������ �擾
//----------------------
OffOn_t apDanbou_GetRunType( void )
{
	OffOn_t danbouSiji = mdRemoconCommu_GetRemoconDanbou();
	return danbouSiji;
}

//----------------------
//	���g �\���� �擾
//----------------------
const YokuDanRsvInfo_t* apYokuDan_GetRsvInfo( void )
{
	static YokuDanRsvInfo_t ret;
	YokuDanJikokuDisp_t onDisp = apYokuDan_GetYoyakuJikokuDisp();

	ret.sw = apYokuDan_GetYoyakuSiji();
	ret.rsvTime.hour = apYokuDan_GetYoyakuJikoku().hour;
	ret.rsvTime.minute = apYokuDan_GetYoyakuJikoku().minute;

	switch(onDisp){
	case Z_YokuDanJikokuDisp_On:
		ret.onDisp = Z_eTimerDisp_Tentou;
		break;
	case Z_YokuDanJikokuDisp_Tenmetu:
		ret.onDisp = Z_eTimerDisp_Tenmetu;
		break;
	case Z_YokuDanJikokuDisp_Off:
		ret.onDisp = Z_eTimerDisp_Syoutou;
		break;
	default:
		ret.onDisp = Z_eTimerDisp_Syoutou;
		break;
	}

	return &ret;
}

//----------------------
//	�����h�~���[�h��� �擾
//----------------------
TouketuMode_e apTouketu_GetMode( void )
{
	TouketuMode_e ret;
	OffOn_t furoTouketu = mdRemoconCommu_GetHuroTouketuYobouUnten();
	OffOn_t danbouTouketu = mdRemoconCommu_GetDanbouTouketuYobouUnten();

	//���C�����\�h�^�]�� and �g�[�����\�h�^�]��
	if((furoTouketu == Z_ON)
	&& (danbouTouketu == Z_ON)){
		ret = Z_eTouketuMode_FuroDanbouTouketu;
	}
	//���C�����\�h�^�]��
	else if(furoTouketu == Z_ON){
		ret = Z_eTouketuMode_FuroTouketu;
	}
	//�g�[�����\�h�^�]��
	else if(danbouTouketu == Z_ON){
		ret = Z_eTouketuMode_DanbouTouketu;
	}
	else{
		ret = Z_eTouketuMode_Nothing;
	}

	return ret;
}


//----------------------
//	�G�R���[�h�^�]
//----------------------
OffOn_t apEcoMode_GetEcoModeUnten( void )
{
	OffOn_t ret = mdRemoconCommu_GetEcoModeUnten();
	return ret;
}

//----------------------
//	�K�X �G�l���M�[�p�����[�^
//----------------------
const EnergyParam_t* apConfig_GetConf_GasEnergyParam( void )
{
	static EnergyParam_t param;

	param.unitPrice	 = apEneLookSettei_GetGasTanka();
	param.Co2Keisuu	 = apEneLookSettei_GetGasCO2Keisuu();
	param.ZennenRate = apEneLookSettei_GetZennenhirituGas();
	param.useRyou	 = apEneLookSettei_GetSetteiSiyouryouGas();
	param.usePrice	 = apEneLookSettei_GetSetteiSiyouRyoukinGas();

	return &param;
}

//----------------------
//	���� �G�l���M�[�p�����[�^
//----------------------
const EnergyParam_t* apConfig_GetConf_SuidouEnergyParam( void )
{
	static EnergyParam_t param;

	param.unitPrice	 = apEneLookSettei_GetSuidouTanka();
	param.Co2Keisuu	 = apEneLookSettei_GetSuidouCO2Keisuu();
	param.ZennenRate = apEneLookSettei_GetZennenhirituSuidou();
	param.useRyou	 = apEneLookSettei_GetSetteiSiyouryouSuidou();
	param.usePrice	 = apEneLookSettei_GetSetteiSiyouRyoukinSuidou();

	return &param;
}

//----------------------
//	�d�C �G�l���M�[�p�����[�^
//----------------------
const EnergyParam_t* apConfig_GetConf_ElecEnergyParam( void )
{
	static EnergyParam_t param;

	param.unitPrice	 = apEneLookSettei_GetDenkiTanka();
	param.Co2Keisuu	 = apEneLookSettei_GetDenkiCO2Keisuu();
	param.ZennenRate = apEneLookSettei_GetZennenhirituDenki();
	param.useRyou	 = apEneLookSettei_GetSetteiSiyouryouDenki();
	param.usePrice	 = apEneLookSettei_GetSetteiSiyouRyoukinDenki();

	return &param;
}


//----------------------
//	���M�����
//----------------------
KounetuhiKind_e apConfig_GetConf_KounetuhiKind( void )
{
	KounetuhiKind_e ret;
	KounetuhiUtiwakeSettei_t setting = apEneLookSettei_GetKounetuhiUtiwakeSettei();

	switch(setting){
	case Z_KounetuhiUtiwakeGasSuidouDenki:
		ret = Z_eKounetuhiKind_GasSuidouElec;
		break;
	case Z_KounetuhiUtiwakeGasDenki:
		ret = Z_eKounetuhiKind_GasElec;
		break;
	case Z_KounetuhiUtiwakeSuidouDenki:
		ret = Z_eKounetuhiKind_SuidouElec;
		break;
	default:
		ret = Z_eKounetuhiKind_GasSuidou;
		break;
	}

	return ret;
}

//----------------------
//	�{�̎��
//----------------------
BoilerType_e apConfig_GetConf_BoilerType( void )
{
	BoilerType_e ret;
	FunctionData_t syubetu = mdRemoconCommu_GetHontaiSyubetu();

	switch(syubetu){
	case Z_FunctionData_KyuutouDanbou:
		ret = Z_eBoilerType_KyuuDan;
		break;
	case Z_FunctionData_Nasi:
	default:
		ret = Z_eBoilerType_Nasi;
		break;
	}
	return ret;
}

//----------------------
//	�g�[�@�\
//----------------------
DanbouKinou_e apConfig_GetConf_DanbouKinou( void )
{
	DanbouKinou_e ret;
	DanbouKinou_t danbouKinou = apVariKirikae_GetDanbouKinou();

	switch(danbouKinou){
	case Z_DanbouKinou_Danbou:		// �g�[
		ret = Z_eDanbouKinou_Danbou;
		break;
	case Z_DanbouKinou_OnsuiDanbou:	// �����g�[
		ret = Z_eDanbouKinou_OnsuiDanbou;
		break;
	case Z_DanbouKinou_Yukadan:		// ���g�[
		ret = Z_eDanbouKinou_YukaDanbou;
		break;
	case Z_DanbouKinou_Nasi:		// �Ȃ�
		ret = Z_eDanbouKinou_Nasi;
		break;
	default:
		ret = Z_eDanbouKinou_Nasi;
		break;
	}
	return ret;
}

//----------------------
//	���g�I��
//----------------------
SelectedYukadan_e apConfig_GetConf_SelectedYukadan( void )
{
	SelectedYukadan_e ret;
	YukadanSentaku_t yukaSelect = apVariKirikae_GetYukadanSentaku();

	switch(yukaSelect){
	case Z_YukadanSentaku_1Men:		// 1��
		ret = Z_eSelectedYukadan_OneCourt;
		break;
	case Z_YukadanSentaku_2Men:		// 2��
		ret = Z_eSelectedYukadan_TwoCourts;
		break;
	case Z_YukadanSentaku_3Men:		// 3��
		ret = Z_eSelectedYukadan_ThreeCourts;
		break;
	case Z_YukadanSentaku_Nasi:		// �Ȃ�
	default:
		ret = Z_eSelectedYukadan_Nothing;
		break;
	}

	return ret;
}

//----------------------
//	�G�lLIVE�ݒ�
//----------------------
EneLiveDispType_e apConfig_GetConf_EneLiveDispType( void )
{
	EneLiveDispType_e ret;
	EneLIVESettei_t eneLive = apEneLookSettei_GetEneLIVESettei();

	switch(eneLive){
	case Z_EneLIVE_Gas:
		ret = Z_eEneLiveDispType_Gas;
		break;
	case Z_EneLIVE_Oyu:
		ret = Z_eEneLiveDispType_Oyu;
		break;
	case Z_EneLIVE_Denki:
		ret = Z_eEneLiveDispType_Elec;
		break;
	case Z_EneLIVE_HyoujiSinai:
	default:
		ret = Z_eEneLiveDispType_Nasi;
		break;
	}

	return ret;
}

//----------------------
//	�d�C�g�p�� �擾
//----------------------
u32t apEneLive_GetElecNum( void )
{
	const u32t* pRet = EneLook_GetData( Z_eGet_EneLive_Denki );

	if(pRet == Z_NULL){
		return(0);
	}

	return *pRet;
}

//----------------------
//	�K�X�g�p�� �擾
//----------------------
u32t apEneLive_GetGasNum( void )
{
	const u32t* pRet = EneLook_GetData( Z_eGet_EneLive_Gas );

	if(pRet == Z_NULL){
		return(0);
	}

	return *pRet;
}

//----------------------
//	�����g�p�� �擾
//----------------------
u32t apEneLive_GetOyuNum( void )
{
	const u32t* pRet = EneLook_GetData( Z_eGet_EneLive_Oyu );

	if(pRet == Z_NULL){
		return(0);
	}

	return *pRet;
}

//----------------------
//	�^�]�^�C�v
//----------------------
UntenType_e apConfig_GetConf_UntenType( void )
{
	UntenType_e ret;
	UntenType_t untenType = mdRemoconCommu_GetUntenType();

	switch(untenType){
	case Z_UntenType_ZenJidou:		// �S����
		ret = Z_eUntenType_Suii;
		break;
	case Z_UntenType_NotZenjidou:	// �S�����ȊO
		ret = Z_eUntenType_Yuryou;
		break;
	default:
		ret = Z_eUntenType_Yuryou;
		break;
	}

	return ret;
}

//----------------------
//	�K�X���[�^�[�ڑ�
//----------------------
NasiAri_t apConfig_GetConf_ConnectGasMeter( void )
{
	NasiAri_t ret = apEneLookSettei_GetConnectGasMeter();
	return ret;
}

//----------------------
//	�������[�^�[�ڑ�
//----------------------
NasiAri_t apConfig_GetConf_ConnectSuidouMeter( void )
{
	NasiAri_t ret = apEneLookSettei_GetConnectSuidouMeter();
	return ret;
}

//----------------------
//	�g�[�ڑ�
//----------------------
NasiAri_t apConfig_GetConf_ConnectDanbou( void )
{
	NasiAri_t ret = apVariKirikae_GetDanbouSetuzoku();
	return ret;
}

//----------------------
//	���g�ڑ�
//----------------------
NasiAri_t apConfig_GetConf_ConnectYokuDan( void )
{
	NasiAri_t ret = apVariKirikae_GetYokudanSetuzoku();
	return ret;
}

//----------------------
//	��d���[�h
//----------------------
NasiAri_t apConfig_GetConf_TeidenMode( void )
{
	NasiAri_t ret = apTeidenMode_GetMode();
	return ret;
}

//----------------------
//	�̏�K�C�_���X�G���[
//----------------------
NasiAri_t apConfig_GetConf_DispGuidanceError( void )
{
	NasiAri_t ret = apRemoSettei_GetKosyouGuidanceError();
	return ret;
}

//----------------------
//	�d�͌v�����j�b�g
//----------------------
NasiAri_t apConfig_GetConf_ElecMesuringUnit( void )
{
	NasiAri_t ret = mdRemoconCommu_ConnectDenryokuKeisokuUnit();
	return ret;
}


//----------------------
//	�\�[���[���j�b�g
//----------------------
NasiAri_t apConfig_GetConf_SolarUnit( void )
{
	NasiAri_t ret = mdRemoconCommu_ConnectSolarUnit();
	return ret;
}

//----------------------
//	���v�\���ؑ�
//----------------------
OffOn_t apConfig_GetConf_DispTokei( void )
{
	OffOn_t ret;
	TokeiHyoujiSettei_t setting = apRemoSettei_GetTokeiHyoujiSettei();

	switch(setting){
	case Z_TokeiHyouji_Suru:
		ret = Z_ON;
		break;
	case Z_TokeiHyouji_Sinai:
		ret = Z_OFF;
		break;
	default:
		ret = Z_OFF;
		break;
	}
	return ret;
}

//----------------------
//	����
//----------------------
OffOn_t apConfig_GetConf_Jidou( void )
{
	OffOn_t ret = mdRemoconCommu_GetJidou();
	return ret;
}

//----------------------
//	���C���^�]
//----------------------
OffOn_t apConfig_GetConf_FuroSiunten( void )
{
	OffOn_t ret = mdRemoconCommu_GetFuroSiunten();
	return ret;
}

//----------------------
//	�����g�[(FB3(P2))
//----------------------
OffOn_t apConfig_GetConf_YokusituDanbou( void )
{
	OffOn_t ret = mdRemoconCommu_GetYokusituDanbou();
	return ret;
}

//----------------------
//	�����R�Ē�
//----------------------
OffOn_t apConfig_GetConf_KyuutouNensyou( void )
{
	OffOn_t ret = mdRemoconCommu_GetKyuutouNensyou();
	return ret;
}

//----------------------
//	���ݐ���
//----------------------
u32t apConfig_GetConf_GenzaiSuiryou( void )
{
	u32t ret = apEcoKinou_GetGenzaiSuiryou();
	return ret;
}

//----------------------
//	�ȃG�l������
//----------------------
OffOn_t apConfig_GetConf_SyouEneYuhari( void )
{
	OffOn_t ret = mdRemoconCommu_GetSyouEneYuhari();
	return ret;
}

//----------------------
//	�ߖ�ۉ�
//----------------------
OffOn_t apConfig_GetConf_SetuyakuHoon( void )
{
	OffOn_t ret = mdRemoconCommu_GetSetuyakuHoon();
	return ret;
}

//----------------------
//	�����������I�t
//----------------------
OffOn_t apConfig_GetConf_JidouTasiyuOff( void )
{
	OffOn_t ret = mdRemoconCommu_GetJidouTasiyuOff();
	return ret;
}

//----------------------
//	�G�R��������
//----------------------
OffOn_t apConfig_GetConf_EcoOidaki( void )
{
	OffOn_t ret = mdRemoconCommu_GetEcoOidaki();
	return ret;
}

//----------------------
//	�ȃG�l����
//----------------------
OffOn_t apConfig_GetConf_SyouEneKyuutou( void )
{
	OffOn_t ret = mdRemoconCommu_GetSyouEneKyuutou();
	return ret;
}

//----------------------
//	�������ʐݒ�
//----------------------
OnseiVolume_e apConfig_GetConf_OnseiVolume( void )
{
	OnseiVolume_e ret;
	VoiceVolumeSettei_t volume = apVoiceSettei_Get_VoiceVolumeSettei();

	switch(volume){
	case Z_VoiceVolumeSettei_VoiceKesi:
		ret = Z_eOnseiVolume_Off;
		break;
	case Z_VoiceVolumeSettei_S:
		ret = Z_eOnseiVolume_Small;
		break;
	case Z_VoiceVolumeSettei_M:
		ret = Z_eOnseiVolume_Midium;
		break;
	case Z_VoiceVolumeSettei_L:
		ret = Z_eOnseiVolume_Large;
		break;
	case Z_VoiceVolumeSettei_XL:
		ret = Z_eOnseiVolume_Large;
		break;
	default:
		ret = Z_eOnseiVolume_Off;
		break;
	}
	return ret;
}

//----------------------
//	���邳�ݒ�
//----------------------
Akarusa_e apConfig_GetConf_Akarusa( void )
{
	Akarusa_e ret;
	AkarusaSettei_t akarusa = apRemoSettei_GetAkarusaSettei();

	switch(akarusa){
	case Z_Akarusa_Minus4:
		ret = Z_eAkarusa_Minus4;
		break;
	case Z_Akarusa_Minus3:
		ret = Z_eAkarusa_Minus3;
		break;
	case Z_Akarusa_Minus2:
		ret = Z_eAkarusa_Minus2;
		break;
	case Z_Akarusa_Minus1:
		ret = Z_eAkarusa_Minus1;
		break;
	case Z_Akarusa_0:
		ret = Z_eAkarusa_Default;
		break;
	case Z_Akarusa_Plus1:
		ret = Z_eAkarusa_Plus1;
		break;
	case Z_Akarusa_Plus2:
		ret = Z_eAkarusa_Plus2;
		break;
	case Z_Akarusa_Plus3:
		ret = Z_eAkarusa_Plus3;
		break;
	case Z_Akarusa_Plus4:
		ret = Z_eAkarusa_Plus4;
		break;
	default:
		ret = Z_eAkarusa_Default;
		break;
	}

	return ret;
}

//----------------------
//	�R���g���X�g�ݒ�
//----------------------
Contrast_e apConfig_GetConf_Contrast( void )
{
	Contrast_e ret;
	ContrastSettei_t contrast = apRemoSettei_GetContrastSettei();

	switch(contrast){
	case Z_Contrast_Minus4:
		ret = Z_eContrast_Minus4;
		break;
	case Z_Contrast_Minus3:
		ret = Z_eContrast_Minus3;
		break;
	case Z_Contrast_Minus2:
		ret = Z_eContrast_Minus2;
		break;
	case Z_Contrast_Minus1:
		ret = Z_eContrast_Minus1;
		break;
	case Z_Contrast_0:
		ret = Z_eContrast_Default;
		break;
	case Z_Contrast_Plus1:
		ret = Z_eContrast_Plus1;
		break;
	case Z_Contrast_Plus2:
		ret = Z_eContrast_Plus2;
		break;
	case Z_Contrast_Plus3:
		ret = Z_eContrast_Plus3;
		break;
	case Z_Contrast_Plus4:
		ret = Z_eContrast_Plus4;
		break;
	default:
		ret = Z_eContrast_Default;
		break;
	}

	return ret;
}

//----------------------
//	�G�R�K�C�h�\���ݒ�
//----------------------
EcoGuide_e apConfig_GetConf_EcoGuide( void )
{
	EcoGuide_e ret;
	EcoGuideHyoujiSettei_t ecoGuide = apEneLookSettei_GetEcoGuideHyoujiSettei();

	switch(ecoGuide){
	case Z_EcoGuideHyouji_Gas:
		ret = Z_eEcoGuide_Gas;
		break;
	case Z_EcoGuideHyouji_Suidou:
		ret = Z_eEcoGuide_Suidou;
		break;
	case Z_EcoGuideHyouji_Denki:
		ret = Z_eEcoGuide_Elec;
		break;
	case Z_EcoGuideHyouji_Kounetuhi:
		ret = Z_eEcoGuide_Kounetuhi;
		break;
	case Z_EcoGuideHyouji_CO2:
		ret = Z_eEcoGuide_CO2;
		break;
	case Z_EcoGuideHyouji_HyoujiSinai:
	default:
		ret = Z_eEcoGuide_DontDisp;
		break;
	}
	return ret;
}

//----------------------
//	�����ē��ݒ�
//----------------------
OnseiAnnounce_e apConfig_GetConf_OnseiAnnounce( void )
{
	OnseiAnnounce_e ret = Z_eOnseiAnnounce_Default;		// RBC�l����IF���Ȃ����߉�
//	XXXX annai = apVoice_Get_VoiceAnnnai();

//	switch(annai){
//	case :
//		break;
//	default:
//		break;
//	}
	return ret;
}

//----------------------
//	�d����
//----------------------
Simuke_e apConfig_GetConf_SimukeSaki( void )
{
	Simuke_e ret;
	Simukesaki_t simuke = apKensa_Get_Simukesaki();

	switch(simuke){
	case Z_Simukesaki_R:
		ret = Z_eSimuke_Rinnai;
		break;
	case Z_Simukesaki_OG:
		ret = Z_eSimuke_OsakaGas;
		break;
	default:
		ret = Z_eSimuke_TokyoGas;
		break;
	}

	return ret;
}

//----------------------
//	�������}�b�g�Ή��ݒ�
//----------------------
OffOn_t apConfig_GetConf_HiKourituMat( void )
{
	OffOn_t ret;
	KoukourituMatTaiouSettei_t hiKourituMat = apVariKirikae_GetKoukourituMatTaiouSettei();

	switch(hiKourituMat){
	case Z_KoukourituMatTaiou_Suru:
		ret = Z_ON;
		break;
	case Z_KoukourituMatTaiou_Sinai:
		ret = Z_OFF;
		break;
	default:
		ret = Z_OFF;
		break;
	}
	return ret;
}

//----------------------
//	�����R���@�\���ʐM��(BF6)
//----------------------
NasiAri_t apConfig_GetConf_RemoconHanbetu( void )
{
	NasiAri_t ret = mdRemoconCommu_GetRemoconFunctionHanbetuSignal();
	return ret;
}

//==========================================================================================
//	Local functions
//------------------------------------------------------------------------------------------

