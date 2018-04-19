/*@Author Zeki.Yan*/
#pragma once

#ifdef OS_WIN
	#pragma warning( disable: 4251 )	
#endif

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

// TODO: include common
#include "./Common.h"

// TODO: include MemManager

#ifdef OS_WIN
	//#include "vld.h"
#endif

// TODO: include Platform
#include "./Platform/CTime.h"

// TODO: include Platform.MemManager
//#include "./Platform/MemManager/CMemManager.h"

// TODO: include Platform.File
#include "./Platform/File/CPath.h"
#include "./Platform/File/CFile.h"
#include "./Platform/File/CFolder.h"

// TODO: include Platform.Thread
#include "./Platform/Thread/CSyncObject.h"
#include "./Platform/Thread/CAutoSync.h"
#include "./Platform/Thread/CWaitObject.h"
#include "./Platform/Thread/CThread.h"

// TODO: include Platform.Net
#include "./Platform/Net/ISocketListener.h"
#include "./Platform/Net/CAutoSocket.h"

// TODO: include Platform.ScreenTouch
#include "./Platform/ScreenTouch/CCursor.h"
#include "./Platform/ScreenTouch/IScreenTouchListener.h"
#include "./Platform/ScreenTouch/CScreenTouchServer.h"
#include "./Platform/ScreenTouch/CMsgScreenTouch.h"
#include "./Platform/ScreenTouch/CMsgHardKey.h"
#include "./Platform/ScreenTouch/CMsgScreen.h"

// TODO: include Platform.Timer
#include "./Platform/Timer/ITimerListener.h"
#include "./Platform/Timer/CTimer.h"
#include "./Platform/Timer/CTimerManager.h"
#include "./Platform/Timer/CMsgTimer.h"

// TODO: include ResourceSystem
#include "./ResourceSystem/CPropertySet.h"
#include "./ResourceSystem/CNodeParser.h"
#include "./ResourceSystem/CPluginParser.h"
#include "./ResourceSystem/CConfigParser.h"
#include "./ResourceSystem/CLanguageParser.h"
#include "./ResourceSystem/CFontParser.h"

// TODO: include ResourceSystem.Skin
#include "./ResourceSystem/Skin/CResNode.h"
#include "./ResourceSystem/Skin/CResImage.h"
#include "./ResourceSystem/Skin/CResFont.h"
#include "./ResourceSystem/Skin/CResFontGroup.h"
#include "./ResourceSystem/Skin/CResTextField.h"
#include "./ResourceSystem/Skin/CResImageField.h"
#include "./ResourceSystem/Skin/CResHitArea.h"
#include "./ResourceSystem/Skin/CResFilter.h"
#include "./ResourceSystem/Skin/CResFilterSet.h"
#include "./ResourceSystem/Skin/CResItem.h"
#include "./ResourceSystem/Skin/CResGroup.h"
#include "./ResourceSystem/Skin/CResFuncWidget.h"
#include "./ResourceSystem/Skin/CResCtrlWidget.h"
#include "./ResourceSystem/Skin/CResScreen.h"
#include "./ResourceSystem/Skin/CResPopup.h"
#include "./ResourceSystem/Skin/CSkinParser.h"

// TODO: include RenderEngine
#include "./RenderEngine/CRenderCacheData.h"
#include "./RenderEngine/CRenderFilter.h"
#include "./RenderEngine/CRenderFont.h"
#include "./RenderEngine/CRenderItem.h"
#include "./RenderEngine/CRenderGroup.h"
#include "./RenderEngine/CRenderText.h"
#include "./RenderEngine/CRenderImage.h"
#include "./RenderEngine/CRenderViewport.h"
#include "./RenderEngine/IRenderSession.h"
#include "./RenderEngine/IRenderEngine.h"
#include "./RenderEngine/CRenderEngine.h"

// TODO: include UISystem
#include "./UISystem/ENV.h"
#include "./UISystem/IEventListener.h"
#include "./UISystem/CEventDispatcher.h"
#include "./UISystem/ICommandListener.h"
#include "./UISystem/CCommandDispatcher.h"

