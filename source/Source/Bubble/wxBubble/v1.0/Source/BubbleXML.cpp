//##Agregar licencia y avisos de copyright de terceros en TODOS LOS headers y cpp.

#include "Bubble.h"

#include <wx/msgdlg.h> //##Debug, se va en release. Agregar compilación condicional.
#include <wx/dir.h>
#include <wx/filename.h>


////////////////////////////////////////////////////////////////////////////////////////////
//
//Utils:
//
////////////////////////////////////////////////////////////////////////////////////////////
wxString BubbleXML::getXMLEncodedString(const wxString &value)
{
    wxString result, aux;

    for (unsigned int i=0; i<value.Len(); i++)
    {
        //##Future: See if there is a standard way of doing this with an XML related function. If not, try to
        //index an array instead of doing this linear search (or at least implement a binary search):

        //else if (wxString(value[i]) == "\"") //##Not working
        //    aux = "&#34;";
        //else if (wxString(value[i]) == "&") //##Not working
        //    aux = "&#38;";
        //else if (wxString(value[i]) == "<") //##Not working
        //    aux = "&#60;";

        if (wxString(value[i]) == ">")
            aux = "&#62;";

        else if (wxString(value[i]) == "¡")
            aux = "&#161;";
        else if (wxString(value[i]) == "¢")
            aux = "&#162;";
        else if (wxString(value[i]) == "£")
            aux = "&#163;";
        else if (wxString(value[i]) == "¤")
            aux = "&#164;";
        else if (wxString(value[i]) == "¥")
            aux = "&#165;";
        else if (wxString(value[i]) == "§")
            aux = "&#167;";
        else if (wxString(value[i]) == "¨")
            aux = "&#168;";
        else if (wxString(value[i]) == "©")
            aux = "&#169;";

        else if (wxString(value[i]) == "ª")
            aux = "&#170;";
        else if (wxString(value[i]) == "«")
            aux = "&#171;";
        else if (wxString(value[i]) == "¬")
            aux = "&#172;";
        else if (wxString(value[i]) == "®")
            aux = "&#174;";
        else if (wxString(value[i]) == "¯")
            aux = "&#175;";
        else if (wxString(value[i]) == "±")
            aux = "&#177;";
        else if (wxString(value[i]) == "²")
            aux = "&#178;";
        else if (wxString(value[i]) == "³")
            aux = "&#179;";

        else if (wxString(value[i]) == "´")
            aux = "&#180;";
        else if (wxString(value[i]) == "µ")
            aux = "&#181;";
        else if (wxString(value[i]) == "¶")
            aux = "&#182;";
        else if (wxString(value[i]) == "·")
            aux = "&#183;";
        else if (wxString(value[i]) == "¸")
            aux = "&#184;";
        else if (wxString(value[i]) == "¹")
            aux = "&#185;";
        else if (wxString(value[i]) == "°")
            aux = "&#186;";
        else if (wxString(value[i]) == "»")
            aux = "&#187;";
        else if (wxString(value[i]) == "¼")
            aux = "&#188;";
        else if (wxString(value[i]) == "½")
            aux = "&#189;";

        else if (wxString(value[i]) == "¾")
            aux = "&#190;";
        else if (wxString(value[i]) == "¿")
            aux = "&#191;";
        else if (wxString(value[i]) == "À")
            aux = "&#192;";
        else if (wxString(value[i]) == "Á")
            aux = "&#193;";
        else if (wxString(value[i]) == "Â")
            aux = "&#194;";
        else if (wxString(value[i]) == "Ã")
            aux = "&#195;";
        else if (wxString(value[i]) == "Ä")
            aux = "&#196;";
        else if (wxString(value[i]) == "Å")
            aux = "&#197;";
        else if (wxString(value[i]) == "Æ")
            aux = "&#198;";
        else if (wxString(value[i]) == "Ç")
            aux = "&#199;";

        else if (wxString(value[i]) == "È")
            aux = "&#200;";
        else if (wxString(value[i]) == "É")
            aux = "&#201;";
        else if (wxString(value[i]) == "Ê")
            aux = "&#202;";
        else if (wxString(value[i]) == "Ë")
            aux = "&#203;";
        else if (wxString(value[i]) == "Ì")
            aux = "&#204;";
        else if (wxString(value[i]) == "Í")
            aux = "&#205;";
        else if (wxString(value[i]) == "Î")
            aux = "&#206;";
        else if (wxString(value[i]) == "Ï")
            aux = "&#207;";
        else if (wxString(value[i]) == "Ð")
            aux = "&#208;";
        else if (wxString(value[i]) == "Ñ")
            aux = "&#209;";

        else if (wxString(value[i]) == "Ò")
            aux = "&#210;";
        else if (wxString(value[i]) == "Ó")
            aux = "&#211;";
        else if (wxString(value[i]) == "Ô")
            aux = "&#212;";
        else if (wxString(value[i]) == "Õ")
            aux = "&#213;";
        else if (wxString(value[i]) == "Ö")
            aux = "&#214;";
        else if (wxString(value[i]) == "×")
            aux = "&#215;";
        else if (wxString(value[i]) == "Ø")
            aux = "&#216;";
        else if (wxString(value[i]) == "Ù")
            aux = "&#217;";
        else if (wxString(value[i]) == "Ú")
            aux = "&#218;";
        else if (wxString(value[i]) == "Û")
            aux = "&#219;";

        else if (wxString(value[i]) == "Ü")
            aux = "&#220;";
        else if (wxString(value[i]) == "Ý")
            aux = "&#221;";
        else if (wxString(value[i]) == "Þ")
            aux = "&#222;";
        else if (wxString(value[i]) == "ß")
            aux = "&#223;";
        else if (wxString(value[i]) == "à")
            aux = "&#224;";
        else if (wxString(value[i]) == "á")
            aux = "&#225;";
        else if (wxString(value[i]) == "â")
            aux = "&#226;";
        else if (wxString(value[i]) == "ã")
            aux = "&#227;";
        else if (wxString(value[i]) == "ä")
            aux = "&#228;";
        else if (wxString(value[i]) == "å")
            aux = "&#229;";

        else if (wxString(value[i]) == "æ")
            aux = "&#230;";
        else if (wxString(value[i]) == "ç")
            aux = "&#231;";
        else if (wxString(value[i]) == "è")
            aux = "&#232;";
        else if (wxString(value[i]) == "é")
            aux = "&#233;";
        else if (wxString(value[i]) == "ê")
            aux = "&#234;";
        else if (wxString(value[i]) == "ë")
            aux = "&#235;";
        else if (wxString(value[i]) == "ì")
            aux = "&#236;";
        else if (wxString(value[i]) == "í")
            aux = "&#237;";
        else if (wxString(value[i]) == "î")
            aux = "&#238;";
        else if (wxString(value[i]) == "ï")
            aux = "&#239;";

        else if (wxString(value[i]) == "ð")
            aux = "&#240;";
        else if (wxString(value[i]) == "ñ")
            aux = "&#241;";
        else if (wxString(value[i]) == "ò")
            aux = "&#242;";
        else if (wxString(value[i]) == "ó")
            aux = "&#243;";
        else if (wxString(value[i]) == "ô")
            aux = "&#244;";
        else if (wxString(value[i]) == "õ")
            aux = "&#245;";
        else if (wxString(value[i]) == "ö")
            aux = "&#246;";
        else if (wxString(value[i]) == "÷")
            aux = "&#247;";
        else if (wxString(value[i]) == "ø")
            aux = "&#248;";
        else if (wxString(value[i]) == "ù")
            aux = "&#249;";

        else if (wxString(value[i]) == "ú")
            aux = "&#250;";
        else if (wxString(value[i]) == "û")
            aux = "&#251;";
        else if (wxString(value[i]) == "ü")
            aux = "&#252;";
        else if (wxString(value[i]) == "ý")
            aux = "&#253;";
        else if (wxString(value[i]) == "þ")
            aux = "&#254;";
        else if (wxString(value[i]) == "ÿ")
            aux = "&#255;";

        //##See XML tables for other symbols...
        else
            aux = value[i];
        result += aux;
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////
//
//Blocks structure functions:
//
////////////////////////////////////////////////////////////////////////////////////////////
int BubbleXML::getBlockFilesCount(const wxString& blocksPath, int flags)
{
    wxDir dir(blocksPath);
    if ( !dir.IsOpened() )
    {
        //##According to the wxWidgets documentation, wxDir will log an error message (with extra
        //explanations) but where? ##Check this.
        return -2; //##Futuro: estandaziar errores.
    }

    //##Future: Add the "matrix" (cartesian product with the board-blocks, to really count the valid blocks
    //for the current selected hardware)? Perhaps this is faster and that's not needed (unless this function
    //becomes used by something more important thatn the GUI's progress bar):
    int counter = 0;
    wxString fileName;
    bool result = dir.GetFirst(&fileName, wxEmptyString, flags);
    while (result)
    {
        counter++;
        result = dir.GetNext(&fileName);
    }
    return counter;
}


bool BubbleXML::isXMLVariable(const wxString& variableName) const
{
    if (variableName.Last() == ':')
        return ((variableName.BeforeLast(':')).Last() == ':');
    return false;
}


//The param fileName is necessary so this function can return the "thisBlockPath::" variable:
wxString BubbleXML::getInternalVariableValue(const wxString& variableName, const wxString& fileName) const
{
    if (bubble == NULL)
        return wxString("");

    //Bubble and application paths:
    if (variableName == "componentsRepositoryPath::")
        return bubble->getComponentsRepositoryPath();
    if (variableName == "toolsPath::")
        return bubble->getToolsPath();
    if (variableName == "corePath::")
        return bubble->getCorePath();
    if (variableName == "matrixPath::")
        return bubble->getMatrixPath();
    if (variableName == "libPath::")
        return bubble->getLibPath();
    if (variableName == "blocksPath::")
        return bubble->getBlocksPath();
    if (variableName == "appPath::")
        return bubble->getAppPath();
    if (variableName == "themePath::")
        return bubble->getThemePath();
    if (variableName == "docPath::")
        return bubble->getDocPath();
    if (variableName == "componentPath::")
        return bubble->getComponentPath();
    if (variableName == "outputPath::")
        return bubble->getOutputPath();

    //Current block path (the current block is that to which the XML file -XMLFileName param- is beign read):
    if (variableName == "thisBlockPath::")
    {
//        wxMessageDialog dialog0(parent, fileName, _("fileName")); //##Debug
//        dialog0.ShowModal(); //##Debug
        return fileName.BeforeLast('/'); //##Future: Use OS specific separator for this kind of thing.
    }

    //Variable name does not exists:
    return wxString("");
}


wxString BubbleXML::getVariableValue(const wxString& variableName, const wxString& fileName) const
{
    //Internal variables has priority over XML loaded variables:
    wxString internalVariableValue = getInternalVariableValue(variableName, fileName);
    if (internalVariableValue != "")
        return internalVariableValue;

    BubbleInstance *variable = XMLVariables.getInstance(variableName);
    if (variable)
        return variable->getType(); //Here, we use the type field to store the value.
    return wxString("");
}


bool BubbleXML::loadVariablesFromXML(const wxString& fileName, bool clearVariableList)
{
    if (wxFile::Exists(fileName))
    {
        //##Future: Try to disable the error messages that the wxXmlDocument class fires when encounters errors:
        wxXmlDocument variablesFile;
        if ( variablesFile.Load(fileName, wxString("UTF-8")) )
        {
            wxXmlNode *root = variablesFile.GetRoot();
            if (root == NULL)
                return false;

            //##:Debug:
            //getNotifier()->showMessage(fileName + wxString("\n"), false, true, *wxBLUE); //##Debug.

            //Root's Name doesn't matter...
            wxXmlNode *rootChild = root->GetChildren();
            if (rootChild == NULL)
                return false;

            //##:Debug:
            //getNotifier()->showMessage(rootChild->GetName() + wxString("\n"), false, true, *wxBLUE); //##Debug.

            while (rootChild)
            {
                if (rootChild->GetName() == "variables")
                {
                    if (clearVariableList)
                        XMLVariables.clear();

                    //##Ver si hay que agregar seguridades a todo este proceso:
                    wxXmlNode *child = rootChild->GetChildren();
                    while (child)
                    {
                        wxString varName = child->GetName();
                        if (isXMLVariable(varName))
                        {

                            //Internal variables has priority over XML loaded variables, so if there is an
                            //external variable with the same name as an internal, it's not loaded:
                            if (getInternalVariableValue(varName, fileName) == "") //There is no internal variable with this name.
                            {
                                BubbleInstance *variable = new BubbleInstance(varName, child->GetNodeContent());
                                XMLVariables.setInstance(variable);
                                //##:Debug:
                                //getNotifier()->showMessage(wxString("var name=") + varName + wxString(", value=") + child->GetNodeContent() + wxString("\n"), false, true, *wxBLUE); //##Debug.
                            }
                        }
                        child = child->GetNext();
                    }
                    return true;
                }
                rootChild = rootChild->GetNext();
            }
        }
    }
    return false;
}


bool BubbleXML::loadBlocksPropertiesFromXML(const wxString& fileName, bool clearVariableList)
{
    if (bubble == NULL)
        return false;

    if (wxFile::Exists(fileName))
    {
        //##Future: Try to disable the error messages that the wxXmlDocument class fires when encounters errors:
        wxXmlDocument propertiesFile;
        if ( propertiesFile.Load(fileName, wxString("UTF-8")) )
        {
            wxXmlNode *root = propertiesFile.GetRoot();
            if (root == NULL)
                return false;

            //##:Debug:
            //getNotifier()->showMessage(fileName + wxString("\n"), false, true, *wxBLUE); //##Debug.

            //Root's Name doesn't matter...
            wxXmlNode *rootChild = root->GetChildren();
            if (rootChild == NULL)
                return false;

            //##:Debug:
            //getNotifier()->showMessage(rootChild->GetName() + wxString("\n"), false, true, *wxBLUE); //##Debug.

            while (rootChild)
            {
                //##Make these else ifs?
                if (rootChild->GetName() == "properties")
                {
                    //##Ver si hay que agregar seguridades a todo este proceso:
                    wxXmlNode *child = rootChild->GetChildren();
                    while (child)
                    {
                        //##Make these else ifs?
                        if (child->GetName() == "actionDataType")
                        {
                            bubble->setActionDataType(child->GetNodeContent());
                            //getNotifier()->showMessage(wxString("actionDataType = ") + child->GetNodeContent() + wxString("\n"), false, true, *wxGREEN); //##Debug.
                        }
//                        else if (child->GetName() == "##")
//                        {
//                        }
                    child = child->GetNext();
                    }
                    return true;
                }
                rootChild = rootChild->GetNext();
            }
        }
    }
    return false;
}


//This reads the params width and height in a positional way (the names doesn't matter):
wxSize BubbleXML::loadSizeFromXML(wxXmlNode *node, const wxString& fileName) const
{
    wxSize result(0, 0);
    if (node)
    {
        double returnNumericValue = 0.0;
        wxString returnStringValue("");
        wxXmlNode *fieldNode = node->GetChildren();
        for (unsigned int i=0; i<2; i++)
        {
            if (fieldNode)
            {
                //Read the field value (the name doesn't matter):
                returnStringValue = fieldNode->GetNodeContent();

                //Is the value a variable?
                if (isXMLVariable(returnStringValue))
                    returnStringValue = getVariableValue(returnStringValue, fileName);

                //Anyway, it must be finally converted to a number:
                returnNumericValue = 0.0;
                if (returnStringValue.ToDouble(&returnNumericValue))
                {
                    //##Horrible, but very secure:
                    if (i == 0)
                        result.SetWidth((int)returnNumericValue);
                    else if (i == 1)
                        result.SetHeight((int)returnNumericValue);
                }
                fieldNode = fieldNode->GetNext();
            }
        }
    }
    return result;
}


//This reads the params R, G and B in a positional way (the names doesn't matter):
wxColour BubbleXML::loadColourFromXML(wxXmlNode *node, const wxString& fileName) const
{
    unsigned char   R = 0,
                    G = 0,
                    B = 0;
    wxColour result(0, 0, 0);
    if (node)
    {
        long returnNumericValue = 0;
        wxString returnStringValue("");
        wxXmlNode *fieldNode = node->GetChildren();
        for (unsigned int i=0; i<3; i++)
        {
            if (fieldNode)
            {
                //Read the field value (the name doesn't matter):
                returnStringValue = fieldNode->GetNodeContent();

                //Is the value a variable?
                if (isXMLVariable(returnStringValue))
                    returnStringValue = getVariableValue(returnStringValue, fileName);

                //Anyway, it must be finally converted to a number:
                returnNumericValue = 0;
                if (returnStringValue.ToLong(&returnNumericValue))
                {
                    //##Horrible, but very secure:
                    if (i == 0)
                        R = (unsigned char)returnNumericValue;
                    else if (i == 1)
                        G = (unsigned char)returnNumericValue;
                    else if (i == 2)
                        B = (unsigned char)returnNumericValue;
                }
                fieldNode = fieldNode->GetNext();
            }
        }
    }
    result.Set(R, G, B);
    return result;
}


////##Not necessary:
////This function is for those strings that support Bubble's XML variables:
//wxString BubbleXML::loadStringFromXML(wxXmlNode *node) const
//{
//    wxString result("");
//    if (node)
//    {
//        result = node->GetNodeContent();
//        if (isXMLVariable(result))
//            result = getVariableValue(result);
//    }
//    return result;
//}


bool BubbleXML::loadBlockInfoPropertiesFromXML(wxXmlNode *node, BubbleBlockInfo *blockInfo)
{
    //##:
    if (bubble == NULL)
        return false;

    if (bubble->getNotifier() == NULL) //##Debug.
        return false; //##Debug.

    if (node == NULL)
        return false;
    if (blockInfo == NULL)
        return false;
//    if (node->GetName() == "properties")
//    {
        //##Debug:
        //getNotifier()->showMessage(wxString("properties:\n"), false, true, *wxBLUE); //##Debug.

        wxXmlNode *child = node->GetChildren();
        while (child)
        {
            //##Make these else ifs?
            if (child->GetName() == "name")
            {
                blockInfo->setName(child->GetNodeContent());
                //##getNotifier()->showMessage(wxString("name = ") + child->GetNodeContent() + wxString("%%\n"), false, true, *wxBLUE); //##Debug.
            }
            else if (child->GetName() == "function")
            {
                blockInfo->setFunction(child->GetNodeContent());
            }
            else if (child->GetName() == "componentStart")
            {
                blockInfo->setComponentStart(Bubble::string2bool(child->GetNodeContent()));
            }
            else if (child->GetName() == "blockStart")
            {
                blockInfo->setBlockStart(Bubble::string2bool(child->GetNodeContent()));
            }
            else if (child->GetName() == "variableInit")
            {
                blockInfo->setVariableInit(Bubble::string2bool(child->GetNodeContent()));
            }
            else if (child->GetName() == "loadAction")
            {
                blockInfo->setLoadAction(child->GetNodeContent());
            }
            else if (child->GetName() == "isDraggable")
            {
                blockInfo->setIsDraggable(Bubble::string2bool(child->GetNodeContent()));
            }

            child = child->GetNext();
        }
        if (blockInfo->getVariableInit())
            bubble->setVariableInitName(blockInfo->getName());
        return true;
//    }
//    return false;
}


bool BubbleXML::loadBlockInfoCodeFromXML(wxXmlNode *node, BubbleBlockInfo *blockInfo)
{
    if (node == NULL)
        return false;
    if (blockInfo == NULL)
        return false;
//    if (node->GetName() == "code")
//    {
        //##:
        //getNotifier()->showMessage(wxString("code:\n"), false, true, *wxBLUE); //##Debug.

        wxXmlNode *child = node->GetChildren();
        while (child)
        {
            if (child->GetName() == "dataType")
            {
                blockInfo->setDataType(child->GetNodeContent());
            }
            else if (child->GetName() == "initialCode")
            {
                wxArrayString tempCode;
                wxXmlNode *codeNode = child->GetChildren();
                while (codeNode)
                {
                    //The names of the initialCode subelements are not important, but by convention will be
                    //<line0>, <line1>, ... <lineN>:
                    //getNotifier()->showMessage(wxString("code.line = ") + codeNode->GetNodeContent() + wxString("\n"), false, true, *wxBLUE); //##Debug.
                    tempCode.Add(codeNode->GetNodeContent());
                    blockInfo->setCode(tempCode);
                    codeNode = codeNode->GetNext();
                }
            }
            else if (child->GetName() == "defaultMethodName")
            {
                blockInfo->setDefaultMethodName(child->GetNodeContent());
            }
            else if (child->GetName() == "methodSeparator")
            {
                blockInfo->setMethodSeparator(child->GetNodeContent());
            }
            else if (child->GetName() == "paramSeparator")
            {
                blockInfo->setParamSeparator(child->GetNodeContent());
            }
            else if (child->GetName() == "paramListStart")
            {
                blockInfo->setParamListStart(child->GetNodeContent());
            }
            else if (child->GetName() == "paramListEnd")
            {
                blockInfo->setParamListEnd(child->GetNodeContent());
            }
            child = child->GetNext();
        }
        return true;
//    }
//    return false;
}


bool BubbleXML::loadBlockInfoInterfaceFromXML(wxXmlNode *node, const wxString& fileName, BubbleBlockInfo *blockInfo)
{
    if (node == NULL)
        return false;
    if (blockInfo == NULL)
        return false;

//    if (node->GetName() == "interface")
//    {
        //##:
        //getNotifier()->showMessage(wxString("interface:\n"), false, true, *wxBLUE); //##Debug.

        wxXmlNode *child = node->GetChildren();
        wxString basePath("");
        while (child)
        {
            if (child->GetName() == "originalSize") //##
            {
                blockInfo->setOriginalSize(loadSizeFromXML(child, fileName));
            }
            else if (child->GetName() == "childFactor") //##
            {
                wxString returnStringValue = child->GetNodeContent();
                double returnNumericValue = 1.0;
                if (returnStringValue.ToDouble(&returnNumericValue))
                    blockInfo->setChildFactor((float)returnNumericValue);
                else
                    blockInfo->setChildFactor(1.0);
            }
            else if (child->GetName() == "defaultBackgroundColour0")
            {
                blockInfo->setDefaultBackgroundColour0(loadColourFromXML(child, fileName));
            }
            else if (child->GetName() == "defaultBackgroundColour1")
            {
                blockInfo->setDefaultBackgroundColour1(loadColourFromXML(child, fileName));
            }
            else if (child->GetName() == "defaultMouseOverColour")
            {
                blockInfo->setDefaultMouseOverColour(loadColourFromXML(child, fileName));
            }
            else if (child->GetName() == "defaultErrorColour")
            {
                blockInfo->setDefaultErrorColour(loadColourFromXML(child, fileName));
            }
            else if (child->GetName() == "defaultEditableFieldsColour")
            {
                blockInfo->setDefaultEditableFieldsColour(loadColourFromXML(child, fileName));
            }
            else if (child->GetName() == "defaultSelectionBorderColour")
            {
                blockInfo->setDefaultSelectionBorderColour(loadColourFromXML(child, fileName));
            }
            else if (child->GetName() == "nextBlockIndentationTabs")
            {
                wxString returnStringValue = child->GetNodeContent();
                double returnNumericValue = 0.0;
                if (returnStringValue.ToDouble(&returnNumericValue))
                    blockInfo->setNextBlockIndentationTabs((unsigned int)returnNumericValue);
            }
            else if (child->GetName() == "hasHorizontalParams")
            {
                blockInfo->setHasHorizontalParams(Bubble::string2bool(child->GetNodeContent()));
            }
            else if (child->GetName() == "minParamsCount") //##
            {
                wxString returnStringValue = child->GetNodeContent();
                double returnNumericValue = 0.0;
                if (returnStringValue.ToDouble(&returnNumericValue))
                    blockInfo->setMinParamsCount((unsigned int)returnNumericValue);
            }
            //Buttons:
            //##Future: There must be a function to load buttons from XML:
            else if (child->GetName() == "picker") //##
            {
                wxString imagesBasePath("");
                wxXmlNode *childOfChild = child->GetChildren();
                while (childOfChild)
                {
                    if (childOfChild->GetName() == "imagesBasePath")
                    {
                        imagesBasePath = childOfChild->GetNodeContent();
                        if (isXMLVariable(imagesBasePath))
                            imagesBasePath = getVariableValue(imagesBasePath, fileName);
                    }
                    else if (wxDir::Exists(imagesBasePath))
                    {
                        //Only loads images, if there is a valid base path for them:
                        if (childOfChild->GetName() == "default")
                            blockInfo->setPickerDefaultImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));
                        else if (childOfChild->GetName() == "hover")
                            blockInfo->setPickerHoverImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));
                        else if (childOfChild->GetName() == "pressed")
                            blockInfo->setPickerPressedImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));
                        else if (childOfChild->GetName() == "disabled")
                            blockInfo->setPickerDisabledImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));

                        //##getNotifier()->showMessage(wxString("image: ") + imagesBasePath + wxString("/") + childOfChild->GetNodeContent() + wxString("\n"), false, true, *wxBLUE); //##Debug.
                    }
                    childOfChild = childOfChild->GetNext();
                }
            }
            else if (child->GetName() == "blockButton") //##
            {
                wxString imagesBasePath("");
                wxXmlNode *childOfChild = child->GetChildren();
                while (childOfChild)
                {
                    //imagesBasePath must appear BEFORE the images (the variables in the XML are top-down):
                    if (childOfChild->GetName() == "has")
                    {
                        blockInfo->setHasBlockButton(Bubble::string2bool(childOfChild->GetNodeContent()));
                    }
                    else if (childOfChild->GetName() == "imagesBasePath")
                    {
                        imagesBasePath = childOfChild->GetNodeContent();
                        if (isXMLVariable(imagesBasePath))
                            imagesBasePath = getVariableValue(imagesBasePath, fileName);
                    }
                    else if (wxDir::Exists(imagesBasePath))
                    {
                        //Only loads images, if there is a valid base path for them:
                        if (childOfChild->GetName() == "default")
                            blockInfo->setBlockDefaultImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));
                        else if (childOfChild->GetName() == "hover")
                            blockInfo->setBlockHoverImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));
                        else if (childOfChild->GetName() == "pressed")
                            blockInfo->setBlockPressedImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));
                        else if (childOfChild->GetName() == "disabled")
                            blockInfo->setBlockDisabledImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));

                        //##getNotifier()->showMessage(wxString("image: ") + imagesBasePath + wxString("/") + childOfChild->GetNodeContent() + wxString("\n"), false, true, *wxBLUE); //##Debug.
                    }
                    childOfChild = childOfChild->GetNext();
                }
            }
            else if (child->GetName() == "methodButton") //##
            {
                wxString imagesBasePath("");
                wxXmlNode *childOfChild = child->GetChildren();
                while (childOfChild)
                {
                    //imagesBasePath must appear BEFORE the images (the variables in the XML are top-down):
                    if (childOfChild->GetName() == "has")
                    {
                        blockInfo->setHasMethodButton(Bubble::string2bool(childOfChild->GetNodeContent()));
                    }
                    else if (childOfChild->GetName() == "allowDrag")
                    {
                        blockInfo->setAllowDragOnMethodButton(Bubble::string2bool(childOfChild->GetNodeContent()));
                    }
                    else if (childOfChild->GetName() == "centered")
                    {
                        blockInfo->setCenteredMethodButton(Bubble::string2bool(childOfChild->GetNodeContent()));
                    }
                    else if (childOfChild->GetName() == "imagesBasePath")
                    {
                        imagesBasePath = childOfChild->GetNodeContent();
                        if (isXMLVariable(imagesBasePath))
                            imagesBasePath = getVariableValue(imagesBasePath, fileName);
                    }
                    else if (wxDir::Exists(imagesBasePath))
                    {
                        //Only loads images, if there is a valid base path for them:
                        if (childOfChild->GetName() == "default")
                            blockInfo->setMethodDefaultImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));
                        else if (childOfChild->GetName() == "hover")
                            blockInfo->setMethodHoverImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));
                        else if (childOfChild->GetName() == "pressed")
                            blockInfo->setMethodPressedImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));
                        else if (childOfChild->GetName() == "disabled")
                            blockInfo->setMethodDisabledImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));

                        //##getNotifier()->showMessage(wxString("image: ") + imagesBasePath + wxString("/") + childOfChild->GetNodeContent() + wxString("\n"), false, true, *wxBLUE); //##Debug.
                    }
                    childOfChild = childOfChild->GetNext();
                }
            }
            else if (child->GetName() == "addParamsButton") //##
            {
                wxString imagesBasePath("");
                wxXmlNode *childOfChild = child->GetChildren();
                while (childOfChild)
                {
                    //imagesBasePath must appear BEFORE the images (the variables in the XML are top-down):
                    if (childOfChild->GetName() == "has")
                    {
                        blockInfo->setHasAddParamsButton(Bubble::string2bool(childOfChild->GetNodeContent()));
                    }
                    else if (childOfChild->GetName() == "imagesBasePath")
                    {
                        imagesBasePath = childOfChild->GetNodeContent();
                        if (isXMLVariable(imagesBasePath))
                            imagesBasePath = getVariableValue(imagesBasePath, fileName);
                    }
                    else if (wxDir::Exists(imagesBasePath))
                    {
                        //Only loads images, if there is a valid base path for them:
                        if (childOfChild->GetName() == "default")
                            blockInfo->setAddParamsDefaultImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));
                        else if (childOfChild->GetName() == "hover")
                            blockInfo->setAddParamsHoverImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));
                        else if (childOfChild->GetName() == "pressed")
                            blockInfo->setAddParamsPressedImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));
                        else if (childOfChild->GetName() == "disabled")
                            blockInfo->setAddParamsDisabledImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));

                        //##getNotifier()->showMessage(wxString("image: ") + imagesBasePath + wxString("/") + childOfChild->GetNodeContent() + wxString("\n"), false, true, *wxBLUE); //##Debug.
                    }
                    childOfChild = childOfChild->GetNext();
                }
            }
            else if (child->GetName() == "removeParamsButton") //##
            {
                wxString imagesBasePath("");
                wxXmlNode *childOfChild = child->GetChildren();
                while (childOfChild)
                {
                    //There is no hasRemoveParamsButton, because it depends on the hasAddParamsButton:
                    //imagesBasePath must appear BEFORE the images (the variables in the XML are top-down):
                    if (childOfChild->GetName() == "imagesBasePath")
                    {
                        imagesBasePath = childOfChild->GetNodeContent();
                        if (isXMLVariable(imagesBasePath))
                            imagesBasePath = getVariableValue(imagesBasePath, fileName);
                    }
                    else if (wxDir::Exists(imagesBasePath))
                    {
                        //Only loads images, if there is a valid base path for them:
                        if (childOfChild->GetName() == "default")
                            blockInfo->setRemoveParamsDefaultImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));
                        else if (childOfChild->GetName() == "hover")
                            blockInfo->setRemoveParamsHoverImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));
                        else if (childOfChild->GetName() == "pressed")
                            blockInfo->setRemoveParamsPressedImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));
                        else if (childOfChild->GetName() == "disabled")
                            blockInfo->setRemoveParamsDisabledImage(wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent()));

                        //##Debug:
                        //getNotifier()->showMessage(wxString("image: ") + imagesBasePath + wxString("/") + childOfChild->GetNodeContent() + wxString("\n"), false, true, *wxBLUE); //##Debug.
                    }
                    childOfChild = childOfChild->GetNext();
                }
            }
            //Instance name field:
            else if (child->GetName() == "hasInstanceNameField")
            {
                blockInfo->setHasInstanceNameField(Bubble::string2bool(child->GetNodeContent()));
            }
            else if (child->GetName() == "instanceNameFieldDefaultValue")
            {
                blockInfo->setInstanceNameFieldDefaultValue(child->GetNodeContent());
            }
            else if (child->GetName() == "instanceNameFieldSorted")
            {
                blockInfo->setInstanceNameFieldSorted(Bubble::string2bool(child->GetNodeContent()));
            }
            else if (child->GetName() == "userMustSelectInstanceName")
            {
                blockInfo->setUserMustSelectInstanceName(Bubble::string2bool(child->GetNodeContent()));
            }
            else if (child->GetName() == "instanceType")
            {
                blockInfo->setInstanceType(child->GetNodeContent());
            }
            else if (child->GetName() == "updateVariables")
            {
                blockInfo->setUpdateVariables(Bubble::string2bool(child->GetNodeContent()));
            }
            //Constant field:
            else if (child->GetName() == "hasConstantField")
            {
                blockInfo->setHasConstantField(Bubble::string2bool(child->GetNodeContent()));
            }
            else if (child->GetName() == "constantFieldDefaultValue")
            {
                blockInfo->setConstantFieldDefaultValue(child->GetNodeContent());
            }
            else if (child->GetName() == "constantFieldEnableValidation")
            {
                blockInfo->setConstantFieldEnableValidation(Bubble::string2bool(child->GetNodeContent()));
            }
            else if (child->GetName() == "constantFieldValidatorCharList")
            {
                blockInfo->setConstantFieldValidatorCharList(child->GetNodeContent());
            }
            else if (child->GetName() == "constantFieldValidatorString")
            {
                blockInfo->setConstantFieldValidatorString(child->GetNodeContent());
            }

            child = child->GetNext();
        }
        return true;
