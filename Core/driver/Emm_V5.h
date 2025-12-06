#ifndef __EMM_V5_H
#define __EMM_V5_H

#include "stdbool.h"
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

typedef enum {
	S_VER   = 0,			/* 读取固件版本和对应的硬件版本 */
	S_RL    = 1,			/* 读取读取相电阻和相电感 */
	S_PID   = 2,			/* 读取PID参数 */
	S_VBUS  = 3,			/* 读取总线电压 */
	S_CPHA  = 5,			/* 读取相电流 */
	S_ENCL  = 7,			/* 读取经过线性化校准后的编码器值 */
	S_TPOS  = 8,			/* 读取电机目标位置角度 */
	S_VEL   = 9,			/* 读取电机实时转速 */
	S_CPOS  = 10,			/* 读取电机实时位置角度 */
	S_PERR  = 11,			/* 读取电机位置误差角度 */
	S_FLAG  = 13,			/* 读取使能/到位/堵转状态标志位 */
	S_Conf  = 14,			/* 读取驱动参数 */
	S_State = 15,			/* 读取系统状态参数 */
	S_ORG   = 16,     /* 读取正在回零/回零失败状态标志位 */
}SysParams_t;

/* 左电机函数 */
void Emm_V5_Reset_CurPos_To_Zero_ZUO(uint8_t addr);
void Emm_V5_Read_Sys_Params_ZUO(uint8_t addr, SysParams_t s);
void Emm_V5_Vel_Control_ZUO(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, bool snF);
void Emm_V5_Pos_Control_ZUO(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, uint32_t clk, bool raF, bool snF);
void Emm_V5_Stop_Now_ZUO(uint8_t addr, bool snF);
void Emm_V5_Origin_Set_O_ZUO(uint8_t addr, bool svF);
void Emm_V5_Origin_Modify_Params_ZUO(uint8_t addr, bool svF, uint8_t o_mode, uint8_t o_dir, uint16_t o_vel, uint32_t o_tm, uint16_t sl_vel, uint16_t sl_ma, uint16_t sl_ms, bool potF);
void Emm_V5_Origin_Trigger_Return_ZUO(uint8_t addr, uint8_t o_mode, bool snF);
void Emm_V5_Origin_Interrupt_ZUO(uint8_t addr);

/* 右电机函数 */
void Emm_V5_Reset_CurPos_To_Zero_YOU(uint8_t addr);
void Emm_V5_Read_Sys_Params_YOU(uint8_t addr, SysParams_t s);
void Emm_V5_Vel_Control_YOU(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, bool snF);
void Emm_V5_Pos_Control_YOU(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, uint32_t clk, bool raF, bool snF);
void Emm_V5_Stop_Now_YOU(uint8_t addr, bool snF);
void Emm_V5_Origin_Set_O_ZUO(uint8_t addr, bool svF);
void Emm_V5_Origin_Modify_Params_YOU(uint8_t addr, bool svF, uint8_t o_mode, uint8_t o_dir, uint16_t o_vel, uint32_t o_tm, uint16_t sl_vel, uint16_t sl_ma, uint16_t sl_ms, bool potF);
void Emm_V5_Origin_Trigger_Return_YOU(uint8_t addr, uint8_t o_mode, bool snF);
void Emm_V5_Origin_Interrupt_YOU(uint8_t addr);

#ifdef __cplusplus
}
#endif

#endif /* __EMM_V5_H */
