/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./HLTSequenceLoader.h"

#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTSequenceList.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "./DBHelper.h"

using namespace std;

bool
TrigConf::HLTSequenceLoader::load( HLTFrame& frame ) {
   m_smk=frame.smk();

   m_schemaversion = triggerDBSchemaVersion();
   if(verbose())
      msg() << "HLTSequenceLoader:                TriggerDB Schema version:  " 
            << m_schemaversion << endl;

   HLTSequenceList& sequences = frame.theHLTSequenceList();

   try {
      startSession ();
      loadSequences( sequences );
      commitSession();
   }
   catch (const std::exception& e) {
      msg() << "HLTSequenceLoader:                exception: " << e.what() << endl;
      throw;
   }
   return true;
}


void
TrigConf::HLTSequenceLoader::loadSequences( HLTSequenceList& seqlist ) {

   if(verbose())
      msg() << "HLTSequenceLoader:                Start loading sequences with SMK "
            << m_smk << endl;

   unique_ptr< coral::IQuery > q( m_session.nominalSchema().newQuery() );

   q->addToTableList ( "SUPER_MASTER_TABLE"    , "SM");
   q->addToTableList ( "HLT_MASTER_TABLE"      , "HM");
   q->addToTableList ( "HLT_TM_TO_TC"          , "M2C");
   q->addToTableList ( "HLT_TC_TO_TS"          , "TC2TS");
   q->addToTableList ( "HLT_TS_TO_TE"          , "S2TE");
   q->addToTableList ( "HLT_TRIGGER_ELEMENT"   , "TE");
   q->addToTableList ( "HLT_TE_TO_CP"          , "TE2CP");
   q->addToTableList ( "HLT_TE_TO_TE"          , "TE2TE");
   q->addToTableList ( "HLT_COMPONENT"         , "CP");

   //Bind list
   coral::AttributeList bindings;
   bindings.extend<int>("smid");
   bindings[0].data<int>() = (int)m_smk;

   string theCondition = "";
   theCondition += " SM.SMT_ID     = :smid";
   theCondition += " AND HM.HMT_ID = SM.SMT_HLT_MASTER_TABLE_ID";
   theCondition += " AND HM.HMT_TRIGGER_MENU_ID = M2C.HTM2TC_TRIGGER_MENU_ID";
   theCondition += " AND M2C.HTM2TC_TRIGGER_CHAIN_ID = TC2TS.HTC2TS_TRIGGER_CHAIN_ID";
   theCondition += " AND TC2TS.HTC2TS_TRIGGER_SIGNATURE_ID = S2TE.HTS2TE_TRIGGER_SIGNATURE_ID";
   theCondition += " AND TE.HTE_ID = S2TE.HTS2TE_TRIGGER_ELEMENT_ID";
   theCondition += " AND TE.HTE_ID = TE2CP.HTE2CP_TRIGGER_ELEMENT_ID";
   theCondition += " AND TE.HTE_ID = TE2TE.HTE2TE_TE_ID";
   theCondition += " AND CP.HCP_ID = TE2CP.HTE2CP_COMPONENT_ID";

   q->setCondition( theCondition, bindings );

   //Output data and types
   coral::AttributeList attList;
   attList.extend<int>   ( "TE.HTE_ID" );
   attList.extend<string>( "TE.HTE_NAME" );
   attList.extend<string>( "CP.HCP_NAME" );
   attList.extend<string>( "CP.HCP_ALIAS" );
   attList.extend<int>   ( "TE2CP.HTE2CP_ALGORITHM_COUNTER" );
   attList.extend<string>( "TE2TE.HTE2TE_TE_INP_ID" );
   attList.extend<string>( "TE2TE.HTE2TE_TE_INP_TYPE" );
   attList.extend<int>   ( "TE2TE.HTE2TE_TE_COUNTER" );
   fillQuery(q.get(),attList);

   // the ordering
   q->addToOrderList( "TE.HTE_ID ASC, TE2CP.HTE2CP_ALGORITHM_COUNTER DESC, TE2TE.HTE2TE_TE_COUNTER DESC" );

   // process the query
   q->setDistinct();
   coral::ICursor& cursor = q->execute();

   while ( cursor.next() ) {
      const coral::AttributeList& row = cursor.currentRow();
      string te_name = rmtilde(row["TE.HTE_NAME"].data<string>());
      HLTSequence* seq = seqlist.getSequence(te_name);
      if(seq==0) {
         seq = new HLTSequence( vector<HLTTriggerElement*>(), new HLTTriggerElement(te_name), vector<string>() );
         seqlist.addHLTSequence(seq);
      }


      string alg_name = row["CP.HCP_NAME"].data<string>() + "/" + row["CP.HCP_ALIAS"].data<string>();
      unsigned int alg_pos = row["TE2CP.HTE2CP_ALGORITHM_COUNTER"].data<int>();

      vector<string>& alg_list = seq->algorithms();
      if( alg_list.size() < alg_pos+1 )
         alg_list.resize( alg_pos+1, "" );

      if(alg_list[alg_pos] == "") {
         alg_list[alg_pos] = alg_name;
      } else {
         if(alg_list[alg_pos] != alg_name) {
            cerr << "Two different algs in the same position " << alg_pos << endl;
         }
      }

      string input_te = rmtilde(row["TE2TE.HTE2TE_TE_INP_ID"].data<string>());
      if(input_te!="") {
         unsigned int input_te_pos = row["TE2TE.HTE2TE_TE_COUNTER"].data<int>();
         
         vector<HLTTriggerElement*>& inp_list = seq->inputTEs();
         if( inp_list.size() < input_te_pos+1 )
            inp_list.resize( input_te_pos+1, (HLTTriggerElement*)0 );
         
         if(inp_list[input_te_pos] == 0) {
            inp_list[input_te_pos] = new HLTTriggerElement(input_te);
         } else {
            if(inp_list[input_te_pos]->name() != input_te ) {
               cerr << "Two different input TE's at the same position " << input_te_pos << endl;
            }
         }
      }
   }
   cursor.close();
}