//    }
//    return false;
}


bool BubbleXML::loadBlockInfoParamsFromXML(wxXmlNode *node, const wxString& fileName, BubbleBlockInfo *blockInfo)
{
    //##:
    if (bubble == NULL)
        return false;

    if (bubble->getNotifier() == NULL) //##Debug.
        return false; //##Debug.

    if (node == NULL)
        return false;
    if (blockInfo == NULL)
        return false;
//    if (node->GetName() == "params")
//    {
        //##Debug:
        //##getNotifier()->showMessage(wxString("params:\n"), false, true, *wxBLUE); //##Debug.

        wxString paramName("");
        wxString paramDataType("");
        wxString imagesBasePath("");
        wxImage imageParamNotAssigned;
        wxImage imageParamDefault;
        wxImage imageParamHover;
        wxImage imageParamPressed;
        wxImage imageParamDisabled;
        wxXmlNode *child = node->GetChildren();
        while (child)
        {
            //The order of the params matters. The name is the paramName (no name field inside the param element):
            paramName = child->GetName();
            wxXmlNode *childOfChild = child->GetChildren();
            while (childOfChild)
            {
                if (childOfChild->GetName() == "dataType")
                {
                    paramDataType = childOfChild->GetNodeContent();
                }
                //imagesBasePath must appear BEFORE the images (the variables in the XML are top-down):
                else if (childOfChild->GetName() == "imagesBasePath")
                {
                    imagesBasePath = childOfChild->GetNodeContent();
                    if (isXMLVariable(imagesBasePath))
                        imagesBasePath = getVariableValue(imagesBasePath, fileName);
                }
                else if (wxDir::Exists(imagesBasePath))
                {
                    //Only loads images, if there is a valid base path for them:
                    if (childOfChild->GetName() == "notAssigned")
                        imageParamNotAssigned = wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent());
                    else if (childOfChild->GetName() == "default")
                        imageParamDefault = wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent());
                    else if (childOfChild->GetName() == "hover")
                        imageParamHover = wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent());
                    else if (childOfChild->GetName() == "pressed")
                        imageParamPressed = wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent());
                    else if (childOfChild->GetName() == "disabled")
                        imageParamDisabled = wxImage(imagesBasePath + wxString("/") + childOfChild->GetNodeContent());

                    //##getNotifier()->showMessage(wxString("image: ") + imagesBasePath + wxString("/") + childOfChild->GetNodeContent() + wxString("\n"), false, true, *wxBLUE); //##Debug.
                }
                childOfChild = childOfChild->GetNext();
            }
            blockInfo->addParam(paramName,
                                paramDataType,
                                imageParamNotAssigned,
                                imageParamDefault,
                                imageParamHover,
                                imageParamPressed,
                                imageParamDisabled
                               );
            child = child->GetNext();
        }
        return true;
