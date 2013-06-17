//##Agregar licencia y avisos de copyright de terceros en TODOS LOS headers y cpp.

#include "Bubble.h"

#include <wx/msgdlg.h> //##Debug, se va en release. Agregar compilación condicional.
#include <wx/dir.h>
#include <wx/filename.h>

#if defined (WIN32)
//##@# Possible non-portable code:
#include <windows.h>
#endif

//##2010.10.04: Ver que esto no dé error de runtime: Dejar este mensaje interno, aunque pasarlo a
//inglés, por si se reportara un bug al respecto en el futuro. Al menos dejarlo hasta ver bien cómo
//trabaja WX_DEFINE_OBJARRAY() y ver por qué en algunos compiladores, si se usa este macro sin tener
//en el scope la full-declaration de la clase de los elementos del array, ni siquiera dan warning:
//WX_DEFINE_OBJARRAY(arrayOfCanvas);


//##Ver si hay que usar un wxList acá, o si se usará el sistema de tabs, haciendo que Bubble herede
//de las clases para manejar NoteBooks y Tabs:

//We used wxList instead of wxObjArray because the list does not deletes the objects, which is already
//done by de parent-subsystem in the frame or dialog to which those objets belongs too. Anyway, these
//are canvases, and with a wxList is enough.
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(listOfCanvas);


WX_DEFINE_OBJARRAY(arrayOfBlockInfo);

//##Ver cómo agregar chequeo de que si no setean el parent y el notifier, TODO MAL!
/*
Bubble::Bubble(IBubbleNotifier *notifier) : parent(NULL),
                                            currentCanvas(NULL),
                                            notifier(notifier)
*/

//##Más adelante, puede que arme otro constructor, al que se pasen ambos parámetros (parent y notifier), pero
//tengo que asegurarme que no haya problemas en la destrucción, por las cosas que destruye en parent
//automáticamente, cosa que en esta configuración actual, en la que con este constructor es posible crear
//la instancia en el stack (estática) en vez del heap, y luego hacer los "sets", funciona bien.

//El parent muchas veces no se puede pasar en el constructor de la instancia de Bubble (ya que a veces, como
//en Minibloq, el parent se crea DESPUÉS que la instancia de Bubble), por lo que tampoco se pasa el notifier,
//por consistencia.
Bubble::Bubble(wxLocale& locale) :  parent(NULL),
                                    currentCanvas(NULL),
                                    actionPicker(NULL),
                                    bubbleXML(this, locale),
                                    actionDataType(wxString("")),
                                    variableInitName(wxString("")),
                                    notifier(NULL),
                                    bootPortName(wxString("")),

                                    hardwareManager(NULL),
                                    boardName(wxString("")),

                                    blocksEnabled(true),
                                    visibleLabels(false),

                                    componentsRepositoryPath(wxString("")),
                                    toolsPath(wxString("")),

                                    boardPath(wxString("")),
                                    corePath(wxString("")),
                                    matrixPath(wxString("")),
                                    libPath(wxString("")),
                                    blocksPath(wxString("")),

                                    appPath(wxString("")),
                                    themePath(wxString("")),

                                    //##Will be necessary?:
                                    docPath(wxString("")),

                                    componentPath(wxString("")),
                                    outputPath(wxString("")),

                                    simplifyCode(false)
{
    //##Falan implementar cosas...?
    saved = true;   //##Ver cómo funciona el tema de la herencia con clases abstractas, si se pueden
                    //inicializar las variables en la lista del constructor de la clase abstracta...
}


Bubble::~Bubble()
{
    //##No va: Como los elementos de la lista son descendientes de wxPanel, y tienen sus parents,
    //el frame o dialog al que pertenecen los destruirá. Y aquí, como el objeto canvases es estático,
    //se asegura la llamada a su destructor sin hacer nada:
    //canvases.Clear();

    //##Ver que funcione bien esto:
    if (bootSerialPort.IsOpen())
        bootSerialPort.Close();

    //Destroy all the pickers:
    if (actionPicker)
    {
        actionPicker->Destroy();
        actionPicker = NULL; //Not necessary, but for extra security.
    }
    clearExpressionPickers();
}

#if defined (WIN32)
//##Ver si hago que MinibloqRun llame a ese header en vez de redefinir esta función:
LPWSTR Bubble::cstrToWChar(LPCSTR value)
{
    LPWSTR result = NULL;
    int valueLen = lstrlenA(value);
    int resultLen = MultiByteToWideChar(CP_ACP, 0, value, valueLen, 0, 0);
    if (resultLen > 0)
    {
        result = SysAllocStringLen(0, resultLen);
        MultiByteToWideChar(CP_ACP, 0, value, valueLen, result, resultLen);
    }
    return result;
}
#endif

wxString Bubble::bool2string(const bool value)
{
    if (value)
        return wxString("true");
    return wxString("false");
}


//NOTE: Only checks for "true", any other string is false:
bool Bubble::string2bool(const wxString &value)
{
    if (value == wxString("true"))
        return true;
    return false;
}


bool Bubble::nonListedCharsInString(const wxString &charList, const wxString &value)
{
    for (unsigned int i=0; i<value.Len(); i++)
    {
        if (charList.Find(value[i]) == wxNOT_FOUND)
            return true;
    }
    return false;
}


#if UNDER_DEVELOPMENT
//##No borrar, este código es muy útil: Es una función DC2Bitmap bastante rápida, aunque los problemas que
//tiene si se la quiere usar para capturar los bitmaps asociados a los bloques son los siguientes:
//
//- Sólo toma los bloques si éstos están visibles en pantalla.
//- Además, no captura el dc de comboboxes y edits.
//
//Se la puede llamar con cosas tipo:
//
//  wxClientDC dc(currentBlock);
//  wxBitmap bmp(dc2Image(&dc));
//
static wxImage dc2Image(wxClientDC *dc) //##Pasar el puntero a constante.
{
    if (dc)
    {
        wxCoord dcW, dcH,
                dcX, dcY;
        dc->GetSize(&dcW, &dcH);
        wxImage tempImage(dcW, dcH);
        wxColour tempColour(0, 0, 0);
        for (dcX = 0; dcX < dcW; dcX++)
        {
            for (dcY = 0; dcY < dcH; dcY++)
            {
                if (dc->GetPixel(dcX, dcY, &tempColour))
                {
                    tempImage.SetRGB(dcX, dcY, tempColour.Red(), tempColour.Green(), tempColour.Blue());
                }
            }
        }
        return tempImage;
    }

    //If something goes wrong, return a dummy:
    return wxImage();
}
#endif


bool Bubble::isSaved() const
{
    //##Futuro: Debe recorrer todos los canvases y devolver el AND de todo eso.
    if (currentCanvas)
        return saved && currentCanvas->isSaved();

    return saved;
}


//##This function MUST be called after the construction (this may be improved in the future):
void Bubble::setCanvasesParent(wxWindow *value)
{
    parent = value;
}


void Bubble::setNotifier(IBubbleNotifier *value)
{
    notifier = value;
}


IBubbleNotifier *Bubble::getNotifier()
{
    return notifier;
}


void Bubble::closeTemporalPickers()
{
    BubbleExpressionPicker* iterator = NULL;
    for (unsigned int i = 0; i<expressionPickers.GetCount(); i++)
    {
        iterator = expressionPickers.Item(i);
        if (iterator)
        {
            if (iterator->getCloseAfterPick())
                iterator->Hide();
        }
    }
};


BubbleExpressionPicker* Bubble::getExpressionPicker(const wxString &dataType)
{
    //##Debug:
    if (getNotifier() == NULL)
        return NULL;
    //getNotifier()->showMessage(dataType + wxString("\n"), false, true, *wxGREEN); //##Debug.

    //##Optimize in the future (binary search, if possible):
    BubbleExpressionPicker* iterator = NULL;
    for (unsigned int i = 0; i<expressionPickers.GetCount(); i++)
    {
        //getNotifier()->showMessage((wxString("\n") << expressionPickers.GetCount()) + wxString("\n"), false, true, *wxRED); //##Debug.
        iterator = expressionPickers.Item(i);
        if (iterator)
        {
            //getNotifier()->showMessage(iterator->getDataType() + wxString("\n"), false, true, *wxRED); //##Debug.
            //getNotifier()->showMessage(dataType + wxString("\n"), false, true, *wxRED); //##Debug.
            if (iterator->getDataType() == dataType)
            {
                //getNotifier()->showMessage(_("Picker FOUND\n"), false, true, *wxRED); //##Debug.
                return iterator;
            }
        }
    }

//##Volar esto:
//    arrayOfExpressionPickers::iterator iter;
//    for (iter = expressionPickers.begin(); iter != expressionPickers.end(); ++iter)
//    {
//        BubbleExpressionPicker *currentPicker = *iter;
//        if (currentPicker)
//        {
//            //##getNotifier()->showMessage(currentPicker->getDataType() + wxString("\n"), false, true, *wxRED); //##Debug.
//            if (currentPicker->getDataType() == dataType)
//            {
//                getNotifier()->showMessage(_("Picker FOUND\n"), false, true, *wxRED); //##Debug.
//                return currentPicker;
//            }
//        }
//    }
    return NULL;
}


void Bubble::clearExpressionPickers()
{
    //This does not call expressionPickers.Clear(), because the pickers are dialogs (at least in the)
    //current implementation, and it's better to destroy them calling iterator->Destroy(), not with the
    //delete operator.
    //Then this functions empties the array, but Empty() does not call the delete operator.

    //##If the pickers change to other type of thing instead of dialogs (for example panels), then this will
    //need to be done in a different way, probably calling parent's removeChild() function.

    BubbleExpressionPicker* iterator = NULL;
    for (unsigned int i = 0; i<expressionPickers.GetCount(); i++)
    {
        iterator = expressionPickers.Item(i);
        if (iterator)
        {
            iterator->Destroy();
            iterator = NULL; //For security only.
        }
    }
    expressionPickers.Empty();
}


bool Bubble::addBlockToPicker(BubbleBlockInfo *block, wxWindow *pickersParent)
{
    if (block == NULL)
        return false;
    if (pickersParent == NULL)
        return false;

    //##Debug:
    if (getNotifier() == NULL)
        return false;

    BubblePicker *picker = NULL;
    if (block->getDataType() == getActionDataType())
    {
        picker = actionPicker;
    }
    else
    {
        //##NOTA: No voy a agregar una lista de dataTypes en un XML, sino que ésta se armará aquí automáticamente.
        //Esto no sólo es mucho más amigable para con los desarrolladores de bloques, sino que es la única opción
        //que se me ocurre, realmente compatible con la filosofía "Minibloq" de agregar, y no modificar, ya que
        //para agregar nuevos tipos de datos, basta con agregar los bloques que los utilizan, sin andar listándolos
        //explícitamente por ahí. La contra es que es un poco más lento al cargar los bloques.
        picker = getExpressionPicker(block->getDataType());
        if (picker == NULL)
        {
            //getNotifier()->showMessage(block->getDataType() + wxString("\n"), false, true, *wxRED); //##Debug.
            BubbleExpressionPicker *newPicker = new BubbleExpressionPicker( pickersParent,
                                                                            this,
                                                                            wxNewId(),
                                                                            //##Ver si en el futuro se podrá internacionalizar algo,
                                                                            //o mejorar estas descripciones agregando algún archivo
                                                                            //de documentación, pero por ahora van así:
                                                                            block->getDataType(),
                                                                            block->getDataType(),
                                                                            64, //32, //##
                                                                            wxColour(0, 0, 0), //##Ver tema color.
                                                                            7 //Cols
                                                                          );
            if (newPicker)
            {
                //getNotifier()->showMessage(_("New Picker = ") + block->getDataType() + wxString("\n"), false, true, *wxRED); //##Debug.
                newPicker->setCloseAfterPick(true);
                //getNotifier()->showMessage(newPicker->getDataType() + wxString("\n"), false, true, *wxRED); //##Debug.
                expressionPickers.Add(newPicker);
                //##Falta destruir los pickers al final, o cuando se hace el reload.
                picker = newPicker;
            }
        }
    }

    if (picker)
    {
        picker->addButton(  block->getName(),
                            block->getFunction(),
                            block->getDataType(),
                            block->getToolTip(),
                            block->getDefaultBackgroundColour1(),
                            block->getPickerDefaultImage(),
                            block->getPickerPressedImage(),
                            block->getPickerHoverImage(),
                            block->getPickerDisabledImage()
                         );
    }
    return true;
}


const BubbleBlockInfo& Bubble::getBlockInfo(const wxString& name, const wxString& function)
{
    return bubbleXML.getBlockInfo(name, function);
}


bool Bubble::blockIsValid(const wxString& name, const wxString& type) const
{
    return bubbleXML.blockIsValid(name, type);
}


int Bubble::loadBlocksInfo(wxWindow *pickersParent, bool showPickers)
{
    return bubbleXML.loadBlocksInfo(pickersParent, showPickers);
}


void Bubble::changeBoardPaths()
{
    //wxMessageDialog dialog0(parent, getBoardName(), T("0")); //##Debug
    //dialog0.ShowModal(); //##Debug

    //##Horrible! Un-hardcode all of this:
    if (getBoardName() == wxString("DuinoBot.v1.x.HID"))
    {
        setToolsPath(getComponentsRepositoryPath() + wxString("/WinAVR/v20090313/bin")); //##Un-hardcode...
        setLibPath((getAppPath().BeforeLast(wxFileName::GetPathSeparator())) + wxString("/Lib"));
        setBlocksPath(getLibPath() + wxString("/CPP/Blocks"));

        setMatrixPath(getLibPath() + wxString("/CPP/Targets/DuinoBot.v1.x.HID_Arduino.v1.0/Matrix"));
        setCorePath(getLibPath() + wxString("/CPP/Targets/DuinoBot.v1.x.HID_Arduino.v1.0/Core"));
        setBoardPath(getLibPath() + wxString("/CPP/Targets/DuinoBot.v1.x.HID_Arduino.v1.0"));
    }
    else if (getBoardName() == wxString("DuinoBot.v1.x"))
    {
        setToolsPath(getComponentsRepositoryPath() + wxString("/WinAVR/v20090313/bin")); //##Un-hardcode...
        setLibPath((getAppPath().BeforeLast(wxFileName::GetPathSeparator())) + wxString("/Lib"));
        setBlocksPath(getLibPath() + wxString("/CPP/Blocks"));

        setMatrixPath(getLibPath() + wxString("/CPP/Targets/DuinoBot.v1.x_Arduino.v1.0/Matrix"));
        setCorePath(getLibPath() + wxString("/CPP/Targets/DuinoBot.v1.x_Arduino.v1.0/Core"));
        setBoardPath(getLibPath() + wxString("/CPP/Targets/DuinoBot.v1.x_Arduino.v1.0"));
    }
    else if (getBoardName() == wxString("DuinoBot.Kids.v1.x"))
    {
        setToolsPath(getComponentsRepositoryPath() + wxString("/WinAVR/v20090313/bin")); //##Un-hardcode...
        setLibPath((getAppPath().BeforeLast(wxFileName::GetPathSeparator())) + wxString("/Lib"));
        setBlocksPath(getLibPath() + wxString("/CPP/Blocks"));

        setMatrixPath(getLibPath() + wxString("/CPP/Targets/DuinoBot.Kids.v1.x_Arduino.v1.0/Matrix"));
        setCorePath(getLibPath() + wxString("/CPP/Targets/DuinoBot.Kids.v1.x_Arduino.v1.0/Core"));
        setBoardPath(getLibPath() + wxString("/CPP/Targets/DuinoBot.Kids.v1.x_Arduino.v1.0"));
    }
    else if (getBoardName() == wxString("Seeeduino v2.2x Mega328"))
    {
        setToolsPath(getComponentsRepositoryPath() + wxString("/WinAVR/v20090313/bin")); //##Un-hardcode...
        setLibPath((getAppPath().BeforeLast(wxFileName::GetPathSeparator())) + wxString("/Lib"));
        setBlocksPath(getLibPath() + wxString("/CPP/Blocks"));

        setMatrixPath(getLibPath() + wxString("/CPP/Targets/ArduinoUNO_Arduino.v1.0/Matrix"));
        setCorePath(getLibPath() + wxString("/CPP/Targets/ArduinoUNO_Arduino.v1.0/Core"));
        setBoardPath(getLibPath() + wxString("/CPP/Targets/ArduinoUNO_Arduino.v1.0"));
    }
    else if (getBoardName() == wxString("Seeeduino Mega 1280"))
    {
        setToolsPath(getComponentsRepositoryPath() + wxString("/WinAVR/v20090313/bin")); //##Un-hardcode...
        setLibPath((getAppPath().BeforeLast(wxFileName::GetPathSeparator())) + wxString("/Lib"));
        setBlocksPath(getLibPath() + wxString("/CPP/Blocks"));

        setMatrixPath(getLibPath() + wxString("/CPP/Targets/ArduinoMega1280_Arduino.v1.0/Matrix"));
        setCorePath(getLibPath() + wxString("/CPP/Targets/ArduinoMega1280_Arduino.v1.0/Core"));
        setBoardPath(getLibPath() + wxString("/CPP/Targets/ArduinoMega1280_Arduino.v1.0"));
    }
    else if (getBoardName() == wxString("Arduino Uno"))
    {
        setToolsPath(getComponentsRepositoryPath() + wxString("/WinAVR/v20090313/bin")); //##Un-hardcode...
        setLibPath((getAppPath().BeforeLast(wxFileName::GetPathSeparator())) + wxString("/Lib"));
        setBlocksPath(getLibPath() + wxString("/CPP/Blocks"));

        setMatrixPath(getLibPath() + wxString("/CPP/Targets/ArduinoUNO_Arduino.v1.0/Matrix"));
        setCorePath(getLibPath() + wxString("/CPP/Targets/ArduinoUNO_Arduino.v1.0/Core"));
        setBoardPath(getLibPath() + wxString("/CPP/Targets/ArduinoUNO_Arduino.v1.0"));
    }
    else if (getBoardName() == wxString("Arduino Duemilanove Mega168"))
    {
        setToolsPath(getComponentsRepositoryPath() + wxString("/WinAVR/v20090313/bin")); //##Un-hardcode...
        setLibPath((getAppPath().BeforeLast(wxFileName::GetPathSeparator())) + wxString("/Lib"));
        setBlocksPath(getLibPath() + wxString("/CPP/Blocks"));

        setMatrixPath(getLibPath() + wxString("/CPP/Targets/ArduinoDuemilanove168_Arduino.v1.0/Matrix"));
        setCorePath(getLibPath() + wxString("/CPP/Targets/ArduinoDuemilanove168_Arduino.v1.0/Core"));
        setBoardPath(getLibPath() + wxString("/CPP/Targets/ArduinoDuemilanove168_Arduino.v1.0"));
    }
    else if (getBoardName() == wxString("Arduino Duemilanove Mega328"))
    {
        setToolsPath(getComponentsRepositoryPath() + wxString("/WinAVR/v20090313/bin")); //##Un-hardcode...
        setLibPath((getAppPath().BeforeLast(wxFileName::GetPathSeparator())) + wxString("/Lib"));
        setBlocksPath(getLibPath() + wxString("/CPP/Blocks"));

        setMatrixPath(getLibPath() + wxString("/CPP/Targets/ArduinoUNO_Arduino.v1.0/Matrix"));
        setCorePath(getLibPath() + wxString("/CPP/Targets/ArduinoUNO_Arduino.v1.0/Core"));
        setBoardPath(getLibPath() + wxString("/CPP/Targets/ArduinoUNO_Arduino.v1.0"));
    }
    else if (getBoardName() == wxString("Arduino Mega 2560"))
    {
        setToolsPath(getComponentsRepositoryPath() + wxString("/WinAVR/v20090313/bin")); //##Un-hardcode...
        setLibPath((getAppPath().BeforeLast(wxFileName::GetPathSeparator())) + wxString("/Lib"));
        setBlocksPath(getLibPath() + wxString("/CPP/Blocks"));

        setMatrixPath(getLibPath() + wxString("/CPP/Targets/ArduinoMega2560_Arduino.v1.0/Matrix"));
        setCorePath(getLibPath() + wxString("/CPP/Targets/ArduinoMega2560_Arduino.v1.0/Core"));
        setBoardPath(getLibPath() + wxString("/CPP/Targets/ArduinoMega2560_Arduino.v1.0"));
    }
    else if (getBoardName() == wxString("Arduino Mega 1280"))
    {
        setToolsPath(getComponentsRepositoryPath() + wxString("/WinAVR/v20090313/bin")); //##Un-hardcode...
        setLibPath((getAppPath().BeforeLast(wxFileName::GetPathSeparator())) + wxString("/Lib"));
        setBlocksPath(getLibPath() + wxString("/CPP/Blocks"));

        setMatrixPath(getLibPath() + wxString("/CPP/Targets/ArduinoMega1280_Arduino.v1.0/Matrix"));
        setCorePath(getLibPath() + wxString("/CPP/Targets/ArduinoMega1280_Arduino.v1.0/Core"));
        setBoardPath(getLibPath() + wxString("/CPP/Targets/ArduinoMega1280_Arduino.v1.0"));
    }
    else if (   getBoardName() == wxString("Maple Rev 3+ (to Flash)") ||
                getBoardName() == wxString("Maple Rev 3+ (to RAM)")
            )
    {
    #if defined (WIN32)
        setToolsPath(getComponentsRepositoryPath() + wxString("/ARM_EABI/Win/v2010q1_188/bin")); //##Un-hardcode...
    #else
        //##Test:
        setToolsPath(getComponentsRepositoryPath() + wxString("/ARM_EABI/Linux/v2010q1_188/bin")); //##Un-hardcode...
    #endif
        setLibPath((getAppPath().BeforeLast(wxFileName::GetPathSeparator())) + wxString("/Lib"));
        setBlocksPath(getLibPath() + wxString("/CPP/Blocks"));

        setMatrixPath(getLibPath() + wxString("/CPP/Targets/Maple.Rx.Arduino.v1.0/Matrix"));
        setCorePath(getLibPath() + wxString("/CPP/Targets/Maple.Rx.Arduino.v1.0/Core"));
        setBoardPath(getLibPath() + wxString("/CPP/Targets/Maple.Rx.Arduino.v1.0"));
    }
    else if (   getBoardName() == wxString("ATTiny25 (with ArduinoISP)") ||
                getBoardName() == wxString("ATTiny45 (with ArduinoISP)") ||
                getBoardName() == wxString("ATTiny85 (with ArduinoISP)") ||
                getBoardName() == wxString("ATTiny25 (with Doper)") ||
                getBoardName() == wxString("ATTiny45 (with Doper)") ||
                getBoardName() == wxString("ATTiny85 (with Doper)")
            )
    {
        setToolsPath(getComponentsRepositoryPath() + wxString("/WinAVR/v20090313/bin")); //##Un-hardcode...
        setLibPath((getAppPath().BeforeLast(wxFileName::GetPathSeparator())) + wxString("/Lib"));
        setBlocksPath(getLibPath() + wxString("/CPP/Blocks"));

        setMatrixPath(getLibPath() + wxString("/CPP/Targets/Tiny25.45.85_Arduino.v1.0/Matrix"));
        setCorePath(getLibPath() + wxString("/CPP/Targets/Tiny25.45.85_Arduino.v1.0/Core"));
        setBoardPath(getLibPath() + wxString("/CPP/Targets/Tiny25.45.85_Arduino.v1.0"));
    }

#if defined (linux) && defined(__i386__)
//    setToolsPath(wxString("/usr/bin")); //##Un-hardcode...
    setToolsPath(getComponentsRepositoryPath() + wxString("/avrlinux/i386/v1.0/bin")); //##Un-hardcode...
#endif
#if defined (linux) && defined(__x86_64__)
//    setToolsPath(wxString("/usr/bin")); //##Un-hardcode...
    setToolsPath(getComponentsRepositoryPath() + wxString("/avrlinux/amd64/v1.0/bin")); //##Un-hardcode...
#endif

}


