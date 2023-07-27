/*
 * si446x_defs.h
 *
 *  Created on: 15 ���� 2016 �.
 *      Author: NASA
 */

#ifndef INC_SI446X_DEFS_H_
#define INC_SI446X_DEFS_H_





typedef enum  {
	GPIO_DONOTHING 			= 0,
	GPIO_TRISTATE			= 1,
	GPIO_DRIVE0				= 2,
	GPIO_DRIVE1				= 3,
	GPIO_INPUT				= 4,
	GPIO_CLK_32K			= 5,
	GPIO_CLK_BOOT			= 6,
	GPIO_CLK_DIV			= 7,
	GPIO_CTS 				= 8,
	GPIO_INV_CTS			= 9,
	GPIO_CMD_OVERLAP		= 10,
	GPIO_SDO				= 11,
	GPIO_POR				= 12,
	GPIO_CAL_WUT			= 13,
	GPIO_WUT				= 14,
	GPIO_EN_PA				= 15, // private
	GPIO_TX_DATA_CLK		= 16,
	GPIO_RX_DATA_CLK		= 17,
	GPIO_EN_LNA				= 18, // private
	GPIO_TX_DATA 			= 19, // private
	GPIO_RX_DATA 			= 20,
	GPIO_RX_RAW_DATA 		= 21,
	GPIO_ANTENNA_0_SW		= 22,
	GPIO_ANTENNA_1_SW		= 23,
	GPIO_VALID_PREAMBLE 	= 24,
	GPIO_INVALID_PREAMBLE	= 25,
	GPIO_SYNC_WORD_DETECT 	= 26,
	GPIO_CCA				= 27,
	GPIO_IN_SLEEP			= 28, // private
	GPIO_TX_STATE			= 32,
	GPIO_RX_STATE			= 33,
	GPIO_RX_FIFO_FULL		= 34,
	GPIO_TX_FIFO_EMPTY		= 35,
	GPIO_LOW_BATT			= 36,
	GPIO_CCA_LATCH			= 37,
	GPIO_HOPPED				= 38,
	GPIO_HOP_TABLE_WRAP		= 39
} SI4463_GPIO_FUNC;






typedef enum
{
    /*BOOT COMMANDS-------------------------------------*/
    POWER_UP                = 0x02, // Power up
    /*COMMON COMMANDS-----------------------------------*/
    NOP                     = 0x00, //No operation command.
    PART_INFO               = 0x01, //Reports basic information about the device.
    FUNC_INFO               = 0x10, //Returns the Function revision information of the device.
    GET_INT_STATUS          = 0x20, //Returns the interrupt status of ALL the possible interrupt events
                                    //(both STATUS and PENDING). Optionally, it may be used to
                                    //clear latched (PENDING) interrupt events.
    SET_PROPERTY            = 0x11, //Sets the value of a property.
    GET_PROPERTY            = 0x12, //Retrieve a property's value.
    FIFO_INFO               = 0x15, //Provides access to transmit and receive FIFO counts and reset.
    GPIO_PIN_CFG            = 0x13, //Configures the gpio pins.
    CHANGE_STATE            = 0x34, //Update state machine entries.
    REQUEST_DEVICE_STATE    = 0x33, //Request current device state.
    READ_CMD_BUFF           = 0x44, //Used to read CTS and the command response.
    FRR_A_READ              = 0x50, //Reads the fast response registers starting with A.
    FRR_B_READ              = 0x51, //Reads the fast response registers starting with B.
    FRR_C_READ              = 0x53, //Reads the fast response registers starting with C.
    FRR_D_READ              = 0x57, //Reads the fast response registers starting with D.
    /*IR_CAL_COMMANDS----------------------------------*/
    IRCAL                   = 0x17, //Image rejection calibration.
    /*TX_COMMANDS--------------------------------------*/
    START_TX                = 0x31, //Switches to TX state and starts packet transmission.
    WRITE_TX_FIFO           = 0x66, //Writes the TX FIFO.
    /*RX_COMMANDS--------------------------------------*/
    GET_MODEM_STATUS        = 0x22, //Returns the interrupt status of the Modem Interrupt Group
                                    //(both STATUS and PENDING). Optionally, it may be used
                                    //to clear latched (PENDING) interrupt events.
    PACKET_INFO             = 0x16, //Returns information about the last packet received and
                                    //optionally overrides field length.
    START_RX                = 0x32, //Switches to RX state. Command arguments are retained though
                                    //sleep state, so these only need to be written when they change.
    RX_HOP                  = 0x36, //Hop to a new frequency while in RX.
    READ_RX_FIFO            = 0x77, //READ_RX_FIFO
    /*ADVANCED_COMMANDS-------------------------------*/
    GET_PH_STATUS           = 0x21, //Returns the interrupt status of the Packet Handler Interrupt
                                    //Group (both STATUS and PENDING). Optionally, it may be used
                                    //to clear latched (PENDING) interrupt events.
    GET_CHIP_STATUS         = 0x23, //Returns the interrupt status of the Chip Interrupt Group
                                    //(both STATUS and PENDING). Optionally, it may be used to
                                    //clear latched (PENDING) interrupt events.
    PROTOCOL_CFG            = 0x18, //Sets the chip up for specified protocol.
    GET_ADC_READING         = 0x14  //Performs and retrieve the results of possible ADC conversions.
}SI446X_CMD_X;

