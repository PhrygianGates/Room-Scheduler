#include<fstream>
#include<iostream>
#include<string>
#include<queue>
#include<vector>
using namespace std;

//contruct a graph
const int SIZE = 1001;
vector<vector<int> > graph(SIZE, vector<int>(SIZE));
//the start position of different kinds of nodes in the graph
int SOURCE = 0, TARGET = 1000, PEOPLE = 1, DAYS = 100, ROOMS = 900,
weekdayRoomsNum = 8, weekendRoomsNum = 8, totalRoomsNum = 56;
//some other arrays to record data
vector<string> names;
vector<int> priorities;
vector<vector<int> > flows08(SIZE, vector<int>(SIZE));
vector<vector<int> > flows05(SIZE, vector<int>(SIZE));


//some subroutines
int getPeopleIndex(int k) { // k is the kth person
	return PEOPLE + k;
}
int getDaysIndex(int k, int j) { //k is the kth room, j is the jth person
	int res = 0;
	for (int i = 0; i < 6; i++) {
		if (k < weekdayRoomsNum) {
			break;
		}
		k -= weekdayRoomsNum;
		res++;
	}
	for (int i = 6; i < 8; i++) {
		if (k < weekendRoomsNum) {
			break;
		}
		k -= weekendRoomsNum;
		res++;
	}
	return DAYS + res + 7 * j;
}
int getRoomsIndex(int k) { //k is the kth room
	return ROOMS + k;
}
bool bfs(int s, int t, vector<int>& edgeTo, vector<vector<int> >& rg) {
	queue<int> q;
	vector<bool> visit(SIZE);
	for (int i = 0; i < SIZE; i++) {
		edgeTo[i] = -1;
		visit[i] = false;
	}
	edgeTo[s] = s;
	visit[s] = true;
	q.push(s);
	int p;
	while (!q.empty()) {
		p = q.front();
		q.pop();
		for (int i = 0; i < SIZE; i++) {
			int u = (p + i + 1) % SIZE;
			if (rg[p][u] > 0 && !visit[u]) {
				edgeTo[u] = p;
				visit[u] = true;
				if (u == t) return true;
				q.push(u);
			}
		}
	}
	return false;
}
void EdmondKarp(int s, int t, vector<vector<int> >& rg, vector<vector<int> >& flows) {
	int i;
	vector<int> edgeTo(SIZE);
	while (bfs(s, t, edgeTo, rg)) {
		for (i = t; i != s; i = edgeTo[i]) {
			rg[edgeTo[i]][i]--;
			flows[edgeTo[i]][i]++;
			rg[i][edgeTo[i]]++;
			flows[i][edgeTo[i]]--;
		}
	}
}



