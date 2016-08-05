/**********************************************************************/
/*   ____  ____                                                       */
/*  /   /\/   /                                                       */
/* /___/  \  /                                                        */
/* \   \   \/                                                       */
/*  \   \        Copyright (c) 2003-2009 Xilinx, Inc.                */
/*  /   /          All Right Reserved.                                 */
/* /---/   /\                                                         */
/* \   \  /  \                                                      */
/*  \___\/\___\                                                    */
/***********************************************************************/

/* This file is designed for use with ISim build 0x8ef4fb42 */

#define XSI_HIDE_SYMBOL_SPEC true
#include "xsi.h"
#include <memory.h>
#ifdef __GNUC__
#include <stdlib.h>
#else
#include <malloc.h>
#define alloca _alloca
#endif
static const char *ng0 = "D:/Sync/ExternalProjects/CNC_QuaDdECODER/QuadraDec3x/QuadTest.vhd";



static void work_a_1533991253_2372691052_p_0(char *t0)
{
    char *t1;
    char *t2;
    char *t3;
    char *t4;
    char *t5;
    char *t6;
    int64 t7;
    int64 t8;

LAB0:    t1 = (t0 + 1848U);
    t2 = *((char **)t1);
    if (t2 == 0)
        goto LAB2;

LAB3:    goto *t2;

LAB2:    xsi_set_current_line(83, ng0);
    t2 = (t0 + 2224);
    t3 = (t2 + 32U);
    t4 = *((char **)t3);
    t5 = (t4 + 40U);
    t6 = *((char **)t5);
    *((unsigned char *)t6) = (unsigned char)2;
    xsi_driver_first_trans_fast(t2);
    xsi_set_current_line(84, ng0);
    t2 = (t0 + 1224U);
    t3 = *((char **)t2);
    t7 = *((int64 *)t3);
    t8 = (t7 / 2);
    t2 = (t0 + 1748);
    xsi_process_wait(t2, t8);

LAB6:    *((char **)t1) = &&LAB7;

LAB1:    return;
LAB4:    xsi_set_current_line(85, ng0);
    t2 = (t0 + 2224);
    t3 = (t2 + 32U);
    t4 = *((char **)t3);
    t5 = (t4 + 40U);
    t6 = *((char **)t5);
    *((unsigned char *)t6) = (unsigned char)3;
    xsi_driver_first_trans_fast(t2);
    xsi_set_current_line(86, ng0);
    t2 = (t0 + 1224U);
    t3 = *((char **)t2);
    t7 = *((int64 *)t3);
    t8 = (t7 / 2);
    t2 = (t0 + 1748);
    xsi_process_wait(t2, t8);

LAB10:    *((char **)t1) = &&LAB11;
    goto LAB1;

LAB5:    goto LAB4;

LAB7:    goto LAB5;

LAB8:    goto LAB2;

LAB9:    goto LAB8;

LAB11:    goto LAB9;

}

