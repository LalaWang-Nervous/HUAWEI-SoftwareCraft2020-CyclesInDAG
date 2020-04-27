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

using namespace std;

//----------------------------全局变量声明-----------------------------
map<int, set<int>> graph;
map<int, set<int>> rgraph;
vector<int> path;
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
unordered_map<int, int> visited;
unordered_map<int, int> blocked;
vector<int> cycle;
#define BUF_SIZE 40960
#define MAX_V 1000000
bool used[MAX_V];
int cmp[MAX_V];
vector<int>vs;
map<int, set<int>> SCC;
vector<map<int, set<int>>> subGs;
//-----------------------------END-----------------------------------------


//---------------------------对大图进行分割-----------------------------------
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
//-----------------------------END-----------------------------------------



//------------------------------------根据文件构图、根据图输出文件----------------------
void make_graph(){
    int fd=open("/home/lalawang/Desktop/test_data.txt",O_RDONLY);
    FILE* f=fdopen(fd,"rw");
    fseek(f,0,SEEK_END);
    long ps=ftell(f);
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
        blocked[c1]=0;
        visited[c1]=0;
        blocked[c2]=0;
        visited[c2]=0;
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
//-------------------------------------------END-----------------------------------


//---------------------------------深度优先遍历存储循环路径---------------------
void dfs(map<int, set<int>>& m_graph) {
	if (path.size() > 7)return;
	int ver = path.back();
	for (auto e : m_graph[ver]) {
		if (1 == blocked[e]) {
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
			if (cycle.size() > 2 && cycle.size() <= 7) {
				res.insert(cycle);
			}
			cycle.clear();
			continue;
		}
		else {
			path.push_back(e);
			blocked[e] = 1;
			dfs(m_graph);
			blocked[e] = 0;
			path.pop_back();
		}
	}
}
void processgra(map<int, set<int>>& graph){
    for (auto e:graph)
    {
		path.clear();
		path.push_back(e.first);
		blocked[e.first] = 1;
		dfs(graph);
    }
}
//-------------------------------------------END-----------------------------------


int main()
{
    cout<<"begin to load.."<<endl;
    clock_t time_start1 = clock();
//------------------------------
    path.reserve(50);
    make_graph();
//------------------------------
    clock_t time_end1 = clock();
    cout<<"begin to process.."<<endl;
    clock_t time_start2 = clock();
//---------------------------------
    sep(graph,rgraph);
    cout<<"there are ";
    cout<<subGs.size()<<" subgraphs whose size bigger than 2."<<endl;
    int i=1;
    for(auto e:subGs){
        cout<<"the "<<i<<"th subgraph have "<<e.size()<<" vertexs."<<endl;
        i++;
    }
    for(auto e:subGs)processgra(e);

    vector<vector<int>> res2(res.begin(),res.end());
    cout<<"totally have "<<res.size()<<" cycles."<<endl;
    for(auto e:res){
        for(auto m:e){
            cout<<m<<' ';
        }
        cout<<endl;
    }
    clock_t time_end2 = clock();

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
