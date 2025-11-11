#include <bits/stdc++.h>
using namespace std;

struct Heur{bool mrv=false,lcv=false,fc=false,prop=false;};
struct Stats{uint64_t nodes=0,forced=0,pruned=0;};
static uint64_t nowms(){using namespace std::chrono;return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();}
static uint64_t rng64(uint64_t&s){s^=s>>12;s^=s<<25;s^=s>>27;return s*2685821657736338717ULL;}

struct Args{int n=30;uint64_t maxv=1000,seed=12345;int trials=1;Heur H;};
static vector<string> splitcsv(const string&s){vector<string>v;string c;for(char x:s){if(x==','){if(!c.empty()){v.push_back(c);c.clear();}}else c.push_back(x);}if(!c.empty())v.push_back(c);return v;}
static void parse(int argc,char**argv,Args&A){
  for(int i=1;i<argc;i++){string a=argv[i];auto p=a.find('=');string k=a.substr(0,p),v=p==string::npos?string():a.substr(p+1);
    if(k=="--n")A.n=stoi(v);else if(k=="--max")A.maxv=stoull(v);else if(k=="--seed")A.seed=stoull(v);else if(k=="--trials")A.trials=stoi(v);
    else if(k=="--heuristics"){if(v=="all"){A.H={true,true,true,true};}else if(v=="none"){A.H={false,false,false,false};}else{auto xs=splitcsv(v);Heur H{};for(auto&t:xs){if(t=="mrv")H.mrv=true;else if(t=="lcv")H.lcv=true;else if(t=="fc")H.fc=true;else if(t=="prop")H.prop=true;}A.H=H;}}
  }
}
static string hstr(const Heur&H){vector<string>v;if(H.mrv)v.push_back("mrv");if(H.lcv)v.push_back("lcv");if(H.fc)v.push_back("fc");if(H.prop)v.push_back("prop");if(v.empty())return"none";string s;for(size_t i=0;i<v.size();++i){if(i)s+=',';s+=v[i];}return s;}

struct Solver{
  const vector<uint64_t>&a;Heur H;Stats S;vector<int>order,assign;vector<array<char,2>>dom;vector<tuple<int,int,char>>trailDom;vector<pair<int,int>>trailAsn;long long Scur=0,rem=0;unsigned __int128 T=0;
  Solver(const vector<uint64_t>&A,Heur H):a(A),H(H){assign.assign((int)a.size(),0);dom.assign((int)a.size(),{1,1});order.resize((int)a.size());iota(order.begin(),order.end(),0);for(auto x:a)T+=x;for(auto x:a)rem+=(long long)x;}
  bool setValue(int v,int s){
    if(assign[v]==s)return true; if(assign[v]==-s)return false;
    for(int t=0;t<2;t++){if(t!=(s==+1)){if(dom[v][t]){trailDom.emplace_back(v,t,dom[v][t]);dom[v][t]=0;}}}
    if(!dom[v][s==+1]){trailDom.emplace_back(v,(s==+1),0);dom[v][s==+1]=1;}
    if(!dom[v][0]&&!dom[v][1])return false;
    trailAsn.push_back({v,assign[v]});assign[v]=s;Scur+=(long long)s*(long long)a[v];rem-=(long long)a[v];S.forced++;return true;
  }
  void undo(size_t TA,size_t TD){
    while(trailAsn.size()>TA){auto[v,old]=trailAsn.back();trailAsn.pop_back();rem+=(long long)a[v];Scur-=(long long)assign[v]*(long long)a[v];assign[v]=old;}
    while(trailDom.size()>TD){auto[v,t,old]=trailDom.back();trailDom.pop_back();dom[v][t]=old;}
  }
  bool forward_check(){
    for(int idx=0;idx<(int)order.size();++idx){int v=order[idx];if(assign[v]!=0)continue;
      for(int t=0;t<2;t++){if(!dom[v][t])continue;int s=t?+1:-1;long long r2=rem-(long long)a[v];long long sc=Scur+(long long)s*(long long)a[v];
        if(llabs(sc)>r2){trailDom.emplace_back(v,t,1);dom[v][t]=0;S.pruned++;if(!dom[v][0]&&!dom[v][1])return false;}
      }
    }
    return true;
  }
  bool propagate(){
    bool ch=true;
    while(ch){
      ch=false;
      for(int idx=0;idx<(int)order.size();++idx){int v=order[idx];if(assign[v]!=0)continue;long long Rwo=rem-(long long)a[v];
        if(Scur>Rwo){if(!setValue(v,-1))return false;ch=true;}
        else if(Scur<-Rwo){if(!setValue(v,+1))return false;ch=true;}
      }
      if(H.fc){if(!forward_check())return false;}
    }
    return true;
  }
  int pickVar(){
    if(H.mrv){int best=-1,bs=3;for(int idx=0;idx<(int)order.size();++idx){int v=order[idx];if(assign[v]!=0)continue;int s=dom[v][0]+dom[v][1];if(s<bs){bs=s;best=v;if(s==1)break;}}return best;}
    else{for(int idx=0;idx<(int)order.size();++idx){int v=order[idx];if(assign[v]==0)return v;}return -1;}
  }
  vector<int> valueOrder(int v){
    if(H.lcv){long long dpos=llabs(Scur+(long long)a[v]);long long dneg=llabs(Scur-(long long)a[v]);if(dneg<dpos)return{-1,+1};else return{+1,-1};}
    return{+1,-1};
  }
  bool dfs(){
    bool done=true;for(int i=0;i<(int)a.size();++i)if(assign[i]==0){done=false;break;}
    if(done)return Scur==0;
    int v=pickVar();if(v<0)return false;S.nodes++;auto ord=valueOrder(v);size_t TA=trailAsn.size(),TD=trailDom.size();
    for(int s:ord){if(!dom[v][s==+1])continue;if(!setValue(v,s)){undo(TA,TD);continue;}size_t TA2=trailAsn.size(),TD2=trailDom.size();bool ok=true;
      if(H.prop)ok=propagate();else if(H.fc)ok=forward_check();
      if(ok&&dfs())return true;undo(TA2,TD2);undo(TA,TD);
    }
    return false;
  }
  bool solve(){if(((uint64_t)T&1ULL)!=0)return false;return dfs();}
};

static vector<uint64_t> gen(int n,uint64_t maxv,uint64_t&seed){vector<uint64_t>a(n);for(int i=0;i<n;i++)a[i]=(rng64(seed)%maxv)+1ULL;return a;}

int main(int argc,char**argv){
  ios::sync_with_stdio(false);cin.tie(nullptr);
  Args A;parse(argc,argv,A);
  cout<<"heuristics="<<hstr(A.H)<<" n="<<A.n<<" max="<<A.maxv<<" trials="<<A.trials<<"\n";
  for(int t=0;t<A.trials;t++){
    uint64_t s=A.seed+101u*t+7u;auto a=gen(A.n,A.maxv,s);uint64_t t0=nowms();Solver sv(a,A.H);bool sat=sv.solve();uint64_t ms=nowms()-t0;
  }
  return 0;
}
