#pragma once
#include "../../Platform/Windows/WindowsPlatform.h"
#include "../../CodeReflection/CodeReflectionMacroTag.h"
class CCoreMinimalObject
{
public:
	CCoreMinimalObject();
	virtual ~CCoreMinimalObject();

	template<class T>
	T* CreateObject(CCoreMinimalObject* NewObject)
	{
		return dynamic_cast<T*>(NewObject);
	}
};