//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------



#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>











USEFORM("FormMain.cpp", MainForm);
//---------------------------------------------------------------------------
USEFORM("..\..\..\Desktop\BCB_XE2\Inndy's Hack Injector\FormMain.cpp", MainForm);
//---------------------------------------------------------------------------
USEFORM("..\..\..\Desktop\BCB_XE2\Inndy's Hack Injector\FormMain.cpp", MainForm);
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TMainForm), &MainForm);
        Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