void Bubble::changeBoardHardwareConfig()
{
    if (hardwareManager == NULL)
        return;

    //##Future: See if this methond will remain. But now, it's hardcoded:
    if (getBoardName() == wxString("DuinoBot.v1.x.HID"))
    {
        hardwareManager->setPortSelectorEnabled(false);
        hardwareManager->setPortNameString(_("HID"));
    }
    else
    {
        hardwareManager->setPortSelectorEnabled(true);
        hardwareManager->setPortNameString(wxString(""));
        hardwareManager->updatePorts();
    }
}


void Bubble::setCurrentCanvas(BubbleCanvas *value)
{
    currentCanvas = value;
    if (currentCanvas)
        currentCanvas->SetFocus();
    if (getNotifier()) //So the notified GUI can show the zoom level for the new currentCanvas.
    {
        getNotifier()->zoomNotify();
        closeTemporalPickers();
    }
}


void Bubble::addCanvas( wxWindowID id,
                        bool mainCanvas,
                        int defaultHScrollInc,
                        int defaultVScrollInc
                      )
{
    //##Implementar esto, y sincronizar todo con el sistema de Tabs:
    //##Recibir el color de fondo como parámetro (para que la aplicación lo levante de un XML de configuración):

    //##Levantar todo esto del XML:

    BubbleCanvas *newCanvas = new BubbleCanvas( parent,
                                                this,
                                                id,
                                                defaultHScrollInc,
                                                defaultVScrollInc,
                                                bubbleXML.getCanvasInfo(mainCanvas)
                                              );
    if (newCanvas)
    {
        canvases.Append(newCanvas);
        currentCanvas = newCanvas;
        saved = false;
        loadAcceleratorTableFromFile(); //##Esto va a cambiar casi seguro: Se agregará el nombre
                                        //del archivo del que se cargan los acceleratorKeys, y
                                        //quizá se agreguen más cosas de cofiguración allí, no sé.
    }
	//##newCanvas->SetBackgroundColourAndRefresh(wxColour(r, g, b));
}


//##Ver qué parámetros recibe:
void Bubble::deleteCanvas(BubbleCanvas *const value)
{
//##Try-catch
    saved = false;

    //##El currentCanvas debería ser el canvas anterior, si existe:
    //Esto no está cotemplado ahora, pero puede generar un crash de la aplicación, ya que podría ocurrir
    //que no quede apuntando a ningún lado:
    //Hay que usar canvases.IndexOf(value), restar 1 y ver que exista, para luego asignar el currentCanvas.
    //##Por ahora, asigno el primer canvas como el current tras el borrado, para evitar el posible cuelque
    //si currentCanvas queda apuntado al que se eliminó:
    listOfCanvas::iterator iter = canvases.begin();
    currentCanvas = *iter;

    //Removes the object from the list, but does not destroy it (the application must destroy it):
    canvases.DeleteObject(value);

    if (canvases.empty())
    {
        currentCanvas = NULL;
    }

    if (currentCanvas)
        currentCanvas->SetFocus();
}


bool Bubble::setBoardName(const wxString& value, wxWindow *pickersParent)
{
    if (pickersParent == NULL)
        return false;

    boardName = value;

    changeBoardPaths();
    changeBoardHardwareConfig();

    if (getNotifier())
    {
        //VERY IMPORTANT: The notifier MUST destroy the current canvas, and create a new one:
        getNotifier()->changeBoardNotify();
    }
    else
    {
        //If there is no notifier, the current canvas can't be destroyed, and THIS IS AN ERROR:
        return false;
    }
    enableAllBlocks(false);
    bubbleXML.loadBlocksInfo(pickersParent, getCurrentCanvas() != NULL);  //Only shows the Actions picker if there is
                                                                //already a canvas.
    enableAllBlocks(true);

    return true;
}


bool Bubble::loadTargetFromFile(const wxString& name)
{
#if (!UNDER_DEVELOPMENT)
    //##Implementar...
    return false;
#endif
}


bool Bubble::saveTargetToFile(const wxString& name)
{
    //##Implementar...
    //##Ver si esto estará disponible en la primer versión, o incluso en las subsiguientes... Porque por
    //ahora el target no puede ser editado en Minibloq, aunque en el futuro sí...
#if (!UNDER_DEVELOPMENT)
    //##Implementar...
    return false;
#endif
}


BubbleBlockInfo Bubble::loadBlockFromXML(wxXmlNode *child)
{
    wxString returnValue("");

    //It doen's matter if there is no function attribute (that's because I don't call HasAttribute here):
    returnValue = child->GetAttribute(wxString("function"), wxString(""));
    BubbleBlockInfo blockInfo(bubbleXML.getBlockInfo(child->GetName(), returnValue));
    blockInfo.setLoading(true); //Very important!

    //The rest of the attributes:
    if (child->HasAttribute(wxString("commented")))
    {
        returnValue = child->GetAttribute(wxString("commented"), wxString(""));
        blockInfo.setCommented(string2bool(returnValue));
    }
    if (child->HasAttribute(wxString("instanceName")))
    {
        returnValue = child->GetAttribute(wxString("instanceName"), wxString(""));
        blockInfo.setInstanceNameFieldDefaultValue(returnValue);
    }
    if (child->HasAttribute(wxString("constantValue")))
    {
        //##Ver si hay que guardar y levantar el tipo de dato también:
        returnValue = child->GetAttribute(wxString("constantValue"), wxString(""));
        blockInfo.setConstantFieldDefaultValue(returnValue);
    }
    if (child->HasAttribute(wxString("hasAddParamsButton")))
    {
        //If the block has addParamsButton, then has to add the possible extra paramSlots:
        returnValue = child->GetAttribute(wxString("hasAddParamsButton"), wxString(""));
        if (returnValue == wxString("true"))
        {
            if (child->HasAttribute(wxString("paramsCount")))
            {
                returnValue = child->GetAttribute(wxString("paramsCount"), wxString(""));
                long paramsCount = 0;
                if (returnValue.ToLong(&paramsCount))
                {
                    while (blockInfo.getParamsCount() < (unsigned int)(paramsCount))
                    {
                        //The addParamSlot in the blocks, always add paramSlots equal
                        //to the first one (thats why I use 0 here as index):
                        blockInfo.addParam( blockInfo.getParamName(0),
                                            blockInfo.getParamDataType(0),
                                            blockInfo.getParamNotAssignedImage(0),
                                            blockInfo.getParamDefaultImage(0),
                                            blockInfo.getParamHoverImage(0),
                                            blockInfo.getParamPressedImage(0),
                                            blockInfo.getParamDisabledImage(0)
                                          );
                    }
                }
            }
        }
    }

    return blockInfo;
}


void Bubble::loadParamsFromXML(wxXmlNode *child)
{
    if (child == NULL)
        return;
    if (currentCanvas == NULL)
        return;

    BubbleBlock *currentBlock = currentCanvas->getCurrentBlock();
    if (currentBlock == NULL)
        return;

    unsigned int i = 0;
    wxXmlNode *paramChild = child->GetChildren();
    while (paramChild)
    {
        //##loadXMLParams
        if (paramChild->GetName() != wxString("NULLParam")) //##Un-hardcode.
        {
            currentCanvas->setCurrentBlock(currentBlock);
            currentCanvas->addParam(loadBlockFromXML(paramChild),
                                    currentCanvas->getCurrentBlock()->getParamSlot(i),
                                    false
                                   );
        }
        //If it's a NULL param, just increment the paramSlot index:
        loadParamsFromXML(paramChild);
        i++;
        paramChild = paramChild->GetNext();
    }

    //##loadParamsFromXML(child->GetChildren());
}


bool Bubble::loadComponentFromFile(const wxString& name)
{
    //##Futuro: Agregar robustez, para que no se cuelgue si el XML está mal formateado, etc..

    if (currentCanvas == NULL)
    {
        if (getNotifier())
            getNotifier()->showMessage(_("Internal error: There is no \"currentCanvas\"."), true, true, *wxRED);
        return false;
    }

    //This is to avoid bad painting while loading component, when the open file dialog was maximized before
    //open the file, thus causing the frame to be badly draw for a moment:
    if (parent)
    {
        parent->Refresh();
        parent->Update();
    }

    //This reduces the flicker, specially in the start block:
    currentCanvas->showAllBlocks(false);

    wxXmlDocument componentFile;
    if ( !componentFile.Load(name, wxString("UTF-8")) )
    {
        currentCanvas->showAllBlocks(true);
        return false;
    }
    //componentFile.Save( name + wxString("_") );//##Debug.
    //componentFile.Save( name + wxString("_", wxXML_NO_INDENTATION) );//##Debug.


    wxXmlNode *root = componentFile.GetRoot();
    if (root == NULL)
    {
        currentCanvas->showAllBlocks(true);
        return false;
    }

    if (root->GetName() != wxString("mbqc"))
    {
        currentCanvas->showAllBlocks(true);
        return false;
    }

    //##Ver si hay que agregar seguridades a todo este proceso:
    wxXmlNode *rootChild = root->GetChildren();
    wxXmlNode *child = NULL;

    while (rootChild)
    {
        //Load de properties:
        if (rootChild->GetName() == "properties") //##Un-hardcode?
        {
            child = rootChild->GetChildren();
            while (child)
            {
                if (child->GetName() == "blocks") //##Un-hardcode?
                {
                    //Reset the progress bar:
                    if (getNotifier())
                    {
                        getNotifier()->clearMessage();
                        if ( child->HasAttribute(wxString("count")) )
                        {
                            wxString blocksCount = child->GetAttribute(wxString("count"), wxString("1"));
                            long blocksCountNumber = 1;
                            getNotifier()->setProgressMax(1);
                            if (blocksCount.ToLong(&blocksCountNumber))
                            {
                                if (blocksCountNumber > 1)
                                    getNotifier()->setProgressMax(blocksCountNumber);
                            }
                            getNotifier()->setProgressPosition(0, false, false);
                        }
                    }
                }
                child = child->GetNext();
            }
        }
        //Load variables:
        else if (rootChild->GetName() == "variables") //##Un-hardcode?
        {
            child = rootChild->GetChildren();
            while (child)
            {
                BubbleInstance *var = new BubbleInstance(child->GetName(), wxString("Variable"));
                currentCanvas->setInstance(var);
                child = child->GetNext();
            }
        }
        //Load Blocks:
        else if (rootChild->GetName() == "blocks") //##Un-hardcode? At least define constants...
        {
            child = rootChild->GetChildren();
            while (child)
            {
                //##Pasar todos estos strings de los atributos a constantes, para evitar errores de cut & paste
                //con los otros lugares donde se utilizan, en como la clase BubbleBlock, por ejemplo:
                wxString returnValue("");
                if ( child->HasAttribute(wxString("loadAction")) )
                {
                    returnValue = child->GetAttribute(wxString("loadAction"), wxString(""));
                    if ( returnValue == wxString("load") ) //##Ver si hay que descablear, o hacer una función auxiliar...
                    {
                        //##Esto ya parece funcionar con estructuras con múltiples hermanos, tipo
                        //"if-elseif-...-else-endif", así que sólo faltaría implementar el sistema de
                        //addBrother a los blocks y ver si funciona:
                        currentCanvas->addBlock(loadBlockFromXML(child), false);
                        if (currentCanvas->getCurrentBlock())
                        {
                            BubbleBlock *brother = currentCanvas->getCurrentBlock()->getNextBrother();
                            std::stack<BubbleBlock*> invertingStack;
                            while (brother)
                            {
                                invertingStack.push(brother);
                                brother = brother->getNextBrother();
                            }
                            while(!invertingStack.empty())
                            {
                                brothers.push(invertingStack.top());
                                invertingStack.pop();
                            }
                        }
                        loadParamsFromXML(child);
                    }
                    else if ( returnValue == wxString("brother") ) //##Un-hardcode...
                    {
                        if (!brothers.empty())
                        {
                            BubbleBlock *prevBrother = brothers.top();
                            if (prevBrother)
                            {
                                brothers.pop();
                                currentCanvas->setCurrentBlock(prevBrother);
                                loadParamsFromXML(child);
                            }
                        }
                    }
                    //else if ( returnValue == wxString("noLoad") ) --> Do nothing!
                }
                //Show the loading progress:
                if (getNotifier())
                    getNotifier()->setProgressPosition(getNotifier()->getProgressPosition() + 1, true, false);

                //getNotifier()->showMessage(child->GetName() + wxString(" ") + function + wxString(" ") + instanceName + wxString("\n"), false, true, *wxGREEN); //##Debug
                child = child->GetNext();
            }
        }
        rootChild = rootChild->GetNext();
    }

    currentCanvas->setCurrentBlock(currentCanvas->getFirstBlock());
    currentCanvas->scrollToHomeAbsolute();
    currentCanvas->showAllBlocks(true);
    currentCanvas->forceSaved(true);

    if (getNotifier())
    {
        //getNotifier()->setProgressPosition(getNotifier()->getProgressMax(), true, false);
        getNotifier()->hideMessagesWindow();
    }

    saved = true;
    return true;
}


