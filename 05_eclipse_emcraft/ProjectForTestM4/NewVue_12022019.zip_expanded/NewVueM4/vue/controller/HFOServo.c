/*
 * HFOServo.c
 *
 *  Created on: 3 thg 1, 2019
 *      Author: haxua
 */
#include <deviceTask/DeviceInterface.h>
#include <ipcTask/IpcInterface.h>
#include "HFOController.h"
#include "HFOServo.h"
#include "stdlib.h"

#define MAX_STROKE_VOL_ADJUSTMENT  160
#define MIN_STROKE_VOL_ADJUSTMENT  0
#define NUM_STROKE_INDEX (MAX_STROKE_VOL_ADJUSTMENT - MIN_STROKE_VOL_ADJUSTMENT + 1)
#define MAX_HFO_STEP_POSITION_OLD 2200
#define MAX_HFO_STEP_POSITION_NEW 12800
/*Sinusoidal table for one cycle 1000Points, Feb 2, 1999 	*/
static int16_t  sin_table[1000] = {
		0x0000, 0x00ce, 0x019c, 0x026a, 0x0337, 0x0405, 0x04d3, 0x05a1,
		0x066e, 0x073c, 0x080a, 0x08d7, 0x09a4, 0x0a72, 0x0b3f, 0x0c0c,
		0x0cd9, 0x0da5, 0x0e72, 0x0f3f, 0x100b, 0x10d7, 0x11a3, 0x126f,
		0x133b, 0x1406, 0x14d1, 0x159c, 0x1667, 0x1732, 0x17fc, 0x18c6,
		0x1990, 0x1a5a, 0x1b23, 0x1bec, 0x1cb5, 0x1d7d, 0x1e46, 0x1f0d,
		0x1fd5, 0x209c, 0x2163, 0x222a, 0x22f0, 0x23b6, 0x247c, 0x2541,
		0x2605, 0x26ca, 0x278e, 0x2851, 0x2915, 0x29d7, 0x2a9a, 0x2b5c,
		0x2c1d, 0x2cde, 0x2d9f, 0x2e5f, 0x2f1f, 0x2fde, 0x309d, 0x315b,
		0x3219, 0x32d6, 0x3392, 0x344f, 0x350a, 0x35c5, 0x3680, 0x373a,
		0x37f3, 0x38ac, 0x3965, 0x3a1c, 0x3ad4, 0x3b8a, 0x3c40, 0x3cf5,
		0x3daa, 0x3e5e, 0x3f12, 0x3fc5, 0x4077, 0x4128, 0x41d9, 0x4289,
		0x4339, 0x43e8, 0x4496, 0x4543, 0x45f0, 0x469c, 0x4748, 0x47f2,
		0x489c, 0x4945, 0x49ee, 0x4a96, 0x4b3d, 0x4be3, 0x4c88, 0x4d2d,
		0x4dd1, 0x4e74, 0x4f16, 0x4fb8, 0x5058, 0x50f8, 0x5197, 0x5235,
		0x52d3, 0x536f, 0x540b, 0x54a6, 0x5540, 0x55d9, 0x5671, 0x5709,
		0x579f, 0x5835, 0x58ca, 0x595d, 0x59f0, 0x5a82, 0x5b14, 0x5ba4,
		0x5c33, 0x5cc1, 0x5d4f, 0x5ddb, 0x5e67, 0x5ef1, 0x5f7b, 0x6004,
		0x608b, 0x6112, 0x6198, 0x621c, 0x62a0, 0x6323, 0x63a5, 0x6425,
		0x64a5, 0x6524, 0x65a1, 0x661e, 0x669a, 0x6714, 0x678e, 0x6806,
		0x687e, 0x68f4, 0x696a, 0x69de, 0x6a51, 0x6ac3, 0x6b34, 0x6ba4,
		0x6c13, 0x6c81, 0x6ced, 0x6d59, 0x6dc3, 0x6e2d, 0x6e95, 0x6efc,
		0x6f62, 0x6fc7, 0x702b, 0x708d, 0x70ef, 0x714f, 0x71ae, 0x720c,
		0x7269, 0x72c5, 0x7320, 0x7379, 0x73d1, 0x7428, 0x747e, 0x74d3,
		0x7527, 0x7579, 0x75ca, 0x761a, 0x7669, 0x76b7, 0x7703, 0x774e,
		0x7798, 0x77e1, 0x7828, 0x786f, 0x78b4, 0x78f8, 0x793b, 0x797c,
		0x79bc, 0x79fb, 0x7a39, 0x7a76, 0x7ab1, 0x7aeb, 0x7b24, 0x7b5b,
		0x7b92, 0x7bc7, 0x7bfb, 0x7c2d, 0x7c5e, 0x7c8e, 0x7cbd, 0x7ceb,
		0x7d17, 0x7d42, 0x7d6c, 0x7d94, 0x7dbc, 0x7de2, 0x7e06, 0x7e2a,
		0x7e4c, 0x7e6d, 0x7e8c, 0x7eaa, 0x7ec7, 0x7ee3, 0x7efe, 0x7f17,
		0x7f2f, 0x7f45, 0x7f5b, 0x7f6f, 0x7f81, 0x7f93, 0x7fa3, 0x7fb2,
		0x7fbf, 0x7fcc, 0x7fd7, 0x7fe0, 0x7fe9, 0x7ff0, 0x7ff6, 0x7ffa,
		0x7ffd, 0x7fff, 0x7fff, 0x7fff, 0x7ffd, 0x7ffa, 0x7ff6, 0x7ff0,
		0x7fe9, 0x7fe0, 0x7fd7, 0x7fcc, 0x7fbf, 0x7fb2, 0x7fa3, 0x7f93,
		0x7f81, 0x7f6f, 0x7f5b, 0x7f45, 0x7f2f, 0x7f17, 0x7efe, 0x7ee3,
		0x7ec7, 0x7eaa, 0x7e8c, 0x7e6d, 0x7e4c, 0x7e2a, 0x7e06, 0x7de2,
		0x7dbc, 0x7d94, 0x7d6c, 0x7d42, 0x7d17, 0x7ceb, 0x7cbd, 0x7c8e,
		0x7c5e, 0x7c2d, 0x7bfb, 0x7bc7, 0x7b92, 0x7b5b, 0x7b24, 0x7aeb,
		0x7ab1, 0x7a76, 0x7a39, 0x79fb, 0x79bc, 0x797c, 0x793b, 0x78f8,
		0x78b4, 0x786f, 0x7828, 0x77e1, 0x7798, 0x774e, 0x7703, 0x76b7,
		0x7669, 0x761a, 0x75ca, 0x7579, 0x7527, 0x74d3, 0x747e, 0x7428,
		0x73d1, 0x7379, 0x7320, 0x72c5, 0x7269, 0x720c, 0x71ae, 0x714f,
		0x70ef, 0x708d, 0x702b, 0x6fc7, 0x6f62, 0x6efc, 0x6e95, 0x6e2d,
		0x6dc3, 0x6d59, 0x6ced, 0x6c81, 0x6c13, 0x6ba4, 0x6b34, 0x6ac3,
		0x6a51, 0x69de, 0x696a, 0x68f4, 0x687e, 0x6806, 0x678e, 0x6714,
		0x669a, 0x661e, 0x65a1, 0x6524, 0x64a5, 0x6425, 0x63a5, 0x6323,
		0x62a0, 0x621c, 0x6198, 0x6112, 0x608b, 0x6004, 0x5f7b, 0x5ef1,
		0x5e67, 0x5ddb, 0x5d4f, 0x5cc1, 0x5c33, 0x5ba4, 0x5b14, 0x5a82,
		0x59f0, 0x595d, 0x58ca, 0x5835, 0x579f, 0x5709, 0x5671, 0x55d9,
		0x5540, 0x54a6, 0x540b, 0x536f, 0x52d3, 0x5235, 0x5197, 0x50f8,
		0x5058, 0x4fb8, 0x4f16, 0x4e74, 0x4dd1, 0x4d2d, 0x4c88, 0x4be3,
		0x4b3d, 0x4a96, 0x49ee, 0x4945, 0x489c, 0x47f2, 0x4748, 0x469c,
		0x45f0, 0x4543, 0x4496, 0x43e8, 0x4339, 0x4289, 0x41d9, 0x4128,
		0x4077, 0x3fc5, 0x3f12, 0x3e5e, 0x3daa, 0x3cf5, 0x3c40, 0x3b8a,
		0x3ad4, 0x3a1c, 0x3965, 0x38ac, 0x37f3, 0x373a, 0x3680, 0x35c5,
		0x350a, 0x344f, 0x3392, 0x32d6, 0x3219, 0x315b, 0x309d, 0x2fde,
		0x2f1f, 0x2e5f, 0x2d9f, 0x2cde, 0x2c1d, 0x2b5c, 0x2a9a, 0x29d7,
		0x2915, 0x2851, 0x278e, 0x26ca, 0x2605, 0x2541, 0x247c, 0x23b6,
		0x22f0, 0x222a, 0x2163, 0x209c, 0x1fd5, 0x1f0d, 0x1e46, 0x1d7d,
		0x1cb5, 0x1bec, 0x1b23, 0x1a5a, 0x1990, 0x18c6, 0x17fc, 0x1732,
		0x1667, 0x159c, 0x14d1, 0x1406, 0x133b, 0x126f, 0x11a3, 0x10d7,
		0x100b, 0x0f3f, 0x0e72, 0x0da5, 0x0cd9, 0x0c0c, 0x0b3f, 0x0a72,
		0x09a4, 0x08d7, 0x080a, 0x073c, 0x066e, 0x05a1, 0x04d3, 0x0405,
		0x0337, 0x026a, 0x019c, 0x00ce, 0x0000, 0xff32, 0xfe64, 0xfd96,
		0xfcc9, 0xfbfb, 0xfb2d, 0xfa5f, 0xf992, 0xf8c4, 0xf7f6, 0xf729,
		0xf65c, 0xf58e, 0xf4c1, 0xf3f4, 0xf327, 0xf25b, 0xf18e, 0xf0c1,
		0xeff5, 0xef29, 0xee5d, 0xed91, 0xecc5, 0xebfa, 0xeb2f, 0xea64,
		0xe999, 0xe8ce, 0xe804, 0xe73a, 0xe670, 0xe5a6, 0xe4dd, 0xe414,
		0xe34b, 0xe283, 0xe1ba, 0xe0f3, 0xe02b, 0xdf64, 0xde9d, 0xddd6,
		0xdd10, 0xdc4a, 0xdb84, 0xdabf, 0xd9fb, 0xd936, 0xd872, 0xd7af,
		0xd6eb, 0xd629, 0xd566, 0xd4a4, 0xd3e3, 0xd322, 0xd261, 0xd1a1,
		0xd0e1, 0xd022, 0xcf63, 0xcea5, 0xcde7, 0xcd2a, 0xcc6e, 0xcbb1,
		0xcaf6, 0xca3b, 0xc980, 0xc8c6, 0xc80d, 0xc754, 0xc69b, 0xc5e4,
		0xc52c, 0xc476, 0xc3c0, 0xc30b, 0xc256, 0xc1a2, 0xc0ee, 0xc03b,
		0xbf89, 0xbed8, 0xbe27, 0xbd77, 0xbcc7, 0xbc18, 0xbb6a, 0xbabd,
		0xba10, 0xb964, 0xb8b8, 0xb80e, 0xb764, 0xb6bb, 0xb612, 0xb56a,
		0xb4c3, 0xb41d, 0xb378, 0xb2d3, 0xb22f, 0xb18c, 0xb0ea, 0xb048,
		0xafa8, 0xaf08, 0xae69, 0xadcb, 0xad2d, 0xac91, 0xabf5, 0xab5a,
		0xaac0, 0xaa27, 0xa98f, 0xa8f7, 0xa861, 0xa7cb, 0xa736, 0xa6a3,
		0xa610, 0xa57e, 0xa4ec, 0xa45c, 0xa3cd, 0xa33f, 0xa2b1, 0xa225,
		0xa199, 0xa10f, 0xa085, 0x9ffc, 0x9f75, 0x9eee, 0x9e68, 0x9de4,
		0x9d60, 0x9cdd, 0x9c5b, 0x9bdb, 0x9b5b, 0x9adc, 0x9a5f, 0x99e2,
		0x9966, 0x98ec, 0x9872, 0x97fa, 0x9782, 0x970c, 0x9696, 0x9622,
		0x95af, 0x953d, 0x94cc, 0x945c, 0x93ed, 0x937f, 0x9313, 0x92a7,
		0x923d, 0x91d3, 0x916b, 0x9104, 0x909e, 0x9039, 0x8fd5, 0x8f73,
		0x8f11, 0x8eb1, 0x8e52, 0x8df4, 0x8d97, 0x8d3b, 0x8ce0, 0x8c87,
		0x8c2f, 0x8bd8, 0x8b82, 0x8b2d, 0x8ad9, 0x8a87, 0x8a36, 0x89e6,
		0x8997, 0x8949, 0x88fd, 0x88b2, 0x8868, 0x881f, 0x87d8, 0x8791,
		0x874c, 0x8708, 0x86c5, 0x8684, 0x8644, 0x8605, 0x85c7, 0x858a,
		0x854f, 0x8515, 0x84dc, 0x84a5, 0x846e, 0x8439, 0x8405, 0x83d3,
		0x83a2, 0x8372, 0x8343, 0x8315, 0x82e9, 0x82be, 0x8294, 0x826c,
		0x8244, 0x821e, 0x81fa, 0x81d6, 0x81b4, 0x8193, 0x8174, 0x8156,
		0x8139, 0x811d, 0x8102, 0x80e9, 0x80d1, 0x80bb, 0x80a5, 0x8091,
		0x807f, 0x806d, 0x805d, 0x804e, 0x8041, 0x8034, 0x8029, 0x8020,
		0x8017, 0x8010, 0x800a, 0x8006, 0x8003, 0x8001, 0x8001, 0x8001,
		0x8003, 0x8006, 0x800a, 0x8010, 0x8017, 0x8020, 0x8029, 0x8034,
		0x8041, 0x804e, 0x805d, 0x806d, 0x807f, 0x8091, 0x80a5, 0x80bb,
		0x80d1, 0x80e9, 0x8102, 0x811d, 0x8139, 0x8156, 0x8174, 0x8193,
		0x81b4, 0x81d6, 0x81fa, 0x821e, 0x8244, 0x826c, 0x8294, 0x82be,
		0x82e9, 0x8315, 0x8343, 0x8372, 0x83a2, 0x83d3, 0x8405, 0x8439,
		0x846e, 0x84a5, 0x84dc, 0x8515, 0x854f, 0x858a, 0x85c7, 0x8605,
		0x8644, 0x8684, 0x86c5, 0x8708, 0x874c, 0x8791, 0x87d8, 0x881f,
		0x8868, 0x88b2, 0x88fd, 0x8949, 0x8997, 0x89e6, 0x8a36, 0x8a87,
		0x8ad9, 0x8b2d, 0x8b82, 0x8bd8, 0x8c2f, 0x8c87, 0x8ce0, 0x8d3b,
		0x8d97, 0x8df4, 0x8e52, 0x8eb1, 0x8f11, 0x8f73, 0x8fd5, 0x9039,
		0x909e, 0x9104, 0x916b, 0x91d3, 0x923d, 0x92a7, 0x9313, 0x937f,
		0x93ed, 0x945c, 0x94cc, 0x953d, 0x95af, 0x9622, 0x9696, 0x970c,
		0x9782, 0x97fa, 0x9872, 0x98ec, 0x9966, 0x99e2, 0x9a5f, 0x9adc,
		0x9b5b, 0x9bdb, 0x9c5b, 0x9cdd, 0x9d60, 0x9de4, 0x9e68, 0x9eee,
		0x9f75, 0x9ffc, 0xa085, 0xa10f, 0xa199, 0xa225, 0xa2b1, 0xa33f,
		0xa3cd, 0xa45c, 0xa4ec, 0xa57e, 0xa610, 0xa6a3, 0xa736, 0xa7cb,
		0xa861, 0xa8f7, 0xa98f, 0xaa27, 0xaac0, 0xab5a, 0xabf5, 0xac91,
		0xad2d, 0xadcb, 0xae69, 0xaf08, 0xafa8, 0xb048, 0xb0ea, 0xb18c,
		0xb22f, 0xb2d3, 0xb378, 0xb41d, 0xb4c3, 0xb56a, 0xb612, 0xb6bb,
		0xb764, 0xb80e, 0xb8b8, 0xb964, 0xba10, 0xbabd, 0xbb6a, 0xbc18,
		0xbcc7, 0xbd77, 0xbe27, 0xbed8, 0xbf89, 0xc03b, 0xc0ee, 0xc1a2,
		0xc256, 0xc30b, 0xc3c0, 0xc476, 0xc52c, 0xc5e4, 0xc69b, 0xc754,
		0xc80d, 0xc8c6, 0xc980, 0xca3b, 0xcaf6, 0xcbb1, 0xcc6e, 0xcd2a,
		0xcde7, 0xcea5, 0xcf63, 0xd022, 0xd0e1, 0xd1a1, 0xd261, 0xd322,
		0xd3e3, 0xd4a4, 0xd566, 0xd629, 0xd6eb, 0xd7af, 0xd872, 0xd936,
		0xd9fb, 0xdabf, 0xdb84, 0xdc4a, 0xdd10, 0xddd6, 0xde9d, 0xdf64,
		0xe02b, 0xe0f3, 0xe1ba, 0xe283, 0xe34b, 0xe414, 0xe4dd, 0xe5a6,
		0xe670, 0xe73a, 0xe804, 0xe8ce, 0xe999, 0xea64, 0xeb2f, 0xebfa,
		0xecc5, 0xed91, 0xee5d, 0xef29, 0xeff5, 0xf0c1, 0xf18e, 0xf25b,
		0xf327, 0xf3f4, 0xf4c1, 0xf58e, 0xf65c, 0xf729, 0xf7f6, 0xf8c4,
		0xf992, 0xfa5f, 0xfb2d, 0xfbfb, 0xfcc9, 0xfd96, 0xfe64, 0xff32};
