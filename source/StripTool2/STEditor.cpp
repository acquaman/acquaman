#include "STEditor.h"

STEditor::STEditor(QWidget *parent) : QFrame(parent)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    setLineWidth(0);
}

STEditor::~STEditor()
{

}
