/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TODO:
// make sure that we use only the last of all the trees with the same name !!!

#include <Riostream.h>

#include <TTree.h>
#include <TFile.h>
#include <TChain.h>
#include <TChainElement.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TKey.h>
#include <TLeaf.h>
#include <TStopwatch.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

using namespace std;

class mTree{
public:
    mTree(string a):filename(a), entries(0){}
    string filename;
    long entries;
};

class mTrees{
public:
    mTrees(string a):name(a),leaves(0),totalSize(0){}
    string name;
    int leaves;
    double totalSize;    
    vector<unsigned int> basketSize;// this is the value to be optimized - at beggining = totSize
    vector<unsigned int> totSize;// this is value of ZIPPED SIZES collected from all the files
    vector<unsigned int> baskets;// this is cached to speed up calculation - at beggining = 1 
    void addFile(string s){ m_tree.push_back(mTree(s)); }
    
    void setLeaves(int l){
        leaves=l;
        for (int w=0;w<l;w++){
            totSize.push_back(0); 
            }
    }
    
    void recalculate(){
        cout<<"recalculate baskets"<<endl;
        for (int w=0;w<leaves;w++){
            basketSize.push_back(totSize[w]);
            baskets.push_back(1);
            totalSize+=totSize[w];
        }
    }
    
    void optimize(unsigned long memory){
        
        cout<<"optimize baskets. total tree size:"<<totalSize<<"\t memory: "<<memory<<endl;
        while(true){    
            double totMem=0;
            for (int i=0;i<leaves;i++)
                totMem += basketSize[i];
                 
            if ( totMem < memory) break;
    
            int maxSpare=-1; int mi=-1;
            for (int i=0;i<leaves;i++){
                int spare=totSize[i]/baskets[i]-totSize[i]/(baskets[i]+1);
                if (maxSpare<spare){
                    maxSpare=spare; 
                    mi=i;
                }
            }
            if (mi < 0) continue;
            baskets[mi]++;
            basketSize[mi]=totSize[mi]/baskets[mi];
            // cout<<"totMem: "<<totMem<<"\tSpare: "<<totSize[mi]/(baskets[mi]-1)-totSize[mi]/baskets[mi]<<"\t baskets: "<<baskets[mi]<<"\n";
        }
        roundBasketSize();
    }
    
    void roundBasketSize(){
        
        cout<<"round basket sizes"<<endl;
        double totMem = 0;
        int    totBas = 0;
        unsigned int    maxBs  = 0;
        unsigned int    minBs  = 1024*1024*1024;
        
        for (int w=0;w<leaves;w++){
            if (basketSize[w]==0) basketSize[w]=16;
            basketSize[w] = basketSize[w] - basketSize[w] % 8;
            totMem += basketSize[w];
            totBas += baskets[w];
            if (basketSize[w]<minBs) minBs=basketSize[w];
            if (basketSize[w]>maxBs) maxBs=basketSize[w];
        }
        cout<<"Total memory buffer: "<<totMem/1024<<" kb total baskets: "<<totBas<<"\tmin BS: "<<minBs<<"\tmax BS: "<<maxBs<<endl;
        cout<<"-------------------------------------------------------------------------------------------------"<<endl;
    }
    
    vector<mTree>::const_iterator bi(){return (vector<mTree>::const_iterator) m_tree.begin();};
    vector<mTree>::const_iterator ei(){return (vector<mTree>::const_iterator) m_tree.end();};
    
private:
    vector<mTree> m_tree;
};

std::string sfo="";

vector<mTrees> ALL;

void ren(){
    cout<<"renaming ... "<<endl;
    std::string com="rm "+sfo+".root; for i in *temporary*.root; do mv \"$i\" \"${i/_temporary/}\";done";
    // cout<<"command to execute: "<<com<<endl;
    int ret=system (com.c_str());
    if (!ret) cout<<"files renamed OK"<<endl;
}

void warmUp(const char* fn){
    std::string com="cat "+std::string(fn)+" > /dev/null ";
    cout<<"warming up the cache for file: "<<com<<endl;
    int ret=system (com.c_str());
    if (!ret) cout<<"file warmed up OK"<<endl;
}


