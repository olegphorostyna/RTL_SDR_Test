object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'RTL-SDR a'#1085#1072#1083#1110#1079#1072#1090#1086#1088' c'#1087#1077#1082#1090#1088#1091'  v1.0'
  ClientHeight = 840
  ClientWidth = 1254
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnShow = FormShow
  DesignSize = (
    1254
    840)
  TextHeight = 15
  object Label1: TLabel
    Left = 8
    Top = 690
    Width = 83
    Height = 15
    Caption = #1055#1086#1074#1110#1076#1086#1084#1083#1077#1085#1085#1103':'
    FocusControl = Memo1
  end
  object TrackValue: TLabel
    Left = 8
    Top = 413
    Width = 43
    Height = 15
    Caption = #1063#1072#1089#1090#1086#1090#1072
  end
  object avg_power: TLabel
    Left = 8
    Top = 318
    Width = 57
    Height = 15
    Caption = 'avg_power'
  end
  object Label2: TLabel
    Left = 8
    Top = 240
    Width = 111
    Height = 15
    Caption = #1062#1077#1085#1090#1088#1072#1083#1100#1085#1072' '#1095#1072#1089#1090#1086#1090#1072
  end
  object Label3: TLabel
    Left = 8
    Top = 160
    Width = 64
    Height = 15
    Caption = #1055#1110#1076#1089#1080#1083#1077#1085#1085#1103
  end
  object StartRTLSDR: TButton
    Left = 8
    Top = 8
    Width = 161
    Height = 25
    Cancel = True
    Caption = #1055#1086#1096#1091#1082' '#1087#1088#1080#1089#1090#1088#1086#1102
    DisabledImageName = 'Button1'
    TabOrder = 0
    OnClick = StartRTLSDRClick
  end
  object Button2: TButton
    Left = 8
    Top = 120
    Width = 161
    Height = 25
    Caption = #1042#1110#1076#1082#1083#1102#1095#1080#1090#1080
    TabOrder = 1
    OnClick = Button2Click
  end
  object Memo1: TMemo
    Left = 8
    Top = 711
    Width = 1238
    Height = 124
    ScrollBars = ssVertical
    TabOrder = 2
  end
  object Chart1: TChart
    Left = 192
    Top = 8
    Width = 1054
    Height = 407
    Legend.Visible = False
    Title.Text.Strings = (
      'TChart')
    Title.Visible = False
    BottomAxis.Axis.Width = 1
    ClipPoints = False
    LeftAxis.Axis.Width = 1
    LeftAxis.ExactDateTime = False
    LeftAxis.LabelsFormat.Margins.Left = 0
    LeftAxis.LabelsFormat.Margins.Right = 0
    View3D = False
    TabOrder = 3
    Anchors = [akLeft, akTop, akRight, akBottom]
    DefaultCanvas = 'TGDIPlusCanvas'
    PrintMargins = (
      15
      31
      15
      31)
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
    Caption = #1063#1080#1090#1072#1090#1080' '#1087#1086#1090#1110#1082
    TabOrder = 4
    OnClick = Button1Click
  end
  object Button3: TButton
    Left = 8
    Top = 382
    Width = 161
    Height = 25
    Caption = #1058#1077#1089#1090#1086#1074#1080#1081' '#1089#1080#1075#1085#1072#1083
    TabOrder = 5
    OnClick = Button3Click
  end
  object TrackBar1: TTrackBar
    Left = 8
    Top = 434
    Width = 161
    Height = 21
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
    Top = 351
    Width = 161
    Height = 25
    Caption = #1042#1110#1076#1086#1073#1088#1072#1079#1080#1090#1080' IQ '#1076#1072#1085#1110
    TabOrder = 7
    OnClick = Button4Click
  end
  object NumberBox1: TNumberBox
    Left = 8
    Top = 261
    Width = 161
    Height = 23
    Mode = nbmFloat
    TabOrder = 8
    OnKeyDown = NumberBox1KeyDown
  end
  object Button5: TButton
    Left = 8
    Top = 290
    Width = 161
    Height = 25
    Caption = #1052#1086#1085#1110#1090#1086#1088#1080#1090#1080
    TabOrder = 9
    OnClick = Button5Click
  end
  object TrackBar2: TTrackBar
    Left = 8
    Top = 189
    Width = 161
    Height = 33
    TabOrder = 10
  end
  object infoPanel: TPanel
    Left = 8
    Top = 461
    Width = 161
    Height = 199
    Caption = 'infoPanel'
    ShowCaption = False
    TabOrder = 11
    object Label4: TLabel
      Left = 8
      Top = 8
      Width = 39
      Height = 15
      Caption = #1057#1090#1072#1090#1091#1089':'
    end
    object Label5: TLabel
      Left = 16
      Top = 77
      Width = 114
      Height = 15
      Caption = #1063#1072#1089#1090#1086#1090#1072' '#1076#1080#1089#1088#1077#1090#1080#1079#1072#1094#1110#1111
    end
    object Label6: TLabel
      Left = 16
      Top = 112
      Width = 59
      Height = 15
      Caption = #1056#1086#1079#1084#1110#1088' FFT'
    end
    object Label7: TLabel
      Left = 16
      Top = 152
      Width = 64
      Height = 15
      Caption = #1055#1110#1076#1089#1080#1083#1077#1085#1085#1103
    end
    object Label8: TLabel
      Left = 16
      Top = 37
      Width = 80
      Height = 15
      Caption = #1030#1084#39#1103' '#1087#1088#1080#1089#1090#1088#1086#1102
    end
    object Label9: TLabel
      Left = 85
      Top = 9
      Width = 69
      Height = 15
      Caption = #1042#1080#1082#1086#1085#1091#1108#1090#1100#1089#1103
    end
    object infoDevName: TLabel
      Left = 32
      Top = 56
      Width = 20
      Height = 15
      Caption = '----'
    end
    object infoSampleRate: TLabel
      Left = 32
      Top = 91
      Width = 20
      Height = 15
      Caption = '----'
    end
    object infoFFTSize: TLabel
      Left = 32
      Top = 131
      Width = 20
      Height = 15
      Caption = '----'
    end
    object infoGain: TLabel
      Left = 32
      Top = 173
      Width = 20
      Height = 15
      Caption = '----'
    end
  end
  object Panel2: TPanel
    Left = 192
    Top = 421
    Width = 1054
    Height = 276
    Align = alCustom
    Caption = 'Panel2'
    TabOrder = 12
    object Image1: TImage
      Left = 24
      Top = 12
      Width = 1024
      Height = 251
    end
  end
end