//    }
//    return false;
}


bool BubbleXML::addCatalog(wxString catalog)
{
    //First, tries to add the full language code which may contain a country code also (the format is
    //the suggested by ISO 639-2 -http://www.loc.gov/standards/iso639-2/php/code_list.php- for language
    //and by ISO 3166-1 -http://www.iso.org/iso/country_codes/iso-3166-1_decoding_table.htm- for country):
    if (!locale.AddCatalog(catalog))
    {
        //bubble->getNotifier()->showMessage(catalog + wxString("\n"), false, true, *wxGREEN); //##Debug.

        //If it can't load the full language+country code, tries to load the language only. But finally,
        //if there is no language-country, nor only language, loads the English catalog (which MUST exist):
        if (catalog.BeforeLast('.').BeforeLast('_') != wxEmptyString)
            catalog = catalog.BeforeLast('.').BeforeLast('_') + wxString(".mo");
        else
            catalog = catalog.BeforeLast('.').BeforeLast('.') + wxString(".en.mo");

        //bubble->getNotifier()->showMessage(catalog + wxString("\n"), false, true, *wxBLUE); //##Debug.
        return locale.AddCatalog(catalog);
    }

    return true;
}


bool BubbleXML::loadBlockInfoBrothersFromXML(wxXmlNode *node, const wxString& blockName, BubbleBlockInfo *blockInfo, wxWindow *pickersParent)
{
    //##:
    if (bubble == NULL)
        return false;

    if (bubble->getNotifier() == NULL) //##Debug.
        return false; //##Debug.

    if (node == NULL)
        return false;
    if (blockInfo == NULL)
        return false;
//    if (node->GetName() == "brothers")
//    {
        //##Debug:
        //##getNotifier()->showMessage(wxString("brothers:\n"), false, true, *wxBLUE); //##Debug.

        wxString fullBrotherFileName("");
        BubbleBlockInfo *tempBlockInfo = blockInfo;
        wxXmlNode *child = node->GetChildren();
        while (child)
        {
            //Only matters the order of the brothers, and the file (node content). The
            //node name doesn't matter:
            fullBrotherFileName = bubble->getBlocksPath() + wxString("/") + blockName + wxString("/") + child->GetNodeContent() + wxString(".block");

            if (wxFile::Exists(fullBrotherFileName))
            {
                //##Debug:
//                wxMessageDialog dialog0(parent, fullBrotherFileName, _("0"));
//                dialog0.ShowModal();
//                if (getNotifier())
//                    getNotifier()->showMessage(fullBrotherFileName + wxString("\n"), false, true, *wxBLUE); //##Debug.

                BubbleBlockInfo *brother = loadBlockInfo(child->GetNodeContent(), fullBrotherFileName, pickersParent);
                if (tempBlockInfo)
                {
                    if (brother)
                    {
                        //Loads the documentation in the selected (human) language:
                        wxString aux(bubble->getBlocksPath() + wxString("/") + blockName + wxString("/Doc"));
                        locale.AddCatalogLookupPathPrefix(aux);
                        addCatalog(child->GetNodeContent() + wxString(".") + getLanguageCode() + wxString(".mo"));
                        brother->setToolTip(_(brother->getName() + wxString(".tooltip")));
                        brother->setLabel(_(brother->getName() + wxString(".label")));

                        //##Future: This may change, but by now, brothers are not draggable blocks:
                        brother->setIsDraggable(false);

                        tempBlockInfo->setBrother(brother);
                        blocksInfo.Add(brother);    //##Ver si está bien: Si se cuelga, sacarlo!
                                                    //##Esto es para que cuando se limpie el array, libere
                                                    //la memoria de los brothers también, ya que no se
                                                    //destruyen en ninguna otra parte.
                        tempBlockInfo = brother;
                    }
                }
            }

            child = child->GetNext();
        }
        return true;
//    }
//    return false;
}