void order(int m){
    
    TChain *ch=new TChain(ALL.begin()->name.c_str());
    std::string o=sfo+"*.root";
    ch->Add(o.c_str());
    TObjArray *oaFiles=ch->GetListOfFiles();
    
    for(int i=0;i<oaFiles->GetEntries();i++){
        TChainElement *element = (TChainElement*)oaFiles->At(i);
        const char *fn=element->GetTitle();
        
        TStopwatch timer;    
        timer.Start();
        cout<<"optimizing: "<<fn<<endl;
        // warmUp(fn);
        
        timer.Start();
        TFile *fIn = TFile::Open(fn);
        
        std::stringstream out; out << i;
        std::string s = out.str();
        std::string tmp = sfo+"_temporary_"+s+".root";
        TFile* fOut=TFile::Open(tmp.c_str(),"recreate","",6);
     
     // -  ordering
        for (vector<mTrees>::const_iterator it=ALL.begin();it!=ALL.end();it++){
            TTree *tc2 = (TTree*)fIn->Get(it->name.c_str()) ;
            TTree* TC2;
            if (m==2)  TC2= tc2->CloneTree(-1,"SortBasketsByEntry fast");
            else TC2 = tc2->CloneTree(-1,"SortBasketsByBranch fast");
            TC2->Write("",TObject::kOverwrite);
        }
     // -  
       
        timer.Stop();
        cout<<"ordering file:                   "<<i<<" \t\twallclock time: "<< timer.RealTime()<<"\t CPU time: "<<timer.CpuTime()<<endl;
        
        fOut->Close();
        fIn->Close();
    }
    
    cout<<"ordering done"<<endl;
    ren();
    return;
}

void optimizeBasketSizes(){
    
    for (vector<mTrees>::iterator it=ALL.begin(); it!=ALL.end(); it++) {
        
        cout<<"Doing tree: "<<it->name<<endl;
        // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>          SUMMING UP BRANCH SIZES OVER ALL THE FILES
        for(vector<mTree>::const_iterator itt=it->bi(); itt!=it->ei(); itt++){ // loop over files having this tree
            
            TFile* f = TFile::Open(itt->filename.c_str());
            TTree* t = (TTree*) f->Get(it->name.c_str());
    
            for (int i=0; i<it->leaves; i++) {
                TBranch* branch = (TBranch*)t->GetListOfBranches()->UncheckedAt(i);
                branch->SetAddress(0);
                // cout <<i<<"\t"<<branch->GetName()<<"\t BS: "<< branch->GetBasketSize()<<"\t size: "<< branch->GetTotalSize()<< "\ttotbytes: "<<branch->GetTotBytes() << endl;
                it->totSize[i] += branch->GetTotBytes(); 
            }
             
            f->Close();
        }
        
        cout<<"got basket sizes from all the files"<<endl;
        it->recalculate();
        
        it->optimize(40*1024*1024); // in bytes !
        

    }
}

void createMergedFile(){
    
    std::string oname=sfo+".root";
    TFile* fOut = TFile::Open(oname.c_str(),"RECREATE","",1);
    TDirectory *gDirectory_save = gDirectory;
    if (fOut->IsZombie()){
        cout<<"File "<<oname<<" isZombified. Output file can not be created."<<endl;
        return;
    }
    
    TTree *newtree=0; 
    for (vector<mTrees>::iterator it=ALL.begin(); it!=ALL.end(); it++) {
        
        TFile* f = TFile::Open((it->bi())->filename.c_str());
        TTree* t = (TTree*) f->Get(it->name.c_str());
        
       // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> CREATING NEW TREE
        newtree = (TTree*)t->CloneTree(0); // no copy of events
        
        newtree->SetDirectory(gDirectory_save);
        
        // Once the cloning is done, separate the trees,to avoid as many side-effects as possible
        t->GetListOfClones()->Remove(newtree);
        t->ResetBranchAddresses();
        newtree->ResetBranchAddresses();
       
       // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Setting optimized basket sizes. 
        for (int w=0;w<it->leaves;w++) {     
            TBranch* branch=(TBranch*)newtree->GetListOfBranches()->UncheckedAt(w);
            branch->SetBasketSize(it->basketSize[w]);
            // cout<<"basket size: "<<basketSize[w]<<"\t number of baskets:"<<baskets[w]<<endl;
        }

        // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> copying data.
            
        for(vector<mTree>::const_iterator itt=it->bi(); itt!=it->ei(); itt++){ // loop over files having this tree

            TFile* f = TFile::Open(itt->filename.c_str());
            TTree* t = (TTree*) f->Get(it->name.c_str());
            
            unsigned long nentries = t->GetEntries();    
            newtree->CopyAddresses(t);
            for ( unsigned long e = 0; e < nentries; ++e) {
                t->GetEntry(e);
                newtree->Fill();
            }
            t->ResetBranchAddresses(); // Disconnect from new tree.
            f->Close();
            cout<<"Current file entries: "<<nentries <<endl;
        }
        
        fOut->cd();
        int s = newtree->Write("", TObject::kOverwrite);       
        if (!s) {
            cout<<" WARNING FILE IS NOT OPTIMIZED. MAYBE NOT ENOUGH DISK SPACE? "<<endl;
            return;
        }
    }
       
    fOut=newtree->GetCurrentFile();
    fOut->Write();
    fOut->Close();
    
}



