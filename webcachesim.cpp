#include <fstream>
#include <string>
#include "caches/lru_variants.h"
#include "caches/gd_variants.h"
#include "request.h"
#define REPORT_INTERVAL 1000000

using namespace std;

int main (int argc, char* argv[])
{

  // output help if insufficient params
  if(argc < 4) {
    cerr << "webcachesim traceFile cacheType cacheSizeBytes [cacheParams]" << endl;
    return 1;
  }

  // trace properties
  const char* path = argv[1];

  // create cache
  const string cacheType = argv[2];
  unique_ptr<Cache> webcache = move(Cache::create_unique(cacheType));
  if(webcache == nullptr)
    return 1;

  // configure cache size
  const uint64_t cache_size  = std::stoull(argv[3]);
  webcache->setSize(cache_size);

  // parse cache parameters
  string paramSummary;
  for(int i=4; i<argc; i++) {
	std::string input; 
	input=argv[i]; 
	std::string parName=input.substr(0, input.find("=")); 
	std::string parValue=input.substr(input.find("=")+1); 
	webcache->setPar(parName, parValue); 
	paramSummary += parValue; 
  }

  ifstream infile;
  long long reqs = 0, hits = 0;
  long long t, id, size;

	// per-REPORT_INTERVAL hits 
	long hits_interval = 0; 

  cout << "running..." << endl;

  infile.open(path);
  SimpleRequest* req = new SimpleRequest(0, 0);
  while (infile >> t >> id >> size)
    {
        reqs++;
        
        req->reinit(id,size);
        if(webcache->lookup(req)) {
            hits++;
		hits_interval++; 
        } else {
            webcache->admit(req);
        }

	if(!(reqs%REPORT_INTERVAL)) {
		cerr << reqs << " " << hits << " " << double(hits)/reqs << " " 
		<< double(hits_interval)/REPORT_INTERVAL 
		<< endl; 

		hits_interval=0; 
	}
    }

  delete req;

  infile.close();
  cout << cacheType << " " << cache_size << " " << paramSummary << " "
       << reqs << " " << hits << " "
       << double(hits)/reqs << endl;

  return 0;
}
