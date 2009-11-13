#pragma once
#include "resource.h"

class StatusPage : public CPropertyPageImpl<StatusPage>
{
public:
	enum { IDD = IDD_STATUS_PAGE };

	StatusPage(ATL::_U_STRINGorID title = (LPCTSTR)NULL);
	virtual ~StatusPage(void);
};