//##Esto funciona, pero la indentación es un asco. Ver si en el futuro paso al manejo automático de
//wxXMLDocument también en grabación, aunque no sé, porque esto es muy rápido además:
wxString Bubble::generateXMLFromParams(BubbleBlock *block)
{
    wxString tempXML("");
    if (block == NULL)
        return tempXML;
    tempXML +=  wxString("<") +
                block->getName() +
                block->getAttributes() +
                wxString(">\n");

    //Indentation:
    wxString indentation("\t");
    for (unsigned int i = 0; i < block->getIndentation(); i++)
        indentation += wxString("\t");

    unsigned int paramsCount = block->getParamsCount();
    for (unsigned int paramIndex = 0; paramIndex < paramsCount; paramIndex++)
    {
        BubbleBlock *paramBlock = block->getParamFirstBlock(paramIndex);
        if (paramBlock)
        {
            //##Futuro: Mejorar la indentación, para que no haya 2 tabs en los parámetros:
            tempXML += indentation + wxString("\t") + wxString("\t") + generateXMLFromParams(paramBlock);
        }
        else
        {
            //##Pasar estos strings (como "NULLParam") a constantes para evitar errores y para emprolijar:
            //No param attached to the slot:
            tempXML +=  indentation + wxString("\t") +
                        wxString("<NULLParam loadAction=\"noLoad\">\n") +
                        indentation + wxString("\t") +
                        wxString("</NULLParam>\n");
        }
    }
    tempXML +=  indentation + wxString("\t") +
                wxString("</") +
                block->getName() +
                wxString(">\n");

    return tempXML;
}