//##Pasar a const el puntero blockInfo, para que no lo pueda alterar nadie:
//##¡HACER ESTO CON TODOS LOS PUNTEROS CRÍTICOS, PARA AGREGAR SEGURIDAD!
bool BubbleXML::loadBlockInfoFriendsFromXML(wxXmlNode *node, const wxString& blockName, BubbleBlockInfo *blockInfo, wxWindow *pickersParent)
{
    //##:
    if (bubble == NULL)
        return false;

    if (bubble->getNotifier() == NULL) //##Debug.
        return false; //##Debug.

    if (node == NULL)
        return false;
    if (blockInfo == NULL)
        return false;
//    if (node->GetName() == "friends")
//    {
        //##Debug:
        //getNotifier()->showMessage(wxString("friends:\n"), false, true, *wxBLUE); //##Debug.

        wxString fullFriendFileName("");
        wxXmlNode *child = node->GetChildren();
        while (child)
        {
            //Only matters the order of the friends, and the file (node content). The
            //node name doesn't matter:
            fullFriendFileName = bubble->getBlocksPath() + wxString("/") + blockName + wxString("/") + child->GetNodeContent() + wxString(".block");

            if (wxFile::Exists(fullFriendFileName))
            {
                //##Debug:
//                wxMessageDialog dialog0(parent, fullFriendFileName, _("0"));
//                dialog0.ShowModal();
//                if (getNotifier())
//                    getNotifier()->showMessage(fullFriendFileName + wxString("\n"), false, true, *wxBLUE); //##Debug.

                BubbleBlockInfo *friendBlock = loadBlockInfo(child->GetNodeContent(), fullFriendFileName, pickersParent);
                if (friendBlock)
                {
                    //Loads the documentation in the selected (human) language:
                    wxString aux(bubble->getBlocksPath() + wxString("/") + blockName + wxString("/Doc"));
                    locale.AddCatalogLookupPathPrefix(aux);
                    addCatalog(child->GetNodeContent() + wxString(".") + getLanguageCode() + wxString(".mo"));
                    friendBlock->setToolTip(_(friendBlock->getName() + wxString(".tooltip")));
                    friendBlock->setLabel(_(friendBlock->getName() + wxString(".label")));

                    blocksInfo.Add(friendBlock);
                    bubble->addBlockToPicker(friendBlock, pickersParent);
                }
            }
            child = child->GetNext();
        }
        return true;
//    }
//    return false;
}


