#include "OpenHMI.h"

namespace OpenHMI
{
	Object::Object()
		: m_iRef(0)
	{
		;
	}

	Object::~Object()
	{
		;
	}

	void Object::addRef()
	{
		++m_iRef;
	}

	void Object::release()
	{
		if (m_iRef > 0)
		{
			--m_iRef;
			if (m_iRef == 0)
			{
				delete this;
			}
		}
	}
}