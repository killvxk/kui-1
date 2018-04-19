#include "HMI.h"

namespace OpenHMI
{
	void CLogic::registerAll()
	{
		// TODO: register all logic here
		CManager_Logic *pManager = CManager_Logic::getInstance();
		pManager->registerLogic(L"CLogicManager", CLogicManager::getInstance());
	}

	void CLogic::unRegisterAll()
	{
		// TODO: unregister all logic here
		CLogicManager::deleteInstance();
	}
}