////##Don't delete this function (although the block doc system has changed, the code may still be useful):
////Only loads the documentation for the current language not all of them. This is to start quickly the
////application, only using time to load other idioms if the user change it:
//bool BubbleXML::loadBlockInfoDocFromXML(const wxString& fullDocFileName, BubbleBlockInfo *blockInfo)
//{
//    if (blockInfo == NULL)
//        return false;
//
//    //##Future: Try to disable the error messages that the wxXmlDocument class fires when encounters errors:
//    //wxString docFileName = blockDocPath + wxString("/") + blockName + wxString(".") + languageName;
//
//    //##Debug:
//    //getNotifier()->showMessage(fullDocFileName + wxString("\n"), false, true, *wxRED); //##Debug.
//
//    if (!wxFile::Exists(fullDocFileName))
//        return false; //This is to prevent the messagedialog that appears if componentFile.Load() fails.
//
//    //##Debug:
//    //getNotifier()->showMessage(fullDocFileName + wxString("\n"), false, true, *wxBLUE); //##Debug.
//
//    wxXmlDocument componentFile;
//    if ( !componentFile.Load(fullDocFileName, wxString("UTF-8")) ) //##Use OS separator...
//    {
//        //getNotifier()->showMessage(fullDocFileName + wxString("\n"), false, true, *wxRED); //##Debug.
//        return false;
//    }
//
//    wxXmlNode *root = componentFile.GetRoot();
//    if (root == NULL)
//        return false;
////This check is not necessary (it only makes things more compliated for the user):
////    if (root->GetName() != languageName)
////        return false;
//
//    //##Ver si hay que agregar seguridades a todo este proceso:
//    wxXmlNode *rootChild = root->GetChildren();
//    while (rootChild)
//    {
//        if (rootChild->GetName() == wxString("blockToolTip"))
//        {
//            blockInfo->setToolTip(rootChild->GetNodeContent());
//        }
//        rootChild = rootChild->GetNext();
//    }
//    return true;
//}


