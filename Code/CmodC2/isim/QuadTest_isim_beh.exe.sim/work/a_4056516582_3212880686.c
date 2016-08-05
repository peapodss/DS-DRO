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
static const char *ng0 = "D:/Sync/ExternalProjects/CNC_QuaDdECODER/QuadraDec3x/quad_dec.vhd";
extern char *IEEE_P_2592010699;
extern char *IEEE_P_3620187407;

unsigned char ieee_p_2592010699_sub_1605435078_503743352(char *, unsigned char , unsigned char );
unsigned char ieee_p_2592010699_sub_1744673427_503743352(char *, char *, unsigned int , unsigned int );
unsigned char ieee_p_2592010699_sub_2507238156_503743352(char *, unsigned char , unsigned char );
char *ieee_p_3620187407_sub_674691591_3965413181(char *, char *, char *, char *, unsigned char );
char *ieee_p_3620187407_sub_674763465_3965413181(char *, char *, char *, char *, unsigned char );


static void work_a_4056516582_3212880686_p_0(char *t0)
{
    char *t1;
    char *t2;
    int t3;
    unsigned int t4;
    unsigned int t5;
    unsigned int t6;
    unsigned char t7;
    char *t8;
    char *t9;
    int t10;
    unsigned int t11;
    unsigned int t12;
    unsigned int t13;
    unsigned char t14;
    unsigned char t15;
    char *t16;
    char *t17;
    char *t18;
    char *t19;
    char *t20;
    char *t21;

LAB0:    xsi_set_current_line(34, ng0);

LAB3:    t1 = (t0 + 1144U);
    t2 = *((char **)t1);
    t3 = (0 - 1);
    t4 = (t3 * -1);
    t5 = (1U * t4);
    t6 = (0 + t5);
    t1 = (t2 + t6);
    t7 = *((unsigned char *)t1);
    t8 = (t0 + 1236U);
    t9 = *((char **)t8);
    t10 = (1 - 1);
    t11 = (t10 * -1);
    t12 = (1U * t11);
    t13 = (0 + t12);
    t8 = (t9 + t13);
    t14 = *((unsigned char *)t8);
    t15 = ieee_p_2592010699_sub_2507238156_503743352(IEEE_P_2592010699, t7, t14);
    t16 = (t0 + 3204);
    t17 = (t16 + 32U);
    t18 = *((char **)t17);
    t19 = (t18 + 40U);
    t20 = *((char **)t19);
    *((unsigned char *)t20) = t15;
    xsi_driver_first_trans_fast(t16);

LAB2:    t21 = (t0 + 3128);
    *((int *)t21) = 1;

LAB1:    return;
LAB4:    goto LAB2;

}

static void work_a_4056516582_3212880686_p_1(char *t0)
{
    char *t1;
    char *t2;
    unsigned char t3;
    char *t4;
    int t5;
    unsigned int t6;
    unsigned int t7;
    unsigned int t8;
    unsigned char t9;
    unsigned char t10;
    char *t11;
    char *t12;
    int t13;
    unsigned int t14;
    unsigned int t15;
    unsigned int t16;
    unsigned char t17;
    unsigned char t18;
    char *t19;
    char *t20;
    char *t21;
    char *t22;
    char *t23;
    char *t24;

LAB0:    xsi_set_current_line(35, ng0);

LAB3:    t1 = (t0 + 1604U);
    t2 = *((char **)t1);
    t3 = *((unsigned char *)t2);
    t1 = (t0 + 1236U);
    t4 = *((char **)t1);
    t5 = (0 - 1);
    t6 = (t5 * -1);
    t7 = (1U * t6);
    t8 = (0 + t7);
    t1 = (t4 + t8);
    t9 = *((unsigned char *)t1);
    t10 = ieee_p_2592010699_sub_2507238156_503743352(IEEE_P_2592010699, t3, t9);
    t11 = (t0 + 1144U);
    t12 = *((char **)t11);
    t13 = (1 - 1);
    t14 = (t13 * -1);
    t15 = (1U * t14);
    t16 = (0 + t15);
    t11 = (t12 + t16);
    t17 = *((unsigned char *)t11);
    t18 = ieee_p_2592010699_sub_2507238156_503743352(IEEE_P_2592010699, t10, t17);
    t19 = (t0 + 3240);
    t20 = (t19 + 32U);
    t21 = *((char **)t20);
    t22 = (t21 + 40U);
    t23 = *((char **)t22);
    *((unsigned char *)t23) = t18;
    xsi_driver_first_trans_fast(t19);

LAB2:    t24 = (t0 + 3136);
    *((int *)t24) = 1;

LAB1:    return;
LAB4:    goto LAB2;

}

