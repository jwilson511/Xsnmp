/*
 * Note: this file originally auto-generated by mib2c using
 *  : mib2c.iterate.conf 15999 2007-03-25 22:32:02Z dts12 $
 */
#ifndef XSANVOLUMETABLE_H
#define XSANVOLUMETABLE_H

/* function declarations */
void init_xsanVolumeTable(void);
void initialize_table_xsanVolumeTable(void);
Netsnmp_Node_Handler xsanVolumeTable_handler;
Netsnmp_First_Data_Point  xsanVolumeTable_get_first_data_point;
Netsnmp_Next_Data_Point   xsanVolumeTable_get_next_data_point;
u_long blockSizeForVolumeIndex (long volumeIndex);

/* column number definitions for table xsanVolumeTable */
       #define COLUMN_XSANVOLUMEINDEX		1
       #define COLUMN_XSANVOLUMENAME		2
       #define COLUMN_XSANVOLUMEFSSINDEX		3
       #define COLUMN_XSANVOLUMESTATE		4
       #define COLUMN_XSANVOLUMESTATEMESSAGE		5
       #define COLUMN_XSANVOLUMELASTADMINACTION		6
       #define COLUMN_XSANVOLUMELASTTERMINATION		7
       #define COLUMN_XSANVOLUMELAUNCHCOUNT		8
       #define COLUMN_XSANVOLUMECOREDUMPCOUNT		9
       #define COLUMN_XSANVOLUMEFLAGS		10
       #define COLUMN_XSANVOLUMECREATED		11
       #define COLUMN_XSANVOLUMEACTIVECONNECTIONS		12
       #define COLUMN_XSANVOLUMEFSBLOCKSIZE		13
       #define COLUMN_XSANVOLUMEMSGBUFFERSIZE		14
       #define COLUMN_XSANVOLUMEDISKDEVICES		15
       #define COLUMN_XSANVOLUMESTRIPEGROUPS		16
       #define COLUMN_XSANVOLUMETOTALKBLOCKS		17
       #define COLUMN_XSANVOLUMEFREEKBLOCKS		18
       #define COLUMN_XSANVOLUMETOTALBLOCKS		19
       #define COLUMN_XSANVOLUMEFREEBLOCKS		20
       #define COLUMN_XSANVOLUMEUTILIZATION		21
       #define COLUMN_XSANVOLUMEPID		22
       #define COLUMN_XSANVOLUMEPORT		23
       #define COLUMN_XSANVOLUMETOTALMBYTES		24
       #define COLUMN_XSANVOLUMEFREEMBYTES		25
       #define COLUMN_XSANVOLUMEUSEDMBYTES		26
       #define COLUMN_XSANVOLUMECONTROLLERADDRESS		27
       #define COLUMN_XSANVOLUMECONTROLLERINDEX 28
#endif /* XSANVOLUMETABLE_H */
