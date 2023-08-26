Attribute VB_Name = "$(name)Utils"
Option Explicit
Option Base 0

#If VBA7 Then

Private Declare PtrSafe Function SetDllDirectory Lib "kernel32.dll" Alias "SetDllDirectoryA" ( _
    ByVal lpPathName As String) As Boolean

Private Declare PtrSafe Function GetDesktopWindow _
Lib "user32" () _
As LongPtr
 
Private Declare PtrSafe Function ShellExecute _
Lib "shell32.dll" _
Alias "ShellExecuteA" ( _
ByVal hwnd As LongPtr, _
ByVal lpOperation As String, _
ByVal lpFile As String, _
ByVal lpParameters As String, _
ByVal lpDirectory As String, _
ByVal nShowCmd As Long) _
As LongPtr
 
#Else

Private Declare Function SetDllDirectory Lib "kernel32.dll" Alias "SetDllDirectoryA" ( _
    ByVal lpPathName As String) As Boolean

Private Declare Function GetDesktopWindow _
Lib "user32" () _
As Long
 
Private Declare Function ShellExecute _
Lib "shell32.dll" _
Alias "ShellExecuteA" ( _
ByVal hwnd As Long, _
ByVal lpOperation As String, _
ByVal lpFile As String, _
ByVal lpParameters As String, _
ByVal lpDirectory As String, _
ByVal nShowCmd As Long) _
As Long

#End If

Private Const SW_HIDE As Long = 0
Private Const SW_NORMAL As Long = 1
Private Const SW_MAXIMIZE As Long = 3
Private Const SW_MINIMIZE As Long = 6

'
' This module contains functions which are not specific to a particular product
'

Public Function NumberOfArrayDimensions(Arr As Variant) As Integer
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' NumberOfArrayDimensions
' This function returns the number of dimensions of an array. An unallocated 
' dynamic array has 0 dimensions.
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Dim Ndx As Integer
Dim Res As Integer
On Error Resume Next
' Loop, increasing the dimension index Ndx, until an error occurs.
' An error will occur when Ndx exceeds the number of dimension
' in the array. Return Ndx - 1.
Do
    Ndx = Ndx + 1
    Res = UBound(Arr, Ndx)
Loop Until Err.Number <> 0

NumberOfArrayDimensions = Ndx - 1

End Function


Public Sub SetManualCalculation()

    ' open a new workbook
    ' force the calculation mode to be manual
    ' close the new workbook
    ' do it without screen updating so we don't get anything flashing on the 
    ' screen

    Application.ScreenUpdating = False
    Dim wb As Workbook
    Set wb = Workbooks.Add
    
    wb.Application.Calculation = xlCalculationManual
    wb.Application.CalculateBeforeSave = False
    
    wb.Close False
    
    Application.ScreenUpdating = True

End Sub

'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' ShellExecute API.
' Use Windows file associations to provide the easiest way to
' process operations > Open, Print,Play or Explore
' by passing just the FullPathFilename of the item.
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Public Function ShellOper(strFileExe As String, _
    Optional strOperation As String, _
    Optional nShowCmd As Double) As Long
     
    Dim hWndDesk As Long
     
    hWndDesk = GetDesktopWindow()
    If Len(strOperation) = 0 Then strOperation = "Open"
    If Len(Dir(strFileExe)) = 0 Then GoTo ErrH
     
     '// Failure >> <=32
    ShellOper = ShellExecute(hWndDesk, strOperation, strFileExe, 0, 0, nShowCmd)
    If ShellOper <= 32 Then
        MsgBox "Couldn't " & strOperation & " " & strFileExe
    End If
     
    Exit Function
ErrH:
    ShellOper = -1
End Function
 
  
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' This function sets the given path as the DLL directory
'
' This enables you to find all DLLs in the given directory
'
' Better than temporarily changing to the given directory since in some cases
' other threads might change it back to something else.
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Public Function AddDLLPath(ByRef path As String) As Boolean
    Dim s As String
    s = path & Chr(0)
    AddDLLPath = SetDllDirectory(s)
    
End Function

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Reverses the action of AddDLLPath.
'
' This doesn't restore state - it just resets the DLL path to an
' empty string.
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Public Function RemoveDLLPath() As Boolean
    
    RemoveDLLPath = AddDLLPath("")

End Function

'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Selects an output file for writing via a dialog window using a filter
' (e.g. *.log)
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Function OutputFilename(filter As String) As String
    Dim myDirname As String

    myDirname = Dirname(ThisWorkbook.FullName)
    OutputFilename = Application.GetSaveAsFilename(myDirname, filter)
End Function

'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Selects an output file for reading via a dialog window using a filter
' (e.g. *.log)
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Function InputFileName(filter As String) As String
    Dim myDirname As String

    myDirname = Dirname(ThisWorkbook.FullName)
    ChDir myDirname
    ChDrive myDirname
    InputFileName = Application.GetOpenFilename(filter)
End Function

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Returns the directory name from the full path.
' Equivalent to the dirname command in a unix shell.
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Function Dirname(myFilename As String) As String
    Dim i As Integer

    i = 0
    While InStr(i + 1, myFilename, Application.PathSeparator) > 0
        i = InStr(i + 1, myFilename, Application.PathSeparator)
    Wend
    If i = 0 Then
        Dirname = CurDir
    Else
        Dirname = Left(myFilename, i - 1)
    End If
End Function

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Returns the last part of the file name from the full path.
' Equivalent to the basename command in a unix shell.
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Function Basename(myFilename As String) As String
    Dim i As Integer

    i = 0
    While InStr(i + 1, myFilename, Application.PathSeparator) > 0
        i = InStr(i + 1, myFilename, Application.PathSeparator)
    Wend
    Basename = Right(myFilename, Len(myFilename) - i)
End Function

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Combines directory name and file name to create a full file name.
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Function Filename(myDirname As String, myBasename As String) As String
    Filename = myDirname + Application.PathSeparator + myBasename
End Function