//  priority for SI446x
typedef enum
{
    GLOBAL_XO_TUNE          = 0x0000,
    GLOBAL_CLK_CFG          = 0x0001,
    GLOBAL_LOW_BATT_THRESH  = 0x0001,
    GLOBAL_CONFIG           = 0x0003,
    GLOBAL_WUT_CONFIG       = 0x0004,
    GLOBAL_WUT_M_15_8       = 0x0005,
    GLOBAL_WUT_M_7_0        = 0x0006,
    GLOBAL_WUT_R            = 0x0007,
    GLOBAL_WUT_LDC          = 0x0008,
    GLOBAL_WUT_CAL          = 0x0009,
    INT_CTL_ENABLE          = 0x0100,
    INT_CTL_PH_ENABLE       = 0x0101,
    INT_CTL_MODEM_ENABLE    = 0x0102,
    INT_CTL_CHIP_ENABLE     = 0x0103,
    FRR_CTL_A_MODE          = 0x0200,
    FRR_CTL_B_MODE          = 0x0201,
    FRR_CTL_C_MODE          = 0x0202,
    FRR_CTL_D_MODE          = 0x0203,
    PREAMBLE_TX_LENGTH      = 0x1000,
    PREAMBLE_CONFIG_STD_1   = 0x1001,
    PREAMBLE_CONFIG_NSTD    = 0x1002,
    PREAMBLE_CONFIG_STD_2   = 0x1003,
    PREAMBLE_CONFIG         = 0x1004,
    PREAMBLE_PATTERN_31_24  = 0x1005,
    PREAMBLE_PATTERN_23_16  = 0x1006,
    PREAMBLE_PATTERN_15_8   = 0x1007,
    PREAMBLE_PATTERN_7_0    = 0x1008,
    PREAMBLE_POSTAMBLE_CONFIG           = 0x1009,
    PREAMBLE_POSTAMBLE_PATTERN_31_24    = 0x100A,
    PREAMBLE_POSTAMBLE_PATTERN_23_16 = 0x100B,
    PREAMBLE_POSTAMBLE_PATTERN_15_8     = 0x100C,
    PREAMBLE_POSTAMBLE_PATTERN_7_0      = 0x100D,
    SYNC_CONFIG             = 0x1100,
    SYNC_BITS_31_24         = 0x1101,
    SYNC_BITS_23_16         = 0x1102,
    SYNC_BITS_15_8          = 0x1103,
    SYNC_BITS_7_0           = 0x1104,
    PKT_CRC_CONFIG          = 0x1200,
    PKT_CONFIG1             = 0x1206,
    PKT_LEN                 = 0x1208,
    PKT_LEN_FIELD_SOURCE    = 0x1209,
    PKT_LEN_ADJUST          = 0x120A,
    PKT_TX_THRESHOLD        = 0x120B,
    PKT_RX_THRESHOLD        = 0x120C,
    PKT_FIELD_1_LENGTH_12_8 = 0x120D,
    PKT_FIELD_1_LENGTH_7_0  = 0x120E,
    PKT_FIELD_1_CONFIG      = 0x120F,
    PKT_FIELD_1_CRC_CONFIG  = 0x1210,
    PKT_FIELD_2_LENGTH_12_8 = 0x1211,
    PKT_FIELD_2_LENGTH_7_0  = 0x1212,
    PKT_FIELD_2_CONFIG      = 0x1213,
    PKT_FIELD_2_CRC_CONFIG  = 0x1214,
    PKT_FIELD_3_LENGTH_12_8 = 0x1215,
    PKT_FIELD_3_LENGTH_7_0  = 0x1216,
    PKT_FIELD_3_CONFIG      = 0x1217,
    PKT_FIELD_3_CRC_CONFIG  = 0x1218,
    PKT_FIELD_4_LENGTH_12_8 = 0x1219,
    PKT_FIELD_4_LENGTH_7_0  = 0x121A,
    PKT_FIELD_4_CONFIG      = 0x121B,
    PKT_FIELD_4_CRC_CONFIG  = 0x121C,
    PKT_FIELD_5_LENGTH_12_8 = 0x121D,
    PKT_FIELD_5_LENGTH_7_0  = 0x121E,
    PKT_FIELD_5_CONFIG      = 0x121F,
    PKT_FIELD_5_CRC_CONFIG  = 0x1220,
    PKT_RX_FIELD_1_LENGTH_12_8  = 0x1221,
    PKT_RX_FIELD_1_LENGTH_7_0   = 0x1222,
    PKT_RX_FIELD_1_CONFIG       = 0x1223,
    PKT_RX_FIELD_1_CRC_CONFIG   = 0x1224,
    PKT_RX_FIELD_2_LENGTH_12_8  = 0x1225,
    PKT_RX_FIELD_2_LENGTH_7_0   = 0x1226,
    PKT_RX_FIELD_2_CONFIG   = 0x1227,
    PKT_RX_FIELD_2_CRC_CONFIG   = 0x1228,
    PKT_RX_FIELD_3_LENGTH_12_8 = 0x1229,
    PKT_RX_FIELD_3_LENGTH_7_0   = 0x122A,
    PKT_RX_FIELD_3_CONFIG       = 0x122B,
    PKT_RX_FIELD_3_CRC_CONFIG   = 0x122C,
    PKT_RX_FIELD_4_LENGTH_12_8 = 0x122D,
    PKT_RX_FIELD_4_LENGTH_7_0   = 0x122E,
    PKT_RX_FIELD_4_CONFIG   = 0x122F,
    PKT_RX_FIELD_4_CRC_CONFIG   = 0x1230,
    PKT_RX_FIELD_5_LENGTH_12_8 = 0x1231,
    PKT_RX_FIELD_5_LENGTH_7_0   = 0x1232,
    PKT_RX_FIELD_5_CONFIG       = 0x1233,
    PKT_RX_FIELD_5_CRC_CONFIG   = 0x1234,
    MODEM_MOD_TYPE          = 0x2000,
    MODEM_MAP_CONTROL       = 0x2001,
	MODEM_DSM_CTRL						= 0x2002, // secret-register	//plaz_add
    MODEM_DATA_RATE_2       = 0x2003,
    MODEM_DATA_RATE_1       = 0x2004,
    MODEM_DATA_RATE_0       = 0x2005,
    MODEM_TX_NCO_MODE_3     = 0x2006,
    MODEM_TX_NCO_MODE_2     = 0x2007,
    MODEM_TX_NCO_MODE_1     = 0x2008,
    MODEM_TX_NCO_MODE_0     = 0x2009,
    MODEM_FREQ_DEV_2        = 0x200A,
    MODEM_FREQ_DEV_1        = 0x200B,
    MODEM_FREQ_DEV_0        = 0x200C,
    MODEM_FREQ_OFFSET_1     = 0x200D,
    MODEM_FREQ_OFFSET_0     = 0x200E,
    MODEM_TX_FILTER_COEFF_8 = 0x200F,
    MODEM_TX_FILTER_COEFF_7 = 0x2010,
    MODEM_TX_FILTER_COEFF_6 = 0x2011,
    MODEM_TX_FILTER_COEFF_5 = 0x2012,
    MODEM_TX_FILTER_COEFF_4 = 0x2013,
    MODEM_TX_FILTER_COEFF_3 = 0x2014,
    MODEM_TX_FILTER_COEFF_2 = 0x2015,
    MODEM_TX_FILTER_COEFF_1 = 0x2016,
    MODEM_TX_FILTER_COEFF_0 = 0x2017,
    MODEM_TX_RAMP_DELAY     = 0x2018,
    MODEM_MDM_CTRL          = 0x2019,
    MODEM_IF_CONTROL        = 0x201A,
    MODEM_IF_FREQ_2         = 0x201B,
    MODEM_IF_FREQ_1         = 0x201C,
    MODEM_IF_FREQ_0         = 0x201D,
    MODEM_DECIMATION_CFG1   = 0x201E,
    MODEM_DECIMATION_CFG0   = 0x201F,
    MODEM_BCR_OSR_1         = 0x2022,
    MODEM_BCR_OSR_0         = 0x2023,
    MODEM_BCR_NCO_OFFSET_2  = 0x2024,
    MODEM_BCR_NCO_OFFSET_1  = 0x2025,
    MODEM_BCR_NCO_OFFSET_0  = 0x2026,
    MODEM_BCR_GAIN_1        = 0x2027,
    MODEM_BCR_GAIN_0        = 0x2028,
    MODEM_BCR_GEAR          = 0x2029,
    MODEM_BCR_MISC1         = 0x202A,
    MODEM_BCR_MISC0         = 0x202B,
    MODEM_AFC_GEAR          = 0x202C,
    MODEM_AFC_WAIT          = 0x202D,
    MODEM_AFC_GAIN_1        = 0x202E,
    MODEM_AFC_GAIN_0        = 0x202F,
    MODEM_AFC_LIMITER_1     = 0x2030,
    MODEM_AFC_LIMITER_0     = 0x2031,
    MODEM_AFC_MISC          = 0x2032,
    MODEM_AFC_ZIFOFF        = 0x2033,
    MODEM_ADC_CTRL          = 0x2034,
	MODEM_AGC_CONTROL					= 0x2035, 	//plaz_add
    MODEM_AGC_WINDOW_SIZE   = 0x2038,
    MODEM_AGC_RFPD_DECAY    = 0x2039,
    MODEM_AGC_IFPD_DECAY    = 0x203A,
    MODEM_FSK4_GAIN1        = 0x203B,
    MODEM_FSK4_GAIN0        = 0x203C,
    MODEM_FSK4_TH1          = 0x203D,
    MODEM_FSK4_TH0          = 0x203E,
    MODEM_FSK4_MAP          = 0x203F,
    MODEM_OOK_PDTC          = 0x2040,
    MODEM_OOK_BLOPK         = 0x2041,
    MODEM_OOK_CNT1          = 0x2042,
    MODEM_OOK_MISC          = 0x2043,
    MODEM_RAW_SEARCH        = 0x2044,
    MODEM_RAW_CONTROL       = 0x2045,
    MODEM_RAW_EYE_1         = 0x2046,
    MODEM_RAW_EYE_0         = 0x2047,
    MODEM_ANT_DIV_MODE      = 0x2048,
    MODEM_ANT_DIV_CONTROL   = 0x2049,
    MODEM_RSSI_THRESH       = 0x204A,
    MODEM_RSSI_JUMP_THRESH  = 0x204B,
    MODEM_RSSI_CONTROL      = 0x204C,
    MODEM_RSSI_CONTROL2     = 0x204D,
    MODEM_RSSI_COMP         = 0x204E,
    MODEM_CLKGEN_BAND       = 0x2051,
    MODEM_CHFLT_RX1_CHFLT_COE13_7_0 = 0x2100,
    MODEM_CHFLT_RX1_CHFLT_COE12_7_0 = 0x2101,
    MODEM_CHFLT_RX1_CHFLT_COE11_7_0 = 0x2102,
    MODEM_CHFLT_RX1_CHFLT_COE10_7_0 = 0x2103,
    MODEM_CHFLT_RX1_CHFLT_COE9_7_0  = 0x2104,
    MODEM_CHFLT_RX1_CHFLT_COE8_7_0  = 0x2105,
    MODEM_CHFLT_RX1_CHFLT_COE7_7_0  = 0x2106,
    MODEM_CHFLT_RX1_CHFLT_COE6_7_0  = 0x2107,
    MODEM_CHFLT_RX1_CHFLT_COE5_7_0  = 0x2108,
    MODEM_CHFLT_RX1_CHFLT_COE4_7_0  = 0x2109,
    MODEM_CHFLT_RX1_CHFLT_COE3_7_0 = 0x210A,
    MODEM_CHFLT_RX1_CHFLT_COE2_7_0  = 0x210B,
    MODEM_CHFLT_RX1_CHFLT_COE1_7_0  = 0x210C,
    MODEM_CHFLT_RX1_CHFLT_COE0_7_0 = 0x210D,
    MODEM_CHFLT_RX1_CHFLT_COEM0     = 0x210E,
    MODEM_CHFLT_RX1_CHFLT_COEM1     = 0x210F,
    MODEM_CHFLT_RX1_CHFLT_COEM2     = 0x2110,
    MODEM_CHFLT_RX1_CHFLT_COEM3     = 0x2111,
    MODEM_CHFLT_RX2_CHFLT_COE13_7_0 = 0x2112,
    MODEM_CHFLT_RX2_CHFLT_COE12_7_0 = 0x2113,
    MODEM_CHFLT_RX2_CHFLT_COE11_7_0 = 0x2114,
    MODEM_CHFLT_RX2_CHFLT_COE10_7_0 = 0x2115,
    MODEM_CHFLT_RX2_CHFLT_COE9_7_0  = 0x2116,
    MODEM_CHFLT_RX2_CHFLT_COE8_7_0  = 0x2117,
    MODEM_CHFLT_RX2_CHFLT_COE7_7_0  = 0x2118,
    MODEM_CHFLT_RX2_CHFLT_COE6_7_0  = 0x2119,
    MODEM_CHFLT_RX2_CHFLT_COE5_7_0  = 0x211A,
    MODEM_CHFLT_RX2_CHFLT_COE4_7_0  = 0x211B,
    MODEM_CHFLT_RX2_CHFLT_COE3_7_0  = 0x211C,
    MODEM_CHFLT_RX2_CHFLT_COE2_7_0  = 0x211D,
    MODEM_CHFLT_RX2_CHFLT_COE1_7_0 = 0x211E,
    MODEM_CHFLT_RX2_CHFLT_COE0_7_0  = 0x211F,
    MODEM_CHFLT_RX2_CHFLT_COEM0     = 0x2120,
    MODEM_CHFLT_RX2_CHFLT_COEM1     = 0x2121,
    MODEM_CHFLT_RX2_CHFLT_COEM2     = 0x2122,
    MODEM_CHFLT_RX2_CHFLT_COEM3     = 0x2123,
    PA_MODE                 = 0x2200,
    PA_PWR_LVL              = 0x2201,
    PA_BIAS_CLKDUTY         = 0x2202,
    PA_TC                   = 0x2203,
    PA_RAMP_EX              = 0x2204,
    PA_RAMP_DOWN_DELAY      = 0x2205,
    SYNTH_PFDCP_CPFF        = 0x2300,
    SYNTH_PFDCP_CPINT       = 0x2301,
    SYNTH_VCO_KV            = 0x2302,
    SYNTH_LPFILT3           = 0x2303,
    SYNTH_LPFILT2           = 0x2304,
    SYNTH_LPFILT1           = 0x2305,
    SYNTH_LPFILT0           = 0x2306,
    SYNTH_VCO_KVCAL         = 0x2307,
    MATCH_VALUE_1           = 0x3000,
    MATCH_MASK_1            = 0x3001,
    MATCH_CTRL_1            = 0x3002,
    MATCH_VALUE_2           = 0x3003,
    MATCH_MASK_2            = 0x3004,
    MATCH_CTRL_2            = 0x3005,
    MATCH_VALUE_3           = 0x3006,
    MATCH_MASK_3            = 0x3007,
    MATCH_CTRL_3            = 0x3008,
    MATCH_VALUE_4           = 0x3009,
    MATCH_MASK_4            = 0x300A,
    MATCH_CTRL_4            = 0x300B,
    FREQ_CONTROL_INTE       = 0x4000,
    FREQ_CONTROL_FRAC_2     = 0x4001,
    FREQ_CONTROL_FRAC_1     = 0x4002,
    FREQ_CONTROL_FRAC_0     = 0x4003,
    FREQ_CONTROL_CHANNEL_STEP_SIZE_1    = 0x4004,
    FREQ_CONTROL_CHANNEL_STEP_SIZE_0 = 0x4005,
    FREQ_CONTROL_W_SIZE     = 0x4006,
    FREQ_CONTROL_VCOCNT_RX_ADJ          = 0x4007,
    RX_HOP_CONTROL          = 0x5000,
    RX_HOP_TABLE_SIZE       = 0x5001,
    RX_HOP_TABLE_ENTRY_0    = 0x5002,
    RX_HOP_TABLE_ENTRY_1    = 0x5003,
    RX_HOP_TABLE_ENTRY_2    = 0x5004,
    RX_HOP_TABLE_ENTRY_3    = 0x5005,
    RX_HOP_TABLE_ENTRY_4    = 0x5006,
    RX_HOP_TABLE_ENTRY_5    = 0x5007,
    RX_HOP_TABLE_ENTRY_6    = 0x5008,
    RX_HOP_TABLE_ENTRY_7    = 0x5009,
    RX_HOP_TABLE_ENTRY_8    = 0x500A,
    RX_HOP_TABLE_ENTRY_9    = 0x500B,
    RX_HOP_TABLE_ENTRY_10   = 0x500C,
    RX_HOP_TABLE_ENTRY_11   = 0x500D,
    RX_HOP_TABLE_ENTRY_12   = 0x500E,
    RX_HOP_TABLE_ENTRY_13   = 0x500F,
    RX_HOP_TABLE_ENTRY_14   = 0x5010,
    RX_HOP_TABLE_ENTRY_15   = 0x5011,
    RX_HOP_TABLE_ENTRY_16   = 0x5012,
    RX_HOP_TABLE_ENTRY_17   = 0x5013,
    RX_HOP_TABLE_ENTRY_18   = 0x5014,
    RX_HOP_TABLE_ENTRY_19   = 0x5015,
    RX_HOP_TABLE_ENTRY_20   = 0x5016,
    RX_HOP_TABLE_ENTRY_21   = 0x5017,
    RX_HOP_TABLE_ENTRY_22   = 0x5018,
    RX_HOP_TABLE_ENTRY_23   = 0x5019,
    RX_HOP_TABLE_ENTRY_24   = 0x501A,
    RX_HOP_TABLE_ENTRY_25   = 0x501B,
    RX_HOP_TABLE_ENTRY_26   = 0x501C,
    RX_HOP_TABLE_ENTRY_27   = 0x501D,
    RX_HOP_TABLE_ENTRY_28   = 0x501E,
    RX_HOP_TABLE_ENTRY_29   = 0x501F,
    RX_HOP_TABLE_ENTRY_30   = 0x5020,
    RX_HOP_TABLE_ENTRY_31   = 0x5021,
    RX_HOP_TABLE_ENTRY_32   = 0x5022,
    RX_HOP_TABLE_ENTRY_33   = 0x5023,
    RX_HOP_TABLE_ENTRY_34   = 0x5024,
    RX_HOP_TABLE_ENTRY_35   = 0x5025,
    RX_HOP_TABLE_ENTRY_36   = 0x5026,
    RX_HOP_TABLE_ENTRY_37   = 0x5027,
    RX_HOP_TABLE_ENTRY_38   = 0x5028,
    RX_HOP_TABLE_ENTRY_39   = 0x5029,
    RX_HOP_TABLE_ENTRY_40   = 0x502A,
    RX_HOP_TABLE_ENTRY_41   = 0x502B,
    RX_HOP_TABLE_ENTRY_42   = 0x502C,
    RX_HOP_TABLE_ENTRY_43   = 0x502D,
    RX_HOP_TABLE_ENTRY_44   = 0x502E,
    RX_HOP_TABLE_ENTRY_45   = 0x502F,
    RX_HOP_TABLE_ENTRY_46   = 0x5030,
    RX_HOP_TABLE_ENTRY_47   = 0x5031,
    RX_HOP_TABLE_ENTRY_48   = 0x5032,
    RX_HOP_TABLE_ENTRY_49   = 0x5033,
    RX_HOP_TABLE_ENTRY_50   = 0x5034,
    RX_HOP_TABLE_ENTRY_51   = 0x5035,
    RX_HOP_TABLE_ENTRY_52   = 0x5036,
    RX_HOP_TABLE_ENTRY_53   = 0x5037,
    RX_HOP_TABLE_ENTRY_54   = 0x5038,
    RX_HOP_TABLE_ENTRY_55   = 0x5039,
    RX_HOP_TABLE_ENTRY_56   = 0x503A,
    RX_HOP_TABLE_ENTRY_57   = 0x503B,
    RX_HOP_TABLE_ENTRY_58   = 0x503C,
    RX_HOP_TABLE_ENTRY_59   = 0x503D,
    RX_HOP_TABLE_ENTRY_60   = 0x503E,
    RX_HOP_TABLE_ENTRY_61   = 0x503F,
    RX_HOP_TABLE_ENTRY_62   = 0x5040,
    RX_HOP_TABLE_ENTRY_63   = 0x5041
}SI446X_PROPERTY;


#endif /* INC_SI446X_DEFS_H_ */