static void work_a_1533991253_2372691052_p_1(char *t0)
{
    char *t1;
    char *t2;
    int64 t3;
    char *t4;
    int64 t5;
    int t6;
    int t7;
    char *t8;
    char *t9;
    char *t10;
    char *t11;
    char *t12;
    int t13;

LAB0:    t1 = (t0 + 1992U);
    t2 = *((char **)t1);
    if (t2 == 0)
        goto LAB2;

LAB3:    goto *t2;

LAB2:    xsi_set_current_line(94, ng0);
    t3 = (100 * 1000LL);
    t2 = (t0 + 1892);
    xsi_process_wait(t2, t3);

LAB6:    *((char **)t1) = &&LAB7;

LAB1:    return;
LAB4:    xsi_set_current_line(96, ng0);
    t2 = (t0 + 1224U);
    t4 = *((char **)t2);
    t3 = *((int64 *)t4);
    t5 = (t3 * 10);
    t2 = (t0 + 1892);
    xsi_process_wait(t2, t5);

LAB10:    *((char **)t1) = &&LAB11;
    goto LAB1;

LAB5:    goto LAB4;

LAB7:    goto LAB5;

LAB8:    xsi_set_current_line(100, ng0);
    t2 = (t0 + 4440);
    *((int *)t2) = 0;
    t4 = (t0 + 4444);
    *((int *)t4) = 3;
    t6 = 0;
    t7 = 3;

LAB12:    if (t6 <= t7)
        goto LAB13;

LAB15:    xsi_set_current_line(111, ng0);
    t2 = (t0 + 2332);
    t4 = (t2 + 32U);
    t8 = *((char **)t4);
    t9 = (t8 + 40U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = (unsigned char)3;
    xsi_driver_first_trans_fast(t2);
    xsi_set_current_line(113, ng0);
    t2 = (t0 + 2296);
    t4 = (t2 + 32U);
    t8 = *((char **)t4);
    t9 = (t8 + 40U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = (unsigned char)3;
    xsi_driver_first_trans_fast(t2);
    xsi_set_current_line(114, ng0);
    t2 = (t0 + 1292U);
    t4 = *((char **)t2);
    t3 = *((int64 *)t4);
    t5 = (t3 / 2);
    t2 = (t0 + 1892);
    xsi_process_wait(t2, t5);

LAB35:    *((char **)t1) = &&LAB36;
    goto LAB1;

LAB9:    goto LAB8;

LAB11:    goto LAB9;

LAB13:    xsi_set_current_line(101, ng0);
    t8 = (t0 + 2260);
    t9 = (t8 + 32U);
    t10 = *((char **)t9);
    t11 = (t10 + 40U);
    t12 = *((char **)t11);
    *((unsigned char *)t12) = (unsigned char)3;
    xsi_driver_first_trans_fast(t8);
    xsi_set_current_line(102, ng0);
    t2 = (t0 + 1292U);
    t4 = *((char **)t2);
    t3 = *((int64 *)t4);
    t2 = (t0 + 1892);
    xsi_process_wait(t2, t3);

LAB18:    *((char **)t1) = &&LAB19;
    goto LAB1;

LAB14:    t2 = (t0 + 4440);
    t6 = *((int *)t2);
    t4 = (t0 + 4444);
    t7 = *((int *)t4);
    if (t6 == t7)
        goto LAB15;

LAB32:    t13 = (t6 + 1);
    t6 = t13;
    t8 = (t0 + 4440);
    *((int *)t8) = t6;
    goto LAB12;

LAB16:    xsi_set_current_line(103, ng0);
    t2 = (t0 + 2296);
    t4 = (t2 + 32U);
    t8 = *((char **)t4);
    t9 = (t8 + 40U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = (unsigned char)3;
    xsi_driver_first_trans_fast(t2);
    xsi_set_current_line(104, ng0);
    t2 = (t0 + 1292U);
    t4 = *((char **)t2);
    t3 = *((int64 *)t4);
    t2 = (t0 + 1892);
    xsi_process_wait(t2, t3);

LAB22:    *((char **)t1) = &&LAB23;
    goto LAB1;

LAB17:    goto LAB16;

LAB19:    goto LAB17;

LAB20:    xsi_set_current_line(105, ng0);
    t2 = (t0 + 2260);
    t4 = (t2 + 32U);
    t8 = *((char **)t4);
    t9 = (t8 + 40U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = (unsigned char)2;
    xsi_driver_first_trans_fast(t2);
    xsi_set_current_line(106, ng0);
    t2 = (t0 + 1292U);
    t4 = *((char **)t2);
    t3 = *((int64 *)t4);
    t2 = (t0 + 1892);
    xsi_process_wait(t2, t3);

LAB26:    *((char **)t1) = &&LAB27;
    goto LAB1;

LAB21:    goto LAB20;

LAB23:    goto LAB21;

LAB24:    xsi_set_current_line(107, ng0);
    t2 = (t0 + 2296);
    t4 = (t2 + 32U);
    t8 = *((char **)t4);
    t9 = (t8 + 40U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = (unsigned char)2;
    xsi_driver_first_trans_fast(t2);
    xsi_set_current_line(108, ng0);
    t2 = (t0 + 1292U);
    t4 = *((char **)t2);
    t3 = *((int64 *)t4);
    t2 = (t0 + 1892);
    xsi_process_wait(t2, t3);

LAB30:    *((char **)t1) = &&LAB31;
    goto LAB1;

LAB25:    goto LAB24;

LAB27:    goto LAB25;

LAB28:    goto LAB14;

LAB29:    goto LAB28;

LAB31:    goto LAB29;

LAB33:    xsi_set_current_line(115, ng0);
    t2 = (t0 + 2368);
    t4 = (t2 + 32U);
    t8 = *((char **)t4);
    t9 = (t8 + 40U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = (unsigned char)3;
    xsi_driver_first_trans_fast(t2);
    xsi_set_current_line(116, ng0);
    t2 = (t0 + 1292U);
    t4 = *((char **)t2);
    t3 = *((int64 *)t4);
    t5 = (t3 / 2);
    t2 = (t0 + 1892);
    xsi_process_wait(t2, t5);

LAB39:    *((char **)t1) = &&LAB40;
    goto LAB1;

LAB34:    goto LAB33;

LAB36:    goto LAB34;

LAB37:    xsi_set_current_line(117, ng0);
    t2 = (t0 + 2260);
    t4 = (t2 + 32U);
    t8 = *((char **)t4);
    t9 = (t8 + 40U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = (unsigned char)3;
    xsi_driver_first_trans_fast(t2);
    xsi_set_current_line(118, ng0);
    t2 = (t0 + 1292U);
    t4 = *((char **)t2);
    t3 = *((int64 *)t4);
    t2 = (t0 + 1892);
    xsi_process_wait(t2, t3);

LAB43:    *((char **)t1) = &&LAB44;
    goto LAB1;

LAB38:    goto LAB37;

LAB40:    goto LAB38;

LAB41:    xsi_set_current_line(119, ng0);
    t2 = (t0 + 2296);
    t4 = (t2 + 32U);
    t8 = *((char **)t4);
    t9 = (t8 + 40U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = (unsigned char)2;
    xsi_driver_first_trans_fast(t2);
    xsi_set_current_line(120, ng0);
    t2 = (t0 + 1292U);
    t4 = *((char **)t2);
    t3 = *((int64 *)t4);
    t2 = (t0 + 1892);
    xsi_process_wait(t2, t3);

LAB47:    *((char **)t1) = &&LAB48;
    goto LAB1;

LAB42:    goto LAB41;

LAB44:    goto LAB42;

LAB45:    xsi_set_current_line(121, ng0);
    t2 = (t0 + 2260);
    t4 = (t2 + 32U);
    t8 = *((char **)t4);
    t9 = (t8 + 40U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = (unsigned char)2;
    xsi_driver_first_trans_fast(t2);
    xsi_set_current_line(122, ng0);
    t2 = (t0 + 1292U);
    t4 = *((char **)t2);
    t3 = *((int64 *)t4);
    t5 = (t3 / 2);
    t2 = (t0 + 1892);
    xsi_process_wait(t2, t5);

LAB51:    *((char **)t1) = &&LAB52;
    goto LAB1;

LAB46:    goto LAB45;

LAB48:    goto LAB46;

LAB49:    xsi_set_current_line(123, ng0);
    t2 = (t0 + 2368);
    t4 = (t2 + 32U);
    t8 = *((char **)t4);
    t9 = (t8 + 40U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = (unsigned char)3;
    xsi_driver_first_trans_fast(t2);
    xsi_set_current_line(124, ng0);
    t2 = (t0 + 1292U);
    t4 = *((char **)t2);
    t3 = *((int64 *)t4);
    t5 = (t3 / 2);
    t2 = (t0 + 1892);
    xsi_process_wait(t2, t5);

LAB55:    *((char **)t1) = &&LAB56;
    goto LAB1;

LAB50:    goto LAB49;

LAB52:    goto LAB50;

LAB53:    xsi_set_current_line(127, ng0);
    t2 = (t0 + 4448);
    *((int *)t2) = 0;
    t4 = (t0 + 4452);
    *((int *)t4) = 2;
    t6 = 0;
    t7 = 2;

LAB57:    if (t6 <= t7)
        goto LAB58;

LAB60:    xsi_set_current_line(138, ng0);
    t2 = (t0 + 2260);
    t4 = (t2 + 32U);
    t8 = *((char **)t4);
    t9 = (t8 + 40U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = (unsigned char)3;
    xsi_driver_first_trans_fast(t2);
    xsi_set_current_line(140, ng0);

LAB80:    *((char **)t1) = &&LAB81;
    goto LAB1;

LAB54:    goto LAB53;

LAB56:    goto LAB54;

LAB58:    xsi_set_current_line(128, ng0);
    t8 = (t0 + 2296);
    t9 = (t8 + 32U);
    t10 = *((char **)t9);
    t11 = (t10 + 40U);
    t12 = *((char **)t11);
    *((unsigned char *)t12) = (unsigned char)3;
    xsi_driver_first_trans_fast(t8);
    xsi_set_current_line(129, ng0);
    t2 = (t0 + 1292U);
    t4 = *((char **)t2);
    t3 = *((int64 *)t4);
    t2 = (t0 + 1892);
    xsi_process_wait(t2, t3);

LAB63:    *((char **)t1) = &&LAB64;
    goto LAB1;

LAB59:    t2 = (t0 + 4448);
    t6 = *((int *)t2);
    t4 = (t0 + 4452);
    t7 = *((int *)t4);
    if (t6 == t7)
        goto LAB60;

LAB77:    t13 = (t6 + 1);
    t6 = t13;
    t8 = (t0 + 4448);
    *((int *)t8) = t6;
    goto LAB57;

LAB61:    xsi_set_current_line(130, ng0);
    t2 = (t0 + 2260);
    t4 = (t2 + 32U);
    t8 = *((char **)t4);
    t9 = (t8 + 40U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = (unsigned char)3;
    xsi_driver_first_trans_fast(t2);
    xsi_set_current_line(131, ng0);
    t2 = (t0 + 1292U);
    t4 = *((char **)t2);
    t3 = *((int64 *)t4);
    t2 = (t0 + 1892);
    xsi_process_wait(t2, t3);

LAB67:    *((char **)t1) = &&LAB68;
    goto LAB1;

LAB62:    goto LAB61;

LAB64:    goto LAB62;

LAB65:    xsi_set_current_line(132, ng0);
    t2 = (t0 + 2296);
    t4 = (t2 + 32U);
    t8 = *((char **)t4);
    t9 = (t8 + 40U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = (unsigned char)2;
    xsi_driver_first_trans_fast(t2);
    xsi_set_current_line(133, ng0);
    t2 = (t0 + 1292U);
    t4 = *((char **)t2);
    t3 = *((int64 *)t4);
    t2 = (t0 + 1892);
    xsi_process_wait(t2, t3);

LAB71:    *((char **)t1) = &&LAB72;
    goto LAB1;

LAB66:    goto LAB65;

LAB68:    goto LAB66;

LAB69:    xsi_set_current_line(134, ng0);
    t2 = (t0 + 2260);
    t4 = (t2 + 32U);
    t8 = *((char **)t4);
    t9 = (t8 + 40U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = (unsigned char)2;
    xsi_driver_first_trans_fast(t2);
    xsi_set_current_line(135, ng0);
    t2 = (t0 + 1292U);
    t4 = *((char **)t2);
    t3 = *((int64 *)t4);
    t2 = (t0 + 1892);
    xsi_process_wait(t2, t3);

LAB75:    *((char **)t1) = &&LAB76;
    goto LAB1;

LAB70:    goto LAB69;

LAB72:    goto LAB70;

LAB73:    goto LAB59;

LAB74:    goto LAB73;

LAB76:    goto LAB74;

LAB78:    goto LAB2;

LAB79:    goto LAB78;

LAB81:    goto LAB79;

}


extern void work_a_1533991253_2372691052_init()
{
	static char *pe[] = {(void *)work_a_1533991253_2372691052_p_0,(void *)work_a_1533991253_2372691052_p_1};
	xsi_register_didat("work_a_1533991253_2372691052", "isim/QuadTest_isim_beh.exe.sim/work/a_1533991253_2372691052.didat");
	xsi_register_executes(pe);
}
