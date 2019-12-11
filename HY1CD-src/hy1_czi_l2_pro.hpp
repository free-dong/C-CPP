/*
 * hy1_czi_l2_pro.hpp
 *
 *  Created on: Aug 10, 2018
 *      Author: zhongyong
 */

#ifndef HY1_CZI_L2_PRO_HPP_
#define HY1_CZI_L2_PRO_HPP_

#include <PIK_Common.h>
#include <pthread.h> // Entete relative aux POSIX threads. Elle doit toujours etre la premier dans la list des files inclus
#include <signal.h> // Pour la gestion des signaux
#include <errno.h> // Pour recuperer les erreurs systeme linux
#include <unistd.h> // Fonctions systeme Linux comme sleep()
#include <iterator> //Iterateurs de la STL
#include <vector> // Vecteurs de la STL
#include <map> // entete contenant la map de la STL
#include <string.h> //string de la STL
#include <sstream> //string stream de la STL
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>

// Include relatif a l'utilisation de xerces
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

using namespace xercesc;

#include "PIK_Common.h"  //     Gestion des communs PIK
#include "CDOC_EnumsDocuments.hpp"  //     enumerations du package Documents
#include "CUTIL_EnumsTools.hpp"        //    Enumerations des utilitaires
#include "CUTIL_Exceptions.hpp"        //     Exceptions du module utilitaire
#include "CUTIL_Dates.hpp"        //     Dates
#include "CDOC_EnumsDocuments.hpp" //Enumerations du module Document
#include "CDOC_Document.hpp"        //     document basique
#include "CDOC_AbsPikDocument.hpp" //     document abstrait
#include "CUTIL_ManipStrings.hpp" // Classe utilitaire pour la manipulations des strings de characters
#include "CUTIL_PIKUsage.hpp"

#include "log4cxx/logger.h" // Utilisation de libraire log4cxx
#include "log4cxx/layout.h" // Gestion des layouts pour la librairie log4cxx
#include "log4cxx/patternlayout.h" // Gestion des pattern layouts pour la librairie log4cxx
#include "log4cxx/level.h" // Gestion des niveaux de messages pour la librairie log4cxx
#include "log4cxx/appender.h" // Gestion des appender pour la librairie log4cxx
#include "log4cxx/fileappender.h" // Gestion des file appender pour la librairie log4cxx
#include "log4cxx/consoleappender.h" // Gestion des console appender pour la librairie log4cxx

#include "log4cxx/ndc.h" // Entete relative au contexte specific NDC
#include "log4cxx/propertyconfigurator.h" // Gestion des property configurator pour la librairie log4cxx

#include <log4cxx/logmanager.h> // log manager de log4cxx
#include "log4cxx/helpers/exception.h" // Gestion des exception pour la librairie log4cxx

using namespace log4cxx;
using namespace log4cxx::spi;
using namespace log4cxx::helpers;

#include "CLOG_EnumsLOG.hpp" // Enumerations du modul CLOG
#include "CLOG_Exceptions.hpp" // Prototype des exceptions du package LOG
#include "CLOG_CTLevel.hpp" // Prototype du niveau Progress ajoute a la list des niveaux par defaut de og4cxx
#include "CDOC_EnumsDocuments.hpp" // Enumerations du package Documents
#include "CXML_EnumsXML.hpp" // Prototypes des enumerations du package XML
#include "CDOC_Document.hpp" //     Document basique
#include "CDOC_AbsPikDocument.hpp" //     classe abstraite des documents
#include "CXML_PikXMLDocument.hpp" // Gestion des documents XML de PIK
#include "CUTIL_Exceptions.hpp" // Prototype des exceptions du package Tools
#include "CXML_Exceptions.hpp" // Exceptions utilisees dans le package XML
#include "CXML_NodeXML.hpp" // Gestion des nodes XML
#include "CXML_DataXML.hpp" // Gestion des donnees dans les nodes XML
#include "CXML_ReaderXML.hpp" // Prototype de la classe
#include "CFGEN_Exceptions.hpp" // Gestion des exceptions du package CFGEN
#include "CFGEN_EnumsFGEN.hpp" // Gestion des enumerations du package CFGEN
#include "CFGEN_Report_ProcessingMessage.hpp"
#include "CLOG_CTLogger.hpp" // Prototype de la classe CLOG_CTLogger

#include "CFGEN_EnumsFGEN.hpp" // Enumerations des files generics
#include "CFGEN_Exceptions.hpp" //Exceptions des files generics
#include <CFGEN_JobOrder_File.hpp> // Declarations relatives au descriptif d'un file Input lu dans un joborder
#include <CFGEN_JobOrder_BreakpointFile.hpp> // Declarations relatives au descriptif d'un file breakpoint lu dans un joborder
#include <CFGEN_JobOrder.hpp> // Declarations relatives a un file joborder
#include <CFGEN_HandlerConfiguration.hpp> // Declarations relatives a un file de configuation du gestionnaire

#include <CGEN_EnumsHandler.hpp> // Declarations du gestionnaire d'execution
#include <CGEN_ExceptionsHandler.hpp> // Declarations du gestionnaire d'execution
#include <CGEN_IProcessing.hpp> //Interface communes aux traitements
#include <CGEN_ExecutionHandler.hpp> // Declarations du gestionnaire d'execution

#include "HDF_STRUCT.h"

class HY1_CZI_L2A_PRO: public CGEN_IProcessing
{
    public:
		HY1_CZI_L2A_PRO();
        ~HY1_CZI_L2A_PRO();
//        struct HDF_GEODATA Atmospheric_Correction1(struct HDF_GEODATA DataSet, char* AuxDataPathName, char* PATH_HDF5);
        PIK_Int_t startPreProcessing(CGEN_ExecutionHandler * ai_poHandlerExec);
        PIK_Int_t startProcessing();
        PIK_Int_t startPostProcessing();

        void cancel();

//        void suanfa();
};





#endif /* HY1_CZI_L2_PRO_HPP_ */
