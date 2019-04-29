//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "Unit2.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CGRID"
#pragma resource "*.dfm"
TFormAbout *FormAbout;
//---------------------------------------------------------------------------
__fastcall TFormAbout::TFormAbout(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFormAbout::Button1Click(TObject *Sender)
{

FormAbout->Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TFormAbout::FormMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{


ReleaseCapture();
SendMessage( FormAbout->Handle, WM_NCLBUTTONDOWN, HTCAPTION, 0 );

}
//---------------------------------------------------------------------------

void __fastcall TFormAbout::FormCreate(TObject *Sender)
{

FormAbout->PrinterSim->Checked = false;

}
//---------------------------------------------------------------------------