static const int32_t steps_for_big_ml[NUM_STROKE_INDEX][13] = {
		{0,0,0,0,0,0,0,0,0,0,0,0,0},
		{10,10,10,10,10,10,10,10,10,10,10,10,10},
		{20,20,20,20,20,20,20,19,19,19,19,19,19},
		{30,30,30,30,30,30,29,29,29,29,29,29,29},
		{41,40,40,40,40,40,39,39,38,38,38,38,38},
		{51,51,50,50,50,49,49,48,48,48,48,48,48},
		{61,61,60,60,60,59,59,58,57,57,57,57,57},
		{71,71,70,70,70,69,68,68,67,67,67,67,67},
		{81,81,81,80,79,79,78,77,76,76,76,76,76},
		{91,91,91,90,89,89,88,87,86,86,86,86,86},
		{102,101,101,100,99,99,98,97,96,95,95,95,95},
		{112,111,111,110,109,109,108,106,105,105,105,105,105},
		{122,121,121,120,119,119,117,116,115,114,114,114,114},
		{132,132,131,130,129,128,127,126,124,124,124,124,124},
		{142,142,141,140,139,138,137,135,134,134,133,133,133},
		{152,152,151,150,149,148,147,145,143,143,143,143,143},
		{162,162,161,160,159,158,156,155,153,153,152,152,152},
		{173,172,171,170,169,168,166,164,163,162,162,162,162},
		{183,182,181,180,179,178,176,174,172,172,171,171,171},
		{193,192,191,190,189,188,186,184,182,181,181,181,181},
		{203,202,201,200,199,198,196,193,191,191,190,190,190},
		{213,213,211,210,209,207,205,203,201,200,200,200,200},
		{223,223,222,220,218,217,215,213,210,210,209,209,209},
		{233,233,232,230,228,227,225,222,220,219,219,219,219},
		{244,243,242,240,238,237,235,232,229,229,228,228,228},
		{254,253,252,250,248,247,245,242,239,239,238,238,238},
		{264,263,262,260,258,257,254,251,249,248,247,247,248},
		{274,273,272,270,268,267,264,261,258,258,257,257,257},
		{284,283,282,280,278,277,274,270,268,267,266,266,267},
		{294,293,292,290,288,287,284,280,277,277,276,276,276},
		{305,304,302,300,298,296,293,290,287,286,285,285,286},
		{315,314,312,310,308,306,303,299,296,296,295,295,295},
		{325,324,322,320,318,316,313,309,306,305,304,304,305},
		{335,334,332,330,328,326,323,319,315,315,314,314,314},
		{345,344,342,340,338,336,333,328,325,324,323,323,324},
		{355,354,352,350,348,346,342,338,335,334,333,333,333},
		{365,364,363,360,357,356,352,348,344,343,342,342,343},
		{376,374,373,370,367,366,362,357,354,353,352,352,352},
		{386,385,383,380,377,375,372,367,363,363,361,361,362},
		{396,395,393,390,387,385,381,377,373,372,371,371,371},
		{406,405,403,400,397,395,391,386,382,382,380,380,381},
		{416,415,413,410,407,405,401,396,392,391,390,390,390},
		{426,425,423,420,417,415,411,406,402,401,399,399,400},
		{436,435,433,430,427,425,421,415,411,410,409,409,409},
		{447,445,443,440,437,435,430,425,421,420,418,418,419},
		{457,455,453,450,447,445,440,435,430,429,428,428,428},
		{467,466,463,460,457,454,450,444,440,439,437,437,438},
		{477,476,473,470,467,464,460,454,449,448,447,447,447},
		{487,486,483,480,477,474,469,464,459,458,456,456,457},
		{497,496,493,490,487,484,479,473,468,467,466,466,466},
		{508,506,504,500,497,494,489,483,478,477,475,476,476},
		{518,516,514,509,506,504,499,493,488,487,485,485,486},
		{528,526,524,519,516,514,509,502,497,496,494,495,495},
		{538,536,534,529,526,524,518,512,507,506,504,504,505},
		{548,546,544,539,536,534,528,522,516,515,513,514,514},
		{558,557,554,549,546,543,538,531,526,525,523,523,524},
		{568,567,564,559,556,553,548,541,535,534,532,533,533},
		{579,577,574,569,566,563,557,551,545,544,542,542,543},
		{589,587,584,579,576,573,567,560,554,553,551,552,552},
		{599,597,594,589,586,583,577,570,564,563,561,561,562},
		{609,607,604,599,596,593,587,580,574,572,570,571,571},
		{619,617,614,609,606,603,597,589,583,582,580,580,581},
		{629,627,624,619,616,613,606,599,593,591,589,590,590},
		{639,638,634,629,626,622,616,609,602,601,599,599,600},
		{650,648,644,639,636,632,626,618,612,611,608,609,609},
		{660,658,655,649,645,642,636,628,621,620,618,618,619},
		{670,668,665,659,655,652,645,638,631,630,627,628,628},
		{680,678,675,669,665,662,655,647,641,639,637,637,638},
		{690,688,685,679,675,672,665,657,650,649,646,647,647},
		{700,698,695,689,685,682,675,667,660,658,656,656,657},
		{711,708,705,699,695,692,685,676,669,668,665,666,666},
		{721,719,715,709,705,701,694,686,679,677,675,675,676},
		{731,729,725,719,715,711,704,696,688,687,684,685,685},
		{741,739,735,729,725,721,714,705,698,696,694,694,695},
		{751,749,745,739,735,731,724,715,707,706,703,704,704},
		{761,759,755,749,745,741,734,725,717,716,713,713,714},
		{771,769,765,759,755,751,743,734,727,725,722,723,724},
		{782,779,775,769,765,761,753,744,736,735,732,732,733},
		{792,789,785,779,775,771,763,753,746,744,741,742,743},
		{802,799,796,789,784,781,773,763,755,754,751,751,752},
		{812,810,806,799,794,790,782,773,765,763,760,761,762},
		{822,820,816,809,804,800,792,782,774,773,770,770,771},
		{832,830,826,819,814,810,802,792,784,782,779,780,781},
		{842,840,836,829,824,820,812,802,793,792,789,789,790},
		{853,850,846,839,834,830,822,811,803,801,798,799,800},
		{863,860,856,849,844,840,831,821,813,811,808,808,809},
		{873,870,866,859,854,850,841,831,822,820,817,818,819},
		{883,880,876,869,864,860,851,840,832,830,827,827,828},
		{893,891,886,879,874,869,861,850,841,840,836,837,838},
		{903,901,896,889,884,879,870,860,851,849,846,846,847},
		{914,911,906,899,894,889,880,869,860,859,855,856,847},
		{924,921,916,909,904,899,890,879,870,868,865,865,847},
		{934,931,926,919,914,909,900,889,880,878,874,875,847},
		{944,941,937,929,923,919,910,898,889,887,884,884,847},
		{954,951,947,939,933,929,919,908,899,897,893,894,847},
		{964,961,957,949,943,939,929,918,908,906,903,903,847},
		{974,972,967,959,953,948,939,927,918,916,912,913,847},
		{985,982,977,969,963,958,949,937,927,925,922,922,847},
		{995,992,987,979,973,968,958,947,937,935,931,932,847},
		{1005,1002,997,989,983,978,968,956,946,944,941,941,847},
		{1015,1012,1007,999,993,988,978,966,956,954,950,941,847},
		{1025,1022,1017,1009,1003,998,988,976,966,964,960,941,847},
		{1035,1032,1027,1019,1013,1008,998,985,975,973,969,941,847},
		{1045,1042,1037,1029,1023,1018,1007,995,985,983,979,941,847},
		{1056,1052,1047,1039,1033,1028,1017,1005,994,992,988,941,847},
		{1066,1063,1057,1049,1043,1037,1027,1014,1004,1002,998,941,847},
		{1076,1073,1067,1059,1053,1047,1037,1024,1013,1011,1007,941,847},
		{1086,1083,1077,1069,1063,1057,1046,1034,1023,1021,1007,941,847},
		{1096,1093,1088,1079,1072,1067,1056,1043,1032,1030,1007,941,847},
		{1106,1103,1098,1089,1082,1077,1066,1053,1042,1040,1007,941,847},
		{1117,1113,1108,1099,1092,1087,1076,1063,1052,1049,1007,941,847},
		{1127,1123,1118,1109,1102,1097,1086,1072,1061,1059,1007,941,847},
		{1137,1133,1128,1119,1112,1107,1095,1082,1071,1068,1007,941,847},
		{1147,1144,1138,1129,1122,1116,1105,1092,1080,1068,1007,941,847},
		{1157,1154,1148,1139,1132,1126,1115,1101,1090,1068,1007,941,847},
		{1167,1164,1158,1149,1142,1136,1125,1111,1099,1068,1007,941,847},
		{1177,1174,1168,1159,1152,1146,1134,1121,1109,1068,1007,941,847},
		{1188,1184,1178,1169,1162,1156,1144,1130,1119,1068,1007,941,847},
		{1198,1194,1188,1179,1172,1166,1154,1140,1128,1068,1007,941,847},
		{1208,1204,1198,1189,1182,1176,1164,1150,1138,1068,1007,941,847},
		{1218,1214,1208,1199,1192,1186,1174,1159,1147,1068,1007,941,847},
		{1228,1225,1218,1209,1202,1195,1183,1169,1147,1068,1007,941,847},
		{1238,1235,1229,1219,1211,1205,1193,1179,1147,1068,1007,941,847},
		{1248,1245,1239,1229,1221,1215,1203,1188,1147,1068,1007,941,847},
		{1259,1255,1249,1239,1231,1225,1213,1198,1147,1068,1007,941,847},
		{1269,1265,1259,1249,1241,1235,1223,1208,1147,1068,1007,941,847},
		{1279,1275,1269,1259,1251,1245,1232,1217,1147,1068,1007,941,847},
		{1289,1285,1279,1269,1261,1255,1242,1227,1147,1068,1007,941,847},
		{1299,1295,1289,1279,1271,1265,1252,1236,1147,1068,1007,941,847},
		{1309,1305,1299,1289,1281,1275,1262,1246,1147,1068,1007,941,847},
		{1320,1316,1309,1299,1291,1284,1271,1256,1147,1068,1007,941,847},
		{1330,1326,1319,1309,1301,1294,1281,1256,1147,1068,1007,941,847},
		{1340,1336,1329,1319,1311,1304,1291,1256,1147,1068,1007,941,847},
		{1350,1346,1339,1329,1321,1314,1301,1256,1147,1068,1007,941,847},
		{1360,1356,1349,1339,1331,1324,1311,1256,1147,1068,1007,941,847},
		{1370,1366,1359,1349,1341,1334,1320,1256,1147,1068,1007,941,847},
		{1380,1376,1370,1359,1350,1344,1330,1256,1147,1068,1007,941,847},
		{1391,1386,1380,1369,1360,1354,1340,1256,1147,1068,1007,941,847},
		{1401,1397,1390,1379,1370,1363,1350,1256,1147,1068,1007,941,847},
		{1411,1407,1400,1389,1380,1373,1359,1256,1147,1068,1007,941,847},
		{1421,1417,1410,1399,1390,1383,1369,1256,1147,1068,1007,941,847},
		{1431,1427,1420,1409,1400,1393,1379,1256,1147,1068,1007,941,847},
		{1441,1437,1430,1419,1410,1403,1389,1256,1147,1068,1007,941,847},
		{1451,1447,1440,1429,1420,1413,1399,1256,1147,1068,1007,941,847},
		{1462,1457,1450,1439,1430,1423,1408,1256,1147,1068,1007,941,847},
		{1472,1467,1460,1449,1440,1433,1418,1256,1147,1068,1007,941,847},
		{1482,1478,1470,1459,1450,1442,1428,1256,1147,1068,1007,941,847},
		{1492,1488,1480,1469,1460,1452,1438,1256,1147,1068,1007,941,847},
		{1502,1498,1490,1479,1470,1462,1447,1256,1147,1068,1007,941,847},
		{1512,1508,1500,1489,1480,1472,1447,1256,1147,1068,1007,941,847},
		{1523,1518,1511,1499,1490,1482,1447,1256,1147,1068,1007,941,847},
		{1533,1528,1521,1508,1499,1492,1447,1256,1147,1068,1007,941,847},
		{1543,1538,1531,1518,1509,1502,1447,1256,1147,1068,1007,941,847},
		{1553,1548,1541,1528,1519,1512,1447,1256,1147,1068,1007,941,847},
		{1563,1558,1551,1538,1529,1522,1447,1256,1147,1068,1007,941,847},
		{1573,1569,1561,1548,1539,1522,1447,1256,1147,1068,1007,941,847},
		{1583,1579,1571,1558,1549,1522,1447,1256,1147,1068,1007,941,847},
		{1594,1589,1581,1568,1559,1522,1447,1256,1147,1068,1007,941,847},
		{1604,1599,1591,1578,1569,1522,1447,1256,1147,1068,1007,941,847},
		{1614,1609,1601,1588,1579,1522,1447,1256,1147,1068,1007,941,847},
		{1624,1619,1611,1598,1589,1522,1447,1256,1147,1068,1007,941,847}
};

