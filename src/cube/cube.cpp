#include "cube.hpp"
#include <math.h>
#include <numeric>
#include <map>
#include <utils.hpp>
#include <unordered_map>

#define M 4294967291

unsigned int Cube::f(int i, int h_p) {
	int _f = rand() % 2;
	if (notExists(f_table[i], h_p)) {
		f_table[i].insert(pair<int,int>(h_p, _f));
		return _f;
	} 
	else 
		return getValue(f_table[i], h_p);
}

unsigned int Cube::F(vector<float> p){
	string _F;
	char c;
	int hash;
	for(int i=0; i<k; i++){
		hash = modulo(hash_L2(i,p,v,t,w),M) ;
		// cout << "hash:" << hash<< endl;
		c = intToChar(f(i,hash));
		// cout << c << "," << i << " of " << k << endl;
		// sprintf(&c, "%d", f(i, hash_L2(i,p,v,t,w)));
		_F.push_back(c);
		// cout << _F << endl;
	}
	// cout << "F" << _F << endl;
	// cout << binaryToDecimal(_F) << endl;
	return binaryToDecimal(_F);
}

Cube::Cube(vector<vector<float>> input_vectors,vector<string> input_ids, int k, int argM, int probes, int metric)
{
	vectors=input_vectors;
	ids=input_ids;
	
	//Initialize values
	Cube::k=k;
	Cube::argM=argM;
	Cube::probes=probes;
	f_table = new map<int, int>[k];
	hypercube = new hash_table<hashtable_item_cube>(280834);
	w=uniform_distribution_rng(0,6);
	vectorSize=vectors[0].size();
	n=ids.size();
	tableSize=n/4;
	v = new vector<float>[k];
	t = new float[k];

	for (int i = 0; i < k; i++)
	{
		v[i].push_back(normal_distribution_rng());
		t[i] = uniform_distribution_rng(0,w-1);
	}

	if(metric==L2)
		distance=&eucledian_distance;

	for(int i = 0;i<n;i++)
	{
		hashtable_item_cube p{vectors[i],i};
		hypercube->insert(F(vectors[i]),p);
	}
};

vector<pair<float,unsigned int>> Cube::find_N_nearest(vector<float> p,unsigned int N)
{
	cout << "Tracing N nearest" << endl;
	//Returns indexes of N Nearest elements
	multimap<float, int> distances;
    int counter = 0;
	// cout << "count:" << hypercube->count(F(p)) << endl;
	int* nearbyProbes = getNearbyProbes(F(p), probes, k);
	// if(hypercube->count(F(p))>0)
	for(int i=0; i<probes; i++)
		for (auto it = hypercube->begin(nearbyProbes[i]); it != hypercube->end(nearbyProbes[i]); ++it )
		{
			if(counter >= argM)
				break;
			hashtable_item_cube p_b = *it;
			float distance = Cube::distance(p,p_b.p);
			distances.insert({distance,p_b.index});
			counter++;
		}

	vector<pair<float,unsigned int>> N_Nearest;
	unsigned int y=0;
	auto it=distances.begin();
	for(;it != distances.end() && y<N ;++it,y++)
	{
		N_Nearest.push_back({it->first,it->second});
	}
	return N_Nearest;
}

vector<pair<float,unsigned int>> Cube::find_R_nearest(vector<float> p,int R)
{
	cout << "Tracing R nearest" << endl;
	//Returns indexes of R nearest element
	multimap<float, int> distances;
    int counter = 0;
	// cout << "count:" << hypercube->count(F(p)) << endl;

	// if(hypercube->count(F(p))>0)
	int* nearbyProbes = getNearbyProbes(F(p), probes, k);
	for(int i=0; i<probes; i++)
		for (auto it = hypercube->begin(nearbyProbes[i]); it != hypercube->end(nearbyProbes[i]); ++it )	{
			if(counter >= argM)
				break;
			hashtable_item_cube p_b = *it;
			float distance = Cube::distance(p,p_b.p);
			if(distance<=R)
				distances.insert({distance,p_b.index});
			counter++;
		}

	vector<pair<float,unsigned int>> R_Nearest;
	auto it=distances.begin();
	for(;it != distances.end();++it)
	{
		R_Nearest.push_back({it->first,it->second});
	}

	return R_Nearest;
}


Cube::~Cube()//Destructor
{
	return;
};
