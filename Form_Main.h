//---------------------------------------------------------------------------

#ifndef Form_MainH
#define Form_MainH
//---------------------------------------------------------------------------
#include "Form_Help.h"
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
	TTimer *tmrDetect;
	TGroupBox *GroupBox1;
	TLabel *Label1;
	TComboBox *ComboBox1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TComboBox *ComboBox2;
	TComboBox *ComboBox3;
	TComboBox *ComboBox4;
	TComboBox *ComboBox5;
	TGroupBox *GroupBox2;
	TLabel *Label6;
	TEdit *editHP;
	TLabel *Label7;
	TEdit *editMP;
	TCheckBox *chkProtect;
	TButton *Button1;
	TButton *Button2;
	TGroupBox *GroupBox3;
	TLabel *Label8;
	TLabel *lblHP;
	TLabel *Label10;
	TLabel *lblMP;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall tmrDetectTimer(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall ComboBox1Change(TObject *Sender);
	void __fastcall editHPChange(TObject *Sender);
	void __fastcall editMPChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TFormMain(TComponent* Owner);
	void __fastcall FollowGameWindow();
	void __fastcall ShowHelp();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