//This table is an offset for matching with amplitude curve in Humming V
static const int32_t steps_offset_for_HummingV[NUM_STROKE_INDEX][13] = {

		{0,0,0,0,0,0,0,0,0,0,0,0,0},                           //SV0
		{2,2,2,2,2,2,2,2,2,3,4,4,3},
		{5,5,5,5,5,5,5,5,5,6,8,8,7},
		{8,8,8,8,8,8,8,8,8,9,11,12,11},
		{11,11,11,11,11,11,11,11,11,13,15,16,14},
		{14,14,14,14,14,14,14,14,14,16,19,20,18},
		{17,17,17,17,17,17,17,17,17,20,23,24,21},
		{20,20,20,20,20,20,20,20,20,24,27,27,25},
		{23,23,23,23,23,23,23,23,23,28,30,30,28},
		{26,26,26,26,26,26,26,26,26,31,34,33,31},
		{29,29,29,29,29,29,29,29,29,35,37,36,34},               //SV10
		{32,32,32,32,32,32,32,32,32,38,40,39,36},
		{35,35,35,35,35,35,35,35,35,41,43,41,38},
		{37,37,37,37,37,37,37,37,37,44,46,43,40},
		{39,39,39,39,39,39,39,39,39,46,48,45,42},
		{41,41,41,41,41,41,41,41,41,49,50,47,44},
		{43,43,43,43,43,43,43,43,43,51,53,48,45},
		{45,45,45,45,45,45,45,45,45,53,54,50,46},
		{46,46,46,46,46,46,46,46,46,54,56,51,47},
		{48,48,48,48,48,48,48,48,48,55,57,52,47},
		{49,49,49,49,49,49,49,49,49,56,59,53,48},               //SV20
		{50,50,50,50,50,50,50,50,50,57,60,53,48},
		{51,51,51,51,51,51,51,51,51,57,60,54,47},
		{52,52,52,52,52,52,52,52,52,58,61,54,47},
		{52,52,52,52,52,52,52,52,52,58,61,54,47},
		{53,53,53,53,53,53,53,53,53,57,61,53,46},
		{54,54,54,54,54,54,54,54,54,57,61,53,45},
		{54,54,54,54,54,54,54,54,54,56,61,52,44},
		{54,54,54,54,54,54,54,54,54,55,60,52,42},
		{55,55,55,55,55,55,55,55,55,54,59,51,41},
		{55,55,55,55,55,55,55,55,55,53,58,49,40},                //SV30
		{55,55,55,55,55,55,55,55,55,52,57,48,38},
		{55,55,55,55,55,55,55,55,55,51,56,47,36},
		{55,55,55,55,55,55,55,55,55,50,55,45,34},
		{56,56,56,56,56,56,56,56,56,48,53,43,32},
		{56,56,56,56,56,56,56,56,56,47,52,41,30},
		{56,56,56,56,56,56,56,56,56,45,50,39,28},
		{56,56,56,56,56,56,56,56,56,43,48,37,26},
		{56,56,56,56,56,56,56,56,56,42,46,34,24},
		{56,56,56,56,56,56,56,56,56,40,44,32,21},
		{57,57,57,57,57,57,57,57,57,38,42,29,19},                //SV40
		{57,57,57,57,57,57,57,57,57,37,40,27,16},
		{57,57,57,57,57,57,57,57,57,35,37,24,14},
		{57,57,57,57,57,57,57,57,57,33,35,21,11},
		{57,57,57,57,57,57,57,57,57,32,33,18,9},
		{57,57,57,57,57,57,57,57,57,30,30,15,6},
		{57,57,57,57,57,57,57,57,57,28,27,12,3},
		{57,57,57,57,57,57,57,57,57,27,25,8,0},
		{57,57,57,57,57,57,57,57,57,25,22,5,-3},
		{56,56,56,56,56,56,56,56,56,23,19,2,-6},
		{56,56,56,56,56,56,56,56,56,21,17,-2,-10},               //SV50
		{55,55,55,55,55,55,55,55,55,20,14,-5,-13},
		{54,54,54,54,54,54,54,54,54,18,11,-9,-17},
		{53,53,53,53,53,53,53,53,53,16,8,-12,-20},
		{52,52,52,52,52,52,52,52,52,14,5,-16,-24},
		{51,51,51,51,51,51,51,51,51,12,2,-20,-28},
		{49,49,49,49,49,49,49,49,49,10,-1,-24,-32},
		{47,47,47,47,47,47,47,47,47,8,-4,-27,-36},
		{45,45,45,45,45,45,45,45,45,5,-7,-31,-41},
		{43,43,43,43,43,43,43,43,43,3,-10,-35,-46},
		{40,40,40,40,40,40,40,40,40,0,-14,-39,-50},              //SV60
		{37,37,37,37,37,37,37,37,37,-2,-17,-43,-55},
		{33,33,33,33,33,33,33,33,33,-5,-21,-47,-60},
		{30,30,30,30,30,30,30,30,30,-8,-24,-51,-66},
		{26,26,26,26,26,26,26,26,26,-12,-28,-55,-71},
		{21,21,21,21,21,21,21,21,21,-15,-32,-59,-77},
		{17,17,17,17,17,17,17,17,17,-18,-35,-63,-82},
		{12,12,12,12,12,12,12,12,12,-22,-39,-67,-88},
		{7,7,7,7,7,7,7,7,7,-26,-44,-71,-94},
		{2,2,2,2,2,2,2,2,2,-30,-48,-76,-100},
		{-4,-4,-4,-4,-4,-4,-4,-4,-4,-34,-52,-80,-106},           //SV70
		{-10,-10,-10,-10,-10,-10,-10,-10,-10,-39,-57,-85,-112},
		{-16,-16,-16,-16,-16,-16,-16,-16,-16,-43,-62,-89,-117},
		{-21,-21,-21,-21,-21,-21,-21,-21,-21,-48,-67,-94,-123},
		{-27,-27,-27,-27,-27,-27,-27,-27,-27,-52,-72,-99,-129},
		{-33,-33,-33,-33,-33,-33,-33,-33,-33,-57,-78,-104,-134},
		{-39,-39,-39,-39,-39,-39,-39,-39,-39,-61,-84,-109,-139},
		{-44,-44,-44,-44,-44,-44,-44,-44,-44,-66,-90,-114,-144},
		{-49,-49,-49,-49,-49,-49,-49,-49,-49,-70,-96,-119,-148},
		{-53,-53,-53,-53,-53,-53,-53,-53,-53,-74,-102,-125,-152},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155}, //SV80
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155}, //SV90
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155}, //SV100
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155}, //SV110
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155}, //SV120
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155}, //SV130
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155}, //SV140
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155}, //SV150
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155},
		{-57,-57,-57,-57,-57,-57,-57,-57,-57,-78,-109,-131,-155}  //SV160
};
#define	FIVE_MINUTES	(int32_t)	150000 //5min
#define	FIVE_MPlS10S	(int32_t) 155000  //5min 10s
#define	TEN_MINUTES		(int32_t) 300000
#define THIRTY_MINUTES	(int32_t)	900000