static void work_a_4056516582_3212880686_p_2(char *t0)
{
    char *t1;
    char *t2;
    char *t3;
    char *t4;
    char *t5;
    char *t6;
    char *t7;

LAB0:    xsi_set_current_line(36, ng0);

LAB3:    t1 = (t0 + 1420U);
    t2 = *((char **)t1);
    t1 = (t0 + 3276);
    t3 = (t1 + 32U);
    t4 = *((char **)t3);
    t5 = (t4 + 40U);
    t6 = *((char **)t5);
    memcpy(t6, t2, 8U);
    xsi_driver_first_trans_fast_port(t1);

LAB2:    t7 = (t0 + 3144);
    *((int *)t7) = 1;

LAB1:    return;
LAB4:    goto LAB2;

}

static void work_a_4056516582_3212880686_p_3(char *t0)
{
    char *t1;
    char *t2;
    unsigned char t3;
    char *t4;
    unsigned char t5;
    unsigned char t6;
    char *t7;
    unsigned char t8;
    unsigned char t9;
    char *t10;
    char *t11;
    char *t12;
    char *t13;
    char *t14;

LAB0:    xsi_set_current_line(37, ng0);

LAB3:    t1 = (t0 + 1328U);
    t2 = *((char **)t1);
    t3 = *((unsigned char *)t2);
    t1 = (t0 + 960U);
    t4 = *((char **)t1);
    t5 = *((unsigned char *)t4);
    t6 = ieee_p_2592010699_sub_2507238156_503743352(IEEE_P_2592010699, t3, t5);
    t1 = (t0 + 960U);
    t7 = *((char **)t1);
    t8 = *((unsigned char *)t7);
    t9 = ieee_p_2592010699_sub_1605435078_503743352(IEEE_P_2592010699, t6, t8);
    t1 = (t0 + 3312);
    t10 = (t1 + 32U);
    t11 = *((char **)t10);
    t12 = (t11 + 40U);
    t13 = *((char **)t12);
    *((unsigned char *)t13) = t9;
    xsi_driver_first_trans_fast(t1);

LAB2:    t14 = (t0 + 3152);
    *((int *)t14) = 1;

LAB1:    return;
LAB4:    goto LAB2;

}

