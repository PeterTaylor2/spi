Attribute VB_Name = "$(ns)Functions"
Option Explicit
Option Base 0

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' This module contains functions which are specific to a particular
' product - as determined by namespace.
'
' All functions provided use the namespace as a prefix to the function
' name.
'
' We expect the following functions to be defined elsewhere:
'
' $(ns)AboutAddin
'     Should use a message box to provide information about version,
'     copyright, acknowledgements etc.
'
' $(ns)CloseAddin
'     This should unregister the XLL and remove the menu.
'     We provide functions $(ns)UnloadXLL and $(ns)RemoveMenu
'     to help with this operation. However in case there is more that
'     needs to be done this should be hand coded in the main module.
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Private Const SERVICE_NAME As String = "$(serviceName)"
Private Const FUNCTION_PREFIX As String = "$(ns)"
Private Const XLL As String = "$(xll)"
Private Const START_LOGGING As String = "$(startLoggingFunction)"
Private Const STOP_LOGGING As String = "$(stopLoggingFunction)"
Private Const SET_ERROR_POPUPS As String = "$(setErrorPopups)"
Private Const START_TIMING As String = "$(startTimingFunction)"
Private Const STOP_TIMING As String = "$(stopTimingFunction)"
Private Const CLEAR_TIMINGS As String = "$(clearTimingsFunction)"

Public Sub $(name)LoadXLL
    Dim myPath As String
    Dim curPath As String

'
' Add the directory which contains this workbook as the DLL directory
' Then we can safely open the XLL and find all of its DLLs
'
    myPath = $(name)Utils.Dirname(ThisWorkbook.FullName)
    $(name)Utils.AddDLLPath (myPath)
    
'
' Register the XLL
'
    Application.StatusBar = "Loading " & SERVICE_NAME & " addins..."
    If Not (Application.RegisterXLL(XLL)) Then
        MsgBox ("Failed to load " + XLL)
    End If
    
    $(name)Utils.RemoveDLLPath
    $(name)Utils.FixMicrosoftSolverBug
    Application.StatusBar = False

End Sub