bool Bubble::saveComponentToFile(const wxString& name, bool format)
{
    //##La indentación funciona mal acá, así que por ahora, se creó la función formatComponentFile, que
    //vuelve a abrir el archivo aquí generado, lo carga en un wxXmlDocument y lo graba indentado. Igual es

    //NOTE: I commented all the lines regarding the progressBar, because this caused a lot of flickering in
    //the in screen, specially if the hardware manager was visible. But they can be easily re-activated.
    //So please DON'T DELETE THE COMMENTED LINES (those with the "getNotifier" string) below:

    try
    {
        //First, reset the progress bar:
//        if (getNotifier())
//        {
//            getNotifier()->setProgressPosition(0, false, false);
//            getNotifier()->clearMessage();
//        }

        //##Nota acerca del tipo de archivo: Por ahora usa el typeDefault, que en teoría debería generar
        //archivos con terminación de línea "DOS". Pero más adelante ser verá, y quizá se pase todo a Unix.
        //Si esto se cambia, ver qué funciones hay que tocar (en principio, al menos hay que llamar a Write()
        //con otro tipo de archivo, porque ahora usan el parámetro con valor por defecto). Algo que sí me
        //gustaría es no usar el default, para que todos los Minibloq generen el mismo tipo de archivo, sin
        //importar el sistema operativo, lo cual dará más uniformidad, pero no estoy seguro...

        //##
        //##Los mensajes de grabación, etc., son también rojos si hay error, y azules si todo fue bien. Los de
        //compilación existosos son verdes... (al menos por ahora me gusta así).

        //Try to create the output dir:
        createDirs(outputPath);

        //Try to create the file:
        wxTextFile componentFile;
        wxRemoveFile(name); //##Ver si en el futuro será así: Esto reemplaza el archivo, si éste existía...
        if (!componentFile.Create(name))  //##Un-hardcode!
            return false;

        //##Descablear, ver si estas cosas se sacan del algún archivo en run-time:
        componentFile.AddLine("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
        componentFile.AddLine("<!-- Created with Minibloq (http://minibloq.org/) -->");
        componentFile.AddLine("<mbqc"); //##Ver si esto queda así, o si pasa a algo tipo "MinibloqComponent"...
        componentFile.AddLine("\txmlns=\"http://mbqc.namespaces.minibloq.org\""); //##
        componentFile.AddLine("\txmlns:minibloq=\"http://minibloq.org\""); //##
        componentFile.AddLine("\tMinibloqVersion=\"0.82.Beta\">"); //##Un-hardcode!

        //##Falta recorrer todos los canvases para agregar los bloqs de usuario...

        //##Recorre los bloques para serializarlos al archivo del mbqc (por ahora sólo del currentCanvas):
        if (!currentCanvas)
            return false;

        componentFile.AddLine(wxString("\t<properties>")); //##Ver si esto queda así, o si pasa a algo tipo "MinibloqComponent"...
        componentFile.AddLine(wxString("\t\t<blocks count=\"") + //##Un-hardcode
                              (wxString("") << currentCanvas->getBlocksCount()) + wxString("\"") +
                              wxString(">")); //##
        componentFile.AddLine(wxString("\t\t</blocks>"));
        componentFile.AddLine(wxString("\t</properties>")); //##Ver si esto queda así, o si pasa a algo tipo "MinibloqComponent"...

        //##Recorre todas las variables (##Esto va a cambiar en el futuro cuando haya más tipos de datos):
        componentFile.AddLine("\t<variables>"); //##Ver si esto queda así, o si pasa a algo tipo "MinibloqComponent"...

        //This is a small patch to prevent the following bug: If the user is editing a variableInit block, and
        //hi has already changed the var name, and saves the program without unselecting the variableInit
        //block, then two instance names are saved: the old one and the new one. And this is wrong. So now,
        //this unselects the currentBlock (because the currentBlock is the only that the user may be editing):
        BubbleBlock *tempBlock = currentCanvas->getCurrentBlock();
        if (tempBlock)
        {
            if (tempBlock->getVariableInit())
                tempBlock->addVariableWithInstanceName();
        }

        //##Delete this:
        //Search for at least one setter block with the same variable name:
        //currentCanvas->setAllInstances();

        //##2011.08.12: Delete this:
        //currentCanvas->clearNonDeclaredInstances();

        //##Ahora que se aseguró de que las instancias estén completas, las recorre para generar las
        //declaraciones correspondientes:
        for (int i = 0; i<currentCanvas->getInstancesCount(); i++)
        {
            //##Esto va a cambiar cuando se introduzca el manejo de tipos definitivo con las
            //variables, y cuando se introduzcan clases:
            BubbleInstance *tempInstance = currentCanvas->getInstance(i);
            if (tempInstance)
            {
                if ( tempInstance->getType() == "Variable" ) //##Horrible harcoded: Un-hardcode!!
                {
                    componentFile.AddLine(wxString("\t\t<") + tempInstance->getName() + wxString(">"));
                    componentFile.AddLine(wxString("\t\t</") + tempInstance->getName() + wxString(">"));
                }
            }
        }
        componentFile.AddLine("\t</variables>"); //##Ver si esto queda así, o si pasa a algo tipo "MinibloqComponent"...

        componentFile.AddLine("\t<blocks>"); //##Ver si esto queda así, o si pasa a algo tipo "MinibloqComponent"...
        BubbleBlock *iteratorBlock = currentCanvas->getFirstBlock();
//        if (getNotifier())
//            getNotifier()->setProgressMax(currentCanvas->getBlocksCount());
        while (iteratorBlock)
        {
//            if (getNotifier())
//                getNotifier()->setProgressPosition(getNotifier()->getProgressPosition() + 1, true, false);

            //Indentation:
            //##En cosas como el código G, que quizá tengan sólo un encabezado tipo "comentario",
            //se puede poner acá 0 ó 1. Aquí van 2, porque todo va indentado 1 con respecto al
            //InitializationCode. Así que se hará configurable la "indentación con respecto al
            //InitializationCode, pero por ahora está cableada acá en "un tab":
            wxString indentation("\t\t");
            for (unsigned int i = 0; i < iteratorBlock->getIndentation(); i++)
                indentation += wxString("\t");

            //##Más adelante se implementará un patrón de iteración, o algo así seguramente...
            //Params:
            if (iteratorBlock->getParamsCount() > 0)
            {
                //##Esto seguramente devolverá un stringArray, no un string, ya que los parámetros van uno abajo del otro:
                wxString tempXML = indentation + generateXMLFromParams(iteratorBlock);
                componentFile.AddLine(tempXML.BeforeLast('\n')); //##Eliminar el último enter que metió generateParamsXML.
            }
            else
            {
                componentFile.AddLine(indentation + wxString("<") +
                                                    iteratorBlock->getName() +
                                                    iteratorBlock->getAttributes() +
                                                    wxString(">"));
                componentFile.AddLine(indentation + wxString("</") + iteratorBlock->getName() + wxString(">"));
            }
            iteratorBlock = currentCanvas->getNextBlock(iteratorBlock);
        }
        componentFile.AddLine("\t</blocks>"); //##Ver si esto queda así, o si pasa a algo tipo "MinibloqComponent"...

        //##Un-hardcode? See if these things can be loaded in runtime from a config file:
        componentFile.AddLine("</mbqc>");

        //##Save the changes and closes the file:
        if ( !componentFile.Write() )
        {
//            if (getNotifier())
//                getNotifier()->hideMessagesWindow();
            return false;
        }
        if ( componentFile.Close() )
        {
            saved = true; //##
            currentCanvas->forceSaved(true);
//##Futuro: Dejar esto cuando haya un mejor sistema de "mini-mensajes", de modo que los muestre chiquitos, o grandes
//          según sea conveniente para cada acción (por ejemplo, para la compilación puede ser mejor grandes, etc.)...
//            if ( getNotifier() )
//                getNotifier()->showMessage(_("Component:\n") + name + _("\nSaved ok."), true, false, *wxBLUE);

//            if (getNotifier())
//                getNotifier()->hideMessagesWindow();
            if (format)
                return formatComponentFile(name);
            return true;
        }
//        if (getNotifier())
//            getNotifier()->hideMessagesWindow();
        return false;
    }
    catch(...)
    {
//        if (getNotifier())
//            getNotifier()->hideMessagesWindow();
        return false;
    }
}


//##Patch: Esto debería hacerse más rápido y con menos memoria en el actual save, pero la indentación funcinó mal
//y se implementó así por ahora, para que al menos indente:
bool Bubble::formatComponentFile(const wxString& name)
{
    wxXmlDocument componentFile;
    if ( !componentFile.Load(name, wxString("UTF-8")) )
    {
        currentCanvas->showAllBlocks(true);
        return false;
    }
    return componentFile.Save(name, 2); //##Hacer la indentación configurable, al menos levantarla de un
                                        //archivo de texto (XML también, claro)...
}


bool Bubble::run()
{
    //##Futuro: En los boards que tienen deploy y además una orden extra para ejecutar, Run tiene sentido, y
    //tiene la forma actual (en que primero llama a deploy). Pero en el futuro, cuando se agregue la ejecución
    //en tiempo real con intérprete, run será mucho más complejo, casi con seguridad...
    if (deploy())
    {
        //##Implementar...
    }
    return false;
}


#if defined (WIN32)
//##NO PORTABLE CODE: Only for Windows:
//##Not working yet:
bool Bubble::winInstallINF()
{
    if (parent)
    {
        HINSTANCE instance = ShellExecute(
                                            (HWND)parent->GetHandle(),
                                            cstrToWChar("open"),
                                            cstrToWChar("rundll32.exe"),
                                            wxString(wxString("setupapi,InstallHinfSection DefaultInstall 132 ") +
                                            //##:
                                            wxString("C:/Projects/Multiplo/Soft/Minibloq/v1.0/Soft/Bin/Minibloq-RG.v1.0/Components/Drivers/Arduino/UNO/v1.0/Arduino UNO.inf")).c_str(),
                                            //##wxString("C:/Projects/Multiplo/Soft/Minibloq/v1.0/Soft/Bin/Minibloq-RG.v1.0/Components/Drivers/DuinoBot/v1.0/LUFA_CDC_Bootloader.inf")).c_str(),
                                            NULL,
                                            SW_HIDE
                                         );
        return ((int)instance) > 32;
    }
    return false;
}
#endif

void Bubble::createDirs(const wxString& path)
{
    //##Futuro: Robustecer mucho esto

    //Try to create the output dir structure:
    wxFileName aux(path);
    if (!wxDir::Exists(aux.GetPath()))
        wxMkdir(aux.GetPath());
    if (!wxDir::Exists(path))
        wxMkdir(path);
}


bool Bubble::executeCmd(const wxString& cmd)
{
    //##Implementar...
    BubbleProcess *const process = new BubbleProcess(notifier); //##Ver si esto es correcto, porque me parece más seguro que dejar al notifier
                                                                //que libere la memoria, tal como sugiere el ejemplo de wxWidgets...
    //##Ver qué se hace conel resultado de wxExecute:
    //long pidLast = wxExecute(cmd, wxEXEC_ASYNC, process);
    //wxExecute(cmd, wxEXEC_ASYNC, process);
    wxExecute(cmd, wxEXEC_SYNC, process);

    //##Ver si se hace más algo con esto:
    return true;
}


bool Bubble::isSubstringInArrayString(const wxArrayString &value, const wxString& substring)
{
    size_t count = value.GetCount();
    for (size_t n = 0; n < count; n++)
    {
        if (value[n].Find(substring) != wxNOT_FOUND )
            return true;
    }

    return false;
}


//##Esta función determina por ahora, si vale la pena seguir o no el proceso de compilación, ya que si
//detecta el string de error, devuelve false, y las funciones que la llaman deberían detener el proceso:
bool Bubble::findErrorStringAndShow(const wxArrayString &value)
{
    //##Hacer que el substring "error:" sea configurable en el target (e incluso más adelante, puedo usar
    //expresiones regulares):
    if (    isSubstringInArrayString(value, wxString("error:")) ||
            isSubstringInArrayString(value, wxString("Unable"))
       )
    {
        showStream(value, *wxRED); //##Hacer los colores configurables
        if (getNotifier())
            getNotifier()->showMessage(_("\nThere are errors."), false, false, *wxRED); //##
        return true;
    }
    showStream(value);
    return false;
}


//##Debug:
void Bubble::showStream(const wxArrayString &value, const wxColour& colour)
{
    size_t count = value.GetCount();
    //if ( !count )
    //    return true; //##?

    wxString str("");
    for (size_t n = 0; n < count; n++)
    {
        str += (value[n]) + wxString("\r\n");
        //str += (value[n]) + wxString("\r");
        //str += value[n];
    }

    if (getNotifier())
    {
        getNotifier()->showMessage(str, false, true, colour); //##
    }

    //wxMessageDialog dialog0(parent, str, _("Debug:")); //##Debug.
    //dialog0.ShowModal(); //##Debug.
}


bool Bubble::deploy()
{
    if (getNotifier() == NULL) //##Ver si esto se dejará así, o si se mejorará en el futuro...
        return false;

    if (build())
    {
        //##En el futuro, para optimizar, existirá la opción de resetear antes del build() en los targets
        //que así se especifique (ya que en boards como Arduino, eso no funcionaría, por el time-out del
        //bootloader de Arduino, pero en los boards como DuinoBot.Kids, eso ganaría bastante tiempo):
        getNotifier()->showMessage(_("Reseting the board...\n"), false, false, *wxBLUE);
        getNotifier()->deployStartedNotify();
        resetBoard(); //##Futuro: Usar el retorno booleano de resetBoard...

        //Not necessary: Everything is managed by the hid_bootloader_cli program.
        //if (getBoardName() == wxString("DuinoBot.v1.x.HID"))
        //{
        //}

        if (getBoardName() == wxString("DuinoBot.v1.x") || //##Un-hardcode:
            getBoardName() == wxString("DuinoBot.Kids.v1.x") ) //##Un-hardcode:
        {
            //First, waits until the port desappears:
            int times = 0;

            //##En las placas con ATMega32U4, primero el puerto tiene que desaparecer tras el reset, y una vez que
            //desapareció, recién se verifica (en el while que sigue a éste) que el puerto exista nuevamente:

            //##Hacer la cantidad de iteraciones configurable:
            times = 0;
            while ( (BubbleHardwareManager::serialPortExists(bootPortName)) && (times < 10) )
            {
                times++;
                getNotifier()->showMessage(_(">"), false, false, *wxBLUE);
                wxMilliSleep(200); //##Hacer esto configurable.
            }
            //getNotifier()->showMessage(_("\n"), false, false, *wxBLUE);

            //Waits until the port does exist, but with a timeout:
            getNotifier()->showMessage(_("\nVerifiying port ") + bootPortName, false, false, *wxBLUE);
            //##Hacer la cantidad de iteraciones configurable:
            times = 0;
            while ( (!BubbleHardwareManager::serialPortExists(bootPortName)) && (times < 10) )
            {
                times++;
                getNotifier()->showMessage(_(">"), false, false, *wxBLUE);
                wxMilliSleep(200); //##Hacer esto configurable.
            }
            getNotifier()->showMessage(_("\n\n"), false, false, *wxBLUE);
        }

        //With Maple the port can't be verified this way (it works differently, with a DFU device to be programmed):
        if (    (getBoardName() == wxString("Maple Rev 3+ (to Flash)")) ||
                (getBoardName() == wxString("Maple Rev 3+ (to RAM)"))
           )
        {
            //For reference, see http://leaflabs.com/docs/bootloader.html#maple-rev3-rev5-dfu
            bool dfuDeviceFound = false;
            wxString cmd = wxString("");
            wxArrayString output, errors;
            wxArrayString temp; //##
            getNotifier()->showMessage(_("\nVerifiying DFU device\n") + bootPortName, false, false, *wxBLUE);

            for (unsigned int i=0; i<50; i++) //##60 is arbitrary... Must be configurable.
            {
                //Command to find the DFU devide:
                wxString tempCmd = toolsPath + wxString("/dfu-util -l"); //##Improve this in the future.
                if (getNotifier())
                    getNotifier()->showMessage(tempCmd, false, true, *wxBLUE); //##
                wxExecute(tempCmd, output, errors);
                if (isSubstringInArrayString(output, wxString("0x000")))
                {
                    dfuDeviceFound = true;
                    break;
                }
                getNotifier()->showMessage(wxString("\n"), false, false, *wxBLUE);
                wxMilliSleep(200); //##Hacer esto configurable.

                //if (getNotifier())
                //{
                //    //getNotifier()->showMessage(_("\nErrors:\n"), false, false, *wxRED); //##
                //    //showStream(errors);
                //    getNotifier()->showMessage(_("\nOutput:\n"), false, false, *wxGREEN); //##
                //    showStream(output);
                //}
            }
            if (dfuDeviceFound)
            {

                if (getBoardName() == wxString("Maple Rev 3+ (to Flash)"))
                {
                    cmd =   toolsPath + wxString("/dfu-util -a 1 -R -d 1EAF:0003 -D") +
                            wxString("\"") + outputPath + wxString("/main.cpp.bin\"");
                }
                else if (getBoardName() == wxString("Maple Rev 3+ (to RAM)"))
                {
                    cmd =   toolsPath + wxString("/dfu-util -a 0 -R -d 1EAF:0003 -D") +
                            wxString("\"") + outputPath + wxString("/main.cpp.bin\"");
                }
                wxMilliSleep(500); //##
                getNotifier()->showMessage(_("\nBoard Ok >> Programming...\n\n"), false, false, *wxBLUE);
                //getNotifier()->showMessage(_("cmd = "), false, false, *wxBLUE); //##Debug.
                //getNotifier()->showMessage(cmd + wxString("\n"), false, false, *wxBLUE); //##Debug.

                wxArrayString output, errors;
                //int code = wxExecute(cmd, output, errors);
                wxArrayString temp; //##
                temp.Add(cmd);
                showStream(temp); //##
                wxExecute(cmd, output, errors);
                getNotifier()->showMessage(_("\nOutput:\n"), false, false, *wxGREEN);
                showStream(output); //##
                getNotifier()->showMessage(wxString("\n"), false, false, *wxGREEN);
                findErrorStringAndShow(errors); //##Debug
            }
            else
            {
                getNotifier()->showMessage(_("\nError: Device not found\n"), false, false, *wxRED);
            }
        }
        else if (getBoardName() == wxString("DuinoBot.v1.x.HID"))
        {
            wxString cmd = wxString("");
#if defined (WIN32)
            cmd =   getComponentsRepositoryPath() + wxString("/hid_bootloader_cli/v1.0/hid_bootloader_cli.exe ") + //##
#else
            cmd =   getComponentsRepositoryPath() + wxString("/hid_bootloader_cli/v1.0/hid_bootloader_cli.Linux ") + //##
#endif
                    wxString("-mmcu=atmega32u4 -r -v \"") +
                    outputPath + wxString("/main.cpp.hex\"");

            getNotifier()->showMessage(_("\nBoard Ok >> Programming...\n\n"), false, false, *wxBLUE);
            wxArrayString output, errors;
            wxArrayString temp;
            temp.Add(cmd);
            showStream(temp);
            wxExecute(cmd, output, errors);
            findErrorStringAndShow(errors);
        }
        else
        {
            //##I think it will be better to not use the "-D" option in avrdude:
            //The port may not exist at the time the user calls the delploy() function, so this prevents the
            //program to hang for some time (which may be a lot of time) when this happens:
            if (BubbleHardwareManager::serialPortExists(bootPortName) || //This is more secure than flaging the port existence inside the timeout while.
                getBoardName() == wxString("ATTiny25 (with ArduinoISP)") ||
                getBoardName() == wxString("ATTiny45 (with ArduinoISP)") ||
                getBoardName() == wxString("ATTiny85 (with ArduinoISP)") ||
                getBoardName() == wxString("ATTiny25 (with Doper)") ||
                getBoardName() == wxString("ATTiny45 (with Doper)") ||
                getBoardName() == wxString("ATTiny85 (with Doper)")
               )
            {
                wxString cmd = wxString("");
                if (getBoardName() == wxString("DuinoBot.v1.x") ||      //##Un-hardcode:
                    getBoardName() == wxString("DuinoBot.Kids.v1.x") )  //##Un-hardcode:
                {
                    //##En el futuro, puede que sea mejor implementar al revés la obtención del serialPortName, de modo
                    //que el deploy lo pida directamente al HardwareManager cada vez que es invocado. Esto ahorraría
                    //eventuales problemas de actualización, que dependen del evento del combo (o del selector que se
                    //esté utilizando) del HardwareManager:
#if defined (WIN32)
                    cmd =   toolsPath + wxString("/avrdude.20090314.exe -C\"") +
#else
                    cmd =   toolsPath + wxString("/avrdude -C\"") +//##
#endif
                            toolsPath + wxString("/avrdude.conf") + wxString("\"") +
                            wxString(" -patmega32u4 -cavr109") +
                            //wxString(" -P//./COM48") + //##Serial port number = Make it configurable.
                            wxString(" -P") + bootPortName + //##Serial port number = Make it configurable.
                            //wxString(" -b115200 -D -Uflash:w:\"") +
                            wxString(" -b115200 -Uflash:w:\"") +
                            outputPath + wxString("/main.cpp.hex\":i");
                //        wxMessageDialog dialog0(parent, cmd, _("Debug:")); //##Debug.
                //        dialog0.ShowModal(); //##Debug.

                    //executeCmd(cmd);
                }
                else if (getBoardName() == wxString("Seeeduino v2.2x Mega328") ) //##Un-hardcode.
                {
#if defined (WIN32)
                    cmd =   toolsPath + wxString("/avrdude.20071012.exe -C\"") +//##
#else
                    cmd =   toolsPath + wxString("/avrdude -C\"") +//##
#endif
                            toolsPath + wxString("/avrdude.conf") + wxString("\"") +
                            wxString(" -patmega328p -cstk500v1") +
                            wxString(" -P") + bootPortName +
                            //wxString(" -b57600 -D -Uflash:w:\"") +
                            wxString(" -b57600 -Uflash:w:\"") +
                            outputPath + wxString("/main.cpp.hex\":i");
                }
                else if (getBoardName() == wxString("Arduino Uno"))
                {
#if defined (WIN32)
                    cmd =   toolsPath + wxString("/avrdude.20071012.exe -C\"") +//##
#else
                    cmd =   toolsPath + wxString("/avrdude -F -C\"") +//##
#endif
                            toolsPath + wxString("/avrdude.conf") + wxString("\"") +
                            wxString(" -patmega328p -cstk500v1") +
                            wxString(" -P") + bootPortName +
                            //wxString(" -b115200 -D -Uflash:w:\"") +
                            wxString(" -b115200 -Uflash:w:\"") +
                            outputPath + wxString("/main.cpp.hex\":i");
                    //##Esta línea es valiosa, porque escribe independientemente de la signatura. Arduino UNO y el avrdude
                    //del WinAVR-20090313, por ejemplo, se llevan mal, y el mismo comando que en el avrdude que viene con
                    //el ArduinoIDE-0022 (WinAVR-20081205) sí funciona bien. Así que es probable que deje por defecto el "-F",
                    //aunque por ahora no, porque puede que esto proteja contra malas configuraciones por parte de terceros
                    //que creen targets:
                    //wxString cmd =  toolsPath + wxString("/avrdude.exe -F -C\"") +//##
                    //wxString cmd =  toolsPath + wxString("/avrdude.20071012.exe -F -C\"") +//##
                }
                else if (getBoardName() == wxString("Arduino Mega 2560"))
                {
#if defined (WIN32)
                    cmd =   toolsPath + wxString("/avrdude.20071012.exe -C\"") +//##
#else
                    cmd =   toolsPath + wxString("/avrdude -C\"") +//##
#endif
                            toolsPath + wxString("/avrdude.conf") + wxString("\"") +
                            wxString(" -patmega2560 -cstk500v2") +
                            wxString(" -P") + bootPortName +
                            //wxString(" -b115200 -D -Uflash:w:\"") +
                            wxString(" -b115200 -Uflash:w:\"") +
                            outputPath + wxString("/main.cpp.hex\":i");
                }
                else if (   (getBoardName() == wxString("Seeeduino Mega 1280")) ||
                            (getBoardName() == wxString("Arduino Mega 1280"))
                        )
                {
#if defined (WIN32)
                    cmd =   toolsPath + wxString("/avrdude.20071012.exe -C\"") +//##
#else
                    cmd =   toolsPath + wxString("/avrdude -C\"") +//##
#endif
                            toolsPath + wxString("/avrdude.conf") + wxString("\"") +
                            wxString(" -patmega1280 -cstk500v1") +
                            wxString(" -P") + bootPortName +
                            //wxString(" -b57600 -D -Uflash:w:\"") +
                            wxString(" -b57600 -Uflash:w:\"") + //##This must be user-configurable.
                            outputPath + wxString("/main.cpp.hex\":i");
                }
                else if (getBoardName() == wxString("Arduino Duemilanove Mega328"))
                {
#if defined (WIN32)
                    cmd =   toolsPath + wxString("/avrdude.20071012.exe -C\"") +//##
#else
                    cmd =   toolsPath + wxString("/avrdude -C\"") +//##
#endif
                            toolsPath + wxString("/avrdude.conf") + wxString("\"") +
                            wxString(" -patmega328p -cstk500v1") +
                            wxString(" -P") + bootPortName +
                            //wxString(" -b57600 -D -Uflash:w:\"") +
                            wxString(" -b57600 -Uflash:w:\"") +
                            outputPath + wxString("/main.cpp.hex\":i");
                }
                else if (getBoardName() == wxString("Arduino Duemilanove Mega168"))
                {
#if defined (WIN32)
                    cmd =   toolsPath + wxString("/avrdude.20071012.exe -C\"") +//##
#else
                    cmd =   toolsPath + wxString("/avrdude -C\"") +//##
#endif
                            toolsPath + wxString("/avrdude.conf") + wxString("\"") +
                            wxString(" -patmega168 -cstk500v1") +
                            wxString(" -P") + bootPortName +
                            //wxString(" -b57600 -D -Uflash:w:\"") +
                            wxString(" -b19200 -Uflash:w:\"") +
                            outputPath + wxString("/main.cpp.hex\":i");
                }
                else if (   getBoardName() == wxString("ATTiny25 (with ArduinoISP)") ||
                            getBoardName() == wxString("ATTiny45 (with ArduinoISP)") ||
                            getBoardName() == wxString("ATTiny85 (with ArduinoISP)")
                        )
                {
                    wxString mcu("");
                    if (getBoardName() == wxString("ATTiny25 (with ArduinoISP)"))
                        mcu = "attiny25";
                    else if (getBoardName() == wxString("ATTiny45 (with ArduinoISP)"))
                        mcu = "attiny45";
                    else if (getBoardName() == wxString("ATTiny85 (with ArduinoISP)"))
                        mcu = "attiny85";
                    //ArduinoISP:
#if defined (WIN32)
                    cmd =   toolsPath + wxString("/avrdude.20071012.exe -C\"") +//##
#else
                    cmd =   toolsPath + wxString("/avrdude -C\"") +//##
#endif
                            //toolsPath + wxString("/avrdude.20071012.exe -C\"") +//##
                            toolsPath + wxString("/avrdude.conf\"") +
                            wxString(" -p " + mcu + " -c stk500v1 -b19200 ") +
                            wxString(" -P") + bootPortName +
                            wxString(" -V -F -Uflash:w:\"") +
                            outputPath + wxString("/main.cpp.hex\":i");
                }
                else if (   getBoardName() == wxString("ATTiny25 (with Doper)") ||
                            getBoardName() == wxString("ATTiny45 (with Doper)") ||
                            getBoardName() == wxString("ATTiny85 (with Doper)")
                        )
                {
                    wxString mcu("");
                    if (getBoardName() == wxString("ATTiny25 (with Doper)"))
                        mcu = "attiny25";
                    else if (getBoardName() == wxString("ATTiny45 (with Doper)"))
                        mcu = "attiny45";
                    else if (getBoardName() == wxString("ATTiny85 (with Doper)"))
                        mcu = "attiny85";
                    //##With AVRDoper:
                    //.\avrdude -C .\avrdude.conf -p attiny25 -c stk500v2 -P avrdoper -V -U flash:w:".\LineFollower0.cpp.hex":i
#if defined (WIN32)
                    cmd =   toolsPath + wxString("/avrdude.20071012.exe -C\"") +//##
#else
                    cmd =   toolsPath + wxString("/avrdude -C\"") +//##
#endif
                            toolsPath + wxString("/avrdude.conf\"") +
                            wxString(" -p " + mcu + " -c stk500v2 -P avrdoper") +
                            wxString(" -V -F -Uflash:w:\"") +
                            outputPath + wxString("/main.cpp.hex\":i");

                    //With USBasp:
                    //cmd =   toolsPath + wxString("/avrdude.20071012.exe -C\"") +//##
                    //        toolsPath + wxString("/avrdude.conf") + wxString("\"") +
                    //        wxString(" -p " + mcu + " -c usbasp") +
                    //        wxString(" -V -F -Uflash:w:\"") +
                    //        outputPath + wxString("/main.cpp.hex\":i");
                }
                getNotifier()->showMessage(_("Verifiying the board ") + getBoardName() + wxString("\n"), false, false, *wxBLUE);

                if (getBoardName() == wxString("ATTiny25 (with ArduinoISP)") ||
                    getBoardName() == wxString("ATTiny45 (with ArduinoISP)") ||
                    getBoardName() == wxString("ATTiny85 (with ArduinoISP)") ||
                    getBoardName() == wxString("ATTiny25 (with Doper)") ||
                    getBoardName() == wxString("ATTiny45 (with Doper)") ||
                    getBoardName() == wxString("ATTiny85 (with Doper)")
                   )
                {
                    getNotifier()->showMessage(_("\nBoard Ok >> Programming...\n\n"), false, false, *wxBLUE);

                    wxArrayString output, errors;
                    //int code = wxExecute(cmd, output, errors);
                    wxArrayString temp; //##
                    temp.Add(cmd);
                    showStream(temp); //##
                    wxExecute(cmd, output, errors);
                    findErrorStringAndShow(errors); //##Debug
                }
                else
                {
                    if (verifyBoard())
                    {
                        getNotifier()->showMessage(_("\nBoard Ok >> Programming...\n\n"), false, false, *wxBLUE);
                        wxArrayString output, errors;
                        //int code = wxExecute(cmd, output, errors);
                        wxArrayString temp; //##
                        temp.Add(cmd);
                        showStream(temp); //##
                        wxExecute(cmd, output, errors);
                        findErrorStringAndShow(errors); //##Debug
                    }
                    else
                    {
                        getNotifier()->showMessage(_("\nError: Can't communicate with the board.\nTry reset it manually.") + bootPortName, false, true, *wxRED);
                    }
                }
            }
            else
            {
                if (getNotifier())
                {
                    getNotifier()->showMessage(_("\nCan't open port ") + bootPortName, false, true, *wxRED);
                }
            }
        }
    }

    return false;
}


bool Bubble::build()
{
    //##Profiling:
    wxLongLong millis = wxGetLocalTimeMillis();

    //First, reset the progress bar:
    if (getNotifier())
    {
        //##getNotifier()->setProgressPosition(0, false, false);
        getNotifier()->clearMessage();
    }

    if (getBoardName() == wxString("DuinoBot.v1.x.HID")) //##Un-hardcode.
    {
        //##Optimización futura:
        //Esta secuencia de llamadas genera código ligeramente mayor al generado por el IDE 0021 de Arduino.
        if (generateCodeAndSaveToFile())
        {
            //##Terminar de implementar:

            wxArrayString output, errors;
            wxString cmd("");

            //Generates the main.cpp.o file:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega32u4 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Estos son los directorios de include que luego hay que descablear, obteniéndolos de los bloques, que los
                    //mandarán a un array (habrá 2 arrays: uno de "Includes" y otro de "IncludePaths", o mejor: uno de Libraries, y
                    //otro de LibraryPaths, o algo así):
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x.HID_0022\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x.HID_0022/DCMotor\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x.HID_0022/LUFA\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x.HID_0022/LUFA/Common\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x.HID_0022/LUFA/Drivers\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x.HID_0022/LUFA/Scheduler\" ") +
                    wxString("-I\"") + componentPath + wxString("\" ") +
                    wxString("-I\"") + outputPath + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    wxString("\"") + corePath + wxString("/main.cpp") + wxString("\" ") +

                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\"");
            wxArrayString temp; //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //##Generates the .cpp files for the other blocks:
            //##IRRemote.cpp:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega32u4 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Este path va a cambiar, ya que el core va a estar incluído en dirs de Minibloq:
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x.HID_0022\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x.HID_0022/DCMotor\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x.HID_0022/LUFA\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x.HID_0022/LUFA/Common\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x.HID_0022/LUFA/Drivers\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/DuinoBot_Motor.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    //##Near future: The .cpp from the blocks must come from the arrays:
                    wxString("\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources/IRremote.cpp") + wxString("\" ") +

                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/IRremote.cpp.o") + wxString("\"");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //##Servo.cpp:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega32u4 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Este path va a cambiar, ya que el core va a estar incluído en dirs de Minibloq:
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x.HID_0022\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x.HID_0022/DCMotor\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x.HID_0022/LUFA\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x.HID_0022/LUFA/Common\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x.HID_0022/LUFA/Drivers\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/DuinoBot_Motor.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    //##Archivos .cpp de los bloques, deben salir de los arrays:
                    wxString("\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources/Servo.cpp") + wxString("\" ") +
                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/Servo.cpp.o") + wxString("\"");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            //##Ver cómo hacer para mostrar los errores mejor, con doble click (que va a la ventana de código
            //generado, porque luego estarán los errores de bloques, en otra ventana de mensajes, o en la misma), etc...
            if (findErrorStringAndShow(errors))
                return false;

            //##Ver cómo se hará en el futuro para que no haya conflicto con las librerías que hubo que instalar en los cores debido a la falta
            //de HAL real en Arduino...

            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.elf file:
            cmd =   toolsPath + wxString("/avr-gcc -Os -Wl,--gc-sections -mmcu=atmega32u4 -o") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +  //##
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\" ") + //##

                    //##Archivos de las librerías de los bloques (deben salir de la enumeración):
                    //##Conflicto de timers con Tone:
                    /////////////
                    wxString("\"") + outputPath + wxString("/IRremote.cpp.o") + wxString("\" ") + //##
                    /////////////

                    wxString("\"") + outputPath + wxString("/Servo.cpp.o") + wxString("\" ") + //##

                    //wxString("\"") + outputPath + wxString("/DCMotor.cpp.o") + wxString("\" ") + //##

                    wxString("\"") + corePath + wxString("/core.a") + wxString("\"") +
                    wxString(" -L .\\ -lm");
            //wxArrayString tmp; //##
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.epp file:
            cmd =   toolsPath + wxString("/avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.epp") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.hex file:
            cmd =   toolsPath + wxString("/avr-objcopy -O ihex -R .eeprom ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.hex") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //
            cmd =   toolsPath + wxString("/avr-size --mcu=atmega32u4 --format=avr ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            showStream(output); //##Bug: Ver por qué muestra availabl...

            //##Profiling: This may be disabled by the user in the future:
            long millisResult = 0;
            millisResult = wxGetLocalTimeMillis().ToLong() - millis.ToLong();
            getNotifier()->showMessage((_("milliseconds: ") + (wxString("") << millisResult)) + wxString("\n\n"), false, false, *wxWHITE); //##Debug

            //##Ver qué se hace con el retorno y el control de errores:
            return true;
        }
        return false;
    }
    else if (getBoardName() == wxString("DuinoBot.v1.x")) //##Un-hardcode.
    {
        //##Optimización futura:
        //Esta secuencia de llamadas genera código ligeramente mayor al generado por el IDE 0021 de Arduino.
        if (generateCodeAndSaveToFile())
        {
            //##Terminar de implementar:

            wxArrayString output, errors;
            wxString cmd("");

            //Generates the main.cpp.o file:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega32u4 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Estos son los directorios de include que luego hay que descablear, obteniéndolos de los bloques, que los
                    //mandarán a un array (habrá 2 arrays: uno de "Includes" y otro de "IncludePaths", o mejor: uno de Libraries, y
                    //otro de LibraryPaths, o algo así):
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x_0022\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x_0022/DCMotor\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x_0022/LUFA\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x_0022/LUFA/Common\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x_0022/LUFA/Drivers\" ") +
                    wxString("-I\"") + componentPath + wxString("\" ") +
                    wxString("-I\"") + outputPath + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    wxString("\"") + corePath + wxString("/main.cpp") + wxString("\" ") +

                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\"");
            wxArrayString temp; //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //##Generates the .cpp files for the other blocks:
            //##IRRemote.cpp:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega32u4 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Este path va a cambiar, ya que el core va a estar incluído en dirs de Minibloq:
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x_0022\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x_0022/DCMotor\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x_0022/LUFA\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x_0022/LUFA/Common\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x_0022/LUFA/Drivers\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/DuinoBot_Motor.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    //##Near future: The .cpp from the blocks must come from the arrays:
                    wxString("\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources/IRremote.cpp") + wxString("\" ") +

                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/IRremote.cpp.o") + wxString("\"");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //##Servo.cpp:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega32u4 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Este path va a cambiar, ya que el core va a estar incluído en dirs de Minibloq:
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x_0022\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x_0022/DCMotor\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x_0022/LUFA\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x_0022/LUFA/Common\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.v1.x_0022/LUFA/Drivers\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/DuinoBot_Motor.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    //##Archivos .cpp de los bloques, deben salir de los arrays:
                    wxString("\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources/Servo.cpp") + wxString("\" ") +
                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/Servo.cpp.o") + wxString("\"");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            //##Ver cómo hacer para mostrar los errores mejor, con doble click (que va a la ventana de código
            //generado, porque luego estarán los errores de bloques, en otra ventana de mensajes, o en la misma), etc...
            if (findErrorStringAndShow(errors))
                return false;

            //##Ver cómo se hará en el futuro para que no haya conflicto con las librerías que hubo que instalar en los cores debido a la falta
            //de HAL real en Arduino...

            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.elf file:
            cmd =   toolsPath + wxString("/avr-gcc -Os -Wl,--gc-sections -mmcu=atmega32u4 -o") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +  //##
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\" ") + //##

                    //##Archivos de las librerías de los bloques (deben salir de la enumeración):
                    //##Conflicto de timers con Tone:
                    /////////////
                    wxString("\"") + outputPath + wxString("/IRremote.cpp.o") + wxString("\" ") + //##
                    /////////////

                    wxString("\"") + outputPath + wxString("/Servo.cpp.o") + wxString("\" ") + //##

                    //wxString("\"") + outputPath + wxString("/DCMotor.cpp.o") + wxString("\" ") + //##

                    wxString("\"") + corePath + wxString("/core.a") + wxString("\"") +
                    wxString(" -L .\\ -lm");
            //wxArrayString tmp; //##
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.epp file:
            cmd =   toolsPath + wxString("/avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.epp") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.hex file:
            cmd =   toolsPath + wxString("/avr-objcopy -O ihex -R .eeprom ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.hex") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //
            cmd =   toolsPath + wxString("/avr-size --mcu=atmega32u4 --format=avr ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            showStream(output); //##Bug: Ver por qué muestra availabl...

            //##Profiling: This may be disabled by the user in the future:
            long millisResult = 0;
            millisResult = wxGetLocalTimeMillis().ToLong() - millis.ToLong();
            getNotifier()->showMessage((_("milliseconds: ") + (wxString("") << millisResult)) + wxString("\n\n"), false, false, *wxWHITE); //##Debug

            //##Ver qué se hace con el retorno y el control de errores:
            return true;
        }
        return false;
    }
    else if (getBoardName() == wxString("DuinoBot.Kids.v1.x")) //##Un-hardcode.
    {
        //##Optimización futura:
        //Esta secuencia de llamadas genera código ligeramente mayor al generado por el IDE 0021 de Arduino.
        if (generateCodeAndSaveToFile())
        {
            //##Terminar de implementar:

            wxArrayString output, errors;
            wxString cmd("");

            //Generates the main.cpp.o file:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega32u4 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Estos son los directorios de include que luego hay que descablear, obteniéndolos de los bloques, que los
                    //mandarán a un array (habrá 2 arrays: uno de "Includes" y otro de "IncludePaths", o mejor: uno de Libraries, y
                    //otro de LibraryPaths, o algo así):
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.Kids.v1.x_0022\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.Kids.v1.x_0022/DCMotor\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.Kids.v1.x_0022/LUFA\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.Kids.v1.x_0022/LUFA/Common\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.Kids.v1.x_0022/LUFA/Drivers\" ") +
                    wxString("-I\"") + componentPath + wxString("\" ") +
                    wxString("-I\"") + outputPath + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources") + wxString("\" ") +
                    //##Ver cómo se hará en el futuro, para que no conflictúen bloques como el de motor, que pueden estar incluídos en un
                    //core de Arduino, como el de DuinoBot o el DuinoBot.Kids:
                    //##wxString("-I\"") + libPath + wxString("/CPP/Blocks/DuinoBot_Motor.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    wxString("\"") + corePath + wxString("/main.cpp") + wxString("\" ") +

                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\"");
            wxArrayString temp; //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //##Genera los archivos de los .cpp de los demás bloques:
            //##IRRemote.cpp:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega32u4 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Este path va a cambiar, ya que el core va a estar incluído en dirs de Minibloq:
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.Kids.v1.x_0022\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.Kids.v1.x_0022/DCMotor\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.Kids.v1.x_0022/LUFA\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.Kids.v1.x_0022/LUFA/Common\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.Kids.v1.x_0022/LUFA/Drivers\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources") + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/DuinoBot_Motor.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    //##Archivos .cpp de los bloques, deben salir de los arrays:
                    wxString("\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources/IRremote.cpp") + wxString("\" ") +
                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/IRremote.cpp.o") + wxString("\"");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            //##Ver cómo hacer para mostrar los errores mejor, con doble click (que va a la ventana de código
            //generado, porque luego estarán los errores de bloques, en otra ventana de mensajes, o en la misma), etc...
            if (findErrorStringAndShow(errors))
                return false;

            //##Servo.cpp:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega32u4 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Este path va a cambiar, ya que el core va a estar incluído en dirs de Minibloq:
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.Kids.v1.x_0022\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.Kids.v1.x_0022/DCMotor\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.Kids.v1.x_0022/LUFA\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.Kids.v1.x_0022/LUFA/Common\" ") +
                    wxString("-I\"") + corePath + wxString("/DuinoBot.Kids.v1.x_0022/LUFA/Drivers\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources") + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/DuinoBot_Motor.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    //##Archivos .cpp de los bloques, deben salir de los arrays:
                    wxString("\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources/Servo.cpp") + wxString("\" ") +
                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/Servo.cpp.o") + wxString("\"");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            //##Ver cómo hacer para mostrar los errores mejor, con doble click (que va a la ventana de código
            //generado, porque luego estarán los errores de bloques, en otra ventana de mensajes, o en la misma), etc...
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.elf file:
            cmd =   toolsPath + wxString("/avr-gcc -Os -Wl,--gc-sections -mmcu=atmega32u4 -o") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +  //##
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\" ") + //##

                    //##Archivos de las librerías de los bloques (deben salir de la enumeración):
                    //##Conflicto de timers con Tone:
                    /////////////
                    wxString("\"") + outputPath + wxString("/IRremote.cpp.o") + wxString("\" ") + //##
                    /////////////

                    wxString("\"") + outputPath + wxString("/Servo.cpp.o") + wxString("\" ") + //##

                    //##wxString("\"") + outputPath + wxString("/DCMotor.cpp.o") + wxString("\" ") + //##

                    wxString("\"") + corePath + wxString("/core.a") + wxString("\"") +
                    wxString(" -L .\\ -lm");
            //wxArrayString tmp; //##
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.epp file:
            cmd =   toolsPath + wxString("/avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.epp") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.hex file:
            cmd =   toolsPath + wxString("/avr-objcopy -O ihex -R .eeprom ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.hex") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //
            cmd =   toolsPath + wxString("/avr-size --mcu=atmega32u4 --format=avr ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            showStream(output); //##Bug: Ver por qué muestra availabl...

            //##Profiling: This may be disabled by the user in the future:
            long millisResult = 0;
            millisResult = wxGetLocalTimeMillis().ToLong() - millis.ToLong();
            getNotifier()->showMessage((_("milliseconds: ") + (wxString("") << millisResult)) + wxString("\n\n"), false, false, *wxWHITE); //##Debug

            //##Ver qué se hace con el retorno y el control de errores:
            return true;
        }
        return false;
    }
    else if (   (getBoardName() == wxString("Arduino Uno")) ||          //##Un-hardcode.
                (getBoardName() == wxString("Arduino Duemilanove Mega328")) ||  //##Un-hardcode.
                (getBoardName() == wxString("Seeeduino v2.2x Mega328")) //##Un-hardcode.
            )
    {
        //##Optimización futura:
        //Esta secuencia de llamadas genera código ligeramente mayor al generado por el IDE 0021 de Arduino.
        if (generateCodeAndSaveToFile())
        {
            //##Terminar de implementar:

            wxArrayString output, errors;
            wxString cmd("");

            //##Motor.cpp:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Estos son los directorios de include que luego hay que descablear, obteniéndolos de los bloques, que los
                    //mandarán a un array (habrá 2 arrays: uno de "Includes" y otro de "IncludePaths", o mejor: uno de Libraries, y
                    //otro de LibraryPaths, o algo así):
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/ArduinoUNO_0022\" ") +
                    wxString("-I\"") + componentPath + wxString("\" ") +
                    wxString("-I\"") + outputPath + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/Arduino_IRRemote.v1.0/Sources") + wxString("\" ") +
                    //##Ver cómo se hará en el futuro, para que no conflictúen bloques como el de motor, que pueden estar incluídos en un
                    //core de Arduino, como el de DuinoBot o el DuinoBot.Kids:
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0060.Motor.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##Archivos .cpp de los bloques, deben salir de los arrays:
                    wxString("\"") + libPath + wxString("/CPP/Blocks/action.020.0060.Motor.Arduino.v1.0/Sources/DCMotor.cpp") + wxString("\" ") +

                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/DCMotor.cpp.o") + wxString("\"");
            wxArrayString temp; //##
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //##Generates the .cpp files for the other blocks:
            //##IRRemote.cpp:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Este path va a cambiar, ya que el core va a estar incluído en dirs de Minibloq:
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/ArduinoUNO_0022\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    //##Near future: The .cpp from the blocks must come from the arrays:
                    wxString("\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources/IRremote.cpp") + wxString("\" ") +

                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/IRremote.cpp.o") + wxString("\"");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //##Servo.cpp:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Este path va a cambiar, ya que el core va a estar incluído en dirs de Minibloq:
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/ArduinoUNO_0022\" ") +
                    wxString("-I\"") + componentPath + wxString("\" ") +
                    wxString("-I\"") + outputPath + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/Arduino_IRRemote.v1.0/Sources") + wxString("\" ") +
                    //##Ver cómo se hará en el futuro, para que no conflictúen bloques como el de motor, que pueden estar incluídos en un
                    //core de Arduino, como el de DuinoBot o el DuinoBot.Kids:
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    //##Archivos .cpp de los bloques, deben salir de los arrays:
                    wxString("\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources/Servo.cpp") + wxString("\" ") +
                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/Servo.cpp.o") + wxString("\"");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            //##Ver cómo hacer para mostrar los errores mejor, con doble click (que va a la ventana de código
            //generado, porque luego estarán los errores de bloques, en otra ventana de mensajes, o en la misma), etc...
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.o file:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Estos son los directorios de include que luego hay que descablear, obteniéndolos de los bloques, que los
                    //mandarán a un array (habrá 2 arrays: uno de "Includes" y otro de "IncludePaths", o mejor: uno de Libraries, y
                    //otro de LibraryPaths, o algo así):
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/ArduinoUNO_0022\" ") +
                    wxString("-I\"") + componentPath + wxString("\" ") +
                    wxString("-I\"") + outputPath + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/Arduino_IRRemote.v1.0/Sources") + wxString("\" ") +
                    //##Ver cómo se hará en el futuro, para que no conflictúen bloques como el de motor, que pueden estar incluídos en un
                    //core de Arduino, como el de DuinoBot o el DuinoBot.Kids:
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0060.Motor.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    wxString("\"") + corePath + wxString("/main.cpp") + wxString("\" ") +

                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\"");
            //wxArrayString temp; //##
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.elf file from the ".o" and ".a" files:
            cmd =   toolsPath + wxString("/avr-gcc -Os -Wl,--gc-sections -mmcu=atmega328p -o") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +  //##
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\" ") + //##
                    wxString("\"") + outputPath + wxString("/DCMotor.cpp.o") + wxString("\" ") + //##
                    wxString("\"") + outputPath + wxString("/IRremote.cpp.o") + wxString("\" ") + //##
                    wxString("\"") + outputPath + wxString("/Servo.cpp.o") + wxString("\" ") + //##
                    wxString("\"") + corePath + wxString("/core.a") + wxString("\"") +
                    wxString(" -L .\\ -lm");
            //wxArrayString tmp; //##
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.epp file:
            cmd =   toolsPath + wxString("/avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.epp") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.hex file:
            cmd =   toolsPath + wxString("/avr-objcopy -O ihex -R .eeprom ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.hex") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //
            cmd =   toolsPath + wxString("/avr-size --mcu=atmega328p --format=avr ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            showStream(output); //##Bug: Ver por qué muestra availabl...

            //##Profiling: This may be disabled by the user in the future:
            long millisResult = 0;
            millisResult = wxGetLocalTimeMillis().ToLong() - millis.ToLong();
            getNotifier()->showMessage((_("milliseconds: ") + (wxString("") << millisResult)) + wxString("\n\n"), false, false, *wxWHITE); //##Debug

            //##Ver qué se hace con el retorno y el control de errores:
            return true;
        }
        return false;
    }
    else if (   (getBoardName() == wxString("Seeeduino Mega 1280")) ||
                (getBoardName() == wxString("Arduino Mega 1280"))
            )
    {
        //##Optimización futura:
        //Esta secuencia de llamadas genera código ligeramente mayor al generado por el IDE 0021 de Arduino.
        if (generateCodeAndSaveToFile())
        {
            //##Terminar de implementar:

            wxArrayString output, errors;
            wxString cmd("");

            //##Motor.cpp:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega1280 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Estos son los directorios de include que luego hay que descablear, obteniéndolos de los bloques, que los
                    //mandarán a un array (habrá 2 arrays: uno de "Includes" y otro de "IncludePaths", o mejor: uno de Libraries, y
                    //otro de LibraryPaths, o algo así):
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/ArduinoMega1280_0022\" ") +
                    wxString("-I\"") + componentPath + wxString("\" ") +
                    wxString("-I\"") + outputPath + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/Arduino_IRRemote.v1.0/Sources") + wxString("\" ") +
                    //##Ver cómo se hará en el futuro, para que no conflictúen bloques como el de motor, que pueden estar incluídos en un
                    //core de Arduino, como el de DuinoBot o el DuinoBot.Kids:
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0060.Motor.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##Archivos .cpp de los bloques, deben salir de los arrays:
                    wxString("\"") + libPath + wxString("/CPP/Blocks/action.020.0060.Motor.Arduino.v1.0/Sources/DCMotor.cpp") + wxString("\" ") +

                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/DCMotor.cpp.o") + wxString("\"");
            wxArrayString temp; //##
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //##Generates the .cpp files for the other blocks:
            //##IRRemote.cpp:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega1280 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Este path va a cambiar, ya que el core va a estar incluído en dirs de Minibloq:
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/ArduinoMega1280_0022\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    //##Near future: The .cpp from the blocks must come from the arrays:
                    wxString("\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources/IRremote.cpp") + wxString("\" ") +

                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/IRremote.cpp.o") + wxString("\"");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //##Servo.cpp:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega1280 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Este path va a cambiar, ya que el core va a estar incluído en dirs de Minibloq:
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/ArduinoMega1280_0022\" ") +
                    wxString("-I\"") + componentPath + wxString("\" ") +
                    wxString("-I\"") + outputPath + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/Arduino_IRRemote.v1.0/Sources") + wxString("\" ") +
                    //##Ver cómo se hará en el futuro, para que no conflictúen bloques como el de motor, que pueden estar incluídos en un
                    //core de Arduino, como el de DuinoBot o el DuinoBot.Kids:
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    //##Archivos .cpp de los bloques, deben salir de los arrays:
                    wxString("\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources/Servo.cpp") + wxString("\" ") +
                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/Servo.cpp.o") + wxString("\"");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            //##Ver cómo hacer para mostrar los errores mejor, con doble click (que va a la ventana de código
            //generado, porque luego estarán los errores de bloques, en otra ventana de mensajes, o en la misma), etc...
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.o file:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega1280 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Estos son los directorios de include que luego hay que descablear, obteniéndolos de los bloques, que los
                    //mandarán a un array (habrá 2 arrays: uno de "Includes" y otro de "IncludePaths", o mejor: uno de Libraries, y
                    //otro de LibraryPaths, o algo así):
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/ArduinoMega1280_0022\" ") +
                    wxString("-I\"") + componentPath + wxString("\" ") +
                    wxString("-I\"") + outputPath + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/Arduino_IRRemote.v1.0/Sources") + wxString("\" ") +
                    //##Ver cómo se hará en el futuro, para que no conflictúen bloques como el de motor, que pueden estar incluídos en un
                    //core de Arduino, como el de DuinoBot o el DuinoBot.Kids:
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0060.Motor.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    wxString("\"") + corePath + wxString("/main.cpp") + wxString("\" ") +

                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\"");
            //wxArrayString temp; //##
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.elf file from the ".o" and ".a" files:
            cmd =   toolsPath + wxString("/avr-gcc -Os -Wl,--gc-sections -mmcu=atmega1280 -o") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +  //##
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\" ") + //##
                    wxString("\"") + outputPath + wxString("/DCMotor.cpp.o") + wxString("\" ") + //##
                    wxString("\"") + outputPath + wxString("/IRremote.cpp.o") + wxString("\" ") + //##
                    wxString("\"") + outputPath + wxString("/Servo.cpp.o") + wxString("\" ") + //##
                    wxString("\"") + corePath + wxString("/core.a") + wxString("\"") +
                    wxString(" -L .\\ -lm");
            //wxArrayString tmp; //##
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.epp file:
            cmd =   toolsPath + wxString("/avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.epp") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.hex file:
            cmd =   toolsPath + wxString("/avr-objcopy -O ihex -R .eeprom ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.hex") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //
            cmd =   toolsPath + wxString("/avr-size --mcu=atmega1280 --format=avr ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            showStream(output); //##Bug: Ver por qué muestra availabl...

            //##Profiling: This may be disabled by the user in the future:
            long millisResult = 0;
            millisResult = wxGetLocalTimeMillis().ToLong() - millis.ToLong();
            getNotifier()->showMessage((_("milliseconds: ") + (wxString("") << millisResult)) + wxString("\n\n"), false, false, *wxWHITE); //##Debug

            //##Ver qué se hace con el retorno y el control de errores:
            return true;
        }
        return false;
    }
    else if (getBoardName() == wxString("Arduino Mega 2560"))
    {
        //##Optimización futura:
        //Esta secuencia de llamadas genera código ligeramente mayor al generado por el IDE 0021 de Arduino.
        if (generateCodeAndSaveToFile())
        {
            //##Terminar de implementar:

            wxArrayString output, errors;
            wxString cmd("");

            //##Motor.cpp:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Estos son los directorios de include que luego hay que descablear, obteniéndolos de los bloques, que los
                    //mandarán a un array (habrá 2 arrays: uno de "Includes" y otro de "IncludePaths", o mejor: uno de Libraries, y
                    //otro de LibraryPaths, o algo así):
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/ArduinoMega2560_0022\" ") +
                    wxString("-I\"") + componentPath + wxString("\" ") +
                    wxString("-I\"") + outputPath + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/Arduino_IRRemote.v1.0/Sources") + wxString("\" ") +
                    //##Ver cómo se hará en el futuro, para que no conflictúen bloques como el de motor, que pueden estar incluídos en un
                    //core de Arduino, como el de DuinoBot o el DuinoBot.Kids:
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0060.Motor.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##Archivos .cpp de los bloques, deben salir de los arrays:
                    wxString("\"") + libPath + wxString("/CPP/Blocks/action.020.0060.Motor.Arduino.v1.0/Sources/DCMotor.cpp") + wxString("\" ") +

                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/DCMotor.cpp.o") + wxString("\"");
            wxArrayString temp; //##
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //##Generates the .cpp files for the other blocks:
            //##IRRemote.cpp:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Este path va a cambiar, ya que el core va a estar incluído en dirs de Minibloq:
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/ArduinoMega2560_0022\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    //##Near future: The .cpp from the blocks must come from the arrays:
                    wxString("\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources/IRremote.cpp") + wxString("\" ") +

                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/IRremote.cpp.o") + wxString("\"");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //##Servo.cpp:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Este path va a cambiar, ya que el core va a estar incluído en dirs de Minibloq:
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/ArduinoMega2560_0022\" ") +
                    wxString("-I\"") + componentPath + wxString("\" ") +
                    wxString("-I\"") + outputPath + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/Arduino_IRRemote.v1.0/Sources") + wxString("\" ") +
                    //##Ver cómo se hará en el futuro, para que no conflictúen bloques como el de motor, que pueden estar incluídos en un
                    //core de Arduino, como el de DuinoBot o el DuinoBot.Kids:
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    //##Archivos .cpp de los bloques, deben salir de los arrays:
                    wxString("\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources/Servo.cpp") + wxString("\" ") +
                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/Servo.cpp.o") + wxString("\"");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            //##Ver cómo hacer para mostrar los errores mejor, con doble click (que va a la ventana de código
            //generado, porque luego estarán los errores de bloques, en otra ventana de mensajes, o en la misma), etc...
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.o file:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Estos son los directorios de include que luego hay que descablear, obteniéndolos de los bloques, que los
                    //mandarán a un array (habrá 2 arrays: uno de "Includes" y otro de "IncludePaths", o mejor: uno de Libraries, y
                    //otro de LibraryPaths, o algo así):
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/ArduinoMega2560_0022\" ") +
                    wxString("-I\"") + componentPath + wxString("\" ") +
                    wxString("-I\"") + outputPath + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/Arduino_IRRemote.v1.0/Sources") + wxString("\" ") +
                    //##Ver cómo se hará en el futuro, para que no conflictúen bloques como el de motor, que pueden estar incluídos en un
                    //core de Arduino, como el de DuinoBot o el DuinoBot.Kids:
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0060.Motor.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    wxString("\"") + corePath + wxString("/main.cpp") + wxString("\" ") +

                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\"");
            //wxArrayString temp; //##
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.elf file from the ".o" and ".a" files:
            cmd =   toolsPath + wxString("/avr-gcc -Os -Wl,--gc-sections -mmcu=atmega2560 -o") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +  //##
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\" ") + //##
                    wxString("\"") + outputPath + wxString("/DCMotor.cpp.o") + wxString("\" ") + //##
                    wxString("\"") + outputPath + wxString("/IRremote.cpp.o") + wxString("\" ") + //##
                    wxString("\"") + outputPath + wxString("/Servo.cpp.o") + wxString("\" ") + //##
                    wxString("\"") + corePath + wxString("/core.a") + wxString("\"") +
                    wxString(" -L .\\ -lm");
            //wxArrayString tmp; //##
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.epp file:
            cmd =   toolsPath + wxString("/avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.epp") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.hex file:
            cmd =   toolsPath + wxString("/avr-objcopy -O ihex -R .eeprom ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.hex") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //
            cmd =   toolsPath + wxString("/avr-size --mcu=atmega2560 --format=avr ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            showStream(output); //##Bug: Ver por qué muestra availabl...

            //##Profiling: This may be disabled by the user in the future:
            long millisResult = 0;
            millisResult = wxGetLocalTimeMillis().ToLong() - millis.ToLong();
            getNotifier()->showMessage((_("milliseconds: ") + (wxString("") << millisResult)) + wxString("\n\n"), false, false, *wxWHITE); //##Debug

            //##Ver qué se hace con el retorno y el control de errores:
            return true;
        }
        return false;
    }
    else if (getBoardName() == wxString("Arduino Duemilanove Mega168"))
    {
        //##Optimización futura:
        //Esta secuencia de llamadas genera código ligeramente mayor al generado por el IDE 0021 de Arduino.
        if (generateCodeAndSaveToFile())
        {
            //##Terminar de implementar:

            wxArrayString output, errors;
            wxString cmd("");

            //##Motor.cpp:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega168 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Estos son los directorios de include que luego hay que descablear, obteniéndolos de los bloques, que los
                    //mandarán a un array (habrá 2 arrays: uno de "Includes" y otro de "IncludePaths", o mejor: uno de Libraries, y
                    //otro de LibraryPaths, o algo así):
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/ArduinoM168_0022\" ") +
                    wxString("-I\"") + componentPath + wxString("\" ") +
                    wxString("-I\"") + outputPath + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/Arduino_IRRemote.v1.0/Sources") + wxString("\" ") +
                    //##Ver cómo se hará en el futuro, para que no conflictúen bloques como el de motor, que pueden estar incluídos en un
                    //core de Arduino, como el de DuinoBot o el DuinoBot.Kids:
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0060.Motor.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##Archivos .cpp de los bloques, deben salir de los arrays:
                    wxString("\"") + libPath + wxString("/CPP/Blocks/action.020.0060.Motor.Arduino.v1.0/Sources/DCMotor.cpp") + wxString("\" ") +

                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/DCMotor.cpp.o") + wxString("\"");
            wxArrayString temp; //##
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //##Generates the .cpp files for the other blocks:
            //##IRRemote.cpp:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega168 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Este path va a cambiar, ya que el core va a estar incluído en dirs de Minibloq:
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/ArduinoM168_0022\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    //##Near future: The .cpp from the blocks must come from the arrays:
                    wxString("\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources/IRremote.cpp") + wxString("\" ") +

                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/IRremote.cpp.o") + wxString("\"");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //##Servo.cpp:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega168 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Este path va a cambiar, ya que el core va a estar incluído en dirs de Minibloq:
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/ArduinoM168_0022\" ") +
                    wxString("-I\"") + componentPath + wxString("\" ") +
                    wxString("-I\"") + outputPath + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/Arduino_IRRemote.v1.0/Sources") + wxString("\" ") +
                    //##Ver cómo se hará en el futuro, para que no conflictúen bloques como el de motor, que pueden estar incluídos en un
                    //core de Arduino, como el de DuinoBot o el DuinoBot.Kids:
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    //##Archivos .cpp de los bloques, deben salir de los arrays:
                    wxString("\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources/Servo.cpp") + wxString("\" ") +
                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/Servo.cpp.o") + wxString("\"");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            //##Ver cómo hacer para mostrar los errores mejor, con doble click (que va a la ventana de código
            //generado, porque luego estarán los errores de bloques, en otra ventana de mensajes, o en la misma), etc...
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.o file:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega168 -DF_CPU=16000000L -DARDUINO=22 ") +

                    //##Estos son los directorios de include que luego hay que descablear, obteniéndolos de los bloques, que los
                    //mandarán a un array (habrá 2 arrays: uno de "Includes" y otro de "IncludePaths", o mejor: uno de Libraries, y
                    //otro de LibraryPaths, o algo así):
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/ArduinoM168_0022\" ") +
                    wxString("-I\"") + componentPath + wxString("\" ") +
                    wxString("-I\"") + outputPath + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/Arduino_IRRemote.v1.0/Sources") + wxString("\" ") +
                    //##Ver cómo se hará en el futuro, para que no conflictúen bloques como el de motor, que pueden estar incluídos en un
                    //core de Arduino, como el de DuinoBot o el DuinoBot.Kids:
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0060.Motor.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0070.ServoRC.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    wxString("\"") + corePath + wxString("/main.cpp") + wxString("\" ") +

                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\"");
            //wxArrayString temp; //##
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.elf file from the ".o" and ".a" files:
            cmd =   toolsPath + wxString("/avr-gcc -Os -Wl,--gc-sections -mmcu=atmega168 -o") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +  //##
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\" ") + //##
                    wxString("\"") + outputPath + wxString("/DCMotor.cpp.o") + wxString("\" ") + //##
                    wxString("\"") + outputPath + wxString("/IRremote.cpp.o") + wxString("\" ") + //##
                    wxString("\"") + outputPath + wxString("/Servo.cpp.o") + wxString("\" ") + //##
                    wxString("\"") + corePath + wxString("/core.a") + wxString("\"") +
                    wxString(" -L .\\ -lm");
            //wxArrayString tmp; //##
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.epp file:
            cmd =   toolsPath + wxString("/avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.epp") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.hex file:
            cmd =   toolsPath + wxString("/avr-objcopy -O ihex -R .eeprom ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.hex") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //
            cmd =   toolsPath + wxString("/avr-size --mcu=atmega168 --format=avr ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            showStream(output); //##Bug: Ver por qué muestra availabl...

            //##Profiling: This may be disabled by the user in the future:
            long millisResult = 0;
            millisResult = wxGetLocalTimeMillis().ToLong() - millis.ToLong();
            getNotifier()->showMessage((_("milliseconds: ") + (wxString("") << millisResult)) + wxString("\n\n"), false, false, *wxWHITE); //##Debug

            //##Ver qué se hace con el retorno y el control de errores:
            return true;
        }
        return false;
    }
    else if (getBoardName() == wxString("Maple Rev 3+ (to Flash)"))
    {
        //##Optimización futura:
        //Esta secuencia de llamadas genera código ligeramente mayor al generado por el IDE 0021 de Arduino.
        if (generateCodeAndSaveToFile())
        {
            //##Terminar de implementar:
            wxArrayString output, errors;
            wxString cmd("");
            //##Motor.cpp:
            cmd =   toolsPath + wxString("/arm-none-eabi-g++ -Os -g -mcpu=cortex-m3 -mthumb -march=armv7-m -nostdlib -ffunction-sections ") +
                    wxString("-fdata-sections -Wl,--gc-sections -DBOARD_maple -DMCU_STM32F103RB -DSTM32_MEDIUM_DENSITY -DVECT_TAB_FLASH -DERROR_LED_PORT=GPIOA -DERROR_LED_PIN=5 -DMAPLE_IDE ") +
                    wxString("-I\"") + corePath + wxString("/Maple_0011\" ") +
                    wxString("-fno-rtti -fno-exceptions -Wall ") +
                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/DCMotor.cpp.o") + wxString("\" ") +
                    wxString("-c ") +
                    wxString("\"") + libPath + wxString("/CPP/Blocks/action.020.0060.Motor.Arduino.v1.0/Sources/DCMotor.cpp") + wxString("\" ");
            wxArrayString temp; //##
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

//            //##IRRemote.cpp: Not supported in Maple by the moment:
//            cmd =   toolsPath + wxString("/arm-none-eabi-g++ -Os -g -mcpu=cortex-m3 -mthumb -march=armv7-m -nostdlib -ffunction-sections ") +
//                    wxString("-fdata-sections -Wl,--gc-sections -DBOARD_maple -DMCU_STM32F103RB -DSTM32_MEDIUM_DENSITY -DVECT_TAB_FLASH -DERROR_LED_PORT=GPIOA -DERROR_LED_PIN=5 -DMAPLE_IDE ") +
//                    wxString("-I\"") + corePath + wxString("/Maple_0011\" ") +
//                    wxString("-fno-rtti -fno-exceptions -Wall ") +
//                    wxString("-o") +
//                    wxString("\"") + outputPath + wxString("/IRRemote.cpp.o") + wxString("\" ") +
//                    wxString("-c ") +
//                    //##Futuro cercano:  Renombrar "IRremote.cpp" por "IRRemote.cpp":
//                    wxString("\"") + libPath + wxString("/CPP/Blocks/Arduino_IRRemote.v1.0/Sources/IRremote.cpp") + wxString("\" ");
//            temp.Clear(); //##
//            temp.Add(cmd);
//            showStream(temp); //##
//            wxExecute(cmd, output, errors);
//            if (findErrorStringAndShow(errors))
//                return false;

            //Generates the main.cpp.o file:
            cmd =   toolsPath + wxString("/arm-none-eabi-g++ -Os -g -mcpu=cortex-m3 -mthumb -march=armv7-m -nostdlib -ffunction-sections ") +
                    wxString("-fdata-sections -Wl,--gc-sections -DBOARD_maple -DMCU_STM32F103RB -DSTM32_MEDIUM_DENSITY -DVECT_TAB_FLASH -DERROR_LED_PORT=GPIOA -DERROR_LED_PIN=5 -DMAPLE_IDE ") +
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/Maple_0011\" ") +
                    wxString("-I\"") + componentPath + wxString("\" ") +
                    wxString("-I\"") + outputPath + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0060.Motor.Arduino.v1.0/Sources") + wxString("\" ") + //##
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") + //##
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-fno-rtti -fno-exceptions -Wall ") +
                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\" ") +
                    wxString("-c ") +
                    wxString("\"") + corePath + wxString("/main.cpp") + wxString("\" ");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Linking:
            //##:
            cmd =   toolsPath + wxString("/arm-none-eabi-g++ ") +
                    wxString("-T\"") + corePath + wxString("/Maple_0011/maple/flash.ld\" ") +
                    wxString("-L\"") + corePath + wxString("/Maple_0011\"") +
                    wxString("-mcpu=cortex-m3 -mthumb -Xlinker --gc-sections --print-gc-sections --march=armv7-m -Wall ") +
                    wxString("-o \"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +

                    //Un-hardcode!:
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/exc.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/adc.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/bkp.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/dac.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/descriptors.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/dma.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/exti.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/flash.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/fsmc.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/gpio.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/i2c.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/iwdg.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/nvic.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/pwr.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/rcc.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/spi.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/syscalls.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/systick.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/timer.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/usart.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/usb.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/usb_callbacks.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/usb_core.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/usb_hardware.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/usb_init.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/usb_int.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/usb_mem.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/usb_regs.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/util.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/boards.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/cxxabi-compat.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/ext_interrupts.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/HardwareSerial.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/HardwareSPI.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/HardwareTimer.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/maple.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/maple_mini.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/maple_native.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/maple_RET6.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/Print.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/pwm.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/usb_serial.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/wirish_analog.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/wirish_digital.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/wirish_math.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/wirish_shift.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/Flash/wirish_time.o\" ") +

                    wxString("\"") + outputPath + wxString("/DCMotor.cpp.o") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\" ") +

                    wxString("-L\"") + corePath + wxString("/Maple_0011\"");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //elf to bin file:
            cmd =   toolsPath + wxString("/arm-none-eabi-objcopy -v -Obinary ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.bin") + wxString("\" ");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Computing size:
            cmd =   toolsPath + wxString("/arm-none-eabi-size --target=binary -A ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.bin") + wxString("\" ");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(output))
                return false;

            //##Profiling: This may be disabled by the user in the future:
            long millisResult = 0;
            millisResult = wxGetLocalTimeMillis().ToLong() - millis.ToLong();
            getNotifier()->showMessage((_("milliseconds: ") + (wxString("") << millisResult)) + wxString("\n\n"), false, false, *wxWHITE); //##Debug

            //##Ver qué se hace con el retorno y el control de errores:
            return true;
        }
        return false;
    }
    else if (getBoardName() == wxString("Maple Rev 3+ (to RAM)"))
    {
        //##Optimización futura:
        //Esta secuencia de llamadas genera código ligeramente mayor al generado por el IDE 0021 de Arduino.
        if (generateCodeAndSaveToFile())
        {
            //##Terminar de implementar:
            wxArrayString output, errors;
            wxString cmd("");

            //##Motor.cpp:
            cmd =   toolsPath + wxString("/arm-none-eabi-g++ -Os -g -mcpu=cortex-m3 -mthumb -march=armv7-m -nostdlib -ffunction-sections ") +
                    wxString("-fdata-sections -Wl,--gc-sections -DBOARD_maple -DMCU_STM32F103RB -DSTM32_MEDIUM_DENSITY -DVECT_TAB_RAM -DERROR_LED_PORT=GPIOA -DERROR_LED_PIN=5 -DMAPLE_IDE ") +
                    wxString("-I\"") + corePath + wxString("/Maple_0011\" ") +
                    wxString("-fno-rtti -fno-exceptions -Wall ") +
                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/DCMotor.cpp.o") + wxString("\" ") +
                    wxString("-c ") +
                    wxString("\"") + libPath + wxString("/CPP/Blocks/action.020.0060.Motor.Arduino.v1.0/Sources/DCMotor.cpp") + wxString("\" ");
            wxArrayString temp; //##
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

//            //##IRRemote.cpp: Not supported in Maple by the moment:
//            cmd =   toolsPath + wxString("/arm-none-eabi-g++ -Os -g -mcpu=cortex-m3 -mthumb -march=armv7-m -nostdlib -ffunction-sections ") +
//                    wxString("-fdata-sections -Wl,--gc-sections -DBOARD_maple -DMCU_STM32F103RB -DSTM32_MEDIUM_DENSITY -DVECT_TAB_RAM -DERROR_LED_PORT=GPIOA -DERROR_LED_PIN=5 -DMAPLE_IDE ") +
//                    wxString("-I\"") + corePath + wxString("/Maple_0011\" ") +
//                    wxString("-fno-rtti -fno-exceptions -Wall ") +
//                    wxString("-o") +
//                    wxString("\"") + outputPath + wxString("/IRRemote.cpp.o") + wxString("\" ") +
//                    wxString("-c ") +
//                    //##Futuro cercano:  Renombrar "IRremote.cpp" por "IRRemote.cpp":
//                    wxString("\"") + libPath + wxString("/CPP/Blocks/Arduino_IRRemote.v1.0/Sources/IRremote.cpp") + wxString("\" ");
//            temp.Clear(); //##
//            temp.Add(cmd);
//            showStream(temp); //##
//            wxExecute(cmd, output, errors);
//            if (findErrorStringAndShow(errors))
//                return false;

            //Generates the main.cpp.o file:
            cmd =   toolsPath + wxString("/arm-none-eabi-g++ -Os -g -mcpu=cortex-m3 -mthumb -march=armv7-m -nostdlib -ffunction-sections ") +
                    wxString("-fdata-sections -Wl,--gc-sections -DBOARD_maple -DMCU_STM32F103RB -DSTM32_MEDIUM_DENSITY -DVECT_TAB_RAM -DERROR_LED_PORT=GPIOA -DERROR_LED_PIN=5 -DMAPLE_IDE ") +
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + corePath + wxString("/Maple_0011\" ") +
                    wxString("-I\"") + componentPath + wxString("\" ") +
                    wxString("-I\"") + outputPath + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0060.Motor.Arduino.v1.0/Sources") + wxString("\" ") + //##
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") + //##
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-fno-rtti -fno-exceptions -Wall ") +
                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\" ") +
                    wxString("-c ") +
                    wxString("\"") + corePath + wxString("/main.cpp") + wxString("\" ");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Linking:
            //##:
            cmd =   toolsPath + wxString("/arm-none-eabi-g++ ") +
                    wxString("-T\"") + corePath + wxString("/Maple_0011/maple/ram.ld\" ") +
                    wxString("-L\"") + corePath + wxString("/Maple_0011\"") +
                    wxString("-mcpu=cortex-m3 -mthumb -Xlinker --gc-sections --print-gc-sections --march=armv7-m -Wall ") +
                    wxString("-o \"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +

                    //Un-hardcode!:
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/exc.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/adc.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/bkp.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/dac.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/descriptors.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/dma.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/exti.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/flash.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/fsmc.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/gpio.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/i2c.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/iwdg.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/nvic.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/pwr.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/rcc.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/spi.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/syscalls.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/systick.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/timer.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/usart.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/usb.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/usb_callbacks.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/usb_core.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/usb_hardware.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/usb_init.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/usb_int.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/usb_mem.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/usb_regs.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/util.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/boards.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/cxxabi-compat.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/ext_interrupts.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/HardwareSerial.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/HardwareSPI.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/HardwareTimer.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/maple.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/maple_mini.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/maple_native.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/maple_RET6.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/Print.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/pwm.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/usb_serial.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/wirish_analog.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/wirish_digital.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/wirish_math.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/wirish_shift.o\" ") +
                    wxString("\"") + corePath + wxString("/Maple_0011/Obj/RAM/wirish_time.o\" ") +

                    wxString("\"") + outputPath + wxString("/DCMotor.cpp.o") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\" ") +

                    wxString("-L\"") + corePath + wxString("/Maple_0011\"");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //elf to bin file:
            cmd =   toolsPath + wxString("/arm-none-eabi-objcopy -v -Obinary ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.bin") + wxString("\" ");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Computing size:
            cmd =   toolsPath + wxString("/arm-none-eabi-size --target=binary -A  ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.bin") + wxString("\" ");
            temp.Clear(); //##
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(output))
                return false;

            //##Profiling: This may be disabled by the user in the future:
            long millisResult = 0;
            millisResult = wxGetLocalTimeMillis().ToLong() - millis.ToLong();
            getNotifier()->showMessage((_("milliseconds: ") + (wxString("") << millisResult)) + wxString("\n\n"), false, false, *wxWHITE); //##Debug

            //##Ver qué se hace con el retorno y el control de errores:
            return true;
        }
        return false;
    }
    else if (   getBoardName() == wxString("ATTiny25 (with ArduinoISP)") ||
                getBoardName() == wxString("ATTiny45 (with ArduinoISP)") ||
                getBoardName() == wxString("ATTiny85 (with ArduinoISP)") ||
                getBoardName() == wxString("ATTiny25 (with Doper)") ||
                getBoardName() == wxString("ATTiny45 (with Doper)") ||
                getBoardName() == wxString("ATTiny85 (with Doper)")
            )
    {
        //##Optimización futura:
        wxString mcu("");
        if (getBoardName() == wxString("ATTiny25 (with Doper)") ||
            getBoardName() == wxString("ATTiny25 (with ArduinoISP)"))
            mcu = "attiny25";
        else if (   getBoardName() == wxString("ATTiny45 (with Doper)") ||
                    getBoardName() == wxString("ATTiny45 (with ArduinoISP)"))
            mcu = "attiny45";
        else if (   getBoardName() == wxString("ATTiny85 (with Doper)") ||
                    getBoardName() == wxString("ATTiny85 (with ArduinoISP)"))
            mcu = "attiny85";

        //Esta secuencia de llamadas genera código ligeramente mayor al generado por el IDE 0021 de Arduino.
        if (generateCodeAndSaveToFile())
        {
            //##Terminar de implementar:

            wxArrayString output, errors;
            wxString cmd("");

            //##Generates the .cpp files for the other blocks:
            ////##IRRemote.cpp:
            //cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu="+
            //        mcu + " -DF_CPU=1000000L -DARDUINO=18 ") +
            //        //##Este path va a cambiar, ya que el core va a estar incluído en dirs de Minibloq:
            //        wxString("-I\"") + corePath + wxString("\" ") +
            //        wxString("-I\"") + corePath + wxString("/Tiny25.45.85_0018\" ") +
            //        wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
            //
            //        //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +
            //
            //        //##Near future: The .cpp from the blocks must come from the arrays:
            //        wxString("\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources/IRremote.cpp") + wxString("\" ") +
            //
            //        wxString("-o") +
            //        wxString("\"") + outputPath + wxString("/IRremote.cpp.o") + wxString("\"");
            //

            wxArrayString temp;
            //temp.Clear(); //##
            //temp.Add(cmd);
            //showStream(temp); //##
            //wxExecute(cmd, output, errors);
            //if (findErrorStringAndShow(errors))
            //    return false;

            //Generates the main.cpp.o file:
            cmd =   toolsPath + wxString("/avr-g++ -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu="+
                    mcu + " -DF_CPU=1000000L -DARDUINO=18 ") +

                    //##Estos son los directorios de include que luego hay que descablear, obteniéndolos de los bloques, que los
                    //mandarán a un array (habrá 2 arrays: uno de "Includes" y otro de "IncludePaths", o mejor: uno de Libraries, y
                    //otro de LibraryPaths, o algo así):
                    wxString("-I\"") + corePath + wxString("/Tiny25.45.85_0018\" ") +
                    wxString("-I\"") + corePath + wxString("\" ") +
                    wxString("-I\"") + componentPath + wxString("\" ") +
                    wxString("-I\"") + outputPath + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/Arduino_IRRemote.v1.0/Sources") + wxString("\" ") +
                    //##Ver cómo se hará en el futuro, para que no conflictúen bloques como el de motor, que pueden estar incluídos en un
                    //core de Arduino, como el de DuinoBot o el DuinoBot.Kids:
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0060.Motor.Arduino.v1.0/Sources") + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/action.020.0050.Buzzer.Arduino.v1.0/Sources") + wxString("\" ") +
                    //wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0010.IRRemote.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0020.Ping.Arduino.v1.0/Sources") + wxString("\" ") +
                    wxString("-I\"") + libPath + wxString("/CPP/Blocks/number.200.0030.IRRanger.Arduino.v1.0/Sources") + wxString("\" ") +

                    //##2011.03.07:wxString("\"") + componentPath + wxString("/main.cpp") + wxString("\"") +

                    wxString("\"") + corePath + wxString("/main.cpp") + wxString("\" ") +

                    wxString("-o") +
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\"");
            //wxArrayString temp; //##
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.elf file from the ".o" and ".a" files:
            cmd =   toolsPath + wxString("/avr-gcc -Os -Wl,--gc-sections -mmcu=" + mcu + " -o") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +  //##
                    wxString("\"") + outputPath + wxString("/main.cpp.o") + wxString("\" ") + //##
                    //wxString("\"") + outputPath + wxString("/DCMotor.cpp.o") + wxString("\" ") + //##
                    //wxString("\"") + outputPath + wxString("/IRremote.cpp.o") + wxString("\" ") + //##
                    wxString("\"") + corePath + wxString("/core.a") + wxString("\"") +
                    wxString(" -L .\\ -lm");
            //wxArrayString tmp; //##
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.epp file:
            cmd =   toolsPath + wxString("/avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.epp") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //Generates the main.cpp.hex file:
            cmd =   toolsPath + wxString("/avr-objcopy -O ihex -R .eeprom ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\" ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.hex") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            if (findErrorStringAndShow(errors))
                return false;

            //
            cmd =   toolsPath + wxString("/avr-size --mcu=" + mcu + " --format=avr ") +
                    wxString("\"") + outputPath + wxString("/main.cpp.elf") + wxString("\"");
            temp.Clear();
            temp.Add(cmd);
            showStream(temp); //##
            wxExecute(cmd, output, errors);
            showStream(output); //##Bug: Ver por qué muestra availabl...

            //##Profiling: This may be disabled by the user in the future:
            long millisResult = 0;
            millisResult = wxGetLocalTimeMillis().ToLong() - millis.ToLong();
            getNotifier()->showMessage((_("milliseconds: ") + (wxString("") << millisResult)) + wxString("\n\n"), false, false, *wxWHITE); //##Debug

            //##Ver qué se hace con el retorno y el control de errores:
            return true;
        }
        return false;
    }

    return false;
}


