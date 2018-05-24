#include <bits/stdc++.h>
#include <mpi.h>
#include <md5lib/md5.cpp>
#define RESULT 60
#define DATA 40
using namespace std;

string alphabet = "abcde";
string plaintext = "cbaabbaab";
string startText = ""
int weight[300], rWeight[300];
int base =  alphabet.length();
typedef long long Long;
// weight : char -> int
// rWeight : int -> char
void initWeight(){
	memset(weight,0,sizeof(weight));	
	for(int i=0;i<alphabet.length();i++){
		weight[alphabet[i]] = i;
		rWeight[i] = alphabet[i];
	}
}

Long convertStrToLong(string str){
	Long ans = 0;
	Long fac = 1;
	for(int i=0;i<=str.length();i++){
		ans += fac*weight[str[i]];
		fac*= base;		
	}
	return ans;
}

string convertLongToStr(Long num){
	string ans;
	while(num>0){
		ans.push_back(rWeight[num%base]);		
		num /= base;
	}
	return ans;
}

string nextString(string str){
	str = alphabet[base-1] + str;
	reverse(str.begin(),str.end());
	Long code = convertStrToLong(str);
	code ++;
	string ret = convertLongToStr(code);			
	reverse(ret.begin(),ret.end());	
	return ret.substr(1);
}

string generate_pass(string startpass, string endpass, string pass){
	string s = startpass;	
	int dem = 0;
	while(!endpass.compare(s)){		
		dem ++;
		if(dem %10000 == 0)cout << dem << "  ";
		if(md5(s).compare(pass) == 0){
			cout << "\n\n\nFound it :" << s;
			return s;
		}
		s = nextString(s);
	}
	cout << "\nNot found in that range";
	return "";
}

int np=0;
int rank0(int len_pass) {
    char pass[len_pass];
    string passrs;
    MPI_Status status;    


    for (int i = 1; i < np; i++) {
        MPI_Recv(pass, len_pass, MPI_CHAR, i, RESULT, MPI_COMM_WORLD, &status);
        for (int i = 0; i < len_pass; i++) {
            passrs += pass[i];
        }
    }
    cout << passrs;
    return 0;
}

int ranki(string st_str, string en_str) {
    string t = generate_pass(st_str,en_str,md5(plaintext));
    char *p = new char[t.length()+1];
	strcpy(p,t.c_str());
    MPI_Send(p, t.length(), MPI_CHAR, 0, RESULT, MPI_COMM_WORLD);
    return 0;
}

int main(int argc, char** argv) {
    int rank;
    initWeight();	
    
    // startText, endText, plaintext
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    endText = convertLongToStr(convertStrToLong(plaintext)/2);
    string a_st = alphabet[base-1]+startText;
    string a_en = alphabet[base-1] + endText;
    Long num_start = convertStrToLong(a_st);
    Long number_test = convertStrToLong(a_en) - convertStrToLong(a_st) + 1;
    Long eachProcess = number_test / np + np;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if (rank == 0) {
        rank0(15);
    } else {
		string st = convertLongToStr(num_start + (rank-1)*eachProcess);
		string ed = convertLongToStr(num_start + (rank)*eachProcess)-1;
        ranki(st.substr(1),ed.substr(1));
    }
    MPI_Finalize();
    return 0;
}
