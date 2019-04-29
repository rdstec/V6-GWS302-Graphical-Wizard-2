object Form3: TForm3
  Left = 741
  Top = 123
  BorderStyle = bsDialog
  Caption = 'Serial Ports'
  ClientHeight = 539
  ClientWidth = 310
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 0
    Width = 57
    Height = 13
    Caption = 'Serial Port 1'
  end
  object Label2: TLabel
    Left = 0
    Top = 272
    Width = 57
    Height = 13
    Caption = 'Serial Port 2'
  end
  object Memo1: TMemo
    Left = 0
    Top = 16
    Width = 305
    Height = 248
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object Memo2: TMemo
    Left = 0
    Top = 288
    Width = 305
    Height = 248
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 1
  end
end
