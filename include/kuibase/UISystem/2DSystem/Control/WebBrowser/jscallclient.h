#ifndef __JSCALL_H__
#define __JSCALL_H__

#include "./UISystem/2DSystem/Control/WebBrowser/WebBrowser.h"
#include <atlstr.h>

void ygx_win_close();
void ygx_user_login(CString id,CString name,CString url,CString fcm);
void ygx_user_logout(CString id);
void ygx_set_winAttr(CString title,int with,int height);
void ygx_open_win(CString url,CString title,int with,int height);
void ygx_pop_to_menu(CString url);

class ClientCall:public IDispatch
{
	long _refNum;
public:
	ClientCall()
	{
		_refNum = 1;
	}
	virtual ~ClientCall(void)
	{
		_refNum = 0;
	}
public:

	// IUnknown Methods

	STDMETHODIMP QueryInterface(REFIID iid,void**ppvObject)
	{
		*ppvObject = NULL;
		if (iid == IID_IUnknown)	*ppvObject = this;
		else if (iid == IID_IDispatch)	*ppvObject = (IDispatch*)this;
		if(*ppvObject)
		{
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	STDMETHODIMP_(ULONG) AddRef()
	{
		return ::InterlockedIncrement(&_refNum);
	}

	STDMETHODIMP_(ULONG) Release()
	{
		::InterlockedDecrement(&_refNum);
		if(_refNum == 0)
		{
			delete this;
		}
		return _refNum;
	}

	// IDispatch Methods

	HRESULT _stdcall GetTypeInfoCount(
		unsigned int * pctinfo) 
	{
		return E_NOTIMPL;
	}

	HRESULT _stdcall GetTypeInfo(
		unsigned int iTInfo,
		LCID lcid,
		ITypeInfo FAR* FAR* ppTInfo) 
	{
		return E_NOTIMPL;
	}

	HRESULT _stdcall GetIDsOfNames(
		REFIID riid, 
		OLECHAR FAR* FAR* rgszNames, 
		unsigned int cNames, 
		LCID lcid, 
		DISPID FAR* rgDispId 
		)
	{
		if(lstrcmpi(rgszNames[0], L"ygx_win_close")==0)
		{
			//网页调用window.external.ygx_win_close时，会调用这个方法获取ygx_win_close的ID
			*rgDispId = 100;
		}
		else if(lstrcmpi(rgszNames[0], L"ygx_user_login")==0)
		{
			*rgDispId = 101;
		}
		else if(lstrcmpi(rgszNames[0], L"ygx_user_logout")==0)
		{
			*rgDispId = 102;
		}
		else if(lstrcmpi(rgszNames[0], L"ygx_set_winAttr")==0)
		{
			*rgDispId = 103;
		}
		else if(lstrcmpi(rgszNames[0], L"ygx_open_win")==0)
		{
			*rgDispId = 104;
		}
		else if(lstrcmpi(rgszNames[0], L"ygx_pop_to_menu")==0)
		{
			*rgDispId = 105;
		}
		return S_OK;
	}

	HRESULT _stdcall Invoke(
		DISPID dispIdMember,
		REFIID riid,
		LCID lcid,
		WORD wFlags,
		DISPPARAMS* pDispParams,
		VARIANT* pVarResult,
		EXCEPINFO* pExcepInfo,
		unsigned int* puArgErr
		)
	{
		if(dispIdMember == 100)
		{
			//网页调用ygx_win_close时，或根据获取到的ID调用Invoke方法
			ygx_win_close();
		}
		else if(dispIdMember == 101)
		{
			CString p1,p2,p3,p4;
			if(pDispParams->rgvarg[0].vt != VT_EMPTY)
				p1 = pDispParams->rgvarg[0].bstrVal;
			if(pDispParams->rgvarg[1].vt != VT_EMPTY)
				p2 = pDispParams->rgvarg[1].bstrVal;
			if(pDispParams->rgvarg[2].vt != VT_EMPTY)
				p3 = pDispParams->rgvarg[2].bstrVal;
			if(pDispParams->rgvarg[3].vt != VT_EMPTY)
				p4 = pDispParams->rgvarg[3].bstrVal;
			ygx_user_login(p4,p3,p2,p1);
		}
		else if(dispIdMember == 102)
		{
			CString p1 = pDispParams->rgvarg[0].bstrVal;
			ygx_user_logout(p1);
		}
		else if(dispIdMember == 103)
		{
			int p2=0,p1=0;
			CString p3;
			if(pDispParams->rgvarg[0].vt != VT_EMPTY)
				p1 = pDispParams->rgvarg[0].intVal;
			if(pDispParams->rgvarg[1].vt != VT_EMPTY)
				p2 = pDispParams->rgvarg[1].intVal;
			if(pDispParams->rgvarg[2].vt != VT_EMPTY)
				p3 = pDispParams->rgvarg[2].bstrVal;
			ygx_set_winAttr(p3,p2,p1);
		}
		else if(dispIdMember == 104)
		{
			int p1 = 0,p2 = 0;
			CString p3,p4;
			if(pDispParams->rgvarg[0].vt != VT_EMPTY)
				p1 = pDispParams->rgvarg[0].intVal;
			if(pDispParams->rgvarg[1].vt != VT_EMPTY)
				p2 = pDispParams->rgvarg[1].intVal;
			if(pDispParams->rgvarg[2].vt != VT_EMPTY)
				p3 = pDispParams->rgvarg[2].bstrVal;
			if(pDispParams->rgvarg[3].vt != VT_EMPTY)
				p4 = pDispParams->rgvarg[3].bstrVal;
			ygx_open_win(p4,p3,p2,p1);
		}
		else if(dispIdMember == 105)
		{
			CString p1;
			if(pDispParams->rgvarg[0].vt != VT_EMPTY)
				p1 = pDispParams->rgvarg[0].bstrVal;
			ygx_pop_to_menu(p1);
		}
		return S_OK;
	}
};

typedef void _stdcall JsFunction_Callback(LPVOID pParam);

class JsFunction:public IDispatch
{
	long _refNum;
	JsFunction_Callback *m_pCallback;
	LPVOID m_pParam;
public:
	JsFunction(JsFunction_Callback *pCallback, LPVOID pParam)
	{
		_refNum = 1;
		m_pCallback = pCallback;
		m_pParam = pParam;
	}
	~JsFunction(void)
	{
	}
public:

	// IUnknown Methods

	STDMETHODIMP QueryInterface(REFIID iid,void**ppvObject)
	{
		*ppvObject = NULL;

		if (iid == IID_IUnknown)	*ppvObject = this;
		else if (iid == IID_IDispatch)	*ppvObject = (IDispatch*)this;

		if(*ppvObject)
		{
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	STDMETHODIMP_(ULONG) AddRef()
	{
		return ::InterlockedIncrement(&_refNum);
	}

	STDMETHODIMP_(ULONG) Release()
	{
		::InterlockedDecrement(&_refNum);
		if(_refNum == 0)
		{
			delete this;
		}
		return _refNum;
	}

	// IDispatch Methods

	HRESULT _stdcall GetTypeInfoCount(
		unsigned int * pctinfo) 
	{
		return E_NOTIMPL;
	}

	HRESULT _stdcall GetTypeInfo(
		unsigned int iTInfo,
		LCID lcid,
		ITypeInfo FAR* FAR* ppTInfo) 
	{
		return E_NOTIMPL;
	}

	HRESULT _stdcall GetIDsOfNames(
		REFIID riid, 
		OLECHAR FAR* FAR* rgszNames, 
		unsigned int cNames, 
		LCID lcid, 
		DISPID FAR* rgDispId 
		)
	{
		//令人费解的是，网页调用函数的call方法时，没有调用GetIDsOfNames获取call的ID，而是直接调用Invoke
		return E_NOTIMPL;
	}

	HRESULT _stdcall Invoke(
		DISPID dispIdMember,
		REFIID riid,
		LCID lcid,
		WORD wFlags,
		DISPPARAMS* pDispParams,
		VARIANT* pVarResult,
		EXCEPINFO* pExcepInfo,
		unsigned int* puArgErr
		)
	{
		m_pCallback(m_pParam);
		return S_OK;
	}
};

class CWebForm: public CWebBrowserBase
{
	HWND hWnd;
public:
	CWebForm(HWND hwnd)
	{
		hWnd = hwnd;
	}
	~CWebForm()
	{
	}
	virtual HWND GetHWND(){ return hWnd;}

	static void _stdcall button1_onclick(LPVOID pParam)
	{
		CWebForm *pMainForm = (CWebForm*)pParam;
		MessageBox(pMainForm->hWnd, L"您点击了button1", L"提示(C++)", 0);
	}

	virtual void OnDocumentCompleted()
	{
		if (IsWindowVisible(hWnd))
			CWebBrowserBase::OnDocumentCompleted();
		return;

		VARIANT params[10];

		//获取window
		IDispatch *pHtmlWindow = GetHtmlWindow();

		//获取document
		CVariant document;
		params[0].vt = VT_BSTR;
		params[0].bstrVal = L"document";
		GetProperty(pHtmlWindow, L"document", &document);
		SetProperty(document.pdispVal, L"onclick", params);
	
		//获取button1
		CVariant button1;
		params[0].vt = VT_BSTR;
		params[0].bstrVal = L"button1";
		InvokeMethod(document.pdispVal, L"onclick", &button1, params, 1);

		//处理button1的onclick事件
		params[0].vt = VT_DISPATCH;
		params[0].pdispVal = new JsFunction(button1_onclick, this);
		SetProperty(button1.pdispVal, L"onclick", params);
	}

	virtual HRESULT STDMETHODCALLTYPE GetExternal(IDispatch **ppDispatch)
	{
		//重写GetExternal返回一个ClientCall对象
		*ppDispatch = new ClientCall();
		return S_OK;
	}
};



#endif
