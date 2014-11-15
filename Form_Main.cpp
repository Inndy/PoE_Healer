//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#include <inifiles.hpp>
#pragma hdrstop

#include "Form_Main.h"
#include "Functions.h"
#include "Hook.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#define HP_COOLDOWN 6
#define MP_COOLDOWN 15
TFormMain *FormMain;
int magic, hp, mp;
extern HMODULE dllinst;
HMODULE poe_base = GetModuleHandle(TEXT("PathOfExile.exe"));
TIniFile* settings;
String dll_path;
HWND *pGameHWND;
int index_hp = 0, index_mp = 0;
int key_modes[5] = {0};
int hp_alarm = 0, mp_alarm = 0;
int hp_cooldown = 0, mp_cooldown = 0;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
	: TForm(Owner)
{
	this->help = NULL;
}
//---------------------------------------------------------------------------
void  __fastcall TFormMain::FollowGameWindow()
{
	RECT rect;
	GetWindowRect(*pGameHWND, &rect);
	this->Left = rect.right;
	this->Top = rect.top;
	if (GetModuleHandle(L"kernelbase.dll")) {
		this->Left += GetSystemMetrics(SM_CXSIZEFRAME) / 2;
		this->Top += GetSystemMetrics(SM_CYSIZEFRAME) / 2;
	}
}
//---------------------------------------------------------------------------
void  __fastcall TFormMain::ShowHelp()
{
	if (this->help) {
		delete this->help;
	}
	this->help = new TFormHelp(this);
	this->help->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCreate(TObject *Sender)
{
	TCHAR buffer[MAX_PATH] = TEXT("Powered By Inndy");
	GetModuleFileName(dllinst, buffer, sizeof(buffer));
	dll_path = ExtractFileDir(buffer);

	settings = new TIniFile(dll_path + "\\PoE_Healer.ini");

	Hook(poe_base);

	Button1Click(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::Button1Click(TObject *Sender)
{
	ComboBox1->ItemIndex = key_modes[0] = settings->ReadInteger("PoE_Healer", "KeyMode_1", 0);
	ComboBox2->ItemIndex = key_modes[1] = settings->ReadInteger("PoE_Healer", "KeyMode_2", 0);
	ComboBox3->ItemIndex = key_modes[2] = settings->ReadInteger("PoE_Healer", "KeyMode_3", 0);
	ComboBox4->ItemIndex = key_modes[3] = settings->ReadInteger("PoE_Healer", "KeyMode_4", 0);
	ComboBox5->ItemIndex = key_modes[4] = settings->ReadInteger("PoE_Healer", "KeyMode_5", 0);

	editHP->Text = IntToStr(hp_alarm = settings->ReadInteger("PoE_Healer", "HP", 0));
	editMP->Text = IntToStr(mp_alarm = settings->ReadInteger("PoE_Healer", "MP", 0));
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::Button2Click(TObject *Sender)
{
	settings->WriteInteger("PoE_Healer", "KeyMode_1", ComboBox1->ItemIndex);
	settings->WriteInteger("PoE_Healer", "KeyMode_2", ComboBox2->ItemIndex);
	settings->WriteInteger("PoE_Healer", "KeyMode_3", ComboBox3->ItemIndex);
	settings->WriteInteger("PoE_Healer", "KeyMode_4", ComboBox4->ItemIndex);
	settings->WriteInteger("PoE_Healer", "KeyMode_5", ComboBox5->ItemIndex);

	settings->WriteString("PoE_Healer", "HP", editHP->Text);
    settings->WriteString("PoE_Healer", "MP", editMP->Text);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::tmrDetectTimer(TObject *Sender)
{
	lblHP->Caption = IntToStr(hp);
	lblMP->Caption = IntToStr(mp);

	if (chkProtect->Checked && GetForegroundWindow() == *pGameHWND) {
		int i;

        if (hp_cooldown > 0) {
            hp_cooldown--;
        } else if (hp < hp_alarm) {
			for (i = index_hp; i < 10; i++) {
				if (key_modes[i % 5] & 1) {
					lblHP->Font->Style = TFontStyles() << fsBold;
					Press(i % 5);
					index_hp = (i + 1) % 5;
                    hp_cooldown = HP_COOLDOWN;
					break;
				}
			}
		} else {
			lblHP->Font->Style = TFontStyles();
		}
        if (mp_cooldown > 0) {
            mp_cooldown--;
        } else if (mp < mp_alarm) {
			for (i = index_mp; i < 10; i++) {
				if (key_modes[i % 5] & 2) {
					lblMP->Font->Style = TFontStyles() << fsBold;
					Press(i % 5);
					index_mp = (i + 1) % 5;
                    mp_cooldown = MP_COOLDOWN;
					break;
				}
			}
		} else {
			lblMP->Font->Style = TFontStyles();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	CanClose = false;
	this->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ComboBox1Change(TObject *Sender)
{
	TComboBox *s = (TComboBox *)Sender;
	key_modes[s->Tag] = s->ItemIndex;
	index_hp = index_mp = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::editHPChange(TObject *Sender)
{
	hp_alarm = StrToInt(editHP->Text);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::editMPChange(TObject *Sender)
{
	mp_alarm = StrToInt(editMP->Text);
}
//---------------------------------------------------------------------------
