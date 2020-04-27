#include<iostream>
#include<sstream>
#include<unordered_map>
#include<unordered_set>
#include<algorithm>
#include<vector>
#include<set>
#include<map>
#include<list>
#include<string>
#include<string.h>
#include<fstream>
#include<ctime>
#include<stdio.h>
#include<cstdlib>
#include<cstdio>
#include<sys/mman.h>
#include<unistd.h>
#include<fcntl.h>
#include<mutex>
#define BUF_SIZE 40960
#define MAX_V 1000000
using namespace std;
map<int, set<int>> graph;
map<int, set<int>> rgraph;
vector<int> path;

set<int> unblockuse;
bool compare(const vector<int>& a, const vector<int>& b)
{
	if (a.size() == b.size())
	{
		return a < b;
	}
	else
		return a.size() < b.size();
}

set<vector<int>,decltype(compare)*> res(compare);
unordered_map<int, int> deleted;
unordered_map<int, int> blocked;
unordered_map<int, int> isincycle;
unordered_map<int, set<int>> blockmap;
vector<int> cycle;
bool used[MAX_V];
int cmp[MAX_V];
vector<int>vs;
map<int, set<int>> SCC;
vector<map<int, set<int>>> subGs;





void dfs_s(int v, map<int, set<int>>& G) {
	used[v] = true;
	for (auto e:G[v]) {
		if (!used[e])dfs_s(e, G);
	}
	vs.push_back(v);
}
void rdfs_s(int v, int k, map<int, set<int>>& rG) {
	used[v] = true;
	cmp[v] = k;
	SCC[k].insert(v);
	for (auto e:rG[v]) {
		if (!used[e])rdfs_s(e, k,rG);
	}
}

int scc(map<int, set<int>>& G, map<int, set<int>>& rG) {
	memset(used, 0, sizeof(used));
	vs.clear();
	for (auto e:G) {
		if (!used[e.first])dfs_s(e.first,G);
	}
	memset(used, 0, sizeof(used));
	int k = 0;
	for (int i = vs.size() - 1; i >= 0; i--) {
		if (!used[vs[i]])rdfs_s(vs[i], k++,rG);
	}
	return k;
}

void sep(map<int, set<int>>& G, map<int, set<int>>& rG){

	scc(G,rG);
	for (auto e : SCC) {
		unordered_set<int> subgraver;
		for (auto m : e.second) {
			subgraver.insert(m);
		}
		map<int, set<int>> tmp;
		for (auto m : e.second) {
			for (auto n : G[m]) {
				if (subgraver.find(n) != subgraver.end())tmp[m].insert(n);
			}
		}
		if(tmp.size()>2)subGs.push_back(tmp);
	}
}

void unblockmap(int ver){
     blocked[ver]=0;
     unblockuse.insert(ver);
     if(blockmap.find(ver)!=blockmap.end()){
         for(auto e:blockmap[ver]){
             if(unblockuse.find(e)==unblockuse.end())unblockmap(e);
         }
         blockmap.erase(ver);
     }
}
int seq=1;
bool islim=false;
//int cirline=1;
void dfs(map<int, set<int>>& m_graph) {
	if (path.size() > 7)return;
	int ver = path.back();
        bool flag=false;
	set<int> tmpblock;
	for (auto e : m_graph[ver]) {
	    if(1 != deleted[e]){
            if(1 ==blocked[e]) {
			auto iter = path.begin();
			for (; iter != path.end(); iter++) {
				if (*iter == e)break;
			}
			auto left = iter;
			auto localmin = iter;
			for (; iter != path.end(); iter++) {
				if (*iter < *localmin)localmin = iter;
			}
			for (auto i = localmin; i != path.end(); i++)cycle.push_back(*i);
			for (auto i = left; i != localmin; i++)cycle.push_back(*i);
			if(cycle.size()>2&&cycle.size()<=7){
			                        cout<<seq<<"new cycle:";seq++;
                        for(auto e:cycle)cout<<e<<' ';
                        cout<<endl;
			      res.insert(cycle);
			}

			cycle.clear();
		        if(e != path[0])tmpblock.insert(e);
                        continue;
                }
		    else{
		            
                            path.push_back(e);
			    blocked[e] = 1;
			    dfs(m_graph);			blocked[e] = 0;
			    path.pop_back();
		    }
	    }
	}

	if(0==isincycle[ver]){
	    for (auto e : tmpblock) {
                 blockmap[e].insert(ver);
            }
	}
	else{
             unblockmap(ver);
	}

}