bool BubbleXML::loadBlockIncludes(const wxString& blockName, const wxString& targetName, const wxString& fileName)
{
    //##Implementar:
    return true;
}


bool BubbleXML::loadBlockLibs(const wxString& blockName, const wxString& targetName, const wxString& fileName)
{
    //##Implementar:
    return true;
}


//##Not necessary by now:
//int BubbleXML::getValidBlocksCount(const wxString& path) const
//{
//}


BubbleBlockInfo *BubbleXML::loadBlockInfo(const wxString &fileName, const wxString &fullBlockFileName, wxWindow *pickersParent)
{
    //##Future: Try to disable the error messages that the wxXmlDocument class fires when encounters errors:
    wxXmlDocument componentFile;
    if ( !componentFile.Load(fullBlockFileName, wxString("UTF-8")) )
        return NULL;

    //##Debug:
    //getNotifier()->showMessage(wxString("var name=thisBlockPath::") + wxString(", value=") + getVariableValue(wxString("thisBlockPath::"), fullBlockFileName) + wxString("\n"), false, true, *wxBLUE); //##Debug.
    //getNotifier()->showMessage(fullBlockFileName + wxString("\n"), false, true, *wxBLUE); //##Debug.

    wxXmlNode *root = componentFile.GetRoot();
    if (root == NULL)
        return NULL;
    if (root->GetName() != wxString("block"))
        return NULL;

    BubbleBlockInfo *blockInfo = new BubbleBlockInfo();

    //##Ver si hay que agregar seguridades a todo este proceso:
    wxString tempName("");
    wxXmlNode *rootChild = root->GetChildren();
    while (rootChild)
    {

//        //##Debug:
//        wxMessageDialog dialog0(parent, rootChild->GetName(), fullBlockFileName); //##Debug.
//        dialog0.ShowModal();  //##Debug.

//##:
        //These nested are to optimize: if a tag is loaded, this iteration does not try
        //to load the other tags:
//                        if (!loadBlockInfoPropertiesFromXML(rootChild, blockInfo))
//                            if (!loadBlockInfoCodeFromXML(rootChild, blockInfo))
//                                if (!loadBlockInfoInterfaceFromXML(rootChild, blockInfo))
//                                    loadBlockInfoParamsFromXML(rootChild, blockInfo);
        tempName = rootChild->GetName();
        if (tempName == wxString("properties"))
            loadBlockInfoPropertiesFromXML(rootChild, blockInfo);
        else if (tempName == wxString("code"))
            loadBlockInfoCodeFromXML(rootChild, blockInfo);
        else if (tempName == wxString("interface"))
            loadBlockInfoInterfaceFromXML(rootChild, fullBlockFileName, blockInfo);
        else if (tempName == wxString("params"))
            loadBlockInfoParamsFromXML(rootChild, fullBlockFileName, blockInfo);
        else if (tempName == wxString("brothers"))
        {
            //Only passes the block's name (not the full path), contained in fileName:
            loadBlockInfoBrothersFromXML(rootChild, fileName, blockInfo, pickersParent);
        }
        else if (tempName == wxString("friends"))
        {
            //Now the friend blocks:
            loadBlockInfoFriendsFromXML(rootChild, fileName, blockInfo, pickersParent);
        }

        //##En cuanto esté el subsistema de targets armado, implementar algo como esto:
        //loadBlockIncludes(targetName); //##Esto cargará el código a generar para incluir las librerías asociadas al bloque.
        //loadBlockLibs(targetName); //##Esto cargará el código a COMPILAR para incluir las librerías asociadas al bloque.
        rootChild = rootChild->GetNext();
    }

    return blockInfo;
}


