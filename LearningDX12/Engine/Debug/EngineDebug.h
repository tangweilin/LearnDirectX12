#pragma once

#define ANALYSIS_HRESULT(InValue) \
{\
	HRESULT HandleResult = InValue;\
	if (FAILED(HandleResult))\
	{\
		Engine_Log_Error("Error = %i",(int)InValue);\
		assert(0);\
	}\
}