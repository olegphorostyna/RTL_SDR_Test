object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'Form1'
  ClientHeight = 579
  ClientWidth = 1250
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  OnCreate = FormCreate
  DesignSize = (
    1250
    579)
  TextHeight = 15
  object Label1: TLabel
    Left = 8
    Top = 347
    Width = 58
    Height = 15
    Caption = 'Device log:'
    FocusControl = Memo1
  end
  object TrackValue: TLabel
    Left = 8
    Top = 275
    Width = 55
    Height = 15
    Caption = 'TrackValue'
  end
  object avg_power: TLabel
    Left = 400
    Top = 400
    Width = 57
    Height = 15
    Caption = 'avg_power'
  end
  object StartRTLSDR: TButton
    Left = 8
    Top = 8
    Width = 161
    Height = 25
    Cancel = True
    Caption = 'OpenRTLSDRConnection'
    DisabledImageName = 'Button1'
    TabOrder = 0
    OnClick = StartRTLSDRClick
  end
  object Button2: TButton
    Left = 8
    Top = 120
    Width = 161
    Height = 25
    Caption = 'CloseRTLSDRConnection'
    TabOrder = 1
    OnClick = Button2Click
  end
  object Memo1: TMemo
    Left = 8
    Top = 447
    Width = 1234
    Height = 124
    ScrollBars = ssVertical
    TabOrder = 2
  end
  object Chart1: TChart
    Left = 192
    Top = 8
    Width = 1050
    Height = 385
    Legend.Visible = False
    Title.Text.Strings = (
      'TChart')
    Title.Visible = False
    BottomAxis.Axis.Width = 1
    ClipPoints = False
    LeftAxis.Axis.Width = 1
    View3D = False
    TabOrder = 3
    Anchors = [akLeft, akTop, akRight, akBottom]
    DefaultCanvas = 'TGDIPlusCanvas'
    PrintMargins = (
      15
      30
      15
      30)
    ColorPaletteIndex = 13
    object Series1: TFastLineSeries
      HoverElement = []
      SeriesColor = 15204364
      DrawAllPoints = False
      LinePen.Color = 15204364
      LinePen.Width = 0
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series2: TFastLineSeries
      HoverElement = []
      SeriesColor = 2631935
      LinePen.Color = 2631935
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
  end
  object Button1: TButton
    Left = 8
    Top = 64
    Width = 161
    Height = 25
    Caption = 'StartReading'
    TabOrder = 4
    OnClick = Button1Click
  end
  object Button3: TButton
    Left = 24
    Top = 232
    Width = 121
    Height = 25
    Caption = 'Simulate'
    TabOrder = 5
    OnClick = Button3Click
  end
  object TrackBar1: TTrackBar
    Left = 8
    Top = 296
    Width = 150
    Height = 45
    Max = 1000000
    Min = 1000
    ParentShowHint = False
    Position = 1000
    ShowHint = True
    TabOrder = 6
    OnChange = TrackBar1Change
  end
  object Button4: TButton
    Left = 8
    Top = 416
    Width = 75
    Height = 25
    Caption = 'ReadRawIQ'
    TabOrder = 7
    OnClick = Button4Click
  end
  object NumberBox1: TNumberBox
    Left = 273
    Top = 399
    Width = 121
    Height = 23
    Mode = nbmFloat
    TabOrder = 8
    OnKeyDown = NumberBox1KeyDown
  end
  object Button5: TButton
    Left = 192
    Top = 399
    Width = 75
    Height = 25
    Caption = 'Monitor'
    TabOrder = 9
    OnClick = Button5Click
  end
end