// TODO: include UISystem.Render
#include "./UISystem/Render/IRenderable.h"
#include "./UISystem/Render/CMsgRender.h"

// TODO: include UISystem.ScreenTouch
#include "./UISystem/ScreenTouch/ICursorTracker.h"
#include "./UISystem/ScreenTouch/ICanvas.h"
#include "./UISystem/ScreenTouch/CScreenTouchManager.h"

// TODO: include UISystem.GestureRecognise
#include "./UISystem/GestureRecognizer/CBaseGestureData.h"
#include "./UISystem/GestureRecognizer/IGestureRecognizer.h"
#include "./UISystem/GestureRecognizer/CBaseGestureRecognizer.h"
#include "./UISystem/GestureRecognizer/CMsgGesture.h"
#include "./UISystem/GestureRecognizer/CGestureManager.h"

// TODO: include UISystem.AnimationSystem.Data
#include "./UISystem/AnimationSystem/Data/CBaseAmtData.h"
#include "./UISystem/AnimationSystem/Data/CAmtBoolean.h"
#include "./UISystem/AnimationSystem/Data/CAmtByte.h"
#include "./UISystem/AnimationSystem/Data/CAmtChar.h"
#include "./UISystem/AnimationSystem/Data/CAmtColor.h"
#include "./UISystem/AnimationSystem/Data/CAmtDouble.h"
#include "./UISystem/AnimationSystem/Data/CAmtFloat.h"
#include "./UISystem/AnimationSystem/Data/CAmtInt.h"
#include "./UISystem/AnimationSystem/Data/CAmtLong.h"
#include "./UISystem/AnimationSystem/Data/CAmtPoint.h"
#include "./UISystem/AnimationSystem/Data/CAmtRectangle.h"
#include "./UISystem/AnimationSystem/Data/CAmtShort.h"
#include "./UISystem/AnimationSystem/Data/CAmtString.h"
#include "./UISystem/AnimationSystem/Data/CAmtVector2.h"

// TODO: include UISystem.AnimationSystem.Interpolation
#include "./UISystem/AnimationSystem/Interpolation/CBaseInterpolation.h"
#include "./UISystem/AnimationSystem/Interpolation/CBezierInterpolation.h"
#include "./UISystem/AnimationSystem/Interpolation/CDiscreteInterpolation.h"
#include "./UISystem/AnimationSystem/Interpolation/CLinearInterpolation.h"

// TODO: include UISystem.AnimationSystem.Path
#include "./UISystem/AnimationSystem/Path/CBasePath.h"
#include "./UISystem/AnimationSystem/Path/CArc.h"
#include "./UISystem/AnimationSystem/Path/CBezier.h"
#include "./UISystem/AnimationSystem/Path/CCircle.h"
#include "./UISystem/AnimationSystem/Path/CEclipse.h"
#include "./UISystem/AnimationSystem/Path/CPoliLine.h"
#include "./UISystem/AnimationSystem/Path/CPathGroup.h"

// TODO: include UISystem.AnimationSystem
#include "./UISystem/AnimationSystem/IAnimationListener.h"
#include "./UISystem/AnimationSystem/CAdapterAnimationListener.h"
#include "./UISystem/AnimationSystem/CBaseAnimation.h"
#include "./UISystem/AnimationSystem/CAnimationGroup.h"
#include "./UISystem/AnimationSystem/CAnimationPlayer.h"

// TODO: include UISystem.AnimationSystem.Animation
#include "./UISystem/AnimationSystem/Animation/CValueAnimation.h"
#include "./UISystem/AnimationSystem/Animation/CKeyFrame.h"
#include "./UISystem/AnimationSystem/Animation/CKeyFrameAnimation.h"
#include "./UISystem/AnimationSystem/Animation/CTrackAnimation.h"
#include "./UISystem/AnimationSystem/Animation/CBaseMotionAnimation.h"
#include "./UISystem/AnimationSystem/Animation/CUniformRectilinearMotionAnimation.h"

