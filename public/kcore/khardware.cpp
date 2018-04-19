#include "stdafx.h"
#include <setupapi.h>
#include <uuids.h>
#include <Wbemidl.h>
#include <algorithm>
#include <devguid.h>
#include <cfgmgr32.h>

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "wbemuuid.lib")

START_NAMESPACE_KCORE

void trimstring(tstring& source)
{
	if (source.length() == 0)
		return;

	//for debug.
	//src = TEXT("   d ddddd d dd  ddx ");
	size_t offset = source.find_first_not_of(TEXT(' '));
	if (offset != source.npos && offset != 0)
		source.erase(0, offset);
	offset = source.find_last_not_of(TEXT(' '));
	if (offset != source.npos && (offset + 1) < source.size())
		source.erase(offset+1);

	std::remove(source.begin(), source.end(), TEXT('\''));
	std::remove(source.begin(), source.end(), TEXT('\"'));
	std::remove(source.begin(), source.end(), TEXT('`'));
}

LPCTSTR __stdcall AfxGetFirstDevName(const GUID& devclassguid, LPTSTR szDevName, DWORD dwSize,LPTSTR szDevId, DWORD dwIdSize)
{
	DWORD DeviceIndex = 0;
	SP_DEVINFO_DATA DevData = {sizeof(SP_DEVINFO_DATA)};
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&devclassguid, NULL, NULL, DIGCF_PRESENT);
	while (SetupDiEnumDeviceInfo(hDevInfo, DeviceIndex, &DevData))
	{
		TCHAR szBufferId[MAX_PATH] = {0};
		SetupDiGetDeviceRegistryProperty(hDevInfo,&DevData,SPDRP_HARDWAREID,NULL,(PBYTE)szBufferId, MAX_PATH*sizeof(TCHAR), NULL);
		if(szDevId != NULL && dwIdSize > 0)
			_tcscpy_s(szDevId,dwIdSize,szBufferId);

		if (!SetupDiGetDeviceRegistryProperty(hDevInfo, &DevData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)szDevName, dwSize*sizeof(TCHAR), NULL) || szDevName[0] == 0)
		{
			SetupDiGetDeviceRegistryProperty(hDevInfo, &DevData, SPDRP_DEVICEDESC, 0, (PBYTE)szDevName, dwSize*sizeof(TCHAR), NULL);
		}
		if (szDevName[0] != 0)
		{
			if(IsEqualIID(devclassguid,GUID_DEVCLASS_DISKDRIVE))
			{
				//ÅÐ¶ÏÊÇ·ñ¿ÉÉ¾³ý´ÅÅÌ(UÅÌ)
				DWORD dwRemovable = 0;
				if (SetupDiGetDeviceRegistryProperty(hDevInfo, &DevData, SPDRP_REMOVAL_POLICY, NULL, (PBYTE)&dwRemovable, sizeof(dwRemovable), NULL))
				{
					if (dwRemovable == CM_REMOVAL_POLICY_EXPECT_SURPRISE_REMOVAL ||
						dwRemovable == CM_REMOVAL_POLICY_EXPECT_ORDERLY_REMOVAL)
					{
						++DeviceIndex;
						continue;
					}
				}

				//ÅÐ¶ÏÊÇ·ñÔÆ¸üÐÂÐéÄâÅÌ
				tstring sDevName = szDevName;
				if (sDevName.find(_T("CGM Virtual")) == tstring::npos)
				{
					trimstring(sDevName);
					lstrcpy(szDevName, sDevName.c_str());
					SetupDiDestroyDeviceInfoList(hDevInfo);
					break;
				}
				else
				{
					
					memset(szDevName,0,dwSize);
					if(szDevId != NULL && dwIdSize > 0)
						memset(szDevId,0,dwIdSize);
				}
			}
			else
			{
				tstring sDevName = szDevName;
				trimstring(sDevName);
				lstrcpy(szDevName, sDevName.c_str());
				SetupDiDestroyDeviceInfoList(hDevInfo);
				break;
			}
		}
		++DeviceIndex;
	}

	if (hDevInfo)
		SetupDiDestroyDeviceInfoList(hDevInfo);
	return szDevName;
}

LPCTSTR __stdcall AfxGetDevNameByWmi(LPCTSTR szWql, LPCTSTR szFiled, LPTSTR szDevName, DWORD dwSize)
{
	IWbemLocator *pLoc = NULL;
	IWbemServices *pSvc = NULL;
	IEnumWbemClassObject* pEnumerator = NULL;
	_bstr_t result;
	try
	{
		HRESULT hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
		if (FAILED(hres) && hres != RPC_E_TOO_LATE)
			throw 0;
		hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc);
		if (FAILED(hres))
			throw 0;
		hres = pLoc->ConnectServer(_bstr_t(L"root\\cimv2"), NULL, NULL, NULL, 0, NULL, NULL, &pSvc);
		if (FAILED(hres))
			throw 0;
		hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
		if (FAILED(hres))
			throw 0;
		hres = pSvc->ExecQuery(bstr_t("WQL"), bstr_t(szWql), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
		if (FAILED(hres))
			throw 0;

		IWbemClassObject *pclsObj = NULL;
		ULONG uReturn = 0;
		while (pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn) == S_OK)
		{			
			_variant_t vtProp;
			if (S_OK == pclsObj->Get(_bstr_t(szFiled), 0, &vtProp, 0, 0))
			{
				if (vtProp.vt == VT_BSTR)
				{
					_bstr_t str = vtProp.bstrVal;
					if (result.length())
						result += TEXT("|");
					result += str;
				}
			}
			pclsObj->Release();
		}
	}
	catch (...) {}
	Delete_Interface(pEnumerator);
	Delete_Interface(pSvc);
	Delete_Interface(pLoc);
	lstrcpyn(szDevName, result, dwSize);
	return szDevName;
}

END_NAMESPACE_KCORE