//Returns the number of loaded blocks:
int BubbleXML::loadBlocksInfo(wxWindow *pickersParent, bool showPickers) //##Hacer este puntero constante.
{
    //wxMessageDialog dialog0(parent, wxString("Loading blocks..."), _("0")); //##Debug.
    //dialog0.ShowModal(); //##Debug.
    if (bubble == NULL)
        return -1;

    if (bubble->getNotifier() == NULL) //##Debug, no sé si esto quedará acá.
        return -2; //##Future: standarize errors.
    //getNotifier()->showMessage(blocksPath + wxString("\n"), true, true, *wxBLUE); //##Debug.

    wxDir dir(bubble->getBlocksPath());
    if ( !dir.IsOpened() )
    {
        //##According to the wxWidgets documentation, wxDir will log an error message (with extra
        //explanations) but where? ##Check this.
        return -3; //##Futuro: estandaziar errores.
    }

    bubble->clearExpressionPickers();
    bubble->createActionPicker(pickersParent, showPickers);

    //Load external application variables (internal variable values are computed at run-time in
    //getVariableValue() function):
    loadVariablesFromXML(bubble->getAppPath() + wxString("/Minibloq.xdfc"), false); //##Un-hardcode?

    //Load "blocks-aggregate" variables:
    loadVariablesFromXML(bubble->getBlocksPath() + wxString("/blocksCommonSettings.xml"), false); //##Un-hardcode?

    //Load "blocks-aggregate" properties:
    loadBlocksPropertiesFromXML(bubble->getBlocksPath() + wxString("/blocksCommonSettings.xml"), false); //##Un-hardcode?

    //Deletes all previous blocks info:
    blocksInfo.Clear();

    int blocksCount = getBlockFilesCount(bubble->getBlocksPath(), wxDIR_DIRS);
    //wxMessageDialog dialog0(parent, blocksPath, wxString("") << blocksCount); //##Debug
    //dialog0.ShowModal(); //##Debug
    if (blocksCount >= 0)
    {
        bubble->getNotifier()->setProgressMax(blocksCount);
        bubble->getNotifier()->setProgressPosition(0, true, false);
    }
    int counter = 0;
    wxString fileName;
    bool result = dir.GetFirst(&fileName, wxEmptyString, wxDIR_DIRS); //Only enumerate directories.

    wxArrayString fileNames;


    while (result)
    {
        fileNames.Add( fileName );
        result = dir.GetNext(&fileName);
    }
    fileNames.Sort();
    for (size_t i = 0; i < fileNames.size() ; i++)
    {
        fileName = fileNames[i];
        //##Esta clase de cosas de debug se pueden dejar, para un perfil de usuario que sea de
        //"Development", y que haga explícitas muchas acciones en el messageWindow:
        //getNotifier()->showMessage(fileName + wxString("\n"), false, true, *wxBLUE); //##Debug.

        bubble->getNotifier()->setProgressPosition(counter, false, false);

        //Blocks must have an XML file, with ".bloq" extension, and with exactly the same name as their containing dir:
        //##wxString fullBlockFileName = blocksPath + wxString("/") + fileName + wxString("/") + fileName + wxString(".block");
        wxString fullBlockFileName = bubble->getBlocksPath() + wxString("/") + fileName + wxString("/main.block");
        wxString fullRelFileName = bubble->getMatrixPath() + wxString("/") + fileName + wxString(".rel");
        //getNotifier()->showMessage(fullRelFileName + wxString("\n"), false, true, *wxBLUE); //##Debug.

        if (    wxFile::Exists(fullBlockFileName) &&
                wxFile::Exists(fullRelFileName) )
        {
            BubbleBlockInfo *loadedBlockInfo = loadBlockInfo(fileName, fullBlockFileName, pickersParent);
            if (loadedBlockInfo)
            {
                wxString aux(bubble->getBlocksPath() + wxString("/") + fileName + wxString("/Doc"));
                locale.AddCatalogLookupPathPrefix(aux);
                addCatalog(loadedBlockInfo->getName() + wxString(".") + getLanguageCode() + wxString(".mo"));
                loadedBlockInfo->setToolTip(_(loadedBlockInfo->getName() + wxString(".tooltip")));
                loadedBlockInfo->setLabel(_(loadedBlockInfo->getName() + wxString(".label")));

                //##Acá hay que verificar si el target y el block están relacionados, si no, no se agrega a blocks y no se carga el
                //block a au picker.

                blocksInfo.Add(loadedBlockInfo);

                //Only add the block to the picker if the action is different from "noLoad". This is important to avoid
                //adding blocks like componentStart, for example.
                if ( loadedBlockInfo->getLoadAction() != wxString("noLoad") )
                {
                    bubble->addBlockToPicker(loadedBlockInfo, pickersParent);
                }
                counter++;
            }
        }
        //result = dir.GetNext(&fileName);
    }

    //##Mostrar los pickers recién cuando han sido cargados todos los bloques:
    if (bubble->getActionPicker())
    {
        bubble->getActionPicker()->Show(showPickers);

        //Prevents bad painting (with background elements on the panel's surface):
        bubble->getActionPicker()->Update();
        bubble->getActionPicker()->Refresh();
    }

    //##Ver si esto se queda, por ahora es ##debug:
    //##Si se quiere mostrar la cantidad de bloques cargada, realmente, hay que incrementar también cuando
    //se cargan friends (¿y brothers?, no por ahora):
    //getNotifier()->showMessage(wxString("\nLoaded blocks = ") << counter, false, true, *wxGREEN); //##Debug.

    //getNotifier()->showMessage(wxString("\nOk"), false, true, *wxGREEN); //##Debug.

    bubble->getNotifier()->setProgressPosition(bubble->getNotifier()->getProgressMax(), false, false);
    bubble->getNotifier()->hideMessagesWindow();

    return counter; //##
}


