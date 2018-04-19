#include "OpenHMI.h"

namespace OpenHMI
{
	CModel::CModel()
		: m_pComponent(NULL)
	{
		;
	}

	CModel::~CModel()
	{
		;
	}

	void CModel::notifyModelChanged()
	{
		if (m_pComponent != NULL)
		{
			m_pComponent->refresh();

			m_pComponent->setDirty();
		}
	}
}