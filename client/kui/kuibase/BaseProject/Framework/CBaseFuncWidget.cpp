#include "OpenHMI.h"

namespace OpenHMI
{
	CBaseFuncWidget::CBaseFuncWidget()
	{
		setType(CONFIG::CONTAINER_TYPE_FUNCWIDGET);
	}

	CBaseFuncWidget::~CBaseFuncWidget()
	{
		;
	}

	BEGIN_EVENT_MAP(CBaseFuncWidget)
	END_EVENT_MAP(CContainer)

	void CBaseFuncWidget::refresh()
	{
		pushDataToUI();

		CContainer::refresh();
	}

	void CBaseFuncWidget::destroy()
	{
		if (isDestroyed())
		{
			return;
		}

		CContainer::destroy();
	}
}