// TODO: include UISystem.2DSystem.Misc
#include "./UISystem/2DSystem/Misc/CPoint.h"
#include "./UISystem/2DSystem/Misc/CRectangle.h"
#include "./UISystem/2DSystem/Misc/CRectangleF.h"
#include "./UISystem/2DSystem/Misc/CSize.h"
#include "./UISystem/2DSystem/Misc/CMovieClip.h"
#include "./UISystem/2DSystem/Misc/CButtonGroup.h"

// TODO: include UISystem.2DSystem.HitArea
#include "./UISystem/2DSystem/HitArea/CBaseHitArea.h"
#include "./UISystem/2DSystem/HitArea/CHitAreaRectangle.h"
#include "./UISystem/2DSystem/HitArea/CHitAreaTriangle.h"
#include "./UISystem/2DSystem/HitArea/CHitAreaCircle.h"

// TODO: include UISystem.2DSystem
#include "./UISystem/2DSystem/IMovieClipGenerator.h"
#include "./UISystem/2DSystem/ISoundGenerator.h"
#include "./UISystem/2DSystem/CEventGesture.h"
#include "./UISystem/2DSystem/CModel.h"
#include "./UISystem/2DSystem/CView.h"
#include "./UISystem/2DSystem/CComponent.h"
#include "./UISystem/2DSystem/CContainer.h"

// TODO: include UISystem.2DSystem.Control
#include "./UISystem/2DSystem/Control/TextField/CCtrlTextField.h"

#include "./UISystem/2DSystem/Control/ImageField/CCtrlImageField.h"

#include "./UISystem/2DSystem/Control/Label/CViewLabel.h"
#include "./UISystem/2DSystem/Control/Label/CCtrlLabel.h"

#include "./UISystem/2DSystem/Control/Button/CCtrlButton.h"

#include "./UISystem/2DSystem/Control/Balance/CModelBalance.h"
#include "./UISystem/2DSystem/Control/Balance/CViewBalance.h"
#include "./UISystem/2DSystem/Control/Balance/CCtrlBalance.h"

#include "./UISystem/2DSystem/Control/Progressbar/CModelProgressbar.h"
#include "./UISystem/2DSystem/Control/Progressbar/CViewProgressbar.h"
#include "./UISystem/2DSystem/Control/Progressbar/CCtrlProgressbar.h"

#include "./UISystem/2DSystem/Control/Scrollbar/CModelScrollbar.h"
#include "./UISystem/2DSystem/Control/Scrollbar/CViewScrollbar.h"
#include "./UISystem/2DSystem/Control/Scrollbar/CCtrlScrollbar.h"

#include "./UISystem/2DSystem/Control/List/CBaseListAmt.h"
#include "./UISystem/2DSystem/Control/List/CListAmtElasticity.h"
#include "./UISystem/2DSystem/Control/List/CListAmtFling.h"
#include "./UISystem/2DSystem/Control/List/CListAmtTouchShake.h"
#include "./UISystem/2DSystem/Control/List/CListAmtEnterInterdigitation.h"
#include "./UISystem/2DSystem/Control/List/CModelList.h"
#include "./UISystem/2DSystem/Control/List/IViewList.h"
#include "./UISystem/2DSystem/Control/List/CCtrlList.h"

#include "./UISystem/2DSystem/Control/List/IViewList_Simple.h"
#include "./UISystem/2DSystem/Control/List/CCtrlList_Simple.h"

#include "./UISystem/2DSystem/Control/Table/CBaseTableAmt.h"
#include "./UISystem/2DSystem/Control/Table/CTableAmtElasticity.h"
#include "./UISystem/2DSystem/Control/Table/CTableAmtFling.h"
#include "./UISystem/2DSystem/Control/Table/CTableAmtTouchShake.h"
#include "./UISystem/2DSystem/Control/Table/CTableAmtEnterInterdigitation.h"
#include "./UISystem/2DSystem/Control/Table/CModelTable.h"
#include "./UISystem/2DSystem/Control/Table/IViewTable.h"
#include "./UISystem/2DSystem/Control/Table/CCtrlTable.h"

