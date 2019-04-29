//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "time.h"
#include "Unit1.h"
#include "Unit2.h"
#include "Unit3.h"

extern "C" {
  #include "global.h"
  void sim_startup( void );
  void sim_main( unsigned int );
  }

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TGWiz *GWiz;

POINT CoKey5[9] = { 257,453, 229,432, 243,424, 257,420, 274,418, 292,420, 306,424, 320,432, 292,453 };
POINT CoKey6[9] = { 257,475, 229,496, 243,504, 257,508, 274,510, 292,508, 306,504, 320,496, 292,475 };
POINT CoKey7[9] = { 257,453, 229,432, 217,441, 211,453, 209,464, 211,475, 217,487, 229,496, 257,475 };
POINT CoKey8[9] = { 292,453, 292,475, 320,496, 332,486, 338,475, 340,464, 338,453, 332,441, 320,432 };

static HRGN reg1, reg2, reg3, reg4, reg5, reg6, reg7, reg8;
static HRGN regOFF, regRDS;

unsigned char RunFactorStatus;
unsigned char PowerSwitchedOff;

struct freqInput fIP[MAX_FREQUENCY_MEASUREMENTS];
unsigned char ClockRam[9];

/*
void sim_main( int key )
{

if( key == KEY1 )
  {
  }

GWiz->screen->Canvas->Brush->Color = GWiz->BackGround;
GWiz->screen->Canvas->FillRect( Rect(0, 0, 320, 256 ));

String data = "";
data = data.sprintf( "%08LX", key );
GWiz->screen->Canvas->Brush->Color = GWiz->BackGround;
GWiz->screen->Canvas->Font->Style = TFontStyles() << fsBold;
GWiz->screen->Canvas->Font->Size = -24;
GWiz->screen->Canvas->Font->Color = GWiz->ForeGround;
GWiz->screen->Canvas->TextOut( 4,4, data );
GWiz->screen->Visible = true;

}
//---------------------------------------------------------------------------
*/

//---------------------------------------------------------------------------

extern "C" void WriteToScreen( void )
{
char byte, bitcounter;
int bytepointer;
int x,y;

x = 0;
y = 0;

GWiz->screen->Canvas->Brush->Color = GWiz->BackGround;
GWiz->screen->Canvas->FillRect( Rect(0, 0, 320, 256 ));

GWiz->screen->Canvas->Brush->Color = GWiz->ForeGround;

for( bytepointer = 0 ; bytepointer < 2560 ; bytepointer++)
  {
  byte = adm[bytepointer];
  for( bitcounter = 0 ; bitcounter < 8 ; bitcounter++ )
    {
    if( byte & 0x80 )
      {
      GWiz->screen->Canvas->FillRect( Rect( x, y, x + 2, y + 2 ));
      }
    byte <<= 1;
    x = x + 2;
    if( x >= 320 )
      {
      x = 0;
      y += 2;
      }

    }
  }

GWiz->screen->Visible = true;
}
//------------------------------------------------------------------------

__fastcall TGWiz::TGWiz(TComponent* Owner)
        : TForm(Owner)
{

DoubleBuffered = true;

GWiz->BackGround = GW_BACKGROUND_WHITE;
GWiz->ForeGround = GW_FOREGROUND_WHITE;

}
//---------------------------------------------------------------------------

void __fastcall TGWiz::Image1MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
int key;

key = 0;

if( Button == mbLeft )
  {
  if( PtInRegion( regOFF, X, Y ) )
    {
    GWiz->Close();
    key = -1;
    }
  else if( PtInRegion( regRDS, X, Y ) )
    {
    if( FormAbout->Visible )
      {
      FormAbout->Visible = false;
      }
    else
      {
      FormAbout->Visible = true;
      FormAbout->BringToFront();
      }
    key = -1;
    }
  else if( PtInRegion( reg1, X, Y ) )
    {
    key = KEY1;
    }
  else if( PtInRegion( reg2, X, Y ) )
    {
    key = KEY2;
    }
  else if( PtInRegion( reg3, X, Y ) )
    {
    key = KEY3;
    }
  else if( PtInRegion( reg4, X, Y ) )
    {
    key = KEY4;
    }
  else if( PtInRegion( reg5, X, Y ) )
    {
    key = KEY5;
    }
  else if( PtInRegion( reg6, X, Y ) )
    {
    key = KEY6;
    }
  else if( PtInRegion( reg7, X, Y ) )
    {
    key = KEY7;
    }
  else if( PtInRegion( reg8, X, Y ) )
    {
    key = KEY8;
    }

  if( key && (key != -1) )
    {
    GWiz->Timer1->Enabled = false;
    sim_main( key );
    GWiz->Timer1->Enabled = true;
    }
  }

