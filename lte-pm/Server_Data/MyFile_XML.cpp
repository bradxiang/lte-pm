/*
Original code by Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/
#include "StdAfx.h"
#include "MyFile_XML.h"
using namespace tinyxml2; 


#include <new>		// yes, this one new style header, is in the Android SDK.
#   ifdef ANDROID_NDK
#   include <stddef.h>
#else
#   include <cstddef>
#endif

static const char LINE_FEED				= (char)0x0a;			// all line endings are normalized to LF
static const char LF = LINE_FEED;
static const char CARRIAGE_RETURN		= (char)0x0d;			// CR gets filtered out
static const char CR = CARRIAGE_RETURN;
static const char SINGLE_QUOTE			= '\'';
static const char DOUBLE_QUOTE			= '\"';

// Bunch of unicode info at:
//		http://www.unicode.org/faq/utf_bom.html
//	ef bb bf (Microsoft "lead bytes") - designates UTF-8

static const unsigned char TIXML_UTF_LEAD_0 = 0xefU;
static const unsigned char TIXML_UTF_LEAD_1 = 0xbbU;
static const unsigned char TIXML_UTF_LEAD_2 = 0xbfU;


#define DELETE_NODE( node )	{			\
        if ( node ) {						\
            MemPool* pool = node->_memPool;	\
            node->~XMLNode();				\
            pool->Free( node );				\
        }									\
    }
#define DELETE_ATTRIBUTE( attrib ) {		\
        if ( attrib ) {							\
            MemPool* pool = attrib->_memPool;	\
            attrib->~XMLAttribute();			\
            pool->Free( attrib );				\
        }										\
    }

namespace tinyxml2
{

struct Entity {
    const char* pattern;
    int length;
    char value;
};

static const int NUM_ENTITIES = 5;
static const Entity entities[NUM_ENTITIES] = {
    { "quot", 4,	DOUBLE_QUOTE },
    { "amp", 3,		'&'  },
    { "apos", 4,	SINGLE_QUOTE },
    { "lt",	2, 		'<'	 },
    { "gt",	2,		'>'	 }
};


StrPair::~StrPair()
{
    Reset();
}


void StrPair::Reset()
{
    if ( _flags & NEEDS_DELETE ) {
        delete [] _start;
    }
    _flags = 0;
    _start = 0;
    _end = 0;
}


void StrPair::SetStr( const char* str, int flags )
{
    Reset();
    size_t len = strlen( str );
    _start = new char[ len+1 ];
    memcpy( _start, str, len+1 );
    _end = _start + len;
    _flags = flags | NEEDS_DELETE;
}


char* StrPair::ParseText( char* p, const char* endTag, int strFlags )
{
    TIXMLASSERT( endTag && *endTag );

    char* start = p;	// fixme: hides a member
    char  endChar = *endTag;
    size_t length = strlen( endTag );

    // Inner loop of text parsing.
    while ( *p ) {
        if ( *p == endChar && strncmp( p, endTag, length ) == 0 ) {
            Set( start, p, strFlags );
            return p + length;
        }
        ++p;
    }
    return 0;
}


char* StrPair::ParseName( char* p )
{
    char* start = p;

    if ( !start || !(*start) ) {
        return 0;
    }

    while( *p && ( p == start ? XMLUtil::IsNameStartChar( *p ) : XMLUtil::IsNameChar( *p ) )) {
        ++p;
    }

    if ( p > start ) {
        Set( start, p, 0 );
        return p;
    }
    return 0;
}


void StrPair::CollapseWhitespace()
{
    // Trim leading space.
    _start = XMLUtil::SkipWhiteSpace( _start );

    if ( _start && *_start ) {
        char* p = _start;	// the read pointer
        char* q = _start;	// the write pointer

        while( *p ) {
            if ( XMLUtil::IsWhiteSpace( *p )) {
                p = XMLUtil::SkipWhiteSpace( p );
                if ( *p == 0 ) {
                    break;    // don't write to q; this trims the trailing space.
                }
                *q = ' ';
                ++q;
            }
            *q = *p;
            ++q;
            ++p;
        }
        *q = 0;
    }
}


const char* StrPair::GetStr()
{
    if ( _flags & NEEDS_FLUSH ) {
        *_end = 0;
        _flags ^= NEEDS_FLUSH;

        if ( _flags ) {
            char* p = _start;	// the read pointer
            char* q = _start;	// the write pointer

            while( p < _end ) {
                if ( (_flags & NEEDS_NEWLINE_NORMALIZATION) && *p == CR ) {
                    // CR-LF pair becomes LF
                    // CR alone becomes LF
                    // LF-CR becomes LF
                    if ( *(p+1) == LF ) {
                        p += 2;
                    }
                    else {
                        ++p;
                    }
                    *q++ = LF;
                }
                else if ( (_flags & NEEDS_NEWLINE_NORMALIZATION) && *p == LF ) {
                    if ( *(p+1) == CR ) {
                        p += 2;
                    }
                    else {
                        ++p;
                    }
                    *q++ = LF;
                }
                else if ( (_flags & NEEDS_ENTITY_PROCESSING) && *p == '&' ) {
                    // Entities handled by tinyXML2:
                    // - special entities in the entity table [in/out]
                    // - numeric character reference [in]
                    //   &#20013; or &#x4e2d;

                    if ( *(p+1) == '#' ) {
                        char buf[10] = { 0 };
                        int len;
                        p = const_cast<char*>( XMLUtil::GetCharacterRef( p, buf, &len ) );
                        for( int i=0; i<len; ++i ) {
                            *q++ = buf[i];
                        }
                        TIXMLASSERT( q <= p );
                    }
                    else {
                        int i=0;
                        for(; i<NUM_ENTITIES; ++i ) {
                            if (    strncmp( p+1, entities[i].pattern, entities[i].length ) == 0
                                    && *(p+entities[i].length+1) == ';' ) {
                                // Found an entity convert;
                                *q = entities[i].value;
                                ++q;
                                p += entities[i].length + 2;
                                break;
                            }
                        }
                        if ( i == NUM_ENTITIES ) {
                            // fixme: treat as error?
                            ++p;
                            ++q;
                        }
                    }
                }
                else {
                    *q = *p;
                    ++p;
                    ++q;
                }
            }
            *q = 0;
        }
        // The loop below has plenty going on, and this
        // is a less useful mode. Break it out.
        if ( _flags & COLLAPSE_WHITESPACE ) {
            CollapseWhitespace();
        }
        _flags = (_flags & NEEDS_DELETE);
    }
    return _start;
}




// --------- XMLUtil ----------- //

const char* XMLUtil::ReadBOM( const char* p, bool* bom )
{
    *bom = false;
    const unsigned char* pu = reinterpret_cast<const unsigned char*>(p);
    // Check for BOM:
    if (    *(pu+0) == TIXML_UTF_LEAD_0
            && *(pu+1) == TIXML_UTF_LEAD_1
            && *(pu+2) == TIXML_UTF_LEAD_2 ) {
        *bom = true;
        p += 3;
    }
    return p;
}


void XMLUtil::ConvertUTF32ToUTF8( long input, char* output, int* length )
{
    const unsigned long BYTE_MASK = 0xBF;
    const unsigned long BYTE_MARK = 0x80;
    const unsigned long FIRST_BYTE_MARK[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

    if (input < 0x80) {
        *length = 1;
    }
    else if ( input < 0x800 ) {
        *length = 2;
    }
    else if ( input < 0x10000 ) {
        *length = 3;
    }
    else if ( input < 0x200000 ) {
        *length = 4;
    }
    else {
        *length = 0;    // This code won't covert this correctly anyway.
        return;
    }

    output += *length;

    // Scary scary fall throughs.
    switch (*length) {
        case 4:
            --output;
            *output = (char)((input | BYTE_MARK) & BYTE_MASK);
            input >>= 6;
        case 3:
            --output;
            *output = (char)((input | BYTE_MARK) & BYTE_MASK);
            input >>= 6;
        case 2:
            --output;
            *output = (char)((input | BYTE_MARK) & BYTE_MASK);
            input >>= 6;
        case 1:
            --output;
            *output = (char)(input | FIRST_BYTE_MARK[*length]);
        default:
            break;
    }
}


const char* XMLUtil::GetCharacterRef( const char* p, char* value, int* length )
{
    // Presume an entity, and pull it out.
    *length = 0;

    if ( *(p+1) == '#' && *(p+2) ) {
        unsigned long ucs = 0;
        ptrdiff_t delta = 0;
        unsigned mult = 1;

        if ( *(p+2) == 'x' ) {
            // Hexadecimal.
            if ( !*(p+3) ) {
                return 0;
            }

            const char* q = p+3;
            q = strchr( q, ';' );

            if ( !q || !*q ) {
                return 0;
            }

            delta = q-p;
            --q;

            while ( *q != 'x' ) {
                if ( *q >= '0' && *q <= '9' ) {
                    ucs += mult * (*q - '0');
                }
                else if ( *q >= 'a' && *q <= 'f' ) {
                    ucs += mult * (*q - 'a' + 10);
                }
                else if ( *q >= 'A' && *q <= 'F' ) {
                    ucs += mult * (*q - 'A' + 10 );
                }
                else {
                    return 0;
                }
                mult *= 16;
                --q;
            }
        }
        else {
            // Decimal.
            if ( !*(p+2) ) {
                return 0;
            }

            const char* q = p+2;
            q = strchr( q, ';' );

            if ( !q || !*q ) {
                return 0;
            }

            delta = q-p;
            --q;

            while ( *q != '#' ) {
                if ( *q >= '0' && *q <= '9' ) {
                    ucs += mult * (*q - '0');
                }
                else {
                    return 0;
                }
                mult *= 10;
                --q;
            }
        }
        // convert the UCS to UTF-8
        ConvertUTF32ToUTF8( ucs, value, length );
        return p + delta + 1;
    }
    return p+1;
}


void XMLUtil::ToStr( int v, char* buffer, int bufferSize )
{
    TIXML_SNPRINTF( buffer, bufferSize, "%d", v );
}


void XMLUtil::ToStr( unsigned v, char* buffer, int bufferSize )
{
    TIXML_SNPRINTF( buffer, bufferSize, "%u", v );
}


void XMLUtil::ToStr( bool v, char* buffer, int bufferSize )
{
    TIXML_SNPRINTF( buffer, bufferSize, "%d", v ? 1 : 0 );
}


void XMLUtil::ToStr( float v, char* buffer, int bufferSize )
{
    TIXML_SNPRINTF( buffer, bufferSize, "%f", v );
}


void XMLUtil::ToStr( double v, char* buffer, int bufferSize )
{
    TIXML_SNPRINTF( buffer, bufferSize, "%f", v );
}


bool XMLUtil::ToInt( const char* str, int* value )
{
    if ( TIXML_SSCANF( str, "%d", value ) == 1 ) {
        return true;
    }
    return false;
}

bool XMLUtil::ToUnsigned( const char* str, unsigned *value )
{
    if ( TIXML_SSCANF( str, "%u", value ) == 1 ) {
        return true;
    }
    return false;
}

bool XMLUtil::ToBool( const char* str, bool* value )
{
    int ival = 0;
    if ( ToInt( str, &ival )) {
        *value = (ival==0) ? false : true;
        return true;
    }
    if ( StringEqual( str, "true" ) ) {
        *value = true;
        return true;
    }
    else if ( StringEqual( str, "false" ) ) {
        *value = false;
        return true;
    }
    return false;
}


bool XMLUtil::ToFloat( const char* str, float* value )
{
    if ( TIXML_SSCANF( str, "%f", value ) == 1 ) {
        return true;
    }
    return false;
}

bool XMLUtil::ToDouble( const char* str, double* value )
{
    if ( TIXML_SSCANF( str, "%lf", value ) == 1 ) {
        return true;
    }
    return false;
}


char* MyXMLDocument::Identify( char* p, XMLNode** node )
{
    XMLNode* returnNode = 0;
    char* start = p;
    p = XMLUtil::SkipWhiteSpace( p );
    if( !p || !*p ) {
        return p;
    }

    // What is this thing?
    // - Elements start with a letter or underscore, but xml is reserved.
    // - Comments: <!--
    // - Declaration: <?
    // - Everything else is unknown to tinyxml.
    //

    static const char* xmlHeader		= { "<?" };
    static const char* commentHeader	= { "<!--" };
    static const char* dtdHeader		= { "<!" };
    static const char* cdataHeader		= { "<![CDATA[" };
    static const char* elementHeader	= { "<" };	// and a header for everything else; check last.

    static const int xmlHeaderLen		= 2;
    static const int commentHeaderLen	= 4;
    static const int dtdHeaderLen		= 2;
    static const int cdataHeaderLen		= 9;
    static const int elementHeaderLen	= 1;

#if defined(_MSC_VER)
#pragma warning ( push )
#pragma warning ( disable : 4127 )
#endif
    TIXMLASSERT( sizeof( XMLComment ) == sizeof( XMLUnknown ) );		// use same memory pool
    TIXMLASSERT( sizeof( XMLComment ) == sizeof( XMLDeclaration ) );	// use same memory pool
#if defined(_MSC_VER)
#pragma warning (pop)
#endif
    if ( XMLUtil::StringEqual( p, xmlHeader, xmlHeaderLen ) ) {
        returnNode = new (_commentPool.Alloc()) XMLDeclaration( this );
        returnNode->_memPool = &_commentPool;
        p += xmlHeaderLen;
    }
    else if ( XMLUtil::StringEqual( p, commentHeader, commentHeaderLen ) ) {
        returnNode = new (_commentPool.Alloc()) XMLComment( this );
        returnNode->_memPool = &_commentPool;
        p += commentHeaderLen;
    }
    else if ( XMLUtil::StringEqual( p, cdataHeader, cdataHeaderLen ) ) {
        XMLText* text = new (_textPool.Alloc()) XMLText( this );
        returnNode = text;
        returnNode->_memPool = &_textPool;
        p += cdataHeaderLen;
        text->SetCData( true );
    }
    else if ( XMLUtil::StringEqual( p, dtdHeader, dtdHeaderLen ) ) {
        returnNode = new (_commentPool.Alloc()) XMLUnknown( this );
        returnNode->_memPool = &_commentPool;
        p += dtdHeaderLen;
    }
    else if ( XMLUtil::StringEqual( p, elementHeader, elementHeaderLen ) ) {
        returnNode = new (_elementPool.Alloc()) XMLElement( this );
        returnNode->_memPool = &_elementPool;
        p += elementHeaderLen;
    }
    else {
        returnNode = new (_textPool.Alloc()) XMLText( this );
        returnNode->_memPool = &_textPool;
        p = start;	// Back it up, all the text counts.
    }

    *node = returnNode;
    return p;
}


bool MyXMLDocument::Accept( XMLVisitor* visitor ) const
{
    if ( visitor->VisitEnter( *this ) ) {
        for ( const XMLNode* node=FirstChild(); node; node=node->NextSibling() ) {
            if ( !node->Accept( visitor ) ) {
                break;
            }
        }
    }
    return visitor->VisitExit( *this );
}


// --------- XMLNode ----------- //

XMLNode::XMLNode( MyXMLDocument* doc ) :
    _document( doc ),
    _parent( 0 ),
    _firstChild( 0 ), _lastChild( 0 ),
    _prev( 0 ), _next( 0 ),
    _memPool( 0 )
{
}


XMLNode::~XMLNode()
{
    DeleteChildren();
    if ( _parent ) {
        _parent->Unlink( this );
    }
}


void XMLNode::SetValue( const char* str, bool staticMem )
{
    if ( staticMem ) {
        _value.SetInternedStr( str );
    }
    else {
        _value.SetStr( str );
    }
}


void XMLNode::DeleteChildren()
{
    while( _firstChild ) {
        XMLNode* node = _firstChild;
        Unlink( node );

        DELETE_NODE( node );
    }
    _firstChild = _lastChild = 0;
}


void XMLNode::Unlink( XMLNode* child )
{
    TIXMLASSERT( child->_parent == this );
    if ( child == _firstChild ) {
        _firstChild = _firstChild->_next;
    }
    if ( child == _lastChild ) {
        _lastChild = _lastChild->_prev;
    }

    if ( child->_prev ) {
        child->_prev->_next = child->_next;
    }
    if ( child->_next ) {
        child->_next->_prev = child->_prev;
    }
    child->_parent = 0;
}


void XMLNode::DeleteChild( XMLNode* node )
{
    TIXMLASSERT( node->_parent == this );
    DELETE_NODE( node );
}


XMLNode* XMLNode::InsertEndChild( XMLNode* addThis )
{
    if ( _lastChild ) {
        TIXMLASSERT( _firstChild );
        TIXMLASSERT( _lastChild->_next == 0 );
        _lastChild->_next = addThis;
        addThis->_prev = _lastChild;
        _lastChild = addThis;

        addThis->_next = 0;
    }
    else {
        TIXMLASSERT( _firstChild == 0 );
        _firstChild = _lastChild = addThis;

        addThis->_prev = 0;
        addThis->_next = 0;
    }
    addThis->_parent = this;
    addThis->_memPool->SetTracked();
    return addThis;
}


XMLNode* XMLNode::InsertFirstChild( XMLNode* addThis )
{
    if ( _firstChild ) {
        TIXMLASSERT( _lastChild );
        TIXMLASSERT( _firstChild->_prev == 0 );

        _firstChild->_prev = addThis;
        addThis->_next = _firstChild;
        _firstChild = addThis;

        addThis->_prev = 0;
    }
    else {
        TIXMLASSERT( _lastChild == 0 );
        _firstChild = _lastChild = addThis;

        addThis->_prev = 0;
        addThis->_next = 0;
    }
    addThis->_parent = this;
    addThis->_memPool->SetTracked();
    return addThis;
}


XMLNode* XMLNode::InsertAfterChild( XMLNode* afterThis, XMLNode* addThis )
{
    TIXMLASSERT( afterThis->_parent == this );
    if ( afterThis->_parent != this ) {
        return 0;
    }

    if ( afterThis->_next == 0 ) {
        // The last node or the only node.
        return InsertEndChild( addThis );
    }
    addThis->_prev = afterThis;
    addThis->_next = afterThis->_next;
    afterThis->_next->_prev = addThis;
    afterThis->_next = addThis;
    addThis->_parent = this;
    addThis->_memPool->SetTracked();
    return addThis;
}


const XMLElement* XMLNode::FirstChildElement( const char* value ) const
{
    for( XMLNode* node=_firstChild; node; node=node->_next ) {
        XMLElement* element = node->ToElement();
        if ( element ) {
            if ( !value || XMLUtil::StringEqual( element->Name(), value ) ) {
                return element;
            }
        }
    }
    return 0;
}


const XMLElement* XMLNode::LastChildElement( const char* value ) const
{
    for( XMLNode* node=_lastChild; node; node=node->_prev ) {
        XMLElement* element = node->ToElement();
        if ( element ) {
            if ( !value || XMLUtil::StringEqual( element->Name(), value ) ) {
                return element;
            }
        }
    }
    return 0;
}


const XMLElement* XMLNode::NextSiblingElement( const char* value ) const
{
    for( XMLNode* element=this->_next; element; element = element->_next ) {
        if (    element->ToElement()
                && (!value || XMLUtil::StringEqual( value, element->Value() ))) {
            return element->ToElement();
        }
    }
    return 0;
}


const XMLElement* XMLNode::PreviousSiblingElement( const char* value ) const
{
    for( XMLNode* element=_prev; element; element = element->_prev ) {
        if (    element->ToElement()
                && (!value || XMLUtil::StringEqual( value, element->Value() ))) {
            return element->ToElement();
        }
    }
    return 0;
}


char* XMLNode::ParseDeep( char* p, StrPair* parentEnd )
{
    // This is a recursive method, but thinking about it "at the current level"
    // it is a pretty simple flat list:
    //		<foo/>
    //		<!-- comment -->
    //
    // With a special case:
    //		<foo>
    //		</foo>
    //		<!-- comment -->
    //
    // Where the closing element (/foo) *must* be the next thing after the opening
    // element, and the names must match. BUT the tricky bit is that the closing
    // element will be read by the child.
    //
    // 'endTag' is the end tag for this node, it is returned by a call to a child.
    // 'parentEnd' is the end tag for the parent, which is filled in and returned.

    while( p && *p ) {
        XMLNode* node = 0;

        p = _document->Identify( p, &node );
        if ( p == 0 || node == 0 ) {
            break;
        }

        StrPair endTag;
        p = node->ParseDeep( p, &endTag );
        if ( !p ) {
            DELETE_NODE( node );
            node = 0;
            if ( !_document->Error() ) {
                _document->SetError( XML_ERROR_PARSING, 0, 0 );
            }
            break;
        }

        // We read the end tag. Return it to the parent.
        if ( node->ToElement() && node->ToElement()->ClosingType() == XMLElement::CLOSING ) {
            if ( parentEnd ) {
                *parentEnd = static_cast<XMLElement*>(node)->_value;
            }
			node->_memPool->SetTracked();	// created and then immediately deleted.
            DELETE_NODE( node );
            return p;
        }

        // Handle an end tag returned to this level.
        // And handle a bunch of annoying errors.
        XMLElement* ele = node->ToElement();
        if ( ele ) {
            if ( endTag.Empty() && ele->ClosingType() == XMLElement::OPEN ) {
                _document->SetError( XML_ERROR_MISMATCHED_ELEMENT, node->Value(), 0 );
                p = 0;
            }
            else if ( !endTag.Empty() && ele->ClosingType() != XMLElement::OPEN ) {
                _document->SetError( XML_ERROR_MISMATCHED_ELEMENT, node->Value(), 0 );
                p = 0;
            }
            else if ( !endTag.Empty() ) {
                if ( !XMLUtil::StringEqual( endTag.GetStr(), node->Value() )) {
                    _document->SetError( XML_ERROR_MISMATCHED_ELEMENT, node->Value(), 0 );
                    p = 0;
                }
            }
        }
        if ( p == 0 ) {
            DELETE_NODE( node );
            node = 0;
        }
        if ( node ) {
            this->InsertEndChild( node );
        }
    }
    return 0;
}

// --------- XMLText ---------- //
char* XMLText::ParseDeep( char* p, StrPair* )
{
    const char* start = p;
    if ( this->CData() ) {
        p = _value.ParseText( p, "]]>", StrPair::NEEDS_NEWLINE_NORMALIZATION );
        if ( !p ) {
            _document->SetError( XML_ERROR_PARSING_CDATA, start, 0 );
        }
        return p;
    }
    else {
        int flags = _document->ProcessEntities() ? StrPair::TEXT_ELEMENT : StrPair::TEXT_ELEMENT_LEAVE_ENTITIES;
        if ( _document->WhitespaceMode() == COLLAPSE_WHITESPACE ) {
            flags |= StrPair::COLLAPSE_WHITESPACE;
        }

        p = _value.ParseText( p, "<", flags );
        if ( !p ) {
            _document->SetError( XML_ERROR_PARSING_TEXT, start, 0 );
        }
        if ( p && *p ) {
            return p-1;
        }
    }
    return 0;
}


XMLNode* XMLText::ShallowClone( MyXMLDocument* doc ) const
{
    if ( !doc ) {
        doc = _document;
    }
    XMLText* text = doc->NewText( Value() );	// fixme: this will always allocate memory. Intern?
    text->SetCData( this->CData() );
    return text;
}


bool XMLText::ShallowEqual( const XMLNode* compare ) const
{
    return ( compare->ToText() && XMLUtil::StringEqual( compare->ToText()->Value(), Value() ));
}


bool XMLText::Accept( XMLVisitor* visitor ) const
{
    return visitor->Visit( *this );
}


// --------- XMLComment ---------- //

XMLComment::XMLComment( MyXMLDocument* doc ) : XMLNode( doc )
{
}


XMLComment::~XMLComment()
{
}


char* XMLComment::ParseDeep( char* p, StrPair* )
{
    // Comment parses as text.
    const char* start = p;
    p = _value.ParseText( p, "-->", StrPair::COMMENT );
    if ( p == 0 ) {
        _document->SetError( XML_ERROR_PARSING_COMMENT, start, 0 );
    }
    return p;
}


XMLNode* XMLComment::ShallowClone( MyXMLDocument* doc ) const
{
    if ( !doc ) {
        doc = _document;
    }
    XMLComment* comment = doc->NewComment( Value() );	// fixme: this will always allocate memory. Intern?
    return comment;
}


bool XMLComment::ShallowEqual( const XMLNode* compare ) const
{
    return ( compare->ToComment() && XMLUtil::StringEqual( compare->ToComment()->Value(), Value() ));
}


bool XMLComment::Accept( XMLVisitor* visitor ) const
{
    return visitor->Visit( *this );
}


// --------- XMLDeclaration ---------- //

XMLDeclaration::XMLDeclaration( MyXMLDocument* doc ) : XMLNode( doc )
{
}


XMLDeclaration::~XMLDeclaration()
{
    //printf( "~XMLDeclaration\n" );
}


char* XMLDeclaration::ParseDeep( char* p, StrPair* )
{
    // Declaration parses as text.
    const char* start = p;
    p = _value.ParseText( p, "?>", StrPair::NEEDS_NEWLINE_NORMALIZATION );
    if ( p == 0 ) {
        _document->SetError( XML_ERROR_PARSING_DECLARATION, start, 0 );
    }
    return p;
}


XMLNode* XMLDeclaration::ShallowClone( MyXMLDocument* doc ) const
{
    if ( !doc ) {
        doc = _document;
    }
    XMLDeclaration* dec = doc->NewDeclaration( Value() );	// fixme: this will always allocate memory. Intern?
    return dec;
}


bool XMLDeclaration::ShallowEqual( const XMLNode* compare ) const
{
    return ( compare->ToDeclaration() && XMLUtil::StringEqual( compare->ToDeclaration()->Value(), Value() ));
}



bool XMLDeclaration::Accept( XMLVisitor* visitor ) const
{
    return visitor->Visit( *this );
}

// --------- XMLUnknown ---------- //

XMLUnknown::XMLUnknown( MyXMLDocument* doc ) : XMLNode( doc )
{
}


XMLUnknown::~XMLUnknown()
{
}


char* XMLUnknown::ParseDeep( char* p, StrPair* )
{
    // Unknown parses as text.
    const char* start = p;

    p = _value.ParseText( p, ">", StrPair::NEEDS_NEWLINE_NORMALIZATION );
    if ( !p ) {
        _document->SetError( XML_ERROR_PARSING_UNKNOWN, start, 0 );
    }
    return p;
}


XMLNode* XMLUnknown::ShallowClone( MyXMLDocument* doc ) const
{
    if ( !doc ) {
        doc = _document;
    }
    XMLUnknown* text = doc->NewUnknown( Value() );	// fixme: this will always allocate memory. Intern?
    return text;
}


bool XMLUnknown::ShallowEqual( const XMLNode* compare ) const
{
    return ( compare->ToUnknown() && XMLUtil::StringEqual( compare->ToUnknown()->Value(), Value() ));
}


bool XMLUnknown::Accept( XMLVisitor* visitor ) const
{
    return visitor->Visit( *this );
}

// --------- XMLAttribute ---------- //
char* XMLAttribute::ParseDeep( char* p, bool processEntities )
{
    // Parse using the name rules: bug fix, was using ParseText before
    p = _name.ParseName( p );
    if ( !p || !*p ) {
        return 0;
    }

    // Skip white space before =
    p = XMLUtil::SkipWhiteSpace( p );
    if ( !p || *p != '=' ) {
        return 0;
    }

    ++p;	// move up to opening quote
    p = XMLUtil::SkipWhiteSpace( p );
    if ( *p != '\"' && *p != '\'' ) {
        return 0;
    }

    char endTag[2] = { *p, 0 };
    ++p;	// move past opening quote

    p = _value.ParseText( p, endTag, processEntities ? StrPair::ATTRIBUTE_VALUE : StrPair::ATTRIBUTE_VALUE_LEAVE_ENTITIES );
    return p;
}


void XMLAttribute::SetName( const char* n )
{
    _name.SetStr( n );
}


XMLError XMLAttribute::QueryIntValue( int* value ) const
{
    if ( XMLUtil::ToInt( Value(), value )) {
        return XML_NO_ERROR;
    }
    return XML_WRONG_ATTRIBUTE_TYPE;
}


XMLError XMLAttribute::QueryUnsignedValue( unsigned int* value ) const
{
    if ( XMLUtil::ToUnsigned( Value(), value )) {
        return XML_NO_ERROR;
    }
    return XML_WRONG_ATTRIBUTE_TYPE;
}


XMLError XMLAttribute::QueryBoolValue( bool* value ) const
{
    if ( XMLUtil::ToBool( Value(), value )) {
        return XML_NO_ERROR;
    }
    return XML_WRONG_ATTRIBUTE_TYPE;
}


XMLError XMLAttribute::QueryFloatValue( float* value ) const
{
    if ( XMLUtil::ToFloat( Value(), value )) {
        return XML_NO_ERROR;
    }
    return XML_WRONG_ATTRIBUTE_TYPE;
}


XMLError XMLAttribute::QueryDoubleValue( double* value ) const
{
    if ( XMLUtil::ToDouble( Value(), value )) {
        return XML_NO_ERROR;
    }
    return XML_WRONG_ATTRIBUTE_TYPE;
}


void XMLAttribute::SetAttribute( const char* v )
{
    _value.SetStr( v );
}


void XMLAttribute::SetAttribute( int v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    _value.SetStr( buf );
}


void XMLAttribute::SetAttribute( unsigned v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    _value.SetStr( buf );
}


void XMLAttribute::SetAttribute( bool v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    _value.SetStr( buf );
}

void XMLAttribute::SetAttribute( double v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    _value.SetStr( buf );
}

void XMLAttribute::SetAttribute( float v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    _value.SetStr( buf );
}


// --------- XMLElement ---------- //
XMLElement::XMLElement( MyXMLDocument* doc ) : XMLNode( doc ),
    _closingType( 0 ),
    _rootAttribute( 0 )
{
}


XMLElement::~XMLElement()
{
    while( _rootAttribute ) {
        XMLAttribute* next = _rootAttribute->_next;
        DELETE_ATTRIBUTE( _rootAttribute );
        _rootAttribute = next;
    }
}


XMLAttribute* XMLElement::FindAttribute( const char* name )
{
    XMLAttribute* a = 0;
    for( a=_rootAttribute; a; a = a->_next ) {
        if ( XMLUtil::StringEqual( a->Name(), name ) ) {
            return a;
        }
    }
    return 0;
}


const XMLAttribute* XMLElement::FindAttribute( const char* name ) const
{
    XMLAttribute* a = 0;
    for( a=_rootAttribute; a; a = a->_next ) {
        if ( XMLUtil::StringEqual( a->Name(), name ) ) {
            return a;
        }
    }
    return 0;
}


const char* XMLElement::Attribute( const char* name, const char* value ) const
{
    const XMLAttribute* a = FindAttribute( name );
    if ( !a ) {
        return 0;
    }
    if ( !value || XMLUtil::StringEqual( a->Value(), value )) {
        return a->Value();
    }
    return 0;
}


const char* XMLElement::GetText() const
{
    if ( FirstChild() && FirstChild()->ToText() ) {
        return FirstChild()->ToText()->Value();
    }
    return "";
}


XMLError XMLElement::QueryIntText( int* ival ) const
{
    if ( FirstChild() && FirstChild()->ToText() ) {
        const char* t = FirstChild()->ToText()->Value();
        if ( XMLUtil::ToInt( t, ival ) ) {
            return XML_SUCCESS;
        }
        return XML_CAN_NOT_CONVERT_TEXT;
    }
    return XML_NO_TEXT_NODE;
}


XMLError XMLElement::QueryUnsignedText( unsigned* uval ) const
{
    if ( FirstChild() && FirstChild()->ToText() ) {
        const char* t = FirstChild()->ToText()->Value();
        if ( XMLUtil::ToUnsigned( t, uval ) ) {
            return XML_SUCCESS;
        }
        return XML_CAN_NOT_CONVERT_TEXT;
    }
    return XML_NO_TEXT_NODE;
}


XMLError XMLElement::QueryBoolText( bool* bval ) const
{
    if ( FirstChild() && FirstChild()->ToText() ) {
        const char* t = FirstChild()->ToText()->Value();
        if ( XMLUtil::ToBool( t, bval ) ) {
            return XML_SUCCESS;
        }
        return XML_CAN_NOT_CONVERT_TEXT;
    }
    return XML_NO_TEXT_NODE;
}


XMLError XMLElement::QueryDoubleText( double* dval ) const
{
    if ( FirstChild() && FirstChild()->ToText() ) {
        const char* t = FirstChild()->ToText()->Value();
        if ( XMLUtil::ToDouble( t, dval ) ) {
            return XML_SUCCESS;
        }
        return XML_CAN_NOT_CONVERT_TEXT;
    }
    return XML_NO_TEXT_NODE;
}


XMLError XMLElement::QueryFloatText( float* fval ) const
{
    if ( FirstChild() && FirstChild()->ToText() ) {
        const char* t = FirstChild()->ToText()->Value();
        if ( XMLUtil::ToFloat( t, fval ) ) {
            return XML_SUCCESS;
        }
        return XML_CAN_NOT_CONVERT_TEXT;
    }
    return XML_NO_TEXT_NODE;
}



XMLAttribute* XMLElement::FindOrCreateAttribute( const char* name )
{
    XMLAttribute* last = 0;
    XMLAttribute* attrib = 0;
    for( attrib = _rootAttribute;
            attrib;
            last = attrib, attrib = attrib->_next ) {
        if ( XMLUtil::StringEqual( attrib->Name(), name ) ) {
            break;
        }
    }
    if ( !attrib ) {
        attrib = new (_document->_attributePool.Alloc() ) XMLAttribute();
        attrib->_memPool = &_document->_attributePool;
        if ( last ) {
            last->_next = attrib;
        }
        else {
            _rootAttribute = attrib;
        }
        attrib->SetName( name );
        attrib->_memPool->SetTracked(); // always created and linked.
    }
    return attrib;
}


void XMLElement::DeleteAttribute( const char* name )
{
    XMLAttribute* prev = 0;
    for( XMLAttribute* a=_rootAttribute; a; a=a->_next ) {
        if ( XMLUtil::StringEqual( name, a->Name() ) ) {
            if ( prev ) {
                prev->_next = a->_next;
            }
            else {
                _rootAttribute = a->_next;
            }
            DELETE_ATTRIBUTE( a );
            break;
        }
        prev = a;
    }
}


char* XMLElement::ParseAttributes( char* p )
{
    const char* start = p;
    XMLAttribute* prevAttribute = 0;

    // Read the attributes.
    while( p ) {
        p = XMLUtil::SkipWhiteSpace( p );
        if ( !p || !(*p) ) {
            _document->SetError( XML_ERROR_PARSING_ELEMENT, start, Name() );
            return 0;
        }

        // attribute.
        if (XMLUtil::IsNameStartChar( *p ) ) {
            XMLAttribute* attrib = new (_document->_attributePool.Alloc() ) XMLAttribute();
            attrib->_memPool = &_document->_attributePool;
			attrib->_memPool->SetTracked();

            p = attrib->ParseDeep( p, _document->ProcessEntities() );
            if ( !p || Attribute( attrib->Name() ) ) {
                DELETE_ATTRIBUTE( attrib );
                _document->SetError( XML_ERROR_PARSING_ATTRIBUTE, start, p );
                return 0;
            }
            // There is a minor bug here: if the attribute in the source xml
            // document is duplicated, it will not be detected and the
            // attribute will be doubly added. However, tracking the 'prevAttribute'
            // avoids re-scanning the attribute list. Preferring performance for
            // now, may reconsider in the future.
            if ( prevAttribute ) {
                prevAttribute->_next = attrib;
            }
            else {
                _rootAttribute = attrib;
            }
            prevAttribute = attrib;
        }
        // end of the tag
        else if ( *p == '/' && *(p+1) == '>' ) {
            _closingType = CLOSED;
            return p+2;	// done; sealed element.
        }
        // end of the tag
        else if ( *p == '>' ) {
            ++p;
            break;
        }
        else {
            _document->SetError( XML_ERROR_PARSING_ELEMENT, start, p );
            return 0;
        }
    }
    return p;
}


//
//	<ele></ele>
//	<ele>foo<b>bar</b></ele>
//
char* XMLElement::ParseDeep( char* p, StrPair* strPair )
{
    // Read the element name.
    p = XMLUtil::SkipWhiteSpace( p );
    if ( !p ) {
        return 0;
    }

    // The closing element is the </element> form. It is
    // parsed just like a regular element then deleted from
    // the DOM.
    if ( *p == '/' ) {
        _closingType = CLOSING;
        ++p;
    }

    p = _value.ParseName( p );
    if ( _value.Empty() ) {
        return 0;
    }

    p = ParseAttributes( p );
    if ( !p || !*p || _closingType ) {
        return p;
    }

    p = XMLNode::ParseDeep( p, strPair );
    return p;
}



XMLNode* XMLElement::ShallowClone( MyXMLDocument* doc ) const
{
    if ( !doc ) {
        doc = _document;
    }
    XMLElement* element = doc->NewElement( Value() );					// fixme: this will always allocate memory. Intern?
    for( const XMLAttribute* a=FirstAttribute(); a; a=a->Next() ) {
        element->SetAttribute( a->Name(), a->Value() );					// fixme: this will always allocate memory. Intern?
    }
    return element;
}


bool XMLElement::ShallowEqual( const XMLNode* compare ) const
{
    const XMLElement* other = compare->ToElement();
    if ( other && XMLUtil::StringEqual( other->Value(), Value() )) {

        const XMLAttribute* a=FirstAttribute();
        const XMLAttribute* b=other->FirstAttribute();

        while ( a && b ) {
            if ( !XMLUtil::StringEqual( a->Value(), b->Value() ) ) {
                return false;
            }
            a = a->Next();
            b = b->Next();
        }
        if ( a || b ) {
            // different count
            return false;
        }
        return true;
    }
    return false;
}


bool XMLElement::Accept( XMLVisitor* visitor ) const
{
    if ( visitor->VisitEnter( *this, _rootAttribute ) ) {
        for ( const XMLNode* node=FirstChild(); node; node=node->NextSibling() ) {
            if ( !node->Accept( visitor ) ) {
                break;
            }
        }
    }
    return visitor->VisitExit( *this );
}


// --------- MyXMLDocument ----------- //
MyXMLDocument::MyXMLDocument( bool processEntities, Whitespace whitespace ) :
    XMLNode( 0 ),
    _writeBOM( false ),
    _processEntities( processEntities ),
    _errorID( XML_NO_ERROR ),
    _whitespace( whitespace ),
    _errorStr1( 0 ),
    _errorStr2( 0 ),
    _charBuffer( 0 )
{
    _document = this;	// avoid warning about 'this' in initializer list
}


MyXMLDocument::~MyXMLDocument()
{
    DeleteChildren();
    delete [] _charBuffer;

#if 0
    _textPool.Trace( "text" );
    _elementPool.Trace( "element" );
    _commentPool.Trace( "comment" );
    _attributePool.Trace( "attribute" );
#endif

#ifdef DEBUG
	if ( Error() == false ) {
		TIXMLASSERT( _elementPool.CurrentAllocs()   == _elementPool.Untracked() );
		TIXMLASSERT( _attributePool.CurrentAllocs() == _attributePool.Untracked() );
		TIXMLASSERT( _textPool.CurrentAllocs()      == _textPool.Untracked() );
		TIXMLASSERT( _commentPool.CurrentAllocs()   == _commentPool.Untracked() );
	}
#endif
}


void MyXMLDocument::Clear()
{
    DeleteChildren();

    _errorID = XML_NO_ERROR;
    _errorStr1 = 0;
    _errorStr2 = 0;

    delete [] _charBuffer;
    _charBuffer = 0;
}


XMLElement* MyXMLDocument::NewElement( const char* name )
{
    XMLElement* ele = new (_elementPool.Alloc()) XMLElement( this );
    ele->_memPool = &_elementPool;
    ele->SetName( name );
    return ele;
}


XMLComment* MyXMLDocument::NewComment( const char* str )
{
    XMLComment* comment = new (_commentPool.Alloc()) XMLComment( this );
    comment->_memPool = &_commentPool;
    comment->SetValue( str );
    return comment;
}


XMLText* MyXMLDocument::NewText( const char* str )
{
    XMLText* text = new (_textPool.Alloc()) XMLText( this );
    text->_memPool = &_textPool;
    text->SetValue( str );
    return text;
}


XMLDeclaration* MyXMLDocument::NewDeclaration( const char* str )
{
    XMLDeclaration* dec = new (_commentPool.Alloc()) XMLDeclaration( this );
    dec->_memPool = &_commentPool;
    dec->SetValue( str ? str : "xml version=\"1.0\" encoding=\"UTF-8\"" );
    return dec;
}


XMLUnknown* MyXMLDocument::NewUnknown( const char* str )
{
    XMLUnknown* unk = new (_commentPool.Alloc()) XMLUnknown( this );
    unk->_memPool = &_commentPool;
    unk->SetValue( str );
    return unk;
}


XMLError MyXMLDocument::LoadFile( const char* filename )
{
    Clear();
    FILE* fp = 0;

#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
    errno_t err = fopen_s(&fp, filename, "rb" );
    if ( !fp || err) {
#else
    fp = fopen( filename, "rb" );
    if ( !fp) {
#endif
        SetError( XML_ERROR_FILE_NOT_FOUND, filename, 0 );
        return _errorID;
    }
    LoadFile( fp );
    fclose( fp );
    return _errorID;
}


XMLError MyXMLDocument::LoadFile( FILE* fp )
{
    Clear();

    fseek( fp, 0, SEEK_END );
    size_t size = ftell( fp );
    fseek( fp, 0, SEEK_SET );

    if ( size == 0 ) {
        SetError( XML_ERROR_EMPTY_DOCUMENT, 0, 0 );
        return _errorID;
    }

    _charBuffer = new char[size+1];
    size_t read = fread( _charBuffer, 1, size, fp );
    if ( read != size ) {
        SetError( XML_ERROR_FILE_READ_ERROR, 0, 0 );
        return _errorID;
    }

    _charBuffer[size] = 0;

    const char* p = _charBuffer;
    p = XMLUtil::SkipWhiteSpace( p );
    p = XMLUtil::ReadBOM( p, &_writeBOM );
    if ( !p || !*p ) {
        SetError( XML_ERROR_EMPTY_DOCUMENT, 0, 0 );
        return _errorID;
    }

    ParseDeep( _charBuffer + (p-_charBuffer), 0 );
    return _errorID;
}


XMLError MyXMLDocument::SaveFile( const char* filename, bool compact )
{
    FILE* fp = 0;
#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
    errno_t err = fopen_s(&fp, filename, "w" );
    if ( !fp || err) {
#else
    fp = fopen( filename, "w" );
    if ( !fp) {
#endif
        SetError( XML_ERROR_FILE_COULD_NOT_BE_OPENED, filename, 0 );
        return _errorID;
    }
    SaveFile(fp, compact);
    fclose( fp );
    return _errorID;
}


XMLError MyXMLDocument::SaveFile( FILE* fp, bool compact )
{
    XMLPrinter stream( fp, compact );
    Print( &stream );
    return _errorID;
}


XMLError MyXMLDocument::Parse( const char* p, size_t len )
{
	const char* start = p;
    Clear();

    if ( !p || !*p ) {
        SetError( XML_ERROR_EMPTY_DOCUMENT, 0, 0 );
        return _errorID;
    }
    if ( len == (size_t)(-1) ) {
        len = strlen( p );
    }
    _charBuffer = new char[ len+1 ];
    memcpy( _charBuffer, p, len );
    _charBuffer[len] = 0;

    p = XMLUtil::SkipWhiteSpace( p );
    p = XMLUtil::ReadBOM( p, &_writeBOM );
    if ( !p || !*p ) {
        SetError( XML_ERROR_EMPTY_DOCUMENT, 0, 0 );
        return _errorID;
    }

    ptrdiff_t delta = p - start;	// skip initial whitespace, BOM, etc.
    ParseDeep( _charBuffer+delta, 0 );
    return _errorID;
}


void MyXMLDocument::Print( XMLPrinter* streamer ) const
{
    XMLPrinter stdStreamer( stdout );
    if ( !streamer ) {
        streamer = &stdStreamer;
    }
    Accept( streamer );
}


void MyXMLDocument::SetError( XMLError error, const char* str1, const char* str2 )
{
    _errorID = error;
    _errorStr1 = str1;
    _errorStr2 = str2;
}


void MyXMLDocument::PrintError() const
{
    if ( _errorID ) {
        static const int LEN = 20;
        char buf1[LEN] = { 0 };
        char buf2[LEN] = { 0 };

        if ( _errorStr1 ) {
            TIXML_SNPRINTF( buf1, LEN, "%s", _errorStr1 );
        }
        if ( _errorStr2 ) {
            TIXML_SNPRINTF( buf2, LEN, "%s", _errorStr2 );
        }

        printf( "MyXMLDocument error id=%d str1=%s str2=%s\n",
                _errorID, buf1, buf2 );
    }
}


XMLPrinter::XMLPrinter( FILE* file, bool compact, int depth ) :
    _elementJustOpened( false ),
    _firstElement( true ),
    _fp( file ),
    _depth( depth ),
    _textDepth( -1 ),
    _processEntities( true ),
    _compactMode( compact )
{
    for( int i=0; i<ENTITY_RANGE; ++i ) {
        _entityFlag[i] = false;
        _restrictedEntityFlag[i] = false;
    }
    for( int i=0; i<NUM_ENTITIES; ++i ) {
        TIXMLASSERT( entities[i].value < ENTITY_RANGE );
        if ( entities[i].value < ENTITY_RANGE ) {
            _entityFlag[ (int)entities[i].value ] = true;
        }
    }
    _restrictedEntityFlag[(int)'&'] = true;
    _restrictedEntityFlag[(int)'<'] = true;
    _restrictedEntityFlag[(int)'>'] = true;	// not required, but consistency is nice
    _buffer.Push( 0 );
}


void XMLPrinter::Print( const char* format, ... )
{
    va_list     va;
    va_start( va, format );

    if ( _fp ) {
        vfprintf( _fp, format, va );
    }
    else {
        // This seems brutally complex. Haven't figured out a better
        // way on windows.
#ifdef _MSC_VER
        int len = -1;
        int expand = 1000;
        while ( len < 0 ) {
            len = vsnprintf_s( _accumulator.Mem(), _accumulator.Capacity(), _TRUNCATE, format, va );
            if ( len < 0 ) {
                expand *= 3/2;
                _accumulator.PushArr( expand );
            }
        }
        char* p = _buffer.PushArr( len ) - 1;
        memcpy( p, _accumulator.Mem(), len+1 );
#else
        int len = vsnprintf( 0, 0, format, va );
        // Close out and re-start the va-args
        va_end( va );
        va_start( va, format );
        char* p = _buffer.PushArr( len ) - 1;
        vsnprintf( p, len+1, format, va );
#endif
    }
    va_end( va );
}


void XMLPrinter::PrintSpace( int depth )
{
    for( int i=0; i<depth; ++i ) {
        Print( "    " );
    }
}


void XMLPrinter::PrintString( const char* p, bool restricted )
{
    // Look for runs of bytes between entities to print.
    const char* q = p;
    const bool* flag = restricted ? _restrictedEntityFlag : _entityFlag;

    if ( _processEntities ) {
        while ( *q ) {
            // Remember, char is sometimes signed. (How many times has that bitten me?)
            if ( *q > 0 && *q < ENTITY_RANGE ) {
                // Check for entities. If one is found, flush
                // the stream up until the entity, write the
                // entity, and keep looking.
                if ( flag[(unsigned)(*q)] ) {
                    while ( p < q ) {
                        Print( "%c", *p );
                        ++p;
                    }
                    for( int i=0; i<NUM_ENTITIES; ++i ) {
                        if ( entities[i].value == *q ) {
                            Print( "&%s;", entities[i].pattern );
                            break;
                        }
                    }
                    ++p;
                }
            }
            ++q;
        }
    }
    // Flush the remaining string. This will be the entire
    // string if an entity wasn't found.
    if ( !_processEntities || (q-p > 0) ) {
        Print( "%s", p );
    }
}


void XMLPrinter::PushHeader( bool writeBOM, bool writeDec )
{
    if ( writeBOM ) {
        static const unsigned char bom[] = { TIXML_UTF_LEAD_0, TIXML_UTF_LEAD_1, TIXML_UTF_LEAD_2, 0 };
        Print( "%s", bom );
    }
    if ( writeDec ) {
        PushDeclaration( "xml version=\"1.0\"" );
    }
}


void XMLPrinter::OpenElement( const char* name )
{
    if ( _elementJustOpened ) {
        SealElement();
    }
    _stack.Push( name );

    if ( _textDepth < 0 && !_firstElement && !_compactMode ) {
        Print( "\n" );
    }
    if ( !_compactMode ) {
        PrintSpace( _depth );
    }

    Print( "<%s", name );
    _elementJustOpened = true;
    _firstElement = false;
    ++_depth;
}


void XMLPrinter::PushAttribute( const char* name, const char* value )
{
    TIXMLASSERT( _elementJustOpened );
    Print( " %s=\"", name );
    PrintString( value, false );
    Print( "\"" );
}


void XMLPrinter::PushAttribute( const char* name, int v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    PushAttribute( name, buf );
}


void XMLPrinter::PushAttribute( const char* name, unsigned v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    PushAttribute( name, buf );
}


void XMLPrinter::PushAttribute( const char* name, bool v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    PushAttribute( name, buf );
}


void XMLPrinter::PushAttribute( const char* name, double v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    PushAttribute( name, buf );
}


void XMLPrinter::CloseElement()
{
    --_depth;
    const char* name = _stack.Pop();

    if ( _elementJustOpened ) {
        Print( "/>" );
    }
    else {
        if ( _textDepth < 0 && !_compactMode) {
            Print( "\n" );
            PrintSpace( _depth );
        }
        Print( "</%s>", name );
    }

    if ( _textDepth == _depth ) {
        _textDepth = -1;
    }
    if ( _depth == 0 && !_compactMode) {
        Print( "\n" );
    }
    _elementJustOpened = false;
}


void XMLPrinter::SealElement()
{
    _elementJustOpened = false;
    Print( ">" );
}


void XMLPrinter::PushText( const char* text, bool cdata )
{
    _textDepth = _depth-1;

    if ( _elementJustOpened ) {
        SealElement();
    }
    if ( cdata ) {
        Print( "<![CDATA[" );
        Print( "%s", text );
        Print( "]]>" );
    }
    else {
        PrintString( text, true );
    }
}

void XMLPrinter::PushText( int value )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( value, buf, BUF_SIZE );
    PushText( buf, false );
}


void XMLPrinter::PushText( unsigned value )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( value, buf, BUF_SIZE );
    PushText( buf, false );
}


void XMLPrinter::PushText( bool value )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( value, buf, BUF_SIZE );
    PushText( buf, false );
}


void XMLPrinter::PushText( float value )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( value, buf, BUF_SIZE );
    PushText( buf, false );
}


void XMLPrinter::PushText( double value )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( value, buf, BUF_SIZE );
    PushText( buf, false );
}


void XMLPrinter::PushComment( const char* comment )
{
    if ( _elementJustOpened ) {
        SealElement();
    }
    if ( _textDepth < 0 && !_firstElement && !_compactMode) {
        Print( "\n" );
        PrintSpace( _depth );
    }
    _firstElement = false;
    Print( "<!--%s-->", comment );
}


void XMLPrinter::PushDeclaration( const char* value )
{
    if ( _elementJustOpened ) {
        SealElement();
    }
    if ( _textDepth < 0 && !_firstElement && !_compactMode) {
        Print( "\n" );
        PrintSpace( _depth );
    }
    _firstElement = false;
    Print( "<?%s?>", value );
}


void XMLPrinter::PushUnknown( const char* value )
{
    if ( _elementJustOpened ) {
        SealElement();
    }
    if ( _textDepth < 0 && !_firstElement && !_compactMode) {
        Print( "\n" );
        PrintSpace( _depth );
    }
    _firstElement = false;
    Print( "<!%s>", value );
}


bool XMLPrinter::VisitEnter( const MyXMLDocument& doc )
{
    _processEntities = doc.ProcessEntities();
    if ( doc.HasBOM() ) {
        PushHeader( true, false );
    }
    return true;
}


bool XMLPrinter::VisitEnter( const XMLElement& element, const XMLAttribute* attribute )
{
    OpenElement( element.Name() );
    while ( attribute ) {
        PushAttribute( attribute->Name(), attribute->Value() );
        attribute = attribute->Next();
    }
    return true;
}


bool XMLPrinter::VisitExit( const XMLElement& )
{
    CloseElement();
    return true;
}


bool XMLPrinter::Visit( const XMLText& text )
{
    PushText( text.Value(), text.CData() );
    return true;
}


bool XMLPrinter::Visit( const XMLComment& comment )
{
    PushComment( comment.Value() );
    return true;
}

bool XMLPrinter::Visit( const XMLDeclaration& declaration )
{
    PushDeclaration( declaration.Value() );
    return true;
}


bool XMLPrinter::Visit( const XMLUnknown& unknown )
{
    PushUnknown( unknown.Value() );
    return true;
}

}   // namespace tinyxml2


//------------------------------------------------------------------------------------------------------
//	构造函数
//------------------------------------------------------------------------------------------------------
CMyFile_XML::CMyFile_XML(void)
{
	v_sDev_Name="";														//网元名称
}

//------------------------------------------------------------------------------------------------------
//	析构函数
//------------------------------------------------------------------------------------------------------
CMyFile_XML::~CMyFile_XML(void)
{
	File_Reset();														//变量复位
}

//------------------------------------------------------------------------------------------------------
//	函数功能：XML文件内容读取【配置文件】
//	参    数：v_sFile_Name：文件名称；
//	输    出：v_mObjType容器
//	返    回：true：有效文件；	false：无效文件；
//------------------------------------------------------------------------------------------------------
bool CMyFile_XML::Read_XML(string v_sFile_Name)
{
	XMLElement* measCollecFile;											//指针：measCollecFile
	XMLElement* measData;												//指针：measData
	XMLElement* measInfo;												//指针：measInfo
	XMLElement* measType=NULL;											//指针：measType
	XMLElement* measValue=NULL;											//指针：measValue
	XMLElement* r=NULL;													//指针：r

	mString_String	v_mCounter;											//实例化：counter	[容器map]
	list_Object	v_lObject;												//实例化：Object	[链表]
	File_Object v_cObject;												//实例化：Object	[结构]
	CString		v_sType_Name,v_sR_Value;								//临时变量
	CString		v_sObject,v_sObject_DN;									//临时变量
	mString_String v_mName;												//临时变量

	//清除变量，为读取文件做好准备
	File_Reset();														//变量复位

	//文件读取
	MyXMLDocument* doc = new MyXMLDocument();							//XML实例化
	doc->LoadFile(v_sFile_Name.c_str());								//文件读取	[全部读取到内存中]

	//获得：网元名称
	measCollecFile= doc->FirstChildElement("NrmFile");					//查询：NrmFile 
	if (measCollecFile==NULL)
		return  false;

	//获取：ObjType\Objject
	measData= measCollecFile->FirstChildElement("Objects");				//查询：Objects
	if (measData==NULL)
		return  false;
	measInfo=measData->FirstChildElement("ObjectType");					//查询：ObjectType
	if (measData==NULL)
		return  false;
	v_sDev_Name=measInfo->GetText();									//文件信息

	//获得：FieldName
	measInfo = measInfo->NextSiblingElement("FieldName");				//查询：FieldName
	if(measInfo==NULL)
		return false;
	r=measInfo->FirstChildElement("N");									//查询：N
	while(r!=NULL)														//
	{
		v_sType_Name=r->Attribute("i");
		v_sR_Value=r->GetText();										//获得：Counter值
		v_mName.insert(pair<CString,CString> (v_sType_Name,v_sR_Value));	//增加	[容器：v_mName]
		r=r->NextSiblingElement("N");									//获得：下一个r
	}

	measInfo = measInfo->NextSiblingElement("FieldValue");				//查询：FieldValue
	if(measInfo==NULL)
		return false;
	measValue=measInfo->FirstChildElement("Cm");						//查询：Cm
	while(measValue!=NULL)
	{
		v_sObject=measValue->Attribute("UserLabel");					//DM属性
		ConvertUtf8ToGBK(v_sObject);									//格式转换
		v_sObject_DN=measValue->Attribute("Dn");						//DM属性
		r=measValue->FirstChildElement("V");							//查询：V
		while(r!=NULL)													//
		{
			v_sType_Name=r->Attribute("i");
			v_sR_Value=r->GetText();									//获得：Counter值
			ConvertUtf8ToGBK(v_sR_Value);								//格式转换
			mString_String::iterator v_pName=v_mName.find(v_sType_Name);//查找名称
			if (v_pName!=v_mName.end())
				v_mCounter.insert(pair<CString,CString> (v_pName->second,v_sR_Value));	//增加	[容器：v_mCounter]
			r=r->NextSiblingElement("V");								//获得：下一个r
		}
		v_cObject.v_mCounter = v_mCounter;								//Counter
		v_cObject.v_sObject_ID =v_sObject;								//Object
		v_cObject.v_sObject_DN=v_sObject_DN;							//Object_DN
		v_lObject.push_back(v_cObject);									//增加Object
		v_mCounter.clear();												//复位：v_mCounter [容器：Counter]
		measValue=measValue->NextSiblingElement("Cm");					//获得：下一个Cm
	}
	v_mObjType.insert(pair<CString,list_Object> (v_sDev_Name,v_lObject));//增加：[容器：v_mObjType]
	
	//清除内存空间
	list_Object::iterator v_pIterator_Object=v_lObject.begin();	
	while(v_pIterator_Object!=v_lObject.end())
	{
		v_pIterator_Object->v_mCounter.clear();							//Counter容器：清除
		v_pIterator_Object++;
	}
	v_lObject.clear();													//复位：v_lObject [链表：Object]
	v_mName.clear();													//复位：v_mName [容器：v_mName]

	delete doc;
	doc = NULL;

	return true;														//有效返回
}

//------------------------------------------------------------------------------------------------------
//	函数功能：XML文件内容读取【性能文件】
//	参    数：v_sFile_Name：文件名称；
//	输    出：v_mObjType容器
//	返    回：true：有效文件；	false：无效文件；
//------------------------------------------------------------------------------------------------------
bool CMyFile_XML::Read_XML_PM(string v_sFile_Name)
{
	XMLElement* measCollecFile;											//指针：measCollecFile
	XMLElement* measData;												//指针：measData
	XMLElement* measInfo;												//指针：measInfo
	XMLElement* measType=NULL;											//指针：measType
	XMLElement* measValue=NULL;											//指针：measValue
	XMLElement* r=NULL;													//指针：r

	mString_String	v_mCounter;											//实例化：counter	[容器map]
	list_Object	v_lObject;												//实例化：Object	[链表]
	File_Object v_cObject;												//实例化：Object	[结构]
	CString		v_sType_Name,v_sR_Value,v_sTemp;						//临时变量
	CString		v_sObject,v_sObject_DN;									//临时变量
	mString_String v_mName;												//临时变量
	int			v_iNum=1;												//临时变量

	//清除变量，为读取文件做好准备
	File_Reset();														//变量复位

	//文件读取
	MyXMLDocument* doc = new MyXMLDocument();							//XML实例化
	doc->LoadFile(v_sFile_Name.c_str());								//文件读取	[全部读取到内存中]

	//获得：网元名称
	measCollecFile= doc->FirstChildElement("PmFile");					//查询：PmFile 
	if (measCollecFile==NULL)
		return  false;

	//获取：ObjType\Objject
	measData= measCollecFile->FirstChildElement("Measurements");		//查询：Measurements
	while(measData!=NULL)
	{
		measInfo=measData->FirstChildElement("ObjectType");					//查询：ObjectType
		if (measData==NULL)
			return  false;
		v_sDev_Name=measInfo->GetText();									//文件信息

		//获得：FieldName
		measInfo = measInfo->NextSiblingElement("PmName");					//查询：PmName
		if(measInfo==NULL)
			return false;
		r=measInfo->FirstChildElement("N");									//查询：N
		while(r!=NULL)														//
		{
			v_sType_Name=r->Attribute("i");
			v_sR_Value=r->GetText();										//获得：Counter值
			v_mName.insert(pair<CString,CString>(v_sType_Name,v_sR_Value));	//增加	[容器：v_mName]
			r=r->NextSiblingElement("N");									//获得：下一个r
		}

		measInfo = measInfo->NextSiblingElement("PmData");					//查询：PmData
		if(measInfo==NULL)
			return false;
		measValue=measInfo->FirstChildElement("Pm");						//查询：Pm
		while(measValue!=NULL)
		{
			v_sObject=measValue->Attribute("UserLabel");					//DM属性
			ConvertUtf8ToGBK(v_sObject);									//格式转换
			v_sObject_DN=measValue->Attribute("Dn");						//DM属性
			r=measValue->FirstChildElement("V");							//查询：V
			while(r!=NULL)													//
			{
				v_sType_Name=r->Attribute("i");
				v_sR_Value=r->GetText();									//获得：Counter值
				ConvertUtf8ToGBK(v_sR_Value);								//格式转换
				mString_String::iterator v_pName=v_mName.find(v_sType_Name);//查找名称
				if (v_pName!=v_mName.end())
					v_mCounter.insert(pair<CString,CString> (v_pName->second,v_sR_Value));	//增加	[容器：v_mCounter]
				r=r->NextSiblingElement("V");								//获得：下一个r
			}
			v_cObject.v_mCounter = v_mCounter;								//Counter
			v_cObject.v_sObject_ID =v_sObject;								//Object
			v_cObject.v_sObject_DN=v_sObject_DN;							//Object_DN
			v_lObject.push_back(v_cObject);									//增加Object
			v_mCounter.clear();												//复位：v_mCounter [容器：Counter]
			measValue=measValue->NextSiblingElement("Pm");					//获得：下一个Pm
		}
		v_sTemp.Format("%02d",v_iNum);										//字符串
		v_mObjType.insert(pair<CString,list_Object> (v_sTemp+"_"+v_sDev_Name,v_lObject));	//增加：[容器：v_mObjType]

		v_iNum++;														//序号
		//清除内存空间
		list_Object::iterator v_pIterator_Object=v_lObject.begin();	
		while(v_pIterator_Object!=v_lObject.end())
		{
			v_pIterator_Object->v_mCounter.clear();						//Counter容器：清除
			v_pIterator_Object++;
		}
		v_lObject.clear();												//复位：v_lObject [链表：Object]
		v_mName.clear();												//复位：v_mName [容器：v_mName]
		measData=measData->NextSiblingElement("Measurements");			//获得：下一个Measurements
	}

	delete doc;
	doc = NULL;

	return true;														//有效返回
}

//------------------------------------------------------------------------------------------------------
//	函数功能：XML文件内容读取【性能文件，直接读取】
//	参    数：v_sFile_Name：文件名称；
//	输    出：v_mObjType容器
//	返    回：true：有效文件；	false：无效文件；
//------------------------------------------------------------------------------------------------------
bool CMyFile_XML::Read_XML_PM_Dir(CString v_sFile_Name)
{
	mString_String	v_mCounter;											//实例化：counter	[容器map]
	File_Object		v_cObject;											//实例化：Object	[结构]
	CString			v_sTemp;											//临时变量
	int				v_iNum=0;											//临时变量

	//清除变量，为读取文件做好准备
	File_Reset();														//变量复位

	//文件读取
	if(!Read_XML_PM_Open(v_sFile_Name))									//打开文件
		return false;													//返回

	//获取：ObjType\Objject
	while(Read_XML_PM_Element("<Measurements>"))						//查询：Measurements
	{
		v_sDev_Name=Read_XML_PM_GetText("ObjectType");					//文件信息

		//容器复位：列名称
		v_mName.clear();												//复位：v_mName [容器：v_mName]
		//获得：FieldName
		v_sType_Name="";												//列序号
		v_sR_Value="";													//列名称
		Read_XML_PM_Element("<PmName>");								//查询：主元素【PmName】
		v_sElement_Key="</PmName>";										//获取：主元素
		while(Read_XML_PM_Col())										//获得：列信息
		{
			v_mName.insert(pair<CString,CString>(v_sType_Name,v_sR_Value));	//增加	[容器：v_mName]
		}

		//获取：网元信息
		Read_XML_PM_Element("<PmData>");								//查询：主元素【PmName】
		while(Read_XML_PM_GetKey("Pm"))									//获得：行主键
		{
			v_sElement_Key="</Pm>";										//获取：主元素
			while(Read_XML_PM_Row())									//获得：行信息
			{
				if (!v_sType_Name.IsEmpty())
				{
					mString_String::iterator v_pName=v_mName.find(v_sType_Name);				//查找名称
					if (v_pName!=v_mName.end())
						v_mCounter.insert(pair<CString,CString> (v_pName->second,v_sR_Value));	//增加	[容器：v_mCounter]
				}
			}
			v_cObject.v_mCounter = v_mCounter;							//Counter
			v_cObject.v_sObject_ID=v_sObject;							//Object
			v_cObject.v_sObject_DN=v_sObject_DN;						//Object_DN

			//剔除：抚顺市的小区【中兴文件里包含大连、抚顺小区信息】【问题：若大连市小区以抚顺开头，如何处理？？？】
			if (v_sObject.Left(4)!="抚顺")								//为抚顺小区？？？							
				v_cObject_Child.v_lObject.push_back(v_cObject);			//增加Object

			v_mCounter.clear();											//复位：v_mCounter [容器：Counter]
		}
		v_iNum++;
		v_sTemp.Format("%d",v_iNum);									//序号
		v_mObjType_Child.insert(pair<CString,File_Object_Child> (v_sTemp+"_"+v_sDev_Name,v_cObject_Child));	//增加：【容器：v_mObjType_Child】

		//清除内存空间
		//清除：v_lObject
		list_Object::iterator v_pIterator_Object=v_cObject_Child.v_lObject.begin();	
		while(v_pIterator_Object!=v_cObject_Child.v_lObject.end())
		{
			v_pIterator_Object->v_mCounter.clear();						//Counter容器：清除
			v_pIterator_Object++;
		}
		v_cObject_Child.v_lObject.clear();								//复位：v_lObject [链表：Object]

		//清除：v_mChild
		map_Child::iterator			v_pChild;							//迭代器：map_Child
		map_C_Data::iterator		v_pChild_Data;						//迭代器：map_Child_Data
		v_pChild=v_cObject_Child.v_mChild.begin();
		while(v_pChild!=v_cObject_Child.v_mChild.end())
		{
			if (v_pChild->second.size()!=0)
			{
				v_pChild_Data=v_pChild->second.begin();
				while(v_pChild_Data!=v_pChild->second.end())
				{
					v_pChild_Data->second.clear();						//容器清除：v_mCounter【Counter】
					v_pChild_Data++;									//迭代器++
				}
				v_pChild->second.clear();								//容器清除：v_mChild_Data【Cell】
			}
			v_pChild++;													//迭代器++
		}
		v_cObject_Child.v_mChild.clear();								//容器清除：v_mChild【子项】
	}

	v_fRead.Close();													//关闭文件
	return true;														//有效返回
}

//------------------------------------------------------------------------------------------------------
//	函数功能：打开文件
//	参    数：v_sFile_Name：文件名称；
//	返    回：true：有效文件；	false：无效文件；
//------------------------------------------------------------------------------------------------------
bool CMyFile_XML::Read_XML_PM_Open(CString v_sFile_Name)
{
	return v_fRead.Open(_T(v_sFile_Name),CFile::modeRead);				//打开文件，并返回
}

//------------------------------------------------------------------------------------------------------
//	函数功能：查找【主元素】
//	参    数：v_sElement：元素名称；
//	返    回：true：找到；	false：没找到；
//------------------------------------------------------------------------------------------------------
bool CMyFile_XML::Read_XML_PM_Element(CString v_sElement)
{
	CString		v_sStr;													//临时变量

	while(v_fRead.ReadString(v_sStr))									//数据读取：一行
	{
		int	v_iLen=v_sStr.Find(v_sElement,0);							//查找：元素
		if (v_iLen>=0)													//找到？？？
			return true;												//返回：找到
	}
	return false;														//返回：没找到
}

//------------------------------------------------------------------------------------------------------
//	函数功能：获取【元素值】
//	参    数：v_sElement_Key：元素名称；
//	返    回：true：找到；	false：没找到；
//------------------------------------------------------------------------------------------------------
CString CMyFile_XML::Read_XML_PM_GetText(CString v_sElement)
{
	CString		v_sStr;													//临时变量
	int			v_iLen;													//临时变量

	v_sElement="<"+v_sElement+">";										//完整
	while(v_fRead.ReadString(v_sStr))									//数据读取：一行
	{
		v_iLen=v_sStr.Find(v_sElement,0);								//查找：元素
		if (v_iLen>=0)													//找到？？？
		{
			v_sStr=v_sStr.Mid(v_iLen+v_sElement.GetLength());			//截取
			v_iLen=v_sStr.Find("<",0);									//查找："<"
			if (v_iLen>=0)												//找到？？？
				v_sStr=v_sStr.Left(v_iLen);								//截取
			return v_sStr;												//返回：元素值
		}
	}
	return "";															//返回：没找到
}

//------------------------------------------------------------------------------------------------------
//	函数功能：获得：行主键
//	公共参数：v_sObject：行主键；	v_sObject_DN：Dn；
//	参    数：v_sElement：元素名称【查询】
//	返    回：true：找到；	false：没找到；
//------------------------------------------------------------------------------------------------------
bool CMyFile_XML::Read_XML_PM_GetKey(CString v_sElement)
{
	CString		v_sStr;													//临时变量
	int			v_iLen;													//临时变量

	//查找：v_sElement
	v_sElement="<"+v_sElement;											//完整
	while(v_fRead.ReadString(v_sStr))									//数据读取：一行
	{
		//结束？？？
		v_iLen=v_sStr.Find("</PmData>",0);								//查找：主元素？？？
		if (v_iLen>=0)													//找到？？？
			return false;												//结束：列名称
		v_iLen=v_sStr.Find(v_sElement,0);								//查找：元素
		if (v_iLen>=0)													//找到？？？
		{
			//查找：Dn
			v_sObject_DN="";											//默认
			v_iLen=v_sStr.Find("Dn=\"",0);								//查找：Dn
			if (v_iLen>=0)												//找到？？？
			{
				v_sStr=v_sStr.Mid(v_iLen+4);							//截取
				v_iLen=v_sStr.Find("\"",0);								//查找："""
				if (v_iLen>=0)											//找到？？？
					v_sObject_DN=v_sStr.Left(v_iLen);					//截取
			}
			//查找：行主键
			v_sObject="";												//默认
			v_iLen=v_sStr.Find("UserLabel=\"",0);						//查找：Dn
			if (v_iLen>=0)												//找到？？？
			{
				v_sStr=v_sStr.Mid(v_iLen+11);							//截取
				v_iLen=v_sStr.Find("\"",0);								//查找："""
				if (v_iLen>=0)											//找到？？？
				{
					v_sObject=v_sStr.Left(v_iLen);						//截取
					ConvertUtf8ToGBK(v_sObject);						//格式转换
				}
			}
			return true;												//返回：获取
		}
	}
	return false;														//返回：没找到
}

//------------------------------------------------------------------------------------------------------
//	函数功能：获取【列信息】
//	公共参数：v_sType_Name：列序号；	v_sR_Value：列名称；
//	公共参数：v_sElement：元素名称【结束查询】
//	返    回：true：找到；	false：没找到；
//------------------------------------------------------------------------------------------------------
bool CMyFile_XML::Read_XML_PM_Col()
{
	CString		v_sStr;													//临时变量
	int			v_iLen;													//临时变量

	if(v_fRead.ReadString(v_sStr))										//数据读取：一行
	{
		//结束？？？
		v_iLen=v_sStr.Find(v_sElement_Key,0);							//查找：主元素？？？
		if (v_iLen>=0)													//找到？？？
			return false;												//结束：列名称
		//获取：参数【<N i="1">PHY.CellMaxTxPower</N>】
		v_iLen=v_sStr.Find("=\"",0);									//查找："=""
		if (v_iLen>=0)													//找到？？？
		{
			v_sStr=v_sStr.Mid(v_iLen+2);								//截取
			v_iLen=v_sStr.Find("\"",0);									//查找："""
			if (v_iLen>=0)												//找到？？？
			{
				v_sType_Name=v_sStr.Left(v_iLen);						//截取：列序号
				v_iLen=v_sStr.Find(">",0);								//查找：">"
				if (v_iLen>=0)											//找到？？？
				{
					v_sStr=v_sStr.Mid(v_iLen+1);						//截取
					v_iLen=v_sStr.Find("</N>",0);						//查找："</N>"
					if (v_iLen>=0)										//找到？？？
					{
						v_sR_Value=v_sStr.Left(v_iLen);					//截取：列序号
						return true;
					}
				}
			}
		}
	}
	return false;														//返回：没找到
}


//------------------------------------------------------------------------------------------------------
//	函数功能：获取【行信息】
//	公共参数：v_sType_Name：列序号；	v_sR_Value：列元素值；
//	公共参数：v_sElement_Key：元素名称【结束查询】
//	返    回：true：找到；	false：没找到；
//	工    作：(1)取数：【	<V i="1">3.72</V>】，得：3.72
//			  (2)求和：【	<CV i="7">
//							<SN>CONTEXT.AttRelEnb.HOFailure</SN>
//							<SV>1</SV>
//							<SN>CONTEXT.AttRelEnb.RNL</SN>
//							<SV>2</SV>
//							<SN>CONTEXT.AttRelEnb.UeLost</SN>
//							<SV>3</SV>		
//							</CV>			】
//					得：6
//------------------------------------------------------------------------------------------------------
bool CMyFile_XML::Read_XML_PM_Row()
{
	CString		v_sStr;													//临时变量
	int			v_iLen;													//临时变量

	if(v_fRead.ReadString(v_sStr))										//数据读取：一行
	{
		//结束？？？
		v_iLen=v_sStr.Find(v_sElement_Key,0);							//查找：主元素？？？
		if (v_iLen>=0)													//找到？？？
			return false;												//结束：列名称
		//获取：参数【<V i="1">3.72</V>】
		v_iLen=v_sStr.Find("=\"",0);									//查找：【="】
		if (v_iLen>=0)													//找到？？？
		{
			v_sStr=v_sStr.Mid(v_iLen+2);								//截取
			v_iLen=v_sStr.Find("\"",0);									//查找：【"】
			if (v_iLen>=0)												//找到？？？
			{
				v_sType_Name=v_sStr.Left(v_iLen);						//截取：列序号
				v_sStr=v_sStr.Mid(v_iLen);								//余留【">3.72</V> 或 "> 】
				if (v_sStr.GetLength()>3)								//获取：值
				{
					v_iLen=v_sStr.Find(">",0);							//查找：">"
					if (v_iLen>=0)										//找到？？？
					{
						v_sStr=v_sStr.Mid(v_iLen+1);					//截取
						v_iLen=v_sStr.Find("</V>",0);					//查找："</V>"
						if (v_iLen>=0)									//找到？？？
						{
							v_sR_Value=v_sStr.Left(v_iLen);				//截取：值
							return true;
						}
					}
				}
				else													//求子项和【多项】
				{
					v_sR_Value=Read_XML_PM_Row_Add("</CV>");			//求和：值
					return true;
				}
			}
		}
		v_sType_Name="";												//无效数据
		return true;													//返回：没找到
	}
	return false;														//返回：没找到
}

//------------------------------------------------------------------------------------------------------
//	函数功能：求子项和【把子项加入Child容器】
//	公共参数：v_sEnd：结束符；
//	工    作：求和：【	<CV i="7">
//							<SN>CONTEXT.AttRelEnb.HOFailure</SN>
//							<SV>1</SV>
//							<SN>CONTEXT.AttRelEnb.RNL</SN>
//							<SV>2</SV>
//							<SN>CONTEXT.AttRelEnb.UeLost</SN>
//							<SV>3</SV>		
//							</CV>			】
//					得：6
//------------------------------------------------------------------------------------------------------
CString CMyFile_XML::Read_XML_PM_Row_Add(CString v_sEnd)
{
	CString		v_sStr,v_sKey,v_sValue;									//临时变量
	int			v_iLen,v_iSum=0,v_iType=0;								//临时变量
	double		v_dSum=0,v_dValue;										//临时变量
	bool		v_bType=false;											//临时变量

	map_Child_Data				v_mChild_Data;							//容器：map_Child_Data
	map_Child_Data::iterator	v_pChild_Data;							//迭代器：map_Child_Data
	mString_String				v_mCounter;								//容器：mString_String
	map_Child::iterator			v_pChild;								//迭代器：v_mChild


	bool	v_bOK=false;												//保存有效标志
	if (!v_sType_Name.IsEmpty())										//有效：Counter序号？？？【2】
	{
		mString_String::iterator v_pName = v_mName.find(v_sType_Name);	//有效：Counter序号【2】
		if (v_pName!=v_mName.end())										//找到？？？
		{
			v_pChild = v_cObject_Child.v_mChild.find(v_pName->second);	//查找：列名称？？？
			if (v_pChild==v_cObject_Child.v_mChild.end())				//找到？？？
			{
				v_cObject_Child.v_mChild.insert(pair<CString,map_Child_Data>(v_pName->second,v_mChild_Data));	//增加容器
				v_pChild = v_cObject_Child.v_mChild.find(v_pName->second);		//查找：列名称？？？
			}
			if (v_sObject.Left(4)!="¸§Ë³")								//为抚顺小区？？？
			{
				v_pChild_Data=v_pChild->second.find(v_sObject);			//小区存在？？？
				if (v_pChild_Data==v_pChild->second.end())				//没找到？？
				{
					v_pChild->second.insert(pair<CString,mString_String>(v_sObject,v_mCounter));	//容器增加：小区
					v_pChild_Data=v_pChild->second.find(v_sObject);		//查找
				}
				v_bOK=true;												//置位：保存有效
			}
		}
	}

	while(v_fRead.ReadString(v_sStr))									//数据读取：一行
	{
		//½áÊø£¿£¿£¿
		v_iLen=v_sStr.Find(v_sEnd,0);									//查找：主元素？？？
		if (v_iLen>=0)													//找到？？？
			break;														//退出
		//»ñÈ¡£ºÁÐÃû³Æ×ÓÏî¡¾<SN>CONTEXT.AttRelEnb.RNL</SN>¡¿
		v_iLen=v_sStr.Find("<SN>",0);									//查找："<SN>"
		if (v_iLen>=0)													//找到？？？
		{
			v_sStr=v_sStr.Mid(v_iLen+4);								//截取
			v_iLen=v_sStr.Find("</SN>",0);								//查找："</SN>"
			if (v_iLen>=0)												//找到？？？
			{
				v_sKey = v_sStr.Left(v_iLen);							
				if (!v_bType)											
				{
					v_bType=true;										
					if(v_sKey.Find("Max")>=0)
					{
						v_iType=1;										
						v_dSum=-999999999;							
					}
					else if(v_sKey.Find("Mean")>=0)
						v_iType=2;										
				}
			}
		}
		else
		{																//获取：数值【<SV>2</SV>】
			v_iLen=v_sStr.Find("<SV>",0);								//查找："<SV>"
			if (v_iLen>=0)												///找到？？？
			{
				v_sStr=v_sStr.Mid(v_iLen+4);							//截取
				v_iLen=v_sStr.Find("</SV>",0);							//查找："<SV>"
				if (v_iLen>=0)											//找到？？？
				{
					v_sValue=v_sStr.Left(v_iLen);						//截取
					v_dValue=atof(v_sValue);							
					if (v_dValue!=0)									
					{
						if (v_iType==0)									
							v_dSum+=v_dValue;							
						else if (v_iType==1)							
						{
							if(v_dSum<v_dValue)							
								v_dSum=v_dValue;						
						}
						else											
						{
							v_dSum+=v_dValue;							
							v_iSum++;									
						}
					}
					if (!v_sKey.IsEmpty())								//子项有效？？？
					{													//加入子项容器【v_mChild：Counter，map(子项，数值)】
						if (!v_sType_Name.IsEmpty() && v_bOK)			//有效：Counter序号？？？【2】
						{
							v_pChild_Data->second.insert(pair<CString,CString>(v_sKey,v_sValue));	//增加容器：(子项列，数据)
						}
					}
				}
			}
		}
	}
	if (v_iSum==0)														
	{
		if (v_dSum==-999999999)											
			v_dSum=0;													
		v_sStr.Format("%10.6f",v_dSum);
	}
	else
		v_sStr.Format("%10.6f",v_dSum/v_iSum);							

	return v_sStr;														
}

//------------------------------------------------------------------------------------------------------
//	函数功能：XML文件内容读取【告警文件】
//	参    数：v_sFile_Name：文件名称；
//	输    出：v_mObjType容器
//	返    回：true：有效文件；	false：无效文件；
//------------------------------------------------------------------------------------------------------
bool CMyFile_XML::Read_XML_Alarm(string v_sFile_Name)
{
	XMLElement* measCollecFile;											//指针：measCollecFile
	XMLElement* measData;												//指针：measData
	XMLElement* measInfo;												//指针：measInfo
	XMLElement* measValue=NULL;											//指针：measValue
	XMLElement* r=NULL;													//指针：r

	mString_String	v_mCounter;											//实例化：counter	[容器map]
	list_Object	v_lObject;												//实例化：Object	[链表]
	File_Object v_cObject;												//实例化：Object	[结构]
	CString		v_sType_Name,v_sR_Value;								//临时变量
	CString		v_sObject,v_sObject_DN;									//临时变量
	mString_String v_mName;												//临时变量

	//	v_mName.insert(pair<CString,CString> (v_sType_Name,v_sR_Value));	//增加	[容器：v_mName]
	//清除变量，为读取文件做好准备
	File_Reset();														//变量复位

	//文件读取
	MyXMLDocument* doc = new MyXMLDocument();							//XML实例化
	doc->LoadFile(v_sFile_Name.c_str());								//文件读取	[全部读取到内存中]

	//获得：网元名称
	measCollecFile= doc->FirstChildElement("alarmSynFile");				//查询：alarmSynFile 
	if (measCollecFile==NULL)
		return  false;

	//获取：ObjType\Objject
	measData= measCollecFile->FirstChildElement("fileHeader");			//查询：alarmSynFile
	if (measData==NULL)
		return  false;
	measData=measData->NextSiblingElement("alarmList");					//查询：fileHeader
	if (measData==NULL)
		return  false;
	
	//查询名称
	v_sDev_Name="AlarmList";											//查询名称
	//列名称
	v_mName.insert(pair<CString,CString> ("objectClass",""));			//增加	[容器：v_mName]
	v_mName.insert(pair<CString,CString> ("objectInstance",""));
	v_mName.insert(pair<CString,CString> ("notificationId",""));
	v_mName.insert(pair<CString,CString> ("eventTime",""));
	v_mName.insert(pair<CString,CString> ("alarmClearedTime",""));
	v_mName.insert(pair<CString,CString> ("systemDN",""));
	v_mName.insert(pair<CString,CString> ("vendorSpecificAlarmType",""));
	v_mName.insert(pair<CString,CString> ("notificationType",""));
	v_mName.insert(pair<CString,CString> ("alarmId",""));
	v_mName.insert(pair<CString,CString> ("alarmRaisedTime",""));
	v_mName.insert(pair<CString,CString> ("alarmType",""));
	v_mName.insert(pair<CString,CString> ("probableCause",""));
	v_mName.insert(pair<CString,CString> ("perceivedSeverity",""));
	v_mName.insert(pair<CString,CString> ("specificProblem",""));
	v_mName.insert(pair<CString,CString> ("backedUpStatus",""));
	v_mName.insert(pair<CString,CString> ("trendIndication",""));
	v_mName.insert(pair<CString,CString> ("additionalText",""));
	v_mName.insert(pair<CString,CString> ("additionalInformation_ps",""));
	v_mName.insert(pair<CString,CString> ("additionalInformation_at",""));
	v_mName.insert(pair<CString,CString> ("additionalInformation_ai_vs_other",""));

	measInfo=measData->FirstChildElement("alarmInfo");					//查询：alarmInfo
	while(measInfo!=NULL)
	{
		mString_String::iterator v_pName=v_mName.begin();				//查找名称
		while(v_pName!=v_mName.end())
		{
			measValue=measInfo->FirstChildElement(v_pName->first);		//查找：列名称
			if (measValue!=NULL)
				v_sR_Value=measValue->GetText();						//获得：Counter值
			else
				v_sR_Value="";											//不存在为空
			ConvertUtf8ToGBK(v_sR_Value);								//格式转换
			v_mCounter.insert(pair<CString,CString> (v_pName->first,v_sR_Value));	//增加	[容器：v_mCounter]
			v_pName++;													
		}
		measInfo=measInfo->NextSiblingElement("alarmInfo");				//获得：下一个alarmInfo
		v_cObject.v_mCounter = v_mCounter;								//Counter
		v_mCounter.clear();												//复位：v_mCounter [容器：Counter]
		v_lObject.push_back(v_cObject);									//增加Object
	}
	v_mObjType.insert(pair<CString,list_Object> (v_sDev_Name,v_lObject));//增加：[容器：v_mObjType]

	//清除内存空间
	list_Object::iterator v_pIterator_Object=v_lObject.begin();	
	while(v_pIterator_Object!=v_lObject.end())
	{
		v_pIterator_Object->v_mCounter.clear();							//Counter容器：清除
		v_pIterator_Object++;
	}
	v_lObject.clear();													//Counter容器：清除
	v_mName.clear();													//复位：v_mName [容器：v_mName]

	delete doc;
	doc = NULL;

	return true;														//有效返回
}

//------------------------------------------------------------------------------------------------------
//	函数功能：格式转换 [Utf8ToGBK]
//------------------------------------------------------------------------------------------------------
void CMyFile_XML::ConvertUtf8ToGBK( CString& strUtf8 )   
{   
	int len=MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8, -1, NULL,0);   
	WCHAR * wszGBK = new WCHAR[len+1];   
	memset(wszGBK, 0, len * 2 + 2);   
	MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8, -1, wszGBK, len);   
	  
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);   
	char *szGBK=new char[len + 1];   
	memset(szGBK, 0, len + 1);   
	WideCharToMultiByte (CP_ACP, 0, wszGBK, -1, szGBK, len, NULL,NULL);   
	  
	strUtf8 = szGBK;   
	delete[] szGBK;   
	delete[] wszGBK;   
}   

//------------------------------------------------------------------------------------------------------
//	函数功能：变量复位
//------------------------------------------------------------------------------------------------------
void CMyFile_XML::File_Reset()
{
	v_sDev_Name="";														//网元名称复位

	//容器复位：Objtype【配置、性能】
	map_ObjType::iterator	v_pIterator_ObjType;						//list_ObjType迭代器
	list_Object::iterator	v_pIterator_Object;							//list_Object迭代器
	v_pIterator_ObjType=v_mObjType.begin();
	while(v_pIterator_ObjType!=v_mObjType.end())
	{
		v_pIterator_Object=v_pIterator_ObjType->second.begin();
		while(v_pIterator_Object!=v_pIterator_ObjType->second.end())
		{
			v_pIterator_Object->v_mCounter.clear();						//Counter容器：清除
			v_pIterator_Object++;
		}
		v_pIterator_ObjType->second.clear();							//Object链表：清除
		v_pIterator_ObjType++;
	}
	v_mObjType.clear();													//ObjType容器：清除

	//容器复位：map_ObjType_Child【性能直读】
	map_ObjType_Child::iterator	v_pObjType_Child;						//迭代器：v_mObjType_Child
	map_Child::iterator			v_pChild;								//迭代器：map_Child
	map_Child_Data::iterator	v_pChild_Data;							//迭代器：map_Child_Data
	v_pObjType_Child=v_mObjType_Child.begin();
	while(v_pObjType_Child!=v_mObjType_Child.end())
	{
		//清除：v_lObject
		v_pIterator_Object=v_pObjType_Child->second.v_lObject.begin();
		while(v_pIterator_Object!=v_pObjType_Child->second.v_lObject.end())
		{
			v_pIterator_Object->v_mCounter.clear();						//容器清除：v_mCounter【Counter】
			v_pIterator_Object++;										//迭代器++
		}
		v_pObjType_Child->second.v_lObject.clear();						//容器清除：v_lObject


		//清除：v_mChild
		v_pChild=v_pObjType_Child->second.v_mChild.begin();
		while(v_pChild!=v_pObjType_Child->second.v_mChild.end())
		{
			if (v_pChild->second.size()!=0)
			{
				v_pChild_Data=v_pChild->second.begin();
				while(v_pChild_Data!=v_pChild->second.end())
				{
					v_pChild_Data->second.clear();						//容器清除：v_mCounter【Counter】
					v_pChild_Data++;									
				}
				v_pChild->second.clear();								//容器清除：v_mCounter【Counter】
			}
			v_pChild++;													
		}
		v_pObjType_Child->second.v_mChild.clear();						//容器清除：v_mChild【子项】

		v_pObjType_Child++;
	}
	v_mObjType_Child.clear();											//v_mObjType_Child容器：清除

	//容器复位：列名称
	v_mName.clear();													//复位：v_mName [容器：v_mName]
}

//------------------------------------------------------------------------------------------------------
//	End
//------------------------------------------------------------------------------------------------------