//int ccline=1;
void make_graph(){

    //int fd=open("/home/lalawang/Desktop/new_test_data.txt",O_RDONLY);
    int fd=open("/home/lalawang/Desktop/test_data2.txt",O_RDONLY);
    //int fd=open("/home/lalawang/Desktop/test_data2.txt",O_RDONLY);
    FILE* f=fdopen(fd,"rw");
    fseek(f,0,SEEK_END);
    long ps=ftell(f);
    //cout<<ps<<endl;

    char* p = (char*)mmap(NULL, ps, PROT_READ, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED)
    {
       perror("mmap error");
       exit(1);
    }

    int i=0;
    int c1=0;
    int c2=0;
    while(i<ps){
        c1=0;
	while((*(p+i))<'0' || (*(p+i))>'9')i++;
        while((*(p+i))>='0' && (*(p+i))<='9'){
            c1 = c1*10 + *(p+i)-'0';
            i++;

        }
        while((*(p+i))<'0' || (*(p+i))>'9')i++;
        c2=0;
        while((*(p+i))>='0' && (*(p+i))<='9'){
            c2 = c2*10 + *(p+i)-'0';
            i++;
        }
        while((*(p+i))<'0' || (*(p+i))>'9')i++;
        while((*(p+i))>='0' && (*(p+i))<='9'){
            i++;
        }
        if(i>=ps)return;
        //while((*(p+i))<'0' || (*(p+i))>'9')i++;
        ///cout<<"the "<<ccline<<"th line:";
        //cout<<"c1:"<<c1<<"  "<<"c2:"<<c2<<endl;
        //ccline++;
        blocked[c1]=0;
        //visited[c1]=0;
        blocked[c2]=0;
        //visited[c2]=0;
        graph[c1].insert(c2);
        rgraph[c2].insert(c1);

    }
}
void output(string outpath,vector<vector<int>>& res){
    char buf[BUF_SIZE];
    FILE* out=fopen(outpath.c_str(),"w");
    setvbuf ( out, buf, _IOFBF , BUF_SIZE );
    fprintf(out, "%d\r\n", (int)res.size());
    int j=0;
    for(;j<res.size()-1;j++){
        for(int i = 0; i < res[j].size() - 1; ++i){
            fprintf(out, "%d,", res[j][i]);
	}
	fprintf(out, "%d\n", res[j].back());
    }
    for(int i=0; i < res[j].size() - 1; ++i){
         fprintf(out, "%d,", res[j][i]);
    }
    fprintf(out, "%d", res[j].back());
}
void show() {
    int ver=0;int edge=0;
	for (auto e : graph) {
		ver++;
		cout<<"the "<<e.first<<"th trans:";
		for (auto m : e.second) {
            cout<<m<<' ';
			edge++;
		}
		cout<<endl;
	}
    cout << "ver num:"<<ver<<endl;
    cout << "edge num:"<<edge<<endl;
}



void processgra(map<int, set<int>>& graph){
    int count=0;
    for (auto e:graph)
    {
    if(count==0)islim=true;
             path.clear();
             isincycle.clear();
             blockmap.clear();
             blocked.clear();
	     path.push_back(e.first);
	     blocked[e.first] = 1;
	     dfs(graph);
	     deleted[e.first] = 1;
	     
	     count--;
    }
}




int main()
{
    cout<<"begin to load.."<<endl;
    clock_t time_start1 = clock();
//------------------------------
    path.reserve(50);
    //graph.reserve(50);
    make_graph();
//------------------------------
    clock_t time_end1 = clock();
    //show();




    cout<<"begin to process.."<<endl;
    clock_t time_start2 = clock();
//---------------------------------
    sep(graph,rgraph);
    for(auto e:subGs)processgra(e);
    clock_t time_end2 = clock();
    vector<vector<int>> res2(res.begin(),res.end());

    cout<<"there are ";
    cout<<subGs.size()<<" subgraphs whose size bigger than 2."<<endl;
    int i=1;
    for(auto e:subGs){
        cout<<"the "<<i<<"th subgraph have "<<e.size()<<" vertexs."<<endl;
        i++;
    }
    cout<<"totally have "<<res.size()<<" cycles."<<endl;
    for(auto e:res){
        for(auto m:e){
            cout<<m<<' ';
        }
        cout<<endl;
    }
    cout<<"totally have "<<res.size()<<" cycles."<<endl;

//--------------------------------
    cout<<"begin to output........"<<endl;
    clock_t time_start3 = clock();
    output("/home/lalawang/Desktop/myresult.txt",res2);
    clock_t time_end3 = clock();


    cout<<"load data time:"<<endl;
    cout << 1000 * (double)(time_end1 - time_start1) / (double)CLOCKS_PER_SEC << "ms" << endl;

    cout<<"process data time:"<<endl;
    cout << 1000 * (double)(time_end2 - time_start2) / (double)CLOCKS_PER_SEC << "ms" << endl;

    cout<<"write data time:"<<endl;
    cout << 1000 * (double)(time_end3 - time_start3) / (double)CLOCKS_PER_SEC << "ms" << endl;
}
