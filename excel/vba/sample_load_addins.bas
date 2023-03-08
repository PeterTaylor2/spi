Attribute VB_Name = "sample_load_addins"

Option Explicit

Public Sub Auto_Open()
    sampleFunctions.sampleLoadXLL
    sampleUtils.SetManualCalculation
    sampleFunctions.sampleAddMenu
End Sub

Public Sub Auto_Close()
    sampleFunctions.sampleRemoveMenu
    sampleFunctions.sampleUnloadXLL
End Sub


Sub sampleCloseAddin()
    sampleFunctions.sampleRemoveMenu
    sampleFunctions.sampleUnloadXLL
    ThisWorkbook.Close
End Sub

Sub sampleAboutAddin()
     Dim s As String
     Dim version As String
     Dim alibexpiry As String
     Dim cplversion As String
     Dim rc As Integer
     
     version = Application.Run(theFunctionPrefix & ".Version")
     s = s & "Addins loaded using: " & ThisWorkbook.FullName & Chr(10) & Chr(10)
     s = s & version & Chr(10)
     s = s & copyright & Chr(10) & Chr(10)
      
     rc = MsgBox(s, 0, "About " & theTitle)
     
End Sub