//##Esto será también configurable para cada target (porque el reset podría ocurrir de muchas formas
//diferentes, o sobre señales distintas de los ports):
bool Bubble::resetBoard()
{
    if (getBoardName() == wxString("ATTiny25 (with ArduinoISP)") ||
        getBoardName() == wxString("ATTiny45 (with ArduinoISP)") ||
        getBoardName() == wxString("ATTiny85 (with ArduinoISP)") ||
        getBoardName() == wxString("ATTiny25 (with Doper)") ||
        getBoardName() == wxString("ATTiny45 (with Doper)") ||
        getBoardName() == wxString("ATTiny85 (with Doper)")
       )
    {
        return true;
    }

    if (!bootSerialPort.IsOpen())
		bootSerialPort.Open(bootPortName.char_str());
    if (bootSerialPort.IsOpen()) //This is NOT the same as en "else"!
    {
        if (getBoardName() == wxString("DuinoBot.v1.x") || //##Un-hardcode:
            getBoardName() == wxString("DuinoBot.Kids.v1.x") ) //##Un-hardcode:
        {
            bootSerialPort.SetLineState(wxSERIAL_LINESTATE_DTR);
            wxMilliSleep(100); //##Make this configurable.
            bootSerialPort.ClrLineState(wxSERIAL_LINESTATE_DTR);
        }
        else if (   getBoardName() == wxString("Maple Rev 3+ (to Flash)") || //##Un-hardcode.
                    getBoardName() == wxString("Maple Rev 3+ (to RAM)")
                )
        {
            bootSerialPort.SetLineState(wxSERIAL_LINESTATE_RTS);
            wxMilliSleep(10); //##Make this configurable.
            bootSerialPort.SetLineState(wxSERIAL_LINESTATE_DTR);
            wxMilliSleep(10); //##Make this configurable.
            bootSerialPort.ClrLineState(wxSERIAL_LINESTATE_DTR);
            wxMilliSleep(10); //##Make this configurable.
            char buf[] = "1EAF";
            bootSerialPort.Write(buf, 4);
        }
        else
        {
            //##Standard Arduino:
            bootSerialPort.SetLineState(wxSERIAL_LINESTATE_DTR);
            wxMilliSleep(10); //##Make this configurable.
            bootSerialPort.ClrLineState(wxSERIAL_LINESTATE_DTR);
            //wxMilliSleep(5); //##Make this configurable.
            //bootSerialPort.SetLineState(wxSERIAL_LINESTATE_DTR);
        }
        bootSerialPort.Close();
        return true;
    }
	return false;
}


