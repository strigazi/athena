/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "AGDD2Geo/XercesParser.h"
#include "AGDD2Geo/AGDDController.h"
#include "AGDD2Geo/XMLHandler.h"
#include "AGDD2Geo/XMLHandlerStore.h"
#include "PathResolver/PathResolver.h"

#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/util/XMLException.hpp"

#include <xercesc/dom/DOM.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <xercesc/util/OutOfMemoryException.hpp>

using namespace xercesc;

DOMNode* XercesParser::currentElement=0;

XercesParser::XercesParser():IAGDDParser(),doc(0)
{
//	std::cout<<"+++++++++++> Xerces Parser being created!" <<std::endl;
//	AGDDController::GetController()->SetParser(this);
//	std::cout<<"exiting constructor"<<std::endl;
}
XercesParser::XercesParser(std::string s):IAGDDParser(s),doc(0)
{
//	std::cout<<"+++++++++++> Xerces Parser being created!" <<std::endl;
//	AGDDController::GetController()->SetParser(this);
//	std::cout<<"exiting constructor"<<std::endl;
}
bool XercesParser::ParseFile(std::string s)
{
//	std::cout<<"+++++++++++> Xerces Parser parsing file "<<s <<std::endl;
	fileName=s;
	s=PathResolver::find_file(s,"XMLPATH",PathResolver::RecursiveSearch);
	if (s.empty())
		std::cout<<" something wrong, could not find XML file "<<s<<std::endl;
	else
	{
//		std::cout<<" loading file "<<s<<std::endl;
        XercesDOMParser *parser = new XercesDOMParser;
        bool errorsOccured = false;
    	try
    	{
        	parser->parse(s.c_str());
    	}
    	catch (const OutOfMemoryException&)
    	{
        	XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
        	errorsOccured = true;
    	}
    	catch (const XMLException& e)
    	{
        	XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n   Message: "
             	<< XMLString::transcode(e.getMessage()) << XERCES_STD_QUALIFIER endl;
        	errorsOccured = true;
    	}
        catch (const DOMException& e)
        {
                const unsigned int maxChars = 2047;
        	XMLCh errText[maxChars + 1];

        	XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << s << "'\n"
            	 << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;

        	if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars))
             	XERCES_STD_QUALIFIER cerr << "Message is: " << XMLString::transcode(errText) << XERCES_STD_QUALIFIER endl;

        	errorsOccured = true;
    	}

    	catch (...)
    	{
        	XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n " << XERCES_STD_QUALIFIER endl;
        	errorsOccured = true;
    	}  
		doc=parser->getDocument();
		return errorsOccured;
	}

        return false;

}
bool XercesParser::ParseFileAndNavigate(std::string s)
{
//    std::cout<<"+++++++++++> Xerces Parser parsing and navigating file "<<s <<std::endl;
    bool errorOccured = ParseFile(s);
	if (!errorOccured) navigateTree();
	return true;
}
bool XercesParser::ParseString(std::string s)
{
//	std::cout<<"+++++++++++> Xerces Parser parsing string "<<std::endl;
//	std::cout<<s<<std::endl;
	const char* str=s.c_str();
	static const char* memBufID="prodInfo";
	MemBufInputSource* memBuf = new MemBufInputSource((const XMLByte*)str,strlen(str),memBufID,false);
    XercesDOMParser *parser = new XercesDOMParser;
    bool errorsOccured = false;
    try
    {
    	parser->parse(*memBuf);
    }
    catch (const OutOfMemoryException&)
    {
    	XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
    	errorsOccured = true;
    }
    catch (const XMLException& e)
    {
    	XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n   Message: "
             	<< XMLString::transcode(e.getMessage()) << XERCES_STD_QUALIFIER endl;
    	errorsOccured = true;
    }
    catch (const DOMException& e)
    {
        const unsigned int maxChars = 2047;
    	XMLCh errText[maxChars + 1];

    	XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << s << "'\n"
            	 << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;

   		if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars))
             	XERCES_STD_QUALIFIER cerr << "Message is: " << XMLString::transcode(errText) << XERCES_STD_QUALIFIER endl;

    	errorsOccured = true;
    }

    catch (...)
    {
    	XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n " << XERCES_STD_QUALIFIER endl;
    	errorsOccured = true;
    }  
	doc=parser->getDocument();
	return errorsOccured;
}
bool XercesParser::ParseStringAndNavigate(std::string s)
{
//    std::cout<<"+++++++++++> Xerces Parser parsing and navigating string "<<s <<std::endl;

    bool errorOccured = ParseString(s);
	if (!errorOccured) navigateTree();
	return true;
}

void XercesParser::navigateTree()
{
	if (!doc) std::cout<<" something is wrong! no document set!"<<std::endl;
	DOMNode* node = 0;
	node = dynamic_cast<DOMNode*>(doc->getDocumentElement());
	assert( node );
	currentElement=node;
	elementLoop(node);
}

void XercesParser::elementLoop()
{
}

void XercesParser::elementLoop(DOMNode *e)
{
	if (!e->getNodeType()==DOMNode::ELEMENT_NODE) return;
	currentElement=e;
	XMLHandler *h=XMLHandlerStore::GetHandlerStore()->GetHandler(e);
	bool stopLoop=false;
	if (h)
	{
		h->Handle(e);
		stopLoop=h->IsLoopToBeStopped();
	}
	DOMNode *child;
	std::string sName;
	if (!stopLoop && e)
	{
		char* name=XMLString::transcode(e->getNodeName());
		sName=name;
//		std::cout <<" found element "<<name<<std::endl;
		XMLString::release(&name);
		for (child=e->getFirstChild();child!=0;child=child->getNextSibling())
		{
			if (child->getNodeType()==DOMNode::ELEMENT_NODE) {
//				std::cout<<sName<<" child name "<<XMLString::transcode(child->getNodeName())<<std::endl;
				elementLoop(child);
			}
		}
	}
//	XMLHandler *h=XMLHandlerStore::GetHandlerStore()->GetHandler(element);
//	bool stopLoop=false;
//	if (h)
//	{
//		h->Handle(element);
//		stopLoop=h->IsLoopToBeStopped();
//	}
//	TiXmlElement* subelement;
//	subelement=element->FirstChildElement();
//	
//	if (!stopLoop && subelement) 
//	do
//	{
//	   // std::cout<<" \telement  "<<subelement->Value()<<std::endl;
//	   elementLoop(subelement);
//	   subelement = subelement->NextSiblingElement();
//	} while (subelement);
}

ExpressionEvaluator& XercesParser::Evaluator()
{
	static ExpressionEvaluator eval;
	return eval;
}

bool XercesParser::Initialize()
{
    	try
    	{
        	XMLPlatformUtils::Initialize();
    	}

    	catch(const XMLException &toCatch)
    	{
        	XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
             	<< "  Exception message:"
             	<< XERCES_STD_QUALIFIER endl;
        	return 1;
    	}
	return 0;
}
