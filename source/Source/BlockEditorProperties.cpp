#include "BlockEditorProperties.h"

/*##Ver si se usará:
BEGIN_EVENT_TABLE(BubbleVariablesManager, BubblePanel)
    //EVT_SIZE(BubbleVariablesManager::onSize)
END_EVENT_TABLE()
*/

BlockEditorProperties::BlockEditorProperties(   wxWindow* parent,
                                                wxWindowID id,
                                                const wxColour& colour,
                                                const wxPoint& pos,
                                                const wxSize& size,
                                                long style,
                                                const wxString& name) : BubblePanel(parent,
                                                                                    id,
                                                                                    colour,
                                                                                    true,
                                                                                    pos,
                                                                                    size,
                                                                                    style,
                                                                                    name)
{
    //Hide();

    //SetBackgroundColour(colour);
    SetBackgroundColourAndRefresh(colour);
}


BlockEditorProperties::~BlockEditorProperties()
{
    //##No va: Como los elementos de la lista son descendientes de BubblePanel, y tienen sus parents,
    //el frame o dialog al que pertenecen los destruirá. Y aquí, como el objeto canvases es estático,
    //se asegura la llamada a su destructor sin hacer nada:
    //canvases.Clear();
}


/*##Ver si es necesario este evento...
void BlockEditorProperties::onSize(wxSizeEvent& event)
{
    event.Skip();
}
*/