bool Bubble::verifyBoard()
{
    //##Esto debe ser diferente para cada placa:
    if (getBoardName() == wxString("DuinoBot.v1.x") || //##Un-hardcode:
        getBoardName() == wxString("DuinoBot.Kids.v1.x") ) //##Un-hardcode:
    {
        try
        {
            if (!bootSerialPort.IsOpen())
                bootSerialPort.Open(bootPortName.char_str());
            if (bootSerialPort.IsOpen()) //This is NOT the same as en "else"!
            {
                char c = 'S'; //##Pide el string "LUFACDC" que es el identificador del bootloader...
                bootSerialPort.Write(&c, 1);

                char buffer[12]; //##Hacer el tamaño configurable.
                unsigned int readCount = bootSerialPort.Read(buffer, sizeof(buffer)-1);
                if ( (readCount < sizeof(buffer)) && (readCount > 0) )
                {
                    if (getNotifier())
                        getNotifier()->showMessage(wxString(buffer) + wxString("\n"), false, false, *wxGREEN);
                    if (bootSerialPort.IsOpen())
                        bootSerialPort.Close();
                    return wxString(buffer).Contains(wxString("LUFACDC"));
                }
            }
            if (bootSerialPort.IsOpen())
                bootSerialPort.Close();
            return false;
        }
        catch(...)//##
        {
            if (bootSerialPort.IsOpen())
                bootSerialPort.Close();
            return false;
        }
    }
    else
    {
        return true;
    }
}


