Attribute VB_Name = "$(name)LoadXLL"
Option Explicit
Option Base 0

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' This module contains functions for loading and unloading a single XLL
' If you want to load and unload multiple XLLs then you should write
' your own version of this module and include that instead when building
' the .XLAM file.
'
' You should continue to call that module by the VB_Name above.
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Private Const SERVICE_NAME As String = "$(serviceName)"
Private Const XLL As String = "$(xll)"

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
        MsgBox("Failed to load " + XLL)
    End If
    
    $(name)Utils.RemoveDLLPath
    $(name)Utils.FixMicrosoftSolverBug
    Application.StatusBar = False

End Sub

Public Sub $(name)UnloadXLL

    Application.ExecuteExcel4Macro("RETURN(UNREGISTER(""" + XLL + """))")

End Sub

