/*@Author Zeki.Yan*/
#pragma once

#define MODULE_NAME			L"OpenHMI"

#define WM_HARD_KEY			1025 

#define USER_EVNET_ID_START	1000

//////////////////////////////////////////////////////////////////////////////
#ifdef OS_WIN
	#ifdef OPENHMI_EXPORTS
		#define OPENHMI_API __declspec(dllexport)
	#else
		#define OPENHMI_API __declspec(dllimport)
	#endif
#endif

#ifdef OS_QNX
	#define OPENHMI_API
#endif

#ifdef OS_LINUX
	#define OPENHMI_API
#endif
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
#define DECLARE_EVENT_MAP	virtual void onDispatchEvent(Object *pTarget, UINT uiEventID, Object *pData = NULL);

#define BEGIN_EVENT_MAP(ClassName)	\
	void ClassName::onDispatchEvent(Object *pTarget, UINT uiEventID, Object *pData) \
	{

// private void onFunc(CComponent *pTarget);
#define CONTROL_EVENT_HANDLER(targetComponent, eventID, onFunc) \
		if (targetComponent == pTarget && eventID == uiEventID) \
		{ \
			onFunc(static_cast<CComponent*>(pTarget)); \
			return; \
		}

// private void onFunc(CComponent *pTarget);
#define CONTROL_EVENT_HANDLER_CHAIN(targetParent, eventID, onFunc) \
		if ((targetParent)->contains(pTarget) && eventID == uiEventID) \
		{ \
			onFunc(static_cast<CComponent*>(pTarget)); \
			return; \
		}

// private void onFunc(CBasePopup *pTarget)
#define POPUP_EVENT_HANDLER(targetPopup, eventID, onFunc) \
		if (targetPopup == pTarget && eventID == uiEventID) \
		{ \
			onFunc(static_cast<CBasePopup*>(pTarget)); \
			return; \
		}

// private void onFunc();
#define LOGIC_EVENT_HANDLER(targetLogic, eventID, onFunc) \
		if (targetLogic == pTarget && eventID == uiEventID) \
		{ \
			onFunc(); \
			return; \
		}

// private void onFunc(Object *pData);
#define SERVICE_EVENT_HANDLER(targetService, eventID, onFunc) \
		if (targetService == pTarget && eventID == uiEventID) \
		{ \
			onFunc(pData); \
			return; \
		}

// private void onFunc(Object *pData);
#define GESTURE_EVENT_HANDLER(targetGesture, gestureID, onFunc) \
		if (targetGesture == pTarget && gestureID == uiEventID) \
		{ \
			onFunc(pData); \
			return; \
		}

// private void onFunc(Object *pTarget);
#define EVENT_HANDLER_CHAIN(targetParent, eventID, onFunc) \
		if ((targetParent)->contains(pTarget) && eventID == uiEventID) \
		{ \
			onFunc(pTarget); \
			return; \
		}

#define END_EVENT_MAP(ParentClassName) \
		ParentClassName::onDispatchEvent(pTarget, uiEventID);\
	}

#define END_EVENT_MAP_2 \
	}
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
#define DECLARE_CMD_MAP	virtual BOOLEAN onDispatchCommand(const String &strCommand, Object *pData = NULL);

#define BEGIN_CMD_MAP(ClassName)	\
	BOOLEAN ClassName::onDispatchCommand(const String &strCommand, Object *pData) \
	{

// private void onFunc(Object *pData);
#define CMD_HANDLER(command, onFunc) \
		if (strCommand == command ) \
		{ \
			onFunc(pData); \
			return TRUE; \
		}

#define END_CMD_MAP(ParentClassName) \
		return ParentClassName::onDispatchCommand(strCommand, pData);\
	}

#define END_CMD_MAP_2 \
		return FALSE;\
	}
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
#define DECLARE_SERVICE_METHOD_MAP	virtual BOOLEAN onDispatchServiceMethod(UINT uiMethodID, Object *pData = NULL);

#define BEGIN_SERVICE_METHOD_MAP(ClassName)	\
	BOOLEAN ClassName::onDispatchServiceMethod(UINT uiMethodID, Object *pData) \
	{

// private void onFunc(Object *pData);
#define SERVICE_METHOD_HANDLER(methodID, onFunc) \
		if (methodID == uiMethodID) \
		{ \
			onFunc(pData); \
			return TRUE; \
		}

#define END_SERVICE_METHOD_MAP(ParentClassName) \
		return ParentClassName::onDispatchServiceMethod(uiMethodID, pData);\
	}

#define END_SERVICE_METHOD_MAP_2 \
		return FALSE;\
	}
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
#define	SERVICE_ERROR_EVENT_ID		(UINT)0xFFFFFFFF

#define IMP_IBASESERVICE_ABST	\
	virtual	void	connect()		{ }\
	virtual	void	disconnect()	{ }\
	virtual BOOLEAN	isAvailable()	{ return m_pBaseServcie != NULL ? m_pBaseServcie->isAvailable() : FALSE; }

#define IMP_IBASESERVICE_IMP	\
	virtual	void	connect()		{ m_bAvailable = TRUE; }\
	virtual	void	disconnect()	{ m_bAvailable = FALSE; }\
	virtual BOOLEAN	isAvailable()	{ return m_bAvailable; }
//////////////////////////////////////////////////////////////////////////////