#define		MIN_STROKE_FOR_OPEN_LOOP	10	// 10 scale by 10
#define		MAX_STROKE_FOR_OPEN_LOOP	450 // 45 scale by 10
#define MAP_SERVO_SCALE		(8)
#define HFO_STROKE_LIMIT	(1600)
#define MAX_HFO_FREQ    17
#define MIN_HFO_FREQ    5
#define SHIFT_INCREMENT 	5
#define SCALE 100
#define Kp 2
#define Ki 0.0018
#define Kd 0.0008
#define STANDARD_SETTING 15
#define AJUSTMENT_VALUE 0.00015
#define HFO_MAX_STEP 2000
#define	SIGN_RES			(2)
#define	PRESS_AMP_50		(1)
#define	MAKE_DC_SHIFT	(0)
#define INITIAL_ANGLE	750
#define	H_POINT	630//389
#define HFO_START_OFFSET	H_POINT
#define	LENGTH_SINTBL	1000
#define MIN_FREQ 5
#define DATA_BUFFER_SIZE 500
struct two_16bit{
	int16_t Low16;
	int16_t High16;
};
union One_32bit{
	int32_t Long32;
	struct two_16bit  access16;
};
static union One_32bit  gs_SumUnion;
static bool gs_IsDoAmpCtr;
static bool gs_IsHummingVOn;
static double gs_T;
static double gs_t;
static int32_t gs_CurrentAmp;
static bool gs_SIRequest;
static int32_t gs_DatOmicIntegrator;
static int32_t gs_DAmpIntegrator;
static int32_t gs_ZeroOrAmpIntegrator1;
static int32_t gs_ZeroOrAmpIntegrator2;
static int32_t gs_EHPointIntegrator1;
static int32_t gs_EHPointIntegrator2;
static int32_t gs_ShiftingOffset;
static int32_t gs_DASCounter;
static int32_t gs_TimeToGetTargetPress;
static int32_t gs_PressAmpCmd;
static int32_t gs_PressAmp;
static int32_t gs_FPressAmp;
static int32_t gs_AmpByDAC;
static int32_t gs_PrevAmpByDAC;
static int32_t gs_OpenMtrAmp;
static int32_t gs_IntegratorDAC1;
static int32_t gs_IntegratorDAC2;
static int32_t gs_IntegratorDAC3;
static int32_t gs_IntegratorDAC4;
static int32_t gs_UpperAmpLimit;
static int32_t gs_LowerAmpLimit;
static int16_t gs_ShiftInc;
static int16_t gs_StrokeVol;
static int32_t gs_SVUpdateCounter;
static int32_t gs_Sum;
static int32_t gs_AmpIntegral;
static int32_t gs_AmpDerivative;
static int32_t gs_PrevAmpError;
static bool gs_Enabled;
static const int32_t MAX_STROKE_VOLUME[13] ={160,160,160,160,160,154,148,130,120,112,106,99,89};
static int32_t gs_PressureDataBuffer[DATA_BUFFER_SIZE];
static int16_t gs_DataSize=0;
static int16_t gs_Index=0;
static desiredMotorPositionStruct gs_DesiredMotorPosition;
static int32_t gs_LastStepHFOPosition = 0;
/**
 * Change HFO motor position from old scale (in scale 2200)
 * to new scale (in scale 12800)
 * @param step
 */
