
static const char *gpio_names[] = 
{
 //               Pin Power  gpioindex
    "PAD_SR_GPIO00", // 25	VDDP_1 GPIO0	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_SR_GPIO01", // 26	VDDP_1 GPIO1	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_SR_GPIO02", // 27	VDDP_1 GPIO2	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_SR_GPIO03", // 28	VDDP_1 GPIO3	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_SR_GPIO04", // 29	VDDP_1 GPIO4	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_SR_GPIO05", // 30	VDDP_1 GPIO5	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_SR_GPIO06", // 31	VDDP_1 GPIO6	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_SR_GPIO07", // 32	VDDP_1 GPIO7	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_SR_GPIO08", // 33	VDDP_1 GPIO8	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_SR_GPIO09", // 35	VDDP_1 GPIO9	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_SR_GPIO10", // 36	VDDP_1 GPIO10	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_SR_GPIO11", // 37	VDDP_1 GPIO11	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_SR_GPIO12", // 38	VDDP_1 GPIO12	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_SR_GPIO13", // 39	VDDP_1 GPIO13	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_SR_GPIO14", // 41	VDDP_1 GPIO14	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_SR_GPIO15", // 42	VDDP_1 GPIO15	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_SR_GPIO16", // 24	VDDP_1 GPIO16	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL0", // 51	VDDP_2 GPIO17	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL1", // 52	VDDP_2 GPIO18	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL2", // 53	VDDP_2 GPIO19	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL3", // 54	VDDP_2 GPIO20	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL4", // 55	VDDP_2 GPIO21	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL5", // 56	VDDP_2 GPIO22	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL6", // 57	VDDP_2 GPIO23	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL7", // 58	VDDP_2 GPIO24	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL8", // 59	VDDP_2 GPIO25	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL9", // 60	VDDP_2 GPIO26	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL10", // 61	VDDP_2 GPIO27	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL11", // 63	VDDP_2 GPIO28	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL12", // 65	VDDP_2 GPIO29	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL13", // 66	VDDP_2 GPIO30	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL14", // 67	VDDP_2 GPIO31	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL15", // 68	VDDP_2 GPIO32	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL16", // 69	VDDP_2 GPIO33	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL17", // 70	VDDP_2 GPIO34	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL18", // 71	VDDP_2 GPIO35	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL19", // 72	VDDP_2 GPIO36	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_TTL20", // 73	VDDP_2 GPIO37	input", pull-low	Yes	", 	NO	Rpd", 103K ohm
    "PAD_KEY0", // 75	VDDP_3 GPIO39	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_KEY1", // 76	VDDP_3 GPIO40	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_KEY2", // 77	VDDP_3 GPIO41	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_KEY3", // 78	VDDP_3 GPIO42	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_KEY4", // 79	VDDP_3 GPIO43	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_KEY5", // 80	VDDP_3 GPIO44	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_KEY6", // 81	VDDP_3 GPIO45	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_KEY7", // 82	VDDP_3 GPIO46	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_KEY8", // 83	VDDP_3 GPIO47	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_KEY9", // 84	VDDP_3 GPIO48	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_KEY10", // 85	VDDP_3 GPIO49	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_KEY11", // 86	VDDP_3 GPIO50	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_KEY12", // 87	VDDP_3 GPIO51	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_KEY13", // 88	VDDP_3 GPIO52	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_SD_D1", // 91	VDDP_4 GPIO53	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_SD_D0", // 92	VDDP_4 GPIO54	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_SD_CLK", // 93	VDDP_4 GPIO55	input", pull-low	Yes	", 	YES	Rpu", 90K ohm
    "PAD_SD_CMD", // 94	VDDP_4 GPIO56	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_SD_D3", // 95	VDDP_4 GPIO57	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_SD_D2", // 96	VDDP_4 GPIO58	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_SD_GPIO0",// 98	VDDP_4 GPIO59	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_SD_GPIO1",// 99	VDDP_4 GPIO60	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_GPIO0", // 109	AVDD_XTAL	GPIO61	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_GPIO1", // 110	AVDD_XTAL	GPIO62	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_GPIO2", // 111	AVDD_XTAL	GPIO63	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_GPIO3", // 112	AVDD_XTAL	GPIO64	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_GPIO4", // 113	AVDD_XTAL	GPIO65	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_GPIO5", // 114	AVDD_XTAL	GPIO66	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_GPIO6", // 115	AVDD_XTAL	GPIO67	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_GPIO7", // 116	AVDD_XTAL	GPIO68	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_GPIO8", // 117	AVDD_XTAL	GPIO69	input", pull-up	Yes	", 	YES	Rpu", 90K ohm
    "PAD_PM_SD_CDZ",// 90	VDDP_4 GPIO71	input", pull-up	Yes	",	YES	Rpu", 90K ohm
    "PAD_PM_LED0", // 119	AVDD_XTAL	GPIO78	input", pull-down	Yes	", 	YES	Rpd", 103K ohm
    "PAD_PM_LED1" // 120	AVDD_XTAL	GPIO79	input", pull-down	Yes	", 	YES	Rpd", 103K ohm
};