#include "./UISystem/2DSystem/Control/Tree/CModelTree.h"
#include "./UISystem/2DSystem/Control/Tree/IViewTree.h"
#include "./UISystem/2DSystem/Control/Tree/CCtrlTree.h"

#include "./UISystem/2DSystem/Control/Selector/CCtrlSelector.h"

#include "./UISystem/2DSystem/Control/Viewport/CCtrlViewport.h"

#include "./UISystem/2DSystem/Control/Edit/CCtrlEdit.h"
#include "./UISystem/2DSystem/Control/WebBrowser/WebBrowser.h"
#include "./UISystem/2DSystem/Control/WebBrowser/jscallclient.h"
#include "./UISystem/2DSystem/Control/WebBrowser/CCtrlWebBrowser.h"

// TODO: include UISystem.2DSystem.Container
#include "./UISystem/2DSystem/Container/CWgtMessageBox.h"

#include "./UISystem/2DSystem/Container/Keyboard/CKeyTable.h"
#include "./UISystem/2DSystem/Container/Keyboard/CKeyTable_ASC.h"
#include "./UISystem/2DSystem/Container/Keyboard/CWgtKeyboard.h"
#include "./UISystem/2DSystem/Container/Keyboard/CWgtKeyboard_ASC.h"

// TODO: include UISystem.3DSystem.Misc
#include "./UISystem/3DSystem/Misc/C3DColor.h"
#include "./UISystem/3DSystem/Misc/C3DPoint.h"
#include "./UISystem/3DSystem/Misc/C3DVector.h"
#include "./UISystem/3DSystem/Misc/C3DVector4.h"
#include "./UISystem/3DSystem/Misc/C3DRay.h"
#include "./UISystem/3DSystem/Misc/C3DPlane.h"
#include "./UISystem/3DSystem/Misc/C3DBox.h"
#include "./UISystem/3DSystem/Misc/C3DQuaternion.h"
#include "./UISystem/3DSystem/Misc/C3DMatrix.h"

// TODO: include UISystem.3DSystem

// TODO: include BaseProject
#include "./BaseProject/CONFIG.h"
#include "./BaseProject/INIT.h"

// TODO: include BaseProject.Framework
#include "./BaseProject/Framework/CBaseApplication.h"
#include "./BaseProject/Framework/CBaseLogic.h"
#include "./BaseProject/Framework/CBaseOperationCenter.h"
#include "./BaseProject/Framework/CBaseScreen.h"
#include "./BaseProject/Framework/CBasePopup.h"
#include "./BaseProject/Framework/CBaseFuncWidget.h"
#include "./BaseProject/Framework/CBaseMainFrame.h"
#include "./BaseProject/Framework/CBaseMessage.h"
#include "./BaseProject/Framework/CMain.h"

// TODO: include BaseProject.Service
#include "./BaseProject/Service/CBaseData.h"
#include "./BaseProject/Service/IBaseService.h"
#include "./BaseProject/Service/CBaseService.h"
#include "./BaseProject/Service/CMsgFromServiceThread.h"
#include "./BaseProject/Service/CMsgToServiceThread.h"
#include "./BaseProject/Service/IServiceAdaptor.h"
#include "./BaseProject/Service/CServiceAdaptor.h"

// TODO: include BaseProject.Framework.Misc
#include "./BaseProject/Framework/Misc/CLog.h"
#include "./BaseProject/Framework/Misc/CManager_Application.h"
#include "./BaseProject/Framework/Misc/CManager_Language.h"
#include "./BaseProject/Framework/Misc/CManager_Logic.h"
#include "./BaseProject/Framework/Misc/CManager_Service.h"
#include "./BaseProject/Framework/Misc/CManager_Skin.h"
#include "./BaseProject/Framework/Misc/CLanguage.h"