void HFOServo_HFOMotorMoveToStepPosition(int32_t step)
{
//	MotorController_MoveToStepPositionOld(eHFOMotor,MAX_HFO_STEP_POSITION_OLD-step);
	gs_DesiredMotorPosition.desiredHFOPosition = MAX_HFO_STEP_POSITION_OLD-step;
}
/**
 * Get current step position
 * @return int32_t Step Position
 */
int32_t HFOServo_HFOMotorGetCurrentStepPosition()
{
//	return 	(MAX_HFO_STEP_POSITION_OLD-(MotorController_GetLastStepPosition(eHFOMotor)/MAX_HFO_STEP_POSITION_NEW)*MAX_HFO_STEP_POSITION_OLD);
	return (MAX_HFO_STEP_POSITION_OLD-(gs_LastStepHFOPosition/MAX_HFO_STEP_POSITION_NEW)*MAX_HFO_STEP_POSITION_OLD);
}
/**
 * Get current pressure amplitude
 * @return int32_t Pressure amplitude
 */
static int32_t HFOServo_GetCurrentAmplitude()
{
	int32_t maxValue=gs_PressureDataBuffer[0];
	int32_t minValue=gs_PressureDataBuffer[0];
	for(int i=1;i<gs_DataSize;i++)
	{
		if(gs_PressureDataBuffer[i]>=maxValue)
		{
			maxValue=gs_PressureDataBuffer[i];
		}
		if(gs_PressureDataBuffer[i]<=minValue)
		{
			minValue=gs_PressureDataBuffer[i];
		}
	}
	return maxValue - minValue;
}
/**
 * Initialize pressure buffer
 */
static void HFOServo_InitPressureBuffer()
{
	for(int i=0;i<DATA_BUFFER_SIZE;i++)
	{
		gs_PressureDataBuffer[i]=0;
	}
	gs_Index=0;
	gs_DataSize=0;
}
/**
 * Insert value to pressure buffer
 * @param value
 */
static void HFOServo_InsertValueToPressureBuffer(int32_t value)
{
	if ( gs_DataSize < DATA_BUFFER_SIZE )
		gs_DataSize++;

	gs_PressureDataBuffer[gs_Index] = value;
	gs_Index++;
	gs_Index = gs_Index % DATA_BUFFER_SIZE;
}
/**
 * Do servo
 */