if( key == 0 )
  {
  ReleaseCapture();
  SendMessage( GWiz->Handle, WM_NCLBUTTONDOWN, HTCAPTION, 0 );
  }

}
//---------------------------------------------------------------------------

void __fastcall TGWiz::FormCreate(TObject *Sender)
{
char exepath[1024];
bool defaults_set;
String FileName;

GWiz->BackGround = GW_BACKGROUND_WHITE;
GWiz->ForeGround = GW_FOREGROUND_WHITE;

FileName = "GWiz.ico";
if( FileExists( FileName ) )
  {
  Application->Icon->LoadFromFile( FileName );
  }

FileName = "GWizGeneric.bmp";
if( FileExists( FileName ) )
  {
  GWiz->Image1->Picture->LoadFromFile( FileName );
  }

GWiz->Image1->Canvas->Brush->Color = GWiz->BackGround;
GWiz->Image1->Canvas->FillRect( Rect( 108, 117, 442, 382 ) );

GWiz->screen->Left = 115;
GWiz->screen->Top  = 121;

reg1 = CreateEllipticRgn(  64, 431, 117, 484 );
reg2 = CreateEllipticRgn( 136, 431, 188, 484 );
reg3 = CreateEllipticRgn( 361, 431, 413, 484 );
reg4 = CreateEllipticRgn( 432, 431, 485, 484 );

reg5 = CreatePolygonRgn( &CoKey5[0], 9, WINDING );
reg6 = CreatePolygonRgn( &CoKey6[0], 9, WINDING );
reg7 = CreatePolygonRgn( &CoKey7[0], 9, WINDING );
reg8 = CreatePolygonRgn( &CoKey8[0], 9, WINDING );

regOFF = CreateEllipticRgn( 79, 495, 101, 515 );
regRDS = CreateRectRgn( 462, 138, 500, 169 );

defaults_set = false;

GetModuleFileName( NULL, exepath, 1024 );
//String FileName = ExtractFilePath( (String)exepath );
//ExtractFileName( (String)exepath );
FileName = ChangeFileExt( (String)exepath, ".Fac" );

// create memory stream
TMemoryStream *Factors = new TMemoryStream;
RunFactorStatus = RFS_CLEAR;
if( FileExists( FileName ) )
  {
  Factors->LoadFromFile( FileName );
  Factors->Read( &pFac, sizeof( struct PreservedFactors ) );

  // set defaults if needed
  if( pFac.CheckSum != CheckSumIt( (unsigned char *)&pFac, (unsigned char *)&(pFac.CheckSum) ) )
    {
	RunFactorStatus = RFS_CS1FAIL;
    SetDefaultFactors();
    }
  else if( (pFac.software_type != SOFTWARE) ||
           (pFac.software_issue != ISSUE) ||
		   (pFac.software_revision != REVISION) )
    {
	RunFactorStatus = RFS_SW1FAIL;
    SetDefaultFactors();
    SetDefaultTotals();
    defaults_set = true;
    }
  }
else
  {
  RunFactorStatus = RFS_CS2FAIL;
  SetDefaultFactors();
  }

if( defaults_set == false )
  {
  FileName = ChangeFileExt( (String)exepath, ".Run" );
  // reset stream memory, size &count */
  Factors->Clear();
  if( FileExists( FileName ) )
    {
    Factors->LoadFromFile( FileName );
    Factors->Read( &pRun, sizeof( struct PreservedRuntime ) );

    if( pRun.CheckSum != CheckSumIt( (unsigned char *)&pRun, (unsigned char *)&(pRun.CheckSum) ) )
      {
      RunFactorStatus = RFS_CS3FAIL;
      SetDefaultTotals();
      }

    }
  else
    {
    RunFactorStatus = RFS_CS4FAIL;
    SetDefaultTotals();
    }
  }

delete Factors;

pRun.PowerCount++;

ClockRam[7] = 0x00;

sim_startup();
sim_main( 0 );

}
//---------------------------------------------------------------------------

void PrintToMemo( unsigned char *tail, unsigned char head, unsigned char *buff, TMemo *ptTmemo )
{
unsigned char c, *pt, temp_tail, line[256];
String st;

while( *tail != head )
  {
  Form3->Visible = true;

  temp_tail = *tail;
  while( (temp_tail != head) && (buff[temp_tail] != '\n') )
    {
    temp_tail++;
    }

  if( buff[temp_tail] == '\n' )
    {
    temp_tail++;
    pt = &line[0];
    while( *tail != temp_tail )
      {
      c = buff[(*tail)++]; /* increment value, not pointer */
      if( c == '\r' )
        {
        }
      else if( c == '\n' )
        {
        }
      else if( c < 32 )
        {
        *pt++ = '<';
        /* *pt++ = hex[c / 0x10];
        *pt++ = hex[c % 0x10];*/
        *pt++ = '>';
        }
      else
        {
        *pt++ = c;
        }
      }
    *pt = 0x00;
    ptTmemo->Lines->Add( (char *)line );
    }
  else
    {
    break;
    }
  }

}
//---------------------------------------------------------------------------