static void work_a_4056516582_3212880686_p_4(char *t0)
{
    char t13[16];
    char *t1;
    unsigned char t2;
    char *t3;
    char *t4;
    int t5;
    unsigned int t6;
    unsigned int t7;
    unsigned int t8;
    unsigned char t9;
    char *t10;
    char *t11;
    unsigned char t12;
    char *t14;
    unsigned int t15;
    unsigned char t16;
    char *t17;
    char *t18;
    char *t19;
    char *t20;
    char *t21;
    unsigned char t22;

LAB0:    xsi_set_current_line(42, ng0);
    t1 = (t0 + 568U);
    t2 = ieee_p_2592010699_sub_1744673427_503743352(IEEE_P_2592010699, t1, 0U, 0U);
    if (t2 != 0)
        goto LAB2;

LAB4:
LAB3:    t1 = (t0 + 3160);
    *((int *)t1) = 1;

LAB1:    return;
LAB2:    xsi_set_current_line(43, ng0);
    t3 = (t0 + 1144U);
    t4 = *((char **)t3);
    t5 = (0 - 1);
    t6 = (t5 * -1);
    t7 = (1U * t6);
    t8 = (0 + t7);
    t3 = (t4 + t8);
    t9 = *((unsigned char *)t3);
    t10 = (t0 + 776U);
    t11 = *((char **)t10);
    t12 = *((unsigned char *)t11);
    t14 = ((IEEE_P_2592010699) + 2332);
    t10 = xsi_base_array_concat(t10, t13, t14, (char)99, t9, (char)99, t12, (char)101);
    t15 = (1U + 1U);
    t16 = (2U != t15);
    if (t16 == 1)
        goto LAB5;

LAB6:    t17 = (t0 + 3348);
    t18 = (t17 + 32U);
    t19 = *((char **)t18);
    t20 = (t19 + 40U);
    t21 = *((char **)t20);
    memcpy(t21, t10, 2U);
    xsi_driver_first_trans_fast(t17);
    xsi_set_current_line(44, ng0);
    t1 = (t0 + 1236U);
    t3 = *((char **)t1);
    t5 = (0 - 1);
    t6 = (t5 * -1);
    t7 = (1U * t6);
    t8 = (0 + t7);
    t1 = (t3 + t8);
    t2 = *((unsigned char *)t1);
    t4 = (t0 + 868U);
    t10 = *((char **)t4);
    t9 = *((unsigned char *)t10);
    t11 = ((IEEE_P_2592010699) + 2332);
    t4 = xsi_base_array_concat(t4, t13, t11, (char)99, t2, (char)99, t9, (char)101);
    t15 = (1U + 1U);
    t12 = (2U != t15);
    if (t12 == 1)
        goto LAB7;

LAB8:    t14 = (t0 + 3384);
    t17 = (t14 + 32U);
    t18 = *((char **)t17);
    t19 = (t18 + 40U);
    t20 = *((char **)t19);
    memcpy(t20, t4, 2U);
    xsi_driver_first_trans_fast(t14);
    xsi_set_current_line(45, ng0);
    t1 = (t0 + 960U);
    t3 = *((char **)t1);
    t2 = *((unsigned char *)t3);
    t1 = (t0 + 3420);
    t4 = (t1 + 32U);
    t10 = *((char **)t4);
    t11 = (t10 + 40U);
    t14 = *((char **)t11);
    *((unsigned char *)t14) = t2;
    xsi_driver_first_trans_fast(t1);
    xsi_set_current_line(46, ng0);
    t1 = (t0 + 1696U);
    t3 = *((char **)t1);
    t2 = *((unsigned char *)t3);
    t9 = (t2 == (unsigned char)3);
    if (t9 != 0)
        goto LAB9;

LAB11:    t1 = (t0 + 1512U);
    t3 = *((char **)t1);
    t2 = *((unsigned char *)t3);
    t9 = (t2 == (unsigned char)3);
    if (t9 != 0)
        goto LAB12;

LAB13:
LAB10:    goto LAB3;

LAB5:    xsi_size_not_matching(2U, t15, 0);
    goto LAB6;

LAB7:    xsi_size_not_matching(2U, t15, 0);
    goto LAB8;

LAB9:    xsi_set_current_line(47, ng0);
    t1 = xsi_get_transient_memory(8U);
    memset(t1, 0, 8U);
    t4 = t1;
    memset(t4, (unsigned char)2, 8U);
    t10 = (t0 + 3456);
    t11 = (t10 + 32U);
    t14 = *((char **)t11);
    t17 = (t14 + 40U);
    t18 = *((char **)t17);
    memcpy(t18, t1, 8U);
    xsi_driver_first_trans_fast(t10);
    goto LAB10;

LAB12:    xsi_set_current_line(49, ng0);
    t1 = (t0 + 1604U);
    t4 = *((char **)t1);
    t12 = *((unsigned char *)t4);
    t16 = (t12 == (unsigned char)3);
    if (t16 != 0)
        goto LAB14;

LAB16:    xsi_set_current_line(52, ng0);
    t1 = (t0 + 1420U);
    t3 = *((char **)t1);
    t1 = (t0 + 5824U);
    t4 = ieee_p_3620187407_sub_674763465_3965413181(IEEE_P_3620187407, t13, t3, t1, (unsigned char)3);
    t10 = (t13 + 12U);
    t6 = *((unsigned int *)t10);
    t7 = (1U * t6);
    t2 = (8U != t7);
    if (t2 == 1)
        goto LAB19;

LAB20:    t11 = (t0 + 3456);
    t14 = (t11 + 32U);
    t17 = *((char **)t14);
    t18 = (t17 + 40U);
    t19 = *((char **)t18);
    memcpy(t19, t4, 8U);
    xsi_driver_first_trans_fast(t11);

LAB15:    goto LAB10;

LAB14:    xsi_set_current_line(50, ng0);
    t1 = (t0 + 1420U);
    t10 = *((char **)t1);
    t1 = (t0 + 5824U);
    t11 = ieee_p_3620187407_sub_674691591_3965413181(IEEE_P_3620187407, t13, t10, t1, (unsigned char)3);
    t14 = (t13 + 12U);
    t6 = *((unsigned int *)t14);
    t7 = (1U * t6);
    t22 = (8U != t7);
    if (t22 == 1)
        goto LAB17;

LAB18:    t17 = (t0 + 3456);
    t18 = (t17 + 32U);
    t19 = *((char **)t18);
    t20 = (t19 + 40U);
    t21 = *((char **)t20);
    memcpy(t21, t11, 8U);
    xsi_driver_first_trans_fast(t17);
    goto LAB15;

LAB17:    xsi_size_not_matching(8U, t7, 0);
    goto LAB18;

LAB19:    xsi_size_not_matching(8U, t7, 0);
    goto LAB20;

}


extern void work_a_4056516582_3212880686_init()
{
	static char *pe[] = {(void *)work_a_4056516582_3212880686_p_0,(void *)work_a_4056516582_3212880686_p_1,(void *)work_a_4056516582_3212880686_p_2,(void *)work_a_4056516582_3212880686_p_3,(void *)work_a_4056516582_3212880686_p_4};
	xsi_register_didat("work_a_4056516582_3212880686", "isim/QuadTest_isim_beh.exe.sim/work/a_4056516582_3212880686.didat");
	xsi_register_executes(pe);
}