void read() {
	ifstream in("2020春季学期前八周琴房时间统计.csv");
	/*the first line is nothing, second name, third priority, 
	forth number of rooms every weeks, last remarks*/

	string s;	//use s to get the contents
	int rowIndex = 0, columnIndex = 0;	//to catch where we are
	while (in.good()) {
		getline(in, s, ',');
		if (columnIndex == 1) {
			names.push_back(s);
		}
		else if (columnIndex == 2) {
			priorities.push_back(atoi(s.c_str()));
		}
		else if (columnIndex == 3) {
			graph[SOURCE][getPeopleIndex(rowIndex)] = atoi(s.c_str());
		}
		else if (columnIndex >= 4 && columnIndex < totalRoomsNum + 4 && atoi(s.c_str())) {
			graph[getPeopleIndex(rowIndex)][getDaysIndex(columnIndex - 4, rowIndex)] = 1;
			graph[getDaysIndex(columnIndex - 4, rowIndex)][getRoomsIndex(columnIndex - 4)] = 1;
		}
		else if (columnIndex == totalRoomsNum + 4) {
			columnIndex = 0;
			rowIndex++;
		}
		columnIndex++;
	}
	in.close();
}
void process() {
	//process 208 room for those with priority 1
	vector<vector<int> > rg(graph);
	for (int i = 0; i < totalRoomsNum; i++) {
		rg[getRoomsIndex(i)][TARGET] = 1;
	}
	for (int i = 0; i < names.size(); i++) {
		if (priorities[i] == 1) {
			rg[SOURCE][getPeopleIndex(i)] = 1;
		}
		else {
			rg[SOURCE][getPeopleIndex(i)] = 0;
		}
	}
	EdmondKarp(SOURCE, TARGET, rg, flows08);
	//process 208 room for those with priority 2 or 3
	for (int i = 0; i < SIZE - 1; i++) {
		for (int j = 0; j < SIZE - 1; j++) {
			rg[i][j] = graph[i][j];
		}
	}//don't change the edge connected to TARGET in rg
	for (int i = 0; i < names.size(); i++) {
		if (priorities[i] == 2 || priorities[i] == 3) {
			rg[SOURCE][getPeopleIndex(i)] = 1;
		}
		else {
			rg[SOURCE][getPeopleIndex(i)] = 0;
		}
	}
	EdmondKarp(SOURCE, TARGET, rg, flows08);
	//process 205 room for those with priority 1 or 2
	rg = graph;
	for (int i = 0; i < totalRoomsNum; i++) {
		rg[getRoomsIndex(i)][TARGET] = 1;
	}
	for (int i = 0; i < names.size(); i++) {
		if (priorities[i] == 1 || priorities[i] == 2) {
			rg[SOURCE][getPeopleIndex(i)] -= flows08[SOURCE][getPeopleIndex(i)];
		}
		else {
			rg[SOURCE][getPeopleIndex(i)] = 0;
		}
	}
	EdmondKarp(SOURCE, TARGET, rg, flows05);
	//process 205 room for those with priority 3 or 4
	for (int i = 0; i < SIZE - 1; i++) {
		for (int j = 0; j < SIZE - 1; j++) {
			rg[i][j] = graph[i][j];
		}
	}
	for (int i = 0; i < names.size(); i++) {
		if (priorities[i] == 3 || priorities[i] == 4) {
			rg[SOURCE][getPeopleIndex(i)] -= flows08[SOURCE][getPeopleIndex(i)];
		}
		else {
			rg[SOURCE][getPeopleIndex(i)] = 0;
		}
	}
	EdmondKarp(SOURCE, TARGET, rg, flows05);
}
void write() {
	//strings to write
	vector<vector<string> > times(2, vector<string>(8));
	times[0][0] = "8:00-10:00,";
	times[0][1] = "10:00-12:00,";
	times[0][2] = "12:00-13:30,";
	times[0][3] = "13:30-15:00,";
	times[0][4] = "15:00-17:00,";
	times[0][5] = "17:00-18:30,";
	times[0][6] = "18:30-20:00,";
	times[0][7] = "20:00-22:00,";
	times[1][0] = "8:00-10:00,";
	times[1][1] = "10:00-12:00,";
	times[1][2] = "12:00-13:30,";
	times[1][3] = "13:30-15:00,";
	times[1][4] = "15:00-17:00,";
	times[1][5] = "17:00-18:30,";
	times[1][6] = "18:30-20:00,";
	times[1][7] = "20:00-22:00,";
	//begin to write
	ofstream out;
	out.open("2020春季学期前八周琴表.csv", ios::out | ios::trunc);
	out << "208,周一,周二,周三,周四,周五, ,周六,周日" << endl;
	//write 208 room
	for (int i = 0; i < 10; i++) {
		//write Monday to Friday
		if (i < weekdayRoomsNum) {
			out << times[0][i];
			for (int j = 0; j < 5; j++) {
				bool flag = false;
				for (int k = 0; k < names.size(); k++) {
					if (flows08[DAYS + j + k * 7][getRoomsIndex(j * weekdayRoomsNum + i)]) {
						flag = true;
						out << names[k] << ",";
						flows08[DAYS + j + k * 7][getRoomsIndex(j * weekdayRoomsNum + i)]--;
						break;
					}
				}
				if (!flag) {
					out << ",";
				}
			}
		}
		if (i >= weekdayRoomsNum) {
			out << ", , , , , ,";
		}
		if (i == 9) {
			out << endl;
		}
		//write weekends
		if (i < weekendRoomsNum) {
			out << times[1][i];
			for (int j = 0; j < 2; j++) {
				bool flag = false;
				for (int k = 0; k < names.size(); k++) {
					if (flows08[DAYS + j + 5 + k * 7][getRoomsIndex(5 * weekdayRoomsNum + j * weekendRoomsNum + i)]) {
						flag = true;
						out << names[k] << ",";
						flows08[DAYS + j + 5 + k * 7][getRoomsIndex(5 * weekdayRoomsNum + j * weekendRoomsNum + i)]--;
						break;
					}
				}
				if (!flag) {
					out << ",";
				}
			}
		}
		out << endl;
	}
	//write 205 room
	out << "205,周一,周二,周三,周四,周五, ,周六,周日" << endl;
	for (int i = 0; i < 10; i++) {
		//write Monday to Friday
		if (i < weekdayRoomsNum) {
			out << times[0][i];
			for (int j = 0; j < 5; j++) {
				bool flag = false;
				for (int k = 0; k < names.size(); k++) {
					if (flows05[DAYS + j + k * 7][getRoomsIndex(j * weekdayRoomsNum + i)]) {
						flag = true;
						out << names[k] << ",";
						flows05[DAYS + j + k * 7][getRoomsIndex(j * weekdayRoomsNum + i)]--;
						break;
					}
				}
				if (!flag) {
					out << ",";
				}
			}
		}
		if (i >= weekdayRoomsNum) {
			out << ", , , , , ,";
		}
		if (i == 9) {
			out << endl;
		}
		//write weekends
		if (i < weekendRoomsNum) {
			out << times[1][i];
			for (int j = 0; j < 2; j++) {
				bool flag = false;
				for (int k = 0; k < names.size(); k++) {
					if (flows05[DAYS + j + 5 + k * 7][getRoomsIndex(5 * weekdayRoomsNum + j * weekendRoomsNum + i)]) {
						flag = true;
						out << names[k] << ",";
						flows05[DAYS + j + 5 + k * 7][getRoomsIndex(5 * weekdayRoomsNum + j * weekendRoomsNum + i)]--;
						break;
					}
				}
				if (!flag) {
					out << ",";
				}
			}
		}
		out << endl;
	}
	out.close();
}


int main() {
	read();
	process();
	write();
	return 0;
}