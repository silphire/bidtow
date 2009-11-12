#pragma once
#include "resource.h"

class ConfigPage : public CPropertyPageImpl<ConfigPage>
{
public:
	enum { IDD = IDD_CONFIG_PAGE };

	ConfigPage(ATL::_U_STRINGorID title = (LPCTSTR)NULL);
	virtual ~ConfigPage(void);
};

