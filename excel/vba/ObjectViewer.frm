VERSION 5.00
Begin {C62A69F0-16DC-11CE-9E98-00AA00574A4F} $(name)ObjectViewer 
   Caption         =   "Object Viewer"
   ClientHeight    =   9510
   ClientLeft      =   45
   ClientTop       =   435
   ClientWidth     =   15780
   OleObjectBlob   =   "$(name)ObjectViewer.frx":0000
   ShowModal       =   0   'False
   StartUpPosition =   1  'CenterOwner
End
Attribute VB_Name = "$(name)ObjectViewer"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Option Explicit

Private objectHandle As String

Public OBJECT_COUNT As String
Public OBJECT_GET As String
Public OBJECT_TO_STRING As String
Public OBJECT_FREE As String



Private Sub DisplayAsDate_Click()
    populateFieldData
End Sub

Private Sub FieldNames_Click()
    
    populateFieldData
    
End Sub

Private Sub Recalculate_Click()
    populateObjectData
End Sub
Private Sub populateGlobalStats()

    ObjectCount.value = Application.Run(OBJECT_COUNT)

End Sub
Private Sub populateObjectData()

    Dim objectEncode As Variant
    Dim objectFields As Variant
    Dim activeCellName As Variant
            
    objectHandle = Application.ActiveCell.value

    Dim objectEncodeType As String
    
    Dim i As Integer
    Dim j As Integer
    Dim NDims As Integer
       
   
    EncodedObject.Clear
    FieldNames.Clear
    SelectedFieldValue.Clear
    SelectedFieldName.value = ""
       
        
    activeCellName = Application.ActiveCell.Address
    CellName.value = activeCellName
    CellValue.value = objectHandle
    
    objectEncode = Application.Run(OBJECT_TO_STRING, objectHandle)
    objectFields = Application.Run(OBJECT_GET, objectHandle)
    
    objectEncodeType = TypeName(objectEncode)
  
    If objectEncodeType = "Error" Then
        MsgBox ("'" & objectHandle & "' is not a valid object handle or cannot be encoded")
    Else
        For i = LBound(objectEncode) + 1 To UBound(objectEncode)
            EncodedObject.AddItem (objectEncode(i, 1))
        Next
        NDims = $(name)Utils.NumberOfArrayDimensions(objectFields)
        If NDims = 2 Then
            For i = LBound(objectFields) To UBound(objectFields)
               FieldNames.AddItem (objectFields(i, 1))
            Next
        Else
            For i = LBound(objectFields) To UBound(objectFields)
                FieldNames.AddItem (objectFields(i))
            Next
        End If
    End If
    
    populateGlobalStats
    
End Sub

Private Sub populateFieldData()

    Dim fieldName As String
    Dim fieldValue As Variant
    Dim fieldObject As Variant
    Dim fieldObjectType As String
    Dim displayValues As Variant
    Dim displayValuesType As String
    Dim numDimensions As Integer
    Dim asDate As Boolean
    Dim isObject As Boolean
    Dim result As Variant
        
    Dim i As Integer
    
    SelectedFieldValue.Clear
    
    fieldName = FieldNames.value
    asDate = DisplayAsDate.value
    
    SelectedFieldName.value = fieldName
    
    fieldValue = Application.Run(OBJECT_GET, objectHandle, fieldName)
    
    ' is the fieldValue an objectHandle?
    ' we will test whether we can encode it to determine this
    
    fieldObject = Application.Run(OBJECT_TO_STRING, fieldValue)
    fieldObjectType = TypeName(fieldObject)
    
    If fieldObjectType = "Error" Then
        displayValues = fieldValue
    Else
        displayValues = fieldObject
        ' having extracted and encoded the object we will free it to save memory
        ' since we are not calculating from a cell the handle will not otherwise
        ' be re-used
        result = Application.Run(OBJECT_FREE, fieldValue)
    End If
    
    If TypeName(displayValues) = "Error" Then
        SelectedFieldValue.AddItem ("Cannot extract object value")
    Else
        If IsArray(displayValues) Then
            numDimensions = $(name)Utils.NumberOfArrayDimensions(displayValues)
            If numDimensions = 2 Then
                For i = LBound(displayValues) To UBound(displayValues)
                    addItemToSelectedFieldValue displayValues(i, 1), asDate
                    'SelectedFieldValue.AddItem (displayValues(i, 1))
                Next
            Else
                For i = LBound(displayValues) To UBound(displayValues)
                    addItemToSelectedFieldValue displayValues(i), asDate
                Next
            End If
            
        Else
            addItemToSelectedFieldValue displayValues, asDate
        End If
    End If
    
    populateGlobalStats

End Sub

Private Sub addItemToSelectedFieldValue(value As Variant, asDate As Boolean)

    Dim valueType As String
    Dim dateValue As String
        
    valueType = TypeName(value)
    
    If asDate And valueType = "Double" Then
        dateValue = Format(value, "yyyy-mm-dd")
        SelectedFieldValue.AddItem (dateValue)
    Else
        SelectedFieldValue.AddItem (value)
    End If
    
End Sub


Private Sub UserForm_Activate()
    populateObjectData
End Sub

 