int main(int argc,char* argv[]){
    
    
    if (argc < 5) { 
        cout << "Usage is:\n";
        cout << "\t-in <infile>             --- this can be of the form /data/higgs/my*nobel*_000*.1 \n";
        cout << "\t-out <outfile>           --- this can be of the form /data/out/higgs - as .root will be appended\n";
        cout << "\t-maxsize <tree max size> --- maximum zip size of the main tree (in Mb). The default value is 6000.\n";
		cout << "\t-byBranch                --- baskets in output files will be ordered by branch. The default is by Entry.\n";
        cout << "\t-noCollectionTree        --- TTree named CollectionTree is usualy needed if you will use merged file in Athena\n";
        std::cin.get();
        return 0;
    }
    
    char *fi = NULL;
    Long64_t maxsize=6000;
    bool noCollectionTree=false;
	int toOrder=2;
    
    for (int i = 1; i < argc; i++) {    
        if (! strcmp(argv[i], "-in") ) {
            fi = argv[i + 1];
        } else if (! strcmp(argv[i], "-out") ) {
            sfo=std::string(argv[i + 1]);
        } else if (! strcmp(argv[i], "-maxsize") ) {
            maxsize = atoi(argv[i + 1]);
        } else if (! strcmp(argv[i], "-byBranch") ){
			toOrder=1;
		} else if (! strcmp(argv[i], "-noCollectionTree")){
            noCollectionTree=true;
        }
    }
    
    TTree::SetMaxTreeSize(maxsize*1024*1024);
    
    cout<<"Input files: "<<fi<<"\nOutput file: "<<sfo<<"\nMax Tree Size: "<<maxsize<<endl;
    
    TChain *ch = new TChain("CollectionTree");
    ch->Add(fi);
    
    TObjArray *oaFiles = ch->GetListOfFiles();
    if (!oaFiles->GetEntries()){
        cout<<"No input files found!"<<endl;
        return 0;
    }
    
    
    cout<<"I chain created..."<<endl;
    
    
    
    // ==================================================================== take tree names from the first file
    
    TFile f( ((TChainElement*) oaFiles->At(0))->GetTitle() );
    
    TList* keys = f.GetListOfKeys();
    TKey*  key;  TIter  next( keys );
    string oldname("");
    while (( key = static_cast< TKey * > (next()) )) {
        string name(key->GetName());
        TClass *classPtr = gROOT->GetClass((const char *) key->GetClassName());
        if ( !(classPtr->GetBaseClass("TTree")) ) {
            cout << " WARNING ! object " << name << " will not be merged!" << endl;
            continue; 
        } else {
            if (oldname==name) continue;
            oldname=name;
            if (noCollectionTree and name=="CollectionTree") continue; 
            cout<< "Tree to merge: "<< name <<endl;
            mTrees tt(name);
            ALL.push_back(tt);
        }
    }
    key=NULL;
    f.Close();
    
    
    cout<<"II trees to merge determined ..."<<endl;
    // =====================================================================

    std::vector<TFile*> fFile (oaFiles->GetEntries());
    for(int i=0;i<oaFiles->GetEntries();i++){
        
        const char* fn = ((TChainElement*)oaFiles->At(i))->GetTitle();
        fFile[i] = TFile::Open(fn);
        
        if (fFile[i]->IsZombie()){
            cout<<"File "<<fn<<" isZombified. Its corrupted or not a root file."<<endl;
            continue;
        }
        
        for(vector<mTrees>::iterator it=ALL.begin();it!=ALL.end();it++){
            TTree* tmpt=(TTree*)fFile[i]->Get( (it->name).c_str() );
            int leaves=tmpt->GetListOfBranches()->GetEntriesFast();
            
            if (!leaves){
                cout<<"WARNING! Tree: "<< (it->name).c_str() <<"  in file: "<<fn<<" contains no events. Will not be merged."<<endl;
            }
            else{
                it->addFile(fn);
                if (!it->leaves)
                    it->setLeaves(leaves);
            }
        }
        
        fFile[i]->Close();
    }
    
    delete ch;  ch=NULL;
    oaFiles=NULL;
    cout<<"Files checked and verified..."<<endl;
    // =====================================================================
    
    TStopwatch timer;
    timer.Start();

    optimizeBasketSizes();
    createMergedFile();
    
    timer.Stop();
    cout<<"Done           in wallclock: "<< timer.RealTime()<<"\t\t CPU time: "<<timer.CpuTime()<<endl;
    
    order(toOrder);
    
return 0;   
}




