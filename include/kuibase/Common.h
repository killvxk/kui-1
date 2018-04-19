/*@Author Zeki.Yan*/
#pragma once

// TODO: classes declare here
namespace OpenHMI
{
	// Common
	class Object;
	class CacheObject;
	class ByteArray;
	class String;
	class Math;

	// Platform
	class CTime;

	// Platform.MemManager
	class CMemManager;

	// Platform.File
	class CPath;
	class CFile;
	class CFolder;	

	// Platform.Thread
	class CSyncObject;
	class CAutoSync;
	class CWaitObject;
	class CThread;	

	// Platform.Net
	class ISocketListener;
	class CAutoSocket;

	// Platform.ScreenTouch
	class CCursor;
	class IScreenTouchListener;
	class CScreenTouchServer;
	class CMsgScreenTouch;
	class CMsgHardKey;

	// Platform.Timer
	class ITimerListener;
	class CTimer;	
	class CTimerManager;
	class CMsgTimer;

	// ResourceSystem
	class CPropertySet;
	class CNodeParser;
	class CPluginParser;
	class CConfigParser;
	class CLanguageParser;
	class CFontParser;

	// ResourceSystem.Skin
	class CResNode;
	class CResImage;
	class CResFont;
	class CResFontGroup;
	class CResTextField;
	class CResImageField;
	class CResHitArea;
	class CResFilter;
	class CResFilterSet;
	class CResItem;
	class CResGroup;
	class CResFuncWidget;
	class CResCtrlWidget;
	class CResScreen;
	class CResPopup;
	class CSkinParser;	

	// RenderEngine
	class CRenderCacheData;
	class CRenderBaseFilter;
	class CRenderFltReflection;
	class CRenderFltGlass;
	class CRenderFltRipple;
	class CRenderFltMask;
	class CRenderFont;
	class CRenderItem;
	class CRenderGroup;
	class CRenderText;
	class CRenderImage;
	class CRenderViewport;
	class IRenderSession;
	class IRenderEngine;
	class CRenderEngine;	

	// UISystem
	class ENV;
	class IEventListener;
	class CEventDispatcher;
	class ICommandListener;
	class CCommandDispatcher;

	// UISystem.Render	
	class IRenderable; 
	class CMsgRender;

	// UISystem.ScreenTouch
	class ICursorTracker;
	class ICanvas;
	class CScreenTouchManager;
	
	// UISystem.GestureRecognise
	class CBaseGestureData;	
	class IGestureRecognizer;
	class CBaseGestureRecognizer;	
	class CMsgGesture;
	class CGestureManager;

	// UISystem.AnimationSystem.Data
	class CBaseAmtData;
	class CAmtBoolean;
	class CAmtByte;
	class CAmtChar;
	class CAmtColor;
	class CAmtDouble;
	class CAmtFloat;
	class CAmtInt;
	class CAmtLong;	
	class CAmtPoint;
	class CAmtRectangle;
	class CAmtShort;
	class CAmtString;
	class CAmtVector2;	

	// UISystem.AnimationSystem.Interpolation
	class CBaseInterpolation;	
	class CBezierInterpolation;
	class CDiscreteInterpolation;
	class CLinearInterpolation;

	// UISystem.AnimationSystem.Path
	class CBasePath;
	class CArc;
	class CBezier;
	class CCircle;
	class CEclipse;
	class CPoliLine;
	class CPathGroup;	

	// UISystem.AnimationSystem
	class IAnimationListener;
	class CAdapterAnimationListener;
	class CBaseAnimation;
	class CAnimationGroup;
	class CAnimationPlayer;	

	// UISystem.AnimationSystem.Animation
	class CValueAnimation;
	class CKeyFrame;
	class CKeyFrameAnimation;
	class CTrackAnimation;
	class CBaseMotionAnimation;
	class CUniformRectilinearMotionAnimation;	

	// UISystem.2DSystem.Misc
	class CPoint;
	class CRectangle;
	class CRectangleF;
	class CSize;
	class CMovieClip;
	class CButtonGroup;

	// UISystem.2DSystem.HitArea
	class CBaseHitArea;
	class CHitAreaRectangle;
	class CHitAreaTriangle;
	class CHitAreaCircle;

	// UISystem.2DSystem
	class IMovieClipGenerator;
	class ISoundGenerator;
	class CEventGesture;
	class CModel;
	class CView;
	class CComponent;
	class CContainer;

	// UISystem.2DSystem.Control
	class CCtrlTextField;

	class CCtrlImageField;	

	class CViewLabel;
	class CCtrlLabel;

	class CCtrlButton;	

	class CModelBalance;
	class CViewBalance;
	class CCtrlBalance;

	class CModelProgressbar;
	class CViewProgressbar;
	class CCtrlProgressbar;

	class CModelScrollbar;
	class CViewScrollbar;
	class CCtrlScrollbar;

	class CBaseListAmt;
	class CListAmtElasticity;
	class CListAmtFling;
	class CListAmtTouchShake;
	class CListAmtEnterInterdigitation;
	class CModelList;
	class IViewList;
	class CCtrlList;

	class IViewList_Simple;
	class CCtrlList_Simple;

	class CBaseTableAmt;
	class CTableAmtElasticity;
	class CTableAmtFling;
	class CTableAmtTouchShake;
	class CTableAmtEnterInterdigitation;
	class CModelTable;
	class IViewTable;
	class CCtrlTable;

	class CModelTree;
	class IViewTree;
	class CCtrlTree;	

	class CCtrlSelector;

	class CCtrlViewport;

	// UISystem.2DSystem.Container	
	class CWgtMessageBox;

	class CKeyTable;
	class CKeyTable_ASC;
	class CWgtKeyboard;
	class CWgtKeyboard_ASC;

	// UISystem.3DSystem.Misc
	class C3DColor;
	class C3DPoint;	
	class C3DVector;
	class C3DVector4;
	class C3DRay;
	class C3DPlane;
	class C3DBox;
	class C3DQuaternion;
	class C3DMatrix;

	// UISystem.3DSystem	

	// BaseProject
	class CONFIG;
	class INIT;

	// BaseProject.Framework
	class CBaseApplication;
	class CBaseLogic;
	class CBaseOperationCenter;
	class CBaseScreen;
	class CBasePopup;
	class CBaseFuncWidget;
	class CBaseMainFrame;
	class CBaseMessage;	
	class CMain;

	// BaseProject.Service
	class CBaseData;
	class IBaseService;
	class CBaseService;
	class CMsgFromServiceThread;
	class CMsgToServiceThread;
	class IServiceAdaptor;
	class CServiceAdaptor;

	// BaseProject.Framework.Misc
	class CLog;	
	class CManager_Application;
	class CManager_Language;
	class CManager_Logic;
	class CManager_Service;
	class CManager_Skin;
	class CLanguage;
}

// TODO: include common
#include "./Common/TypeDefine.h"
#include "./Common/MacroDefine.h"
#include "./Common/Object.h"
#include "./Common/CacheObject.h"
#include "./Common/ObjectPtr.h"
#include "./Common/ArrayPtr.h"
#include "./Common/Metatype.h"
#include "./Common/String.h"
#include "./Common/StructDefine.h"
#include "./Common/List.h"
#include "./Common/List2.h"
#include "./Common/Array.h"
#include "./Common/Array2.h"
#include "./Common/ByteArray.h"
#include "./Common/Pair.h"
#include "./Common/Map.h"
#include "./Common/Map2.h"
#include "./Common/Set.h"
#include "./Common/CachePool.h"
#include "./Common/CachePool2.h"
#include "./Common/Math.h"