void Bubble::linesFromArrayToBubbleEditor(const wxArrayString &strings, BubbleEditor *editor)
{
    if (editor)
    {
        for (unsigned int i = 0; i < strings.GetCount(); i++)
        {
            editor->AddText(strings[i] + wxString("\n")); //##Testing needed: Is this correct in all platforms?
        }
    }
}


void Bubble::linesFromArrayToFile(const wxArrayString &strings, wxTextFile *file)
{
    if (file)
    {
        for (unsigned int i = 0; i < strings.GetCount(); i++)
        {
            file->AddLine(strings[i]);
        }
    }
}


wxArrayString Bubble::string2Array(const wxString &value)
{
    wxArrayString result;
    for (unsigned int i=0; i<value.length(); i++)
    {
        result.Add(value[i]);
    }
    return result;
}


unsigned int Bubble::strOcurrencesInArray(const wxString &str, const wxSortedArrayString &array)
{
//##wxWidgets bug in wxSortedArray.Index()?: In theory, the Index funcion must return the index of the FIRST
//ocurrence of the string in the array. But according to these tests, it does not. If Index
//    unsigned int result = 0;
//    int index = array.Index(str);
//    if (index == wxNOT_FOUND)
//        return 0;
//
//    while ( (unsigned)index < array.GetCount() )
//    {
//        result++;
//        if (str != array[index])
//            break;
//        index++;
//    }
//    return result;

    //##Ugly (linear search, not binary), but works:
    unsigned int result = 0;
    int index = 0;
    while ( (unsigned)index < array.GetCount() )
    {
        if (str == array[index])
            result++;
        index++;
    }
    return result;
}


void Bubble::addHeaderCode()
{
//    wxArrayString tempCode;
//
//    //##Implementar: Levantar esto del target. Esto es por si hay que colocar código, comentarios, copyrights,
//    //o lo que sea aún antes de los libraries.
//
//    return tempCode;
}


void Bubble::addLibrariesCode()
{
    //##Implementar: Esto tiene que cargarse del array de "libs" que saldrá directamente de los bloques
    //presetes en los canvases:

    //##When finished, only the necessary includes will be added (from the libraries array):

    //##Un-hardcode:
    if (   (getBoardName() != wxString("Maple Rev 3+ (to Flash)")) &&
           (getBoardName() != wxString("Maple Rev 3+ (to RAM)")) &&
           (getBoardName() != wxString("ATTiny25 (with ArduinoISP)")) &&
           (getBoardName() != wxString("ATTiny45 (with ArduinoISP)")) &&
           (getBoardName() != wxString("ATTiny85 (with ArduinoISP)")) &&
           (getBoardName() != wxString("ATTiny25 (with Doper)")) &&
           (getBoardName() != wxString("ATTiny45 (with Doper)")) &&
           (getBoardName() != wxString("ATTiny85 (with Doper)"))
       )
    {
        generatedCode.Add("#include \"stdlib.h\"");
        generatedCode.Add("#include \"IRremote.h\"");
    }
    //##tempCode.Add("#include \"DCMotor.h\"");
    if (    (getBoardName() != wxString("ATTiny25 (with ArduinoISP)")) &&
           (getBoardName() != wxString("ATTiny45 (with ArduinoISP)")) &&
           (getBoardName() != wxString("ATTiny85 (with ArduinoISP)")) &&
           (getBoardName() != wxString("ATTiny25 (with Doper)")) &&
           (getBoardName() != wxString("ATTiny45 (with Doper)")) &&
           (getBoardName() != wxString("ATTiny85 (with Doper)"))
       )
    {
        generatedCode.Add("#include \"pitches.h\"");
    }
    generatedCode.Add("#include \"Minibloq.h\"");
    generatedCode.Add("");
    generatedCode.Add("");
}


