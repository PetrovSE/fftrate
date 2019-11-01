#ifndef _MATHEXLIB_H_
#define _MATHEXLIB_H_


//================================================================
//  Type definitions
//----------------------------------------------------------------
typedef struct
{
	DATA	diff;
	DATA	gain;

} MSEERR_INST, *PMSEERR_INST;


typedef struct
{
	COMBOTYPE	vMin;
	COMBOTYPE	vMax;

} MSERANGE_INST, *PMSERANGE_INST;


typedef struct
{
	COMBOTYPE	vFirst;
	COMBOTYPE	vMax;

	DWORD	first_pos;
	DWORD	max_pos;
	DWORD	total_pos;

} MSEDIFF_INST, *PMSEDIFF_INST;


typedef struct
{
	DWORD	flag;
	DWORD	count;

	MSEERR_INST		err_inst;
	MSERANGE_INST	range_inst;
	MSEDIFF_INST	diff_inst;

} MSE_INST, *PMSE_INST;

#endif // _MATHEXLIB_H_