void HFOServo_DoServo()
{
	static int s_PrevFreqSetting = 7;
	static int s_PrevMapSetting = 3;
	static int32_t s_TimeDuringSI = 0;
	static int32_t s_PrevStep = HFO_MAX_STEP;
	static bool s_IsSIReqPrev = false;
	static int s_Delay = 500;


	static int32_t s_ThirtyPercentDelay = 0;

	static unsigned char s_StopDirectAmpCtrl = MAKE_DC_SHIFT;

	static int16_t s_PhaseAngle = (int16_t) INITIAL_ANGLE; // 3Pai /2

	static int16_t s_PrevStrokeVol = 0;
	static bool s_DisconnectFlag = false;

	static int16_t s_OpenLoopShiftAmount = 0;

	/* value to scale the sine to the correct volume */

	int32_t fPresAmpCmd,Temp32,finalStep, Dat_Omic,HFOMposition;
	int16_t phase,stepAmp,DAmp,ZeroOrAmp,SinPos,EHPoint;
	int16_t settingStrokeVol,settingFreq;

	int16_t freq = 5;

	// Just to make sure that we have final step setup.
	finalStep = HFO_START_OFFSET;

	// Check and make sure freq is valid.
	settingFreq =  HFOController_GetCurrentFreq();
	if ( (settingFreq >= MIN_HFO_FREQ) && (settingFreq <= MAX_HFO_FREQ))
		freq = settingFreq;

	if(gs_SIRequest == true)
	{
		settingStrokeVol=0;
		gs_StrokeVol = 0;
		s_IsSIReqPrev = true;
		s_TimeDuringSI = s_TimeDuringSI+2;
	}

	else if(s_IsSIReqPrev)
	{
		s_IsSIReqPrev = false;
		settingStrokeVol=0;
		gs_StrokeVol = 0;
		s_Delay = s_Delay -2;
	}
	else
	{
		s_IsSIReqPrev = false;
		s_Delay = 500;
		s_TimeDuringSI = 0;

		if(gs_IsDoAmpCtr)
			settingStrokeVol = HFOServo_DoPID();
		else
			settingStrokeVol = devIf_GetSetting()->hfo.strokeVolume/10;
	}


	// Slowly update stroke volume setting.
	if(gs_StrokeVol != settingStrokeVol)
	{
		gs_SVUpdateCounter++;

		if(gs_SVUpdateCounter>=50)
		{
			gs_SVUpdateCounter=0;
			if ( settingStrokeVol > gs_StrokeVol)
			{
				gs_StrokeVol += 10;
				if(gs_StrokeVol > settingStrokeVol)
					gs_StrokeVol = settingStrokeVol;
			}
			else if (settingStrokeVol < gs_StrokeVol)
			{
				gs_StrokeVol -= 10;
				if(gs_StrokeVol < settingStrokeVol)
					gs_StrokeVol = settingStrokeVol;
			}
		}
		if(abs(gs_StrokeVol - settingStrokeVol)<10)
		{
			gs_StrokeVol = settingStrokeVol;
			gs_SVUpdateCounter=0;
		}
	}

	if ( gs_StrokeVol >= HFO_STROKE_LIMIT)
		gs_StrokeVol = HFO_STROKE_LIMIT;

	gs_PressAmp = HFOServo_GetCurrentAmplitude()/10;

	// Calculate the phase angle.
	{
		Temp32 = 2*freq;
		{
			int32_t	error;

			Dat_Omic = (gs_DatOmicIntegrator >> MAP_SERVO_SCALE);

			error = Temp32 - Dat_Omic;

			error <<= MAP_SERVO_SCALE;

			gs_DatOmicIntegrator += error/50; // 500 / 10;

		}
		s_PhaseAngle += Dat_Omic;


		if(s_PhaseAngle >= (int16_t)LENGTH_SINTBL)
			s_PhaseAngle -= (int16_t)LENGTH_SINTBL;
	}

	phase = sin_table[s_PhaseAngle];

	// Read the final step from the table, remember stroke have been scaled by 10
	// to include the decimal portion for the setting less than 10.
	if ( gs_StrokeVol < 200)
	{
		if(gs_IsHummingVOn == true)
		{
			finalStep += steps_for_big_ml[gs_StrokeVol/10][freq - MIN_FREQ] + gs_StrokeVol%10 + steps_offset_for_HummingV[gs_StrokeVol/10][freq - MIN_FREQ];
		}
		else
		{
			finalStep += steps_for_big_ml[gs_StrokeVol/10][freq - MIN_FREQ] + gs_StrokeVol%10;
		}
	}
	else
	{
		if(gs_IsHummingVOn == true)
		{
			finalStep += steps_for_big_ml[gs_StrokeVol/10][freq - MIN_FREQ] + steps_offset_for_HummingV[gs_StrokeVol/10][freq - MIN_FREQ];;
		}
		else
		{
			// 81 is he first large stroke volume.
			finalStep += steps_for_big_ml[gs_StrokeVol/10][freq - MIN_FREQ];
		}
	}

	// Step amplitude calculate from the origin position.
	stepAmp = finalStep - (int32_t)H_POINT;

	// Change of stroke volume will start the direct amplitude control
	// servo.
	int32_t currentMapSetting = devIf_GetSetting()->hfo.map;


	if (( s_PrevStrokeVol != settingStrokeVol)||(s_PrevFreqSetting != freq)
			||(s_PrevMapSetting != currentMapSetting))
	{
		s_PrevMapSetting = currentMapSetting;
		s_PrevStrokeVol = settingStrokeVol;
		s_PrevFreqSetting = freq;
		HFOServo_SetParameterForMotorAtOriginalPos();
	}


	// Step amplitude calculate from the origin position.
	Temp32 = stepAmp;


	// Filter of stroke volume block, result is compute 2*Amp = DAmp
	{
		int32_t	error;

		DAmp = (gs_DAmpIntegrator >> MAP_SERVO_SCALE);

		error = Temp32 - DAmp;

		error <<= MAP_SERVO_SCALE;

		gs_DAmpIntegrator += error/100; // 500 / 10;

	}

	Temp32 = DAmp;

	/////////////////////////////////////////////////////
	{
		int16_t	output1;
		int32_t	error;
		output1 = (gs_ZeroOrAmpIntegrator1 >> MAP_SERVO_SCALE);
		error = Temp32 - output1;
		error <<= MAP_SERVO_SCALE;
		gs_ZeroOrAmpIntegrator1 += (error*6)/1000; // 500 / 10;
		ZeroOrAmp = (gs_ZeroOrAmpIntegrator2 >> MAP_SERVO_SCALE);
		error = output1 - ZeroOrAmp;
		error <<= MAP_SERVO_SCALE;
		gs_ZeroOrAmpIntegrator2 += (error/100); // 500 / 10;
	}
	// This is the case where stroke volume set to less than 10, but
	if ( (gs_StrokeVol < MIN_STROKE_FOR_OPEN_LOOP) || (gs_StrokeVol > MAX_STROKE_FOR_OPEN_LOOP) )
	{

		gs_ShiftingOffset = s_OpenLoopShiftAmount;

		gs_TimeToGetTargetPress = 0;
		gs_DASCounter = 0;

		gs_SumUnion.Long32 = (int32_t)ZeroOrAmp*phase;
		SinPos  =  gs_SumUnion.access16.High16;
		SinPos += (ZeroOrAmp>>1);
	}
	else
	{
		//PresAmp_Cmd is pressure amplitude measured by previous open oscillation
		//Pres_Amp is real amplitude of pressure which is received from main CPU every 100 ms
		// Amp_ByDAC is Motor amplitude generated by direct amplitude control servo


		static bool s_PrintMsgOpenLoopFlag = true;
		static bool s_PrintMsgDCtrlFlag = false;
		int32_t	scaleUpInput,error;
		int32_t FAmpByDAC,TempAmp,Temp_int32;


		// Assume it is still making direct amplitude control.
		// The next couple of block will determine if it actually does.
		// If it is not the it will clear.
		s_StopDirectAmpCtrl = MAKE_DC_SHIFT;
		// 300 % rule, and 30 % rule. If this happens, then go to open loop.
		if ( gs_FPressAmp >= (3*gs_PressAmpCmd) )
		{
			HFOServo_SetParameterForMotorAtOriginalPos();
		}

		// If SI then go to open loop.
		if ( gs_SIRequest )
		{
			s_StopDirectAmpCtrl = SIGN_RES;
			if ( gs_DASCounter < FIVE_MINUTES)
			{
				HFOServo_SetParameterForMotorAtOriginalPos();
			}
			else
			{
				gs_DASCounter = FIVE_MPlS10S;
			}
		}
		// 30 % rule could trigger because of SI
		if( gs_FPressAmp <= ((gs_PressAmpCmd*3)/10)  )
		{

			if ( s_ThirtyPercentDelay >= 500 )
			{
				s_StopDirectAmpCtrl = PRESS_AMP_50;
				// Notice use "less than", NOT "less than or equal"
				if ( gs_TimeToGetTargetPress < THIRTY_MINUTES )
				{
					// Notice the shifting_offset did not reset to zero.
					HFOServo_SetParameterForMotorAtOriginalPos();
				}
				else
				{

					if ( s_DisconnectFlag == false)
					{
						s_DisconnectFlag = true;
						gs_DASCounter = FIVE_MPlS10S;
					}

				}
			}
			else
			{
				s_ThirtyPercentDelay++;
			}
		}
		else
		{
			s_ThirtyPercentDelay = 0;

			if ( s_DisconnectFlag == true )
			{
				gs_DASCounter = FIVE_MINUTES;
				s_DisconnectFlag = false;
			}
		}



		if ( gs_TimeToGetTargetPress < THIRTY_MINUTES)
		{
			gs_TimeToGetTargetPress++;
		}



		fPresAmpCmd = (gs_IntegratorDAC1 >> MAP_SERVO_SCALE);

		if(gs_DASCounter>=FIVE_MINUTES)
			error = 0;
		else
			error = gs_PressAmpCmd - fPresAmpCmd;

		error <<= MAP_SERVO_SCALE;

		gs_IntegratorDAC1 += error/500; // 500 / 1;

		if ( s_StopDirectAmpCtrl != MAKE_DC_SHIFT)
			gs_AmpByDAC = gs_PrevAmpByDAC;
		else
			gs_AmpByDAC = (gs_IntegratorDAC3 >> MAP_SERVO_SCALE);

		FAmpByDAC = (gs_IntegratorDAC2 >> MAP_SERVO_SCALE);

		error = gs_AmpByDAC - FAmpByDAC;

		error <<= MAP_SERVO_SCALE;

		gs_IntegratorDAC2 += error/50; // 500 / 10;
		//..................................................................

		gs_FPressAmp = (gs_IntegratorDAC4 >> MAP_SERVO_SCALE);

		error = gs_PressAmp - gs_FPressAmp;

		error <<= MAP_SERVO_SCALE;

		gs_IntegratorDAC4 += error/100; // 500 / 5;

		/* Fpres_Amp : Filtered Pres_Amp												*/
		if(s_StopDirectAmpCtrl == MAKE_DC_SHIFT)
			error = fPresAmpCmd - gs_PressAmp;
		else
			error = 0;

		error = error<<1;

		scaleUpInput = (error << MAP_SERVO_SCALE);


		if((gs_DASCounter < FIVE_MPlS10S)&&(gs_DASCounter>=FIVE_MINUTES))
		{
			s_PrintMsgOpenLoopFlag = true;
			if ( s_PrintMsgDCtrlFlag )
			{
				s_PrintMsgDCtrlFlag = false;
			}
			// Direct amplitude control
			if(FAmpByDAC>=gs_UpperAmpLimit)
			{
				if(scaleUpInput<0)
					gs_IntegratorDAC3 += (scaleUpInput*3)/(2*5000);//
				gs_AmpByDAC = gs_UpperAmpLimit;
			}
			else if(FAmpByDAC<=gs_LowerAmpLimit)
			{
				if(scaleUpInput>0)
					gs_IntegratorDAC3 += (scaleUpInput*3)/(2*5000);
				gs_AmpByDAC = gs_LowerAmpLimit;
			}
			else
				gs_IntegratorDAC3 += (scaleUpInput*3)/(2*5000);

			gs_SumUnion.Long32 = (int32_t)gs_AmpByDAC*phase;
			SinPos  =  gs_SumUnion.access16.High16;
			SinPos += (gs_AmpByDAC>>1);

		}
		else
		{
			// Open loop turn
			s_PrintMsgDCtrlFlag = true;
			if ( s_PrintMsgOpenLoopFlag )
			{
				s_PrintMsgOpenLoopFlag = false;
			}

			// Do the target making
			if(gs_DASCounter<FIVE_MINUTES)
			{
				TempAmp = ZeroOrAmp;
				gs_PressAmpCmd = gs_PressAmp;
				gs_IntegratorDAC3 = (TempAmp << MAP_SERVO_SCALE);
			}
			else
			{
				TempAmp = gs_AmpByDAC;
			}

			// Setup limit for next FBC. 90, 110 % rule.
			gs_UpperAmpLimit = (11*TempAmp)/10;
			gs_LowerAmpLimit = (9*TempAmp)/10;



			/*200 % rule. Limit from exceed it 200 % of step amplitude from origin position.
                                    ZeroOrAmp is initial step amplitude */
			Temp_int32 = (stepAmp*20)/10;
			if( gs_UpperAmpLimit > Temp_int32)
				gs_UpperAmpLimit = Temp_int32;

			// 80 % rule. Khoi add this per Mr Kayahara request.
			Temp_int32 = (stepAmp*8)/10;
			if( gs_LowerAmpLimit < Temp_int32)
				gs_LowerAmpLimit = Temp_int32;

			gs_OpenMtrAmp = TempAmp;

			gs_SumUnion.Long32 = (int32_t)TempAmp*phase;
			SinPos  =  gs_SumUnion.access16.High16;
			SinPos += (TempAmp>>1);
		}

		/* ABove two sentences could be deleted if DC should be used
		 * for SV from 0 ~ 80												  */

		if(s_StopDirectAmpCtrl == MAKE_DC_SHIFT)
			gs_DASCounter++;

		// Reset counter to 5:00, and do the shifting.
		if(gs_DASCounter>TEN_MINUTES)
		{
			gs_DASCounter=FIVE_MINUTES;

			gs_PrevAmpByDAC = gs_AmpByDAC;

			gs_ShiftingOffset += gs_ShiftInc;

			if((gs_ShiftingOffset+H_POINT+gs_OpenMtrAmp)>=1800)
			{
				gs_ShiftInc = -SHIFT_INCREMENT;
			}
			if(gs_ShiftingOffset<0)
			{
				gs_ShiftInc = SHIFT_INCREMENT;
				gs_ShiftingOffset = 0;
			}
		}
	}

	{
		Temp32 = (int32_t)H_POINT;
		// if making center shifting, add the following lines to Temp32 += shfiting_offset.
		Temp32 += gs_ShiftingOffset;
	}
	/////////////////////////////////////////////////////////
	{
		int16_t	output1;
		int32_t	error;

		output1 = (gs_EHPointIntegrator1 >> MAP_SERVO_SCALE);

		error = Temp32 - output1;

		error <<= MAP_SERVO_SCALE;

		gs_EHPointIntegrator1 += (error)/50;			//15)/500;

		EHPoint = (gs_EHPointIntegrator2 >> MAP_SERVO_SCALE);

		error = output1 - EHPoint;

		error <<= MAP_SERVO_SCALE;

		gs_EHPointIntegrator2 += (error/500);			//12/500);
	}
	HFOMposition = EHPoint;
	HFOMposition += (int32_t)SinPos;


	if( HFOMposition < 0 )
		HFOMposition = 0;

	if(HFOMposition < s_PrevStep)
	{
		s_PrevStep = HFOMposition;
	}

	static bool s_IsSendToMapCtn = false;
	if(s_IsSIReqPrev)
	{
		HFOServo_HFOMotorMoveToStepPosition(H_POINT);
		gs_Sum +=HFOMposition;
		{

			if(HFOServo_HFOMotorGetCurrentStepPosition()<=H_POINT)
			{
				if(!s_IsSendToMapCtn)
				{
					HFOController_SetSIRequest(gs_SIRequest);
					s_IsSendToMapCtn = true;
				}
				HFOServo_SetStatic();
			}
			gs_Sum = 0;
		}
	}
	else
	{
		gs_Sum = 0;
		HFOServo_HFOMotorMoveToStepPosition(HFOMposition);
		s_PrevStep = HFO_MAX_STEP;
		s_IsSendToMapCtn = false;
	}
}
/**
 * Initialize all static variables to 0
 */