//##Este if condicional se va luego, cuando esté reemplazada por completo la función getBlockInfo():
const BubbleBlockInfo& BubbleXML::getBlockInfo(const wxString& name, const wxString& function) //##Off for debug: const
//BubbleBlockInfo Bubble::getBlockInfo(const wxString& name, const wxString& function) //##Off for debug: const
{
    if (bubble == NULL)
        return emptyBubbleInfo;

    //##Debug:
    if (bubble->getNotifier() == NULL)
        return emptyBubbleInfo;
    //##getNotifier()->showMessage(name + wxString("\n"), false, true, *wxGREEN); //##Debug.

    //##Optimize in the future: use a wxHashmap or at least a binary search (but the blocksInfo will have to be ordered by name):
    BubbleBlockInfo *iterator = NULL;
    for (unsigned int i = 0; i<blocksInfo.GetCount(); i++)
    {
        iterator = &(blocksInfo.Item(i)); //##In theory, this is faster than the other index based form, but I'm not sure yet...
        if (iterator)
        {
            if (iterator->getName() == name)
            {
                //getNotifier()->showMessage(name + wxString("\n"), false, true, *wxGREEN); //##Debug.
                if (iterator->getFunction() == function)
                {
                    //##:
//                    getNotifier()->showMessage(wxString("\n") + name + wxString(" - ") + function + wxString("\n"), false, true, *wxGREEN); //##Debug.
                    //getNotifier()->showMessage( iterator->getDefaultBackgroundColour0().GetAsString() + wxString("\n"), false, true, *wxGREEN); //##Debug.
                    return *iterator;
                }
            }
        }
    }

    //##Horrible, but works nice! (I don't want to return a whole copy of a big BubbleBlockInfo object, so
    //this returns a const reference, and this is instead of a NULL):
    return emptyBubbleInfo;
}


bool BubbleXML::blockIsValid(const wxString& name, const wxString& type) const
{
    //##Implementar:

    //Contra el name y el type, decide si el bloque existe (para que luego se llame
    //a getBlockInfo() y se obtenga la información completa. En el futuro se puede optimizar esto para que no
    //haga la búsqueda 2 veces, de modo que si la última llamada a blockIsValid coincide en parámetros con
    //la llamada a getBlockInfo, quede indexado de alguna manera y vaya más rápido...

    return true; //##
}


////////////////////////////////////////////////////////////////////////////////////////////
//
//Canvas structure functions:
//
////////////////////////////////////////////////////////////////////////////////////////////
bool BubbleXML::loadBoardInstancesFromXML(wxXmlNode *node, BubbleCanvasInfo *canvasInfo)
{
    if (node == NULL)
        return false;
    if (canvasInfo == NULL)
        return false;

    wxXmlNode *child = node->GetChildren();
    while (child)
    {
        BubbleInstance *newInstance = new BubbleInstance(child->GetName(),
                                                         child->GetAttribute(wxString("instanceType"),
                                                         wxString("")));
        if (newInstance)
            canvasInfo->setInstance(newInstance);

        child = child->GetNext();
    }

    return true;
}


BubbleCanvasInfo BubbleXML::getCanvasInfo(bool mainCanvas)
{
    BubbleCanvasInfo info(  mainCanvas,
                            //wxColour(255, 249, 189),
                            //wxColour(128, 128, 128),
                            //wxColour(92, 92, 92),
                            //wxColour(72, 72, 72),
                            wxColour(62, 62, 62),
                            //wxColour(0, 0, 0),
                            67, //tabWidth
                            28, //initialBlockX
                            28  //initialBlockY
                         );

    if (bubble == NULL)
        return info; //##Add error control in the future.

    //Canvas preinstatiated ojects:
    if (mainCanvas) //##¡Ver si esto cambia por currentCanvas!
    {
        wxString fullBoardFileName = bubble->getBoardPath() + wxString("/main.board");
        if ( !wxFile::Exists(fullBoardFileName) )
            return info; //##Add error control.

        wxXmlDocument boardFile;
        if ( !boardFile.Load(fullBoardFileName, wxString("UTF-8")) )
            return info; //##Add error control.

        wxXmlNode *root = boardFile.GetRoot();
        if (root == NULL)
            return info; //##Add error control.
        if (root->GetName() != wxString("board"))
            return info; //##Add error control.

        wxString tempName("");
        wxXmlNode *rootChild = root->GetChildren();
        while (rootChild)
        {
            tempName = rootChild->GetName();

            //##Future: Configure every board aspect with XML files:
            //if (tempName == wxString("properties"))
            //    loadBoardInfoPropertiesFromXML(rootChild, info);
            //else if (tempName == wxString("instances"))

            if (tempName == wxString("instances"))
                loadBoardInstancesFromXML(rootChild, &info);
            rootChild = rootChild->GetNext();
        }
    }
    return info;
}
