//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------


// clWhite;
// RGB( 185, 185, 195 );
// RGB( 155, 157, 156 ); // G-Wiz
#define GW_BACKGROUND_WHITE ((TColor)RGB( 221, 214, 225 )) // GTK-Psi

// clBlack;
// RGB( 45, 45, 125 );
// RGB( 56, 70, 115 ); // G-Wiz
#define GW_FOREGROUND_WHITE ((TColor)RGB(  91,  58,  75 )) // GTK-Psi

class TGWiz : public TForm
{
__published:	// IDE-managed Components
        TTimer *Timer1;
        TImage *screen;
        TImage *Image1;
    TShape *Shape1;
        void __fastcall Image1MouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall Image1MouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall Image1MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
        TColor ForeGround, BackGround;
        int GWizLeft;

        __fastcall TGWiz(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TGWiz *GWiz;
//---------------------------------------------------------------------------
#endif