Public Sub $(name)UnloadXLL

    Application.ExecuteExcel4Macro ("RETURN(UNREGISTER(""" + XLL + """))")

End Sub

Public Sub $(name)AddMenu

'
' This subroutine is called immediately after the XLL has been successfully
' loaded. At this point we expect to define the menu. We will be using the
' SERVICE_NAME as the main menu item.
'
    Const menuBarName As String = "Worksheet Menu Bar"
    Dim conbar As CommandBarControl
    Dim helpBar As CommandBar
    Dim helpIndex As Integer

    Dim controlCount As Integer
    Dim myBar As CommandBarControl
    Dim myItems As CommandBar
    Dim myBarIndex As Integer

    Dim button As CommandBarControl

    ' delete the old menu

    For Each conbar In CommandBars(menuBarName).Controls
        If conbar.Caption = SERVICE_NAME Then
            conbar.Delete
        End If
    Next

    ' determine the index of the standard "Help" Command bar control
    controlCount = 0
    For Each conbar In CommandBars(menuBarName).Controls
        If conbar.Caption = "&Help" Then
            helpIndex = conbar.Index
            GoTo endofLoop
        Else
            controlCount = controlCount + 1
        End If
    Next
    helpIndex = controlCount

endofLoop:

    Set helpBar = CommandBars(menuBarName).Controls(helpIndex).CommandBar

    ' now add my menu item

    Set myBar = CommandBars(menuBarName).Controls.Add(msoControlPopup, before:=helpIndex, temporary:=True)
    myBar.Visible = True
    myBar.Caption = SERVICE_NAME
    myBarIndex = myBar.Index

    Set myItems = CommandBars(menuBarName).Controls(myBarIndex).CommandBar

    Set button = myItems.Controls.Add(msoControlButton)
    button.Caption = "&About " & SERVICE_NAME
    button.OnAction = "$(name)AboutAddin"
    button.BeginGroup = False ' don't add a separator bar before this control

    Set button = myItems.Controls.Add(msoControlButton)
    button.Caption = "&Close " & SERVICE_NAME
    button.OnAction = "$(name)CloseAddin"
    button.BeginGroup = False ' don't add a separator before this control

    Set button = myItems.Controls.Add(msoControlButton)
    button.Caption = "&Object Viewer"
    button.OnAction = "$(name)LoadObjectViewer"
    button.BeginGroup = True
    
    Set button = myItems.Controls.Add(msoControlButton)
    button.Caption = "&Start Logging..."
    button.OnAction = "$(name)StartLogging"
    button.BeginGroup = False

    Set button = myItems.Controls.Add(msoControlButton)
    button.Caption = "S&top Logging"
    button.OnAction = "$(name)StopLogging"
    button.BeginGroup = False
    
    Set button = myItems.Controls.Add(msoControlButton)
    button.Caption = "&Pop-up Errors On"
    button.OnAction = "$(name)PopupErrorsOn"
    button.BeginGroup = False

    Set button = myItems.Controls.Add(msoControlButton)
    button.Caption = "Pop-up Errors Off"
    button.OnAction = "$(name)PopupErrorsOff"
    button.BeginGroup = False
    
    Set button = myItems.Controls.Add(msoControlButton)
    button.Caption = "Start &Timing"
    button.OnAction = "$(name)StartTiming"
    button.BeginGroup = False

    Set button = myItems.Controls.Add(msoControlButton)
    button.Caption = "Stop Timing"
    button.OnAction = "$(name)StopTiming"
    button.BeginGroup = False

    Set button = myItems.Controls.Add(msoControlButton)
    button.Caption = "Clear Timings"
    button.OnAction = "$(name)ClearTimings"
    button.BeginGroup = False

    Set button = myItems.Controls.Add(msoControlButton)
    button.Caption = "&User Guide"
    button.OnAction = "$(name)LoadUserGuide"
    button.BeginGroup = True
    
    
End Sub

Public Sub $(name)RemoveMenu
'
' This subroutine should be called immediately before the add-in is unloaded.
' It will remove the menu defined by $(ns)AddMenu.
'
    Const menuBarName As String = "Worksheet Menu Bar"
    Dim conbar As CommandBarControl
    Dim item As CommandBarButton

    ' delete our menu

    For Each conbar In CommandBars(menuBarName).Controls
        If conbar.Caption = SERVICE_NAME Then
            conbar.Delete
        End If
    Next
End Sub

Private Sub $(name)StartLogging()
    ' starts logging allowing the user to choose the log filename
    Dim logfilename As String
    Dim result As String
    Dim filename As String

    filename = $(name)Utils.OutputFilename("Log Files (*.log), *.log")
    If filename <> "False" Then
        result = Application.Run(START_LOGGING, filename)
    End If
End Sub

Private Sub $(name)StopLogging()
    Dim result As Boolean
    result = Application.Run(STOP_LOGGING)
End Sub

Private Sub $(name)PopupErrorsOn()
    Dim result as Boolean
    result = Application.Run(SET_ERROR_POPUPS, True)
End Sub

Private Sub $(name)PopupErrorsOff()
    Dim result as Boolean
    result = Application.Run(SET_ERROR_POPUPS, False)
End Sub

Private Sub $(name)StartTiming()
    Dim result as Boolean
    result = Application.Run(START_TIMING)
End Sub

Private Sub $(name)StopTiming()
    Dim result as Boolean
    result = Application.Run(STOP_TIMING)
End Sub

Private Sub $(name)ClearTimings()
    Dim result as Boolean
    result = Application.Run(CLEAR_TIMINGS)
End Sub

Private Sub $(name)LoadObjectViewer()

    $(name)ObjectViewer.OBJECT_GET = "$(object_get)"
    $(name)ObjectViewer.OBJECT_FREE = "$(object_free)"
    $(name)ObjectViewer.OBJECT_COUNT = "$(object_count)"
    $(name)ObjectViewer.OBJECT_TO_STRING = "$(object_to_string)"
    $(name)ObjectViewer.Show
    
End Sub

Private Sub $(name)LoadUserGuide()

    Dim myDirname As String
    Dim pdfFilename As String
        myDirname = dirname(ThisWorkbook.FullName)

    myDirname = $(name)Utils.Dirname(ThisWorkbook.FullName)
    pdfFilename = $(name)Utils.Filename(myDirName, "$(pdf).pdf")
    
    $(name)Utils.ShellOper (pdfFilename)
    
End Sub