void Bubble::addInitializationCode()
{
    //##Implementar: Esto hay que levantarlo del target, aunque puede que haya optimizaciones del tipo "sólo
    //instanciar motores si estos se utilizan..."
    //##Ver también luego qué relación tendrá con la pantalla de hardware...

    //##This will be generated only if necessary:
//    tempCode.Add("MotorDC motor0(22, 20, 21);");
//    tempCode.Add("MotorDC motor1(3, 4, 8);");
//    tempCode.Add("");
    //tempCode.Add("");

    generatedCode.Add("void setup()");
    generatedCode.Add("{");

//##2011.07.31: Writing new variables subsystem:
#if 0
    //##Local variables declarations (this will change when the procedure calls will be added):
    //##Preinstantiated objects:
    if (currentCanvas)
    {
        //##2011.07.31: Ver si esto descripto aquí sigue vigente aún con el nuevo subsistema de variables:
        //##Hay un caso en el cual no se genera la instancia de una variable apropiadamente: Si el usuario
        //crea diferentes variables, les completa sus parámetros, y finalmente entra en el instanceNameField
        //de alguna y la modifica, y SIN deseleccionar dicho bloque genera el código (o compila), el último
        //cambio de nombre no se verá reflejado. Es por esto que se hace este primer for, agregándose las
        //variables directamente con un setInstance, ya que dicha función elimina las repeticiones
        //automáticamente. //##Por supuesto, esto en el futuro puede ser muy optimizado, ya que se podría
        //hacer esto en el recorrido por los bloques, puesto que se podría generar el código de
        //inicialización LUEGO del código principal del canvas, porque nada obliga a generar primero el
        //código de inicialización. Luego se puede ensamblar todo. De esa forma, los bloques se recorrerían
        //sólo una vez, ganando velocidad:
        //Search for at least one setter block with the same variable name:
        //currentCanvas->setAllInstances();
        currentCanvas->clearNonDeclaredInstances();

        //##Ahora que se aseguró de que las instancias estén completas, las recorre para generar las
        //declaraciones correspondientes:
        bool thereAreVariables = false;
        for (int i = 0; i<currentCanvas->getInstancesCount(); i++)
        {
            //##Esto va a cambiar cuando se introduzca el manejo de tipos definitivo con las
            //variables, y cuando se introduzcan clases:
            BubbleInstance *tempInstance = currentCanvas->getInstance(i);
            if (tempInstance)
            {
                if ( tempInstance->getType() == "Variable" ) //##Horrible hardcoded: Un-hardcode!!
                {
                    generatedCode.Add(wxString("\tfloat ") + tempInstance->getName() + wxString(" = 0.0;"));
                    thereAreVariables = true;
                }
            }
        }
        //##Este if es sólo para no dejar la línea en blanco si no hay nada:
        if (thereAreVariables)
            generatedCode.Add("");
    }
#endif

    //##:Por ahora son entradas, después habrá que modificar esto bien:
//    tempCode.Add("\tpinMode(0, INPUT);");
//    tempCode.Add("\tpinMode(1, INPUT);");
//    tempCode.Add("\tpinMode(2, INPUT);");
//    tempCode.Add("\tpinMode(3, INPUT);");
//    tempCode.Add("\tpinMode(4, INPUT);");
//    tempCode.Add("\tpinMode(5, INPUT);");
    generatedCode.Add("\tinitBoard();");
    generatedCode.Add("");
}


void Bubble::addFinalizationCode()
{
    //##Implementar: Levantar esto del target:

    generatedCode.Add("}");
    generatedCode.Add("");

    //##By the moment, we do not use loop:
    generatedCode.Add("void loop()");
    generatedCode.Add("{");
    generatedCode.Add("}");
}


wxString Bubble::generateParamsCode(BubbleBlock *block)
{
    wxString tempCode("");
    if (block == NULL)
        return tempCode;
    if (block->getCode().IsEmpty())
        return tempCode;

    //##Antes se soportaban números negativos, porque es más rápido escribir código así, pero es redundante
    //(ya que se agregó el bloque numberNegative).
    //Y por lo tanto podría confundir, con respecto al bloque numberNegative, el cual es imprescindible, para
    //poder "negativizar" variables y expresiones completas. Así que puede que tenga que tocar este código.
    //Pero se lo deja (al menos por ahora) porque podría ocurrir que en alguna implementación se soporten
    //constantes negativas directamente, sin que se desee que se armen expresiones con numberNegative, o
    //incluso que se soporten ambas formas (con numberNegative y con constantes numéricas negativas):

    //If the block is a number, and it's negative, add the ParamListStart and the ParamListEnd strings:
    if (block->getDataType() == wxString("number")) //##Future: Now there are run-time loadable types, this must be rewritten...
    {
        double x;
        if (block->getCode()[0].ToDouble(&x))
        {
            if (x < 0)
                //tempCode += block->getParamListStart() + block->getCode()[0] + block->getParamListEnd() +block->getParamListStart();
                tempCode += wxString("(") + block->getCode()[0] + wxString(")") + block->getParamListStart(); //##Descablear el símbolo de "asilación de signo" (en este caso los paréntesis).
            else
                tempCode += block->getCode()[0] + block->getParamListStart();
        }
        else
        {
            //Not necessary, but safe:
            tempCode += block->getCode()[0] + block->getParamListStart();
        }
    }
    else
    {
        tempCode += block->getCode()[0] + block->getParamListStart();
    }

    unsigned int paramsCount = block->getParamsCount();
    for (unsigned int paramIndex = 0; paramIndex < paramsCount; paramIndex++)
    {
        BubbleBlock *paramBlock = block->getParamFirstBlock(paramIndex);
        if (paramBlock)
        {
            if (paramIndex == (paramsCount - 1)) //Is the last param?
                tempCode += generateParamsCode(paramBlock) + block->getParamListEnd();
            else
                tempCode += generateParamsCode(paramBlock) + block->getParamSeparator();
        }
    }

    return tempCode;
}


bool Bubble::simplifyCodeLine(wxString &code)
{
    //##Implementar:
    return true;
}


int Bubble::getCodeFirstModifiedLineNumber() const
{
    for (unsigned int i = 0; i < generatedCode.GetCount(); i++)
    {
        if (i < prevGeneratedCode.GetCount() )
        {
            if (generatedCode[i] != prevGeneratedCode[i])
                return (int)i;
        }
        else
        {
            //If had reached the end of the second array, returns wxNOT_FOUND if the arrays has the same
            //quantity of elements (here this means that the arrays are equal, because it already compares
            //each item):
            if (generatedCode.GetCount() == prevGeneratedCode.GetCount())
                return wxNOT_FOUND;
            return (int)i;
        }
    }
    return wxNOT_FOUND;
}


int Bubble::getCodeLastModifiedLineNumber() const
{
    int j = prevGeneratedCode.GetCount() - 1;
    for (unsigned int i = generatedCode.GetCount() - 1; i >= 0; i--)
    {
        if (j >= 0 )
        {
            if (generatedCode[i] != prevGeneratedCode[j])
                return (int)i;
        }
        else
        {
            //If had reached the begin of the second array, returns wxNOT_FOUND if the arrays has the same
            //quantity of elements (here this means that the arrays are equal, because it already compares
            //each item):
            if (generatedCode.GetCount() == prevGeneratedCode.GetCount())
                return wxNOT_FOUND;
            return (int)i;
        }
        j--;
    }
    return wxNOT_FOUND;
}


bool Bubble::updateCode()
{
    //##In the future, this must be extended to support multiple canvases, but will have to be optimzed to
    //only regenerate those canvases wich has changed since the last generation of code:
    try
    {
        //##Ver si esto se hará así, o si se implementará en una sóla pasada, ya que si se hace de esta forma,
        //el programa verifica los errores pasando por todos los bloques, y luego los recorre nuevamente para
        //generar el código. Casi seguro lo haré en una sóla pasada, pero puede que no para la primer versión...
        //##Update del comentario anterior: Lo más probable es que no utilice esta función porque la verificación
        //de errores se hace en tiempo real. Si esto es así, ##Volar esto:
        //if (!verify())
        //    return false;

        //Generates the code:
        prevGeneratedCode = generatedCode;
        generatedCode.Clear();

        addHeaderCode();
        addLibrariesCode();
        addInitializationCode();

        //##Falta recorrer todos los canvases para agregar los Blocks de usuario...

        //##Recorre los bloques para generar el código (por ahora sólo del currentCanvas):
        if (!currentCanvas)
            return false;

        //##En ocasiones se me colgó esto, tengo que ver si es cuando el avrdude tiene algún problema, o si persite
        //algún bug en este while: Pareciera que es el avrdude, pero pasó algo muy raro:
        //
        //Generé un programa con un if y adentro sonidos y delays, lo bajé y anduvo bien. Traté deliveradamente
        //de bajarlo con el micro sin resetear (o sea corriendo el mismo programita recién bajado), y el software quedó a la espera,
        //pero lo más raro de todo es que los botones de los métodos ¡pasaron a disabled! Muy pero muy extraño, pareciera ser de memoria,
        //pero puede que corriendo en async, etc. o utilizando la API del SO se pueda hacer algo, porque podría ser un temita con el
        //wxExecute.

        //Igual luego cuando lo corrí, si reseteaba el micro mientras estaba "colgado", resultaba en que el soft "volvía del avrdude".
        //así que puedo poner un timer, o un thread o algo (ver el XDFTerminal), para que si la cosa pasa de cierto time-out, avise al
        //usuario de pasa algo.
        //Además se pueden hacer chequeos extra del puerto hasta que todo sea realmente cómodo para el usuario.

        BubbleBlock *iteratorBlock = currentCanvas->getFirstBlock();
        while (iteratorBlock)
        {
            //##Test "comments" stuff this a lot!
            wxString commentBegin("");
            wxString commentEnd("");
            if (iteratorBlock->isCommented())
            {
                commentBegin = wxString("// "); //Un-hardcode!
                //commentBegin = wxString("/* ");
                //commentEnd = wxString(" */");
            }

            //##En cosas como el código G, que quizá tengan sólo un encabezado tipo "comentario",
            //se puede poner acá 0 ó 1. Aquí van 2, porque todo va indentado 1 con respecto al
            //InitializationCode. Así que se hará configurable la "indentación con respecto al
            //InitializationCode, pero por ahora está cableada acá en "un tab":
            wxString indentation("\t");

            //Indentation:
            for (unsigned int i = 0; i < iteratorBlock->getIndentation(); i++)
                indentation += wxString("\t");

            wxArrayString blockCode = iteratorBlock->getCode();

            //##2011.02.01: Acá falta recorrer toda la lista de parámetros, tal cual se hace para el borrado, etc.
            //##2011.02.01: Quizá los operadores no necesiten ser tratados de forma diferente:
            //  - Quizá con cargar vacíos los Start y End, y colocando al operador en el paramSeparator la cosa esté.
            //  - Falta ver qué hacer cuando aparecen operadores unarios (como en los números negativos, por ejemplo).

            //##Más adelante se implementará un patrón de iteración, o algo así seguramente...

            if (iteratorBlock->getParamsCount() > 0)
            {
                //Params:

                //generateParamsCode returns not only de params themselves, but the getCode[0] too, that's why
                //the indentation string is added first:
                wxString tempCode = indentation + commentBegin + generateParamsCode(iteratorBlock);
                generatedCode.Add(tempCode + commentEnd);

                //Params follow the first blockCode line (blockCode[0]):
                for (unsigned int i = 1; i < blockCode.GetCount(); i++)
                {
                    if (getSimplifyCode())
                        simplifyCodeLine(blockCode[i]);
                    generatedCode.Add(indentation + commentBegin + blockCode[i] + commentEnd);
                }
            }
            else
            {
                for (unsigned int i = 0; i < blockCode.GetCount(); i++)
                {
                    if (getSimplifyCode())
                        simplifyCodeLine(blockCode[i]);
                    generatedCode.Add(indentation + commentBegin + blockCode[i] + commentEnd);
                }
            }

            //##Debug:
            //wxMessageDialog dialog0(parent, iteratorBlock->getCode(), _("generateCode()"));
            //dialog0.ShowModal();

            iteratorBlock = currentCanvas->getNextBlock(iteratorBlock);
        }

        addFinalizationCode();
        return true;
    }
    catch(...)
    {
        return false;
    }
}


bool Bubble::generateCodeAndSaveToFile()
{
    try
    {
        //##Nota acerca del tipo de archivo: Por ahora usa el typeDefault, que en teoría debería generar archivos
        //con terminación de línea "DOS". Pero más adelante ser verá, y quizá se pase todo a Unix. De todos modos
        //la edición con scintilla en el mismo entorno debería funcionar bien igual. Si esto se cambia, ver
        //qué funciones hay que tocar (en principio, al menos hay que llamar a Write() con otro tipo de
        //archivo, porque ahora usan el parámetro con valor por defecto). Algo que sí me gustaría es no usar
        //el default, para que todos los Minibloq generen el mismo tipo de archivo, sin importar el
        //sistema operativo, lo cual dará más uniformidad, pero no estoy seguro...

        //##Luego avisar de errores, etc.:
        if (!clean())
            return false;

        //Try to create the output dir structure:
        createDirs(outputPath);

        //Try to create the file:
        wxTextFile main_pde;
        if ( !main_pde.Create(outputPath + wxString("/main.pde")) ) //##Un-hardcode!
            return false;

        //Refresh the generated code, and obtains it:
        if (!updateCode())
            return false;
        linesFromArrayToFile(getGeneratedCode(), &main_pde);

        //##Save the changes and closes the file:
        //##Si hay errores con el archivo, reportarlo en Messages:
        if ( !main_pde.Write() )
            return false;
        if ( main_pde.Close() )
        {
//##:¿Esto se queda?
            if ( getNotifier() )
                getNotifier()->refreshGeneratedCodeNotify();

//##:¿Esto se queda?
//            currentCanvas->Update();
//            currentCanvas->Refresh();
            return true;
        }
        return false;
    }
    catch(...)
    {
        return false;
    }
}


bool Bubble::clean()
{
    //##This will be made by searching all the ".cpp", ".o", etc. in the project dir, but not now. ##And the
    //target's XML file could specify more files to be deleted:
    //if (wxfileExsist) //Not necessary
    wxRemoveFile(outputPath + wxString("/main.pde"));
    wxRemoveFile(outputPath + wxString("/main.cpp.o"));
    wxRemoveFile(outputPath + wxString("/main.cpp.elf"));
    wxRemoveFile(outputPath + wxString("/main.cpp.hex"));
    wxRemoveFile(outputPath + wxString("/main.cpp.epp"));

    //##Not used by now:
    return true;
}


//##Casi seguro esto se va:
//bool Bubble::verify()
//{
//    //##Esta función debe llamar a todas las funciones verifyXXX, y si alguna falla, retorna falso. ##Ver qué
//    //otras funciones verifyXXX hay que implementar...
//    if (!verifyMissingParams())
//        return false;
//
//    //##Implementar: Aquí se hará el control de errores (por ejemplo, si falta un parámetro en un bloque, etc.):
//    return true;
//}


bool Bubble::verifyMissingParams()
{
    //##Implementar: Recorre los bloques y busca si faltan parámetros, por completar por parte del usuario.
    //Cada vez que encuentra uno, agrega el error a la ventana de mensajes, o a donde corresponda. La idea
    //es que en una rápida pasada, pueda encontrar todos los errores de falta de parámetros.
    return true;
}


bool Bubble::makePortable()
{
#if (!UNDER_DEVELOPMENT)
    //##Implementar...
    return true;
#endif
}


void Bubble::loadAcceleratorTableFromFile()
{
    //##Implementar: Por ahora está cableado, pero hay que levantar todo esto de un archivo:
    //##Recorrer todos los canvas en el futuro:
    if (getCurrentCanvas())
    {
        //##Esto va a cambiar, ahora está cableado, pero va a pasar casi seguro a una función llamada
        //"addAcceleratorKey" olgo así, miembro de BubbleCanvas:
        getCurrentCanvas()->addAcceleratorKeys();
    }
}


BubbleActionPicker *Bubble::createActionPicker(wxWindow *pickerParent, bool show)
{
    if (pickerParent == NULL)
        return NULL;

    wxPoint prevPosition(0, 0);
    if (actionPicker)
    {
        //actionPicker->Close(true);
        prevPosition = actionPicker->GetPosition();
        actionPicker->Destroy();
        actionPicker = NULL;
        //return NULL;//##Debug.
//        wxMessageDialog dialog0(parent, _("Closing action picker"), _("0"));
//        dialog0.ShowModal();
    }

    actionPicker = new BubbleActionPicker(  pickerParent,
                                            this,
                                            wxNewId(),
                                            getActionDataType(),
                                            _("Actions"),
                                            64,
                                            //wxColour(64, 64, 64), //##Ver tema color.
                                            wxColour(0, 0, 0), //##Ver tema color.
                                            2 //3
                                          );
    if (actionPicker == NULL)
        return NULL;

    actionPicker->Move(prevPosition);
    return actionPicker; //##No sé si es prolijo retornarlo, ya que es una variable interna de la instancia...
}


void Bubble::showPicker(BubblePicker* picker, const wxPoint& position, bool value)//##, bool &pickerFirstTime)
{
    if (picker)
    {
        if (value)
        {
            /* ##La idea es ver si entra en la pantalla o no, si no es así, entonces lo corre al borde, como
            ocurre automáticamente al desplegar un menú, pero por ahora se prefirió centrar al picker. En el
            futuro se verá qué se hace...
            if ((position.x + picker->GetSize().GetWidth()) > )
            {
                picker->Move(position);
            }
            else
            {
            }
            */

            //##Esto tiene que cambiar, no me gusta enviar este parámetro así, para cada picker, es desprolijo:

            //##if (pickerFirstTime)
            //{
            //    pickerFirstTime = false;
            //picker->Centre();  //##En una versión futura se mejorará el posicionamiento inicial para que
                                        //resulte más cómodo, y se agregará una "chinche" a la ventana, para que
                                        //se quede donde el usuario la dejó, o para que se autoposicione cada vez.
            //}
            //##Es casi seguro que todo esto se irá cuando se pasen los pickers como paneles para que los
            //administre wxAUI:
/*
            if (parent)
            {
                //##Luego esto sale de algún archivo de configuración, donde recuerda la última posición:
//                picker->Move(parent->GetClientSize().GetWidth() - picker->GetSize().GetWidth(),
//                            //picker->GetSize().GetWidth(),
//                            parent->GetPosition().y - picker->GetSize().GetWidth());
                picker->Move(parent->GetPosition().x + parent->GetClientSize().GetWidth() - picker->GetSize().GetWidth(),
                             parent->GetPosition().y //+ (parent->GetSize().GetHeight() - parent->GetClientSize().GetHeight())
                            );
            }
            else
                picker->Centre(); //##Esto puede que se vaya también.
*/
            picker->Move(position);
            picker->Show();

            //Prevents bad painting (with background elements on the panel's surface):
            picker->Update();
            picker->Refresh();

            picker->SetFocus();//##
            //actionPicker->SetFocusIgnoringChildren();
        }
        else
            picker->Show(false);
    }
}


void Bubble::togglePicker(BubblePicker* picker, const wxPoint& position)
{
    if (picker)
        showPicker(picker, position, !picker->IsShown());
}


void Bubble::enableAllBlocks(bool value)
{
    //##Future: travel all canvases:
    if (getCurrentCanvas())
    {
        getCurrentCanvas()->enableAllBlocks(value);
        getCurrentCanvas()->Update();
        getCurrentCanvas()->Refresh();
        blocksEnabled = value;
    }
}


void Bubble::setVisibleLabels(bool value)
{
    //##Implementar:
    //Recorre todos los canvases y les setea los labels en true...
    if (getCurrentCanvas())
    {
        getCurrentCanvas()->setVisibleLabels(value);
        visibleLabels = getCurrentCanvas()->getVisibleLabels();
    }
}
