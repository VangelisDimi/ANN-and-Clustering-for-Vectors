#include "lsh.hpp"
#include <math.h>
#include <numeric>
#include <map>
#include <fstream>
#include <chrono>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

unsigned int LSH::g(vector<float> p,unsigned int j){
	return modulo(ID(p,j),tableSize);
}

unsigned long long int LSH::ID(vector<float> p,unsigned int j) {
	unsigned long long int *h = new unsigned long long int[k];
	for(int i=0; i<k; i++)
		h[i] = modulo(hash_L2(i,p,v[j],t[j],w),M);
	unsigned long long int _g = 0;
	for(int i=0; i<k; i++)
		_g += modulo((r[i]*h[i]),M);
	return modulo(_g,M);
}

vector<pair<float,unsigned int>> LSH::find_N_nearest(vector<float> p,unsigned int N)
{
	//Returns indexes of N Nearest elements
	multimap<float, int> distances;
	for (int y=0 ; y<L ; y++)
	{
		unsigned long long int ID = LSH::ID(p,y);
		for (auto it = hashtables[y].begin(modulo(ID,tableSize)); it != hashtables[y].end(modulo(ID,tableSize)); ++it )
		{
			hashtable_item_lsh p_b = *it;
			if (p_b.ID == ID)
			{
				float distance = LSH::distance(p,p_b.p);
				if(distances.find(distance) == distances.end() || distances.find(distance)->second != p_b.index)
					distances.insert({distance,p_b.index});
			}
		}
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

vector<pair<float,unsigned int>> LSH::find_R_nearest(vector<float> p,int R)
{
	//Returns indexes of R nearest element
	multimap<float, int> distances;
	for (int y=0 ; y<L ; y++)
	{
		unsigned long long int ID = LSH::ID(p,y);
		for (auto it = hashtables[y].begin(modulo(ID,tableSize)); it != hashtables[y].end(modulo(ID,tableSize)); ++it )
		{
			hashtable_item_lsh p_b = *it;
			if (p_b.ID == ID)
			{
				float distance = LSH::distance(p,p_b.p);
				if(distance<=R)
				{
					if(distances.find(distance) == distances.end() || distances.find(distance)->second != p_b.index)
						distances.insert({distance,p_b.index});
				}
			}
		}
	}

	vector<pair<float,unsigned int>> R_Nearest;
	auto it=distances.begin();
	for(;it != distances.end();++it)
	{
		R_Nearest.push_back({it->first,it->second});
	}

	return R_Nearest;
}

LSH::LSH(vector<vector<float>> input_vectors,vector<string> input_ids,int k,int L,int metric)//Constructor
{
	vectors=input_vectors;
	ids=input_ids;

	//Initialize values
	LSH::L=L;
	LSH::k=k;
	w=300;//Should be average of vector distances times 3
	vectorSize=vectors[0].size();
	n=ids.size();
	tableSize=n/4;
	hashtables = new hash_table<hashtable_item_lsh>[L];
	for (int i = 0; i < L; i++)
	{
		hashtables[i]=hash_table<hashtable_item_lsh>(tableSize);
	}
	
	v = new vector<float>*[L];
	t = new float*[L];

	for (int x = 0; x < k; x++)
	{
		r.push_back(uniform_distribution_rng(0,INT32_MAX));
	}
	for (int i = 0; i < L; i++)
	{
		v[i] = new vector<float>[k];
		t[i] = new float[k];
		for (int x = 0; x < k; x++)
		{
			t[i][x]=uniform_distribution_rng(0,w-1);
			for (int y = 0; y < vectorSize; y++)
			{
				v[i][x].push_back(normal_distribution_rng());
			}
		}
	}
	if(metric==L2)
		distance=&eucledian_distance;

	//Add vectors to L hashtables
	for(int i = 0;i<n;i++)
	{
		for(int y=0;y<L;y++)
		{
			hashtable_item_lsh p{vectors[i],ID(vectors[i],y),i};
			hashtables[y].insert(modulo(p.ID,tableSize),p);
		}
	}
};

LSH::~LSH()//Destructor
{
	return;
};