void HFOServo_Init()
{
	gs_Enabled = false;
	gs_CurrentAmp=0;
	gs_IsDoAmpCtr=false;
	gs_T=0.0;
	gs_t=0.0;
	gs_ShiftingOffset=0;
	gs_DASCounter=0;
	gs_TimeToGetTargetPress=0;
	gs_PressAmpCmd=0;
	gs_PressAmp=0;
	gs_FPressAmp=0;
	gs_AmpByDAC=0;
	gs_PrevAmpByDAC=0;
	gs_OpenMtrAmp=0;
	gs_IntegratorDAC1=0;
	gs_IntegratorDAC2=0;
	gs_IntegratorDAC3=0;
	gs_IntegratorDAC4=0;
	gs_UpperAmpLimit=0;
	gs_LowerAmpLimit=0;
	gs_SVUpdateCounter=0;
	gs_ShiftInc = SHIFT_INCREMENT;
	gs_DatOmicIntegrator = 0;
	gs_DAmpIntegrator = 0;
	gs_ZeroOrAmpIntegrator1=0;
	gs_ZeroOrAmpIntegrator2=0;
	gs_EHPointIntegrator1=0;
	gs_EHPointIntegrator2=0;
	gs_Sum = 0;
	gs_SIRequest=false;
	gs_AmpIntegral=0;
	gs_AmpDerivative=0;
	gs_StrokeVol=0;
	gs_PrevAmpError=0;
	int32_t valueInFile = true;
	if(valueInFile == true)
	{
		gs_IsHummingVOn = true;
	}
	else
	{
		gs_IsHummingVOn = false;
	}
	HFOServo_InitPressureBuffer();
}
/**
 * Handle process of HFO servo
 * @param monitor
 * @param HFOMotorStep
 * @return desiredMotorPositionStruct gs_DesiredMotorPosition Desired motor position
 */