void __fastcall TGWiz::Timer1Timer(TObject *Sender)
{
	static unsigned char loop = 0xff;

	struct tm *time_now;
	time_t secs_now;

	if( PowerSwitchedOff )
	{
		GWiz->Close();
		return;
	}

	if( loop > 5 )
	{
		loop = 0;
		tzset();
		time( &secs_now );
		time_now = localtime( &secs_now );

		ClockRam[0] = btobcd( time_now->tm_sec ); // seconds
		ClockRam[1] = btobcd( time_now->tm_min ); // minutes
		ClockRam[2] = btobcd( time_now->tm_hour ); // hours
		ClockRam[3] = btobcd( time_now->tm_wday ); // day of week
		ClockRam[4] = btobcd( time_now->tm_mday ); // day of month
		ClockRam[5] = (((time_now->tm_year + 1900) > 2099) ? 0x00 : 0x80) | btobcd( time_now->tm_mon + 1 ); // century & month
		ClockRam[6] = btobcd( (time_now->tm_year + 1900) % 100 ); // year
	}
	loop++;

	if( savecal )
	{
		savecal = 0;
		// write factor to flash if needed
		pFac.CheckSum = CheckSumIt( (unsigned char *)&pFac, (unsigned char *)&(pFac.CheckSum) );

		GWiz->Shape1->Visible = false;
	}
	else
	{
		if( pFac.CheckSum != CheckSumIt( (unsigned char *)&pFac, (unsigned char *)&(pFac.CheckSum) ) )
		{
			GWiz->Shape1->Visible = true;
		}
	}

	sim_main( -1 );

	// buzlength is in ms, but timer is called every 100 ms

	if( buzz_length )
	{
		if( buzz_length >= 100 )
		{
			buzz_length -= 100;
		}
		else
		{
			buzz_length = 0;
		}
	}
	else if( off_length )
	{
		if( off_length >= 100 )
		{
			off_length -= 100;
		}
		else
		{
			off_length = 0;
		}
	}
	else if( buzz_count )
	{
		buzz_count--;
		buzz_length = BEEP_ON;
		off_length = BEEP_OFF;
	}

	/* HDC gwdc = GetDC( GWiz->Handle );
	   InvertRgn( gwdc, reg8 );
	   ReleaseDC( GWiz->Handle, gwdc );*/

	/*if( FormAbout->PrinterSim->Checked == true )
    {
    	PrintToMemo( &s0tx_tail, s0tx_head, s0tx_buff, Form3->Memo1 );
		PrintToMemo( &s1tx_tail, s1tx_head, s1tx_buff, Form3->Memo2 );
	}
	else
	{
		s0tx_tail = s0tx_head;
		s1tx_tail = s1tx_head;
	}*/
}
//---------------------------------------------------------------------------

void __fastcall TGWiz::Image1MouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{

if( PtInRegion( regOFF, X, Y ) ||
    PtInRegion( regRDS, X, Y ) ||
    PtInRegion( reg1, X, Y ) ||
    PtInRegion( reg2, X, Y ) ||
    PtInRegion( reg3, X, Y ) ||
    PtInRegion( reg4, X, Y ) ||
    PtInRegion( reg5, X, Y ) ||
    PtInRegion( reg6, X, Y ) ||
    PtInRegion( reg7, X, Y ) ||
    PtInRegion( reg8, X, Y ) )
  {
  Image1->Cursor = crHandPoint;
  }
else
  {
  Image1->Cursor = crDefault;
  }

}
//---------------------------------------------------------------------------

void __fastcall TGWiz::Image1MouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{

// cancel key press when mouse released
sim_main( 0 );

}
//---------------------------------------------------------------------------

void __fastcall TGWiz::FormClose(TObject *Sender, TCloseAction &Action)
{
char exepath[1024];
String FileName;

GetModuleFileName( NULL, exepath, 1024 );

FileName = ChangeFileExt( (String)exepath, ".Fac" );

pFac.CheckSum = CheckSumIt( (unsigned char *)&pFac, (unsigned char *)&(pFac.CheckSum) );

// create memory stream
TMemoryStream *Factors = new TMemoryStream;
Factors->Write( (unsigned char *)&pFac, sizeof( struct PreservedFactors ) );
Factors->SaveToFile( FileName );

FileName = ChangeFileExt( (String)exepath, ".Run" );

pRun.CheckSum = CheckSumIt( (unsigned char *)&pRun, (unsigned char *)&(pRun.CheckSum) );

// reset stream memory, size &count */
Factors->Clear();
Factors->Write( (unsigned char *)&pRun, sizeof( struct PreservedRuntime ) );
Factors->SaveToFile( FileName );

delete Factors;

}

