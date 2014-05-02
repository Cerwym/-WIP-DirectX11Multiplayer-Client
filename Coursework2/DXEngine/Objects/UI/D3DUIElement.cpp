#include "D3DUIElement.h"

D3DUIElement::D3DUIElement()
{
	m_TexureData = 0;
	IsVisble = true;
}

D3DUIElement::~D3DUIElement()
{
	S_DELETE( m_TexureData );
}