desiredMotorPositionStruct HFOServo_Run(RealTimeM4ToA53* monitor,int32_t HFOMotorStep)
{
	gs_DesiredMotorPosition.desiredAirPosition=-1;
	gs_DesiredMotorPosition.desiredExhPosition=-1;
	gs_DesiredMotorPosition.desiredHFOPosition=-1;
	gs_DesiredMotorPosition.desiredO2Position=-1;
	gs_LastStepHFOPosition = HFOMotorStep;

	HFOServo_HFOMotorMoveToStepPosition(H_POINT);
	if(gs_Enabled==false)
	{
		gs_SIRequest = false;
		return gs_DesiredMotorPosition;
	}
	HFOServo_InsertValueToPressureBuffer(monitor->inhPress.lastReading);
	HFOServo_DoServo();
	return gs_DesiredMotorPosition;
}
/**
 * Clear static variable
 */
void HFOServo_ClearStaticVariable()
{
	gs_DatOmicIntegrator =0;
	gs_DAmpIntegrator =0;
	gs_ZeroOrAmpIntegrator1 = 0;
	gs_ZeroOrAmpIntegrator2 = 0;
	gs_EHPointIntegrator1 = 0;
	gs_EHPointIntegrator2 = 0;
	gs_StrokeVol = 0;
	gs_SVUpdateCounter = 0;
	gs_ShiftingOffset =0;
	gs_DASCounter = 0;
	gs_TimeToGetTargetPress = 0;
	gs_PressAmpCmd = 0;
	gs_PressAmp = 0;
	gs_FPressAmp = 0;
	gs_AmpByDAC = 0;
	gs_PrevAmpByDAC = 0;
	gs_OpenMtrAmp = 0;
	gs_IntegratorDAC1 = 0;
	gs_IntegratorDAC2 = 0;
	gs_IntegratorDAC3 = 0;
	gs_IntegratorDAC4 = 0;
	gs_UpperAmpLimit = 0;
	gs_LowerAmpLimit = 0;
}
/**
 * Enable HFO servo
 */
void HFOServo_Enable()
{
	gs_Enabled = true;
}
/**
 * Disable HFO servo
 */
void HFOServo_Disable()
{
	gs_Enabled = false;
}
/**
 * Check whether HFO servo is enable
 * @return bool gs_Enabled Return True if HFO servo is enable,
 * 						   otherwise, return False
 */
bool HFOServo_IsEnable()
{
	return gs_Enabled;
}
/**
 * set SI request
 * @param SIRequest
 */
void HFOServo_SetSIRequest(bool SIRequest)
{
	gs_SIRequest = SIRequest;
}
/**
 * Reset PID static
 */
void HFOServo_ResetPIDStatic()
{
	gs_AmpIntegral = 0;
	gs_AmpDerivative = 0;
	gs_CurrentAmp=0;
}
/**
 * Request do amplitude control
 */
void HFOServo_RequestDoAmpCtr()
{
	gs_CurrentAmp = HFOServo_GetCurrentAmplitude()/SCALE;
	int32_t settingAmp = devIf_GetSetting()->hfo.amplitude/SCALE;
	int32_t errorAmp = settingAmp - gs_CurrentAmp;
	gs_AmpDerivative = errorAmp;
	int32_t freqSetting = devIf_GetSetting()->hfo.frequency/SCALE;
	gs_AmpIntegral = (gs_StrokeVol - (Kp+Kd)*errorAmp)/(Ki+(STANDARD_SETTING - freqSetting)*AJUSTMENT_VALUE); // adjustment ki parameter to each Hz based on 15Hz
	gs_IsDoAmpCtr = true;
	if(gs_SIRequest)
		HFOServo_ResetPIDStatic();
}
/**
 * Request do Stroke volume control
 */
void HFOServo_RequestDoSVCtr()
{
	gs_IsDoAmpCtr=false;
}
/**
 * Stop do amplitude control
 */
void HFOServo_StopDoAmpCtr()
{
	Command sendMsg;
	sendMsg.id = eUpdateSettingsHfoServo;
	sendMsg.data.strokeVol = gs_StrokeVol*10;
	ipcIf_sendCommandMsg(&sendMsg);
	gs_IsDoAmpCtr = false;
}
/**
 * Set Humming V on
 * @param value
 */
void HFOServo_SetHummingVON(bool value)
{
	gs_IsHummingVOn = value;
	//	 MainHandlerDelegate::S_GetInstance()->postCustomEvent<E_Bool>(eHummingVEvent, value);
	//
	//	    FileTaskEvent fEvent;
	//	    fEvent.infoID = SETTING_IDX(eHummingVSettingID);
	//	    fEvent.info = QString::number((LONG)value);
	//	    LogFileHandler::S_GetInstance()->postCustomEvent<FileTaskEvent>(LogFileHandler::eLogHandlerVentSettingFileType, fEvent);
}
/**
 * Check whther Humming V is On
 * @return bool gs_IsHummingVOn Return True if Humming V is on,
 * 								otherwise return False
 */
bool HFOServo_GetHummingVON()
{
	return gs_IsHummingVOn;
}
/**
 * Set parameter for motor at original position
 */
void HFOServo_SetParameterForMotorAtOriginalPos()
{
	gs_ShiftingOffset=0;
	gs_DASCounter = 0;
	gs_TimeToGetTargetPress=0;
	gs_ShiftInc =SHIFT_INCREMENT;
}
/**
 * Set static variables
 */
void HFOServo_SetStatic()
{
	gs_DAmpIntegrator = 255;
	gs_ZeroOrAmpIntegrator1 = 255;
	gs_ZeroOrAmpIntegrator2 = 254;
	gs_EHPointIntegrator1 = 99588;
	gs_EHPointIntegrator2 = 99328;
	gs_DASCounter = 0;
	gs_TimeToGetTargetPress = 0;
	gs_UpperAmpLimit = 18;
	gs_LowerAmpLimit = 15;
	gs_PressAmpCmd = 1;
	gs_PressAmp = 1;
	gs_FPressAmp = 2;
	gs_AmpByDAC = 9;
	gs_PrevAmpByDAC = 0;
	gs_OpenMtrAmp = 18;
	gs_IntegratorDAC1=22509;
	gs_IntegratorDAC2=4950;
	gs_IntegratorDAC3=4608;
	gs_IntegratorDAC4=16104;

}
/**
 * Do PID
 * @return int16_t strokeVol
 */
int16_t HFOServo_DoPID()
{
	int16_t strokeVol;
	gs_CurrentAmp = HFOServo_GetCurrentAmplitude()/SCALE;
	int32_t settingAmp = devIf_GetSetting()->hfo.amplitude/SCALE;
	if(settingAmp == 0)
		return 0;
	int32_t errorAmp = settingAmp - gs_CurrentAmp;
	gs_AmpIntegral = gs_AmpIntegral + errorAmp*2; // integral = integral + errorAmp*dt
	gs_AmpDerivative = (errorAmp - gs_PrevAmpError)/2;
	int32_t freqSetting = devIf_GetSetting()->hfo.frequency/SCALE;
	strokeVol = Kp*errorAmp + (Ki+(STANDARD_SETTING - freqSetting)*AJUSTMENT_VALUE)*gs_AmpIntegral + Kd*gs_AmpDerivative;// adjustment ki parameter to each Hz based on 15Hz
	gs_PrevAmpError = errorAmp;
	int index = freqSetting - 5;
	int32_t Max_SV = MAX_STROKE_VOLUME[index];
	if(strokeVol > Max_SV*10)
	{
		strokeVol = Max_SV*10;
	}
	if(strokeVol<0)
		strokeVol = 0;
	return strokeVol;
}

