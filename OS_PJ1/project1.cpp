#include <queue>
#include <stdio.h>
#include <vector>

using namespace std;

typedef struct{
	int PID;
	int CPU_BT;
	int IO_BT;
	int PRIORITY;		
	int NUM_CYCLE;
	int BT;
}ps;

struct compare {
	bool operator()(const ps& ps1, const ps& ps2) {
		return ps1.CPU_BT < ps2.CPU_BT;
	}
};

queue <ps> RQ0; // 0 : PID, 1 : CPU_BT, 2 : IO_BT, 3 : PRIOIRTY, 4 : NUM_CYCLE, 5 : BT
queue <ps> RQ1; // 0 : PID, 1 : CPU_BT, 2 : IO_BT, 3 : PRIOIRTY, 4 : NUM_CYCLE, 5 : BT
priority_queue <ps, vector<ps>, compare> RQ2; // 0 : PID, 1 : CPU_BT, 2 : IO_BT, 3 : PRIOIRTY, 4 : NUM_CYCLE, 5 : BT
vector <ps> IOQ; // 0 : PID, 1 : CPU_BT, 2 : IO_BT, 3 : PRIOIRTY, 4 : NUM_CYCLE, 5 : BT

/* CPU REG */
int CPU_CPU_BT;
int CPU_IO_BT;
int CPU_PID;
int CPU_PRIORITY;
int CPU_NUM_CYCLE;
int CPU_TIME_SLICE;
int CPU_BT;
int CPU_USED = 0;
int CPU_FLAG[100000];

void INSERT_TO_CPU(int q){
	if(q == 0){
		CPU_CPU_BT = RQ0.front().CPU_BT;
		CPU_IO_BT = RQ0.front().IO_BT;
		CPU_PID = RQ0.front().PID;
		CPU_PRIORITY = RQ0.front().PRIORITY;
		CPU_NUM_CYCLE = RQ0.front().NUM_CYCLE;
		CPU_BT = RQ0.front().BT;
		CPU_TIME_SLICE = 2;
		CPU_USED = 1;
		RQ0.pop();
	}
	else if(q == 1){
		CPU_CPU_BT = RQ1.front().CPU_BT;
		CPU_IO_BT = RQ1.front().IO_BT;
		CPU_PID = RQ1.front().PID;
		CPU_PRIORITY = RQ1.front().PRIORITY;
		CPU_NUM_CYCLE = RQ1.front().NUM_CYCLE;
		CPU_BT = RQ1.front().BT;
		CPU_TIME_SLICE = 4;
		CPU_USED = 1;
		RQ1.pop();
	}
	else if(q == 2){
		CPU_CPU_BT = -RQ2.top().CPU_BT;
		CPU_IO_BT = RQ2.top().IO_BT;
		CPU_PID = RQ2.top().PID;
		CPU_PRIORITY = RQ2.top().PRIORITY;
		CPU_NUM_CYCLE = RQ2.top().NUM_CYCLE;
		CPU_BT = RQ2.top().BT;
		CPU_USED = 1;
		RQ2.pop();
	}
}

void scheduling(){
	if(!RQ0.empty()){
		INSERT_TO_CPU(0);
	}
	else if(!RQ1.empty()){
		INSERT_TO_CPU(1);
	}
	else if(!RQ2.empty()){
		INSERT_TO_CPU(2);
	}
}

int main(){

	/* 1. DATA INPUT */
	vector <int> info_ps [1001]; //0 : PID, 1 : AT, 2: INIT_Q, 3: NUM_CYCLE, 4 : TT, 5 : WT, 6 ~ : CPU_BT, 7 ~ : IO_BT
	int pid, at, init_q, num_cycle;
	int cpu_bt, io_bt;
	int num_ps;		
	FILE *fp = fopen("input.txt","r");
	fscanf(fp,"%d", &num_ps);
	for (int i = 1 ; i <= num_ps ; i++) {
		fscanf(fp, "%d %d %d %d", &pid, &at, &init_q, &num_cycle);
		info_ps[i].push_back(pid);
		info_ps[i].push_back(at);
		info_ps[i].push_back(init_q);
		info_ps[i].push_back(num_cycle);
		info_ps[i].push_back(0); //TT
		info_ps[i].push_back(0); //WT
		for (int j = 0; j < num_cycle ; j++) {
			if(j < num_cycle - 1) {
				fscanf(fp, "%d %d", &cpu_bt, &io_bt);
				if(cpu_bt == 0 || io_bt == 0){
					printf("invalid input in burst info\n");
					return 0;
				}
				info_ps[i].push_back(cpu_bt);
				info_ps[i].push_back(io_bt);
			} else {
				fscanf(fp, "%d", &cpu_bt);
				if(cpu_bt == 0){
					printf("invalid input in burst info\n");
					return 0;
				}
				info_ps[i].push_back(cpu_bt);
				info_ps[i].push_back(0); // End of Burst
			}
		}
	}
	fclose(fp);

	int time = 0;
	int done_ps = 0;
	printf(" -------------\n");
	while(done_ps != num_ps){
		printf("%s%3d%-1s%6s","|    ",time,"s","|");
		/* 2. new process PUSH to RQ */
		for(int i = 1; i <= num_ps ; i++){ 
			if(info_ps[i][1] == time){ //AT == time
				printf("<-ps%d",info_ps[i][0]);
				if(info_ps[i][2] == 0){ //INIT_Q == 0
					RQ0.push({info_ps[i][0], info_ps[i][6], info_ps[i][7], 0, 1, 0});
				}
				else if(info_ps[i][2] == 1){ //INIT_Q == 1
					RQ1.push({info_ps[i][0], info_ps[i][6], info_ps[i][7], 1, 1, 0});
				}
				else if(info_ps[i][2] == 2){ //INIT_Q == 2
					RQ2.push({info_ps[i][0], -info_ps[i][6], info_ps[i][7], 2, 1, 0});
				}
			}
		}
		printf("\n");
		printf(" -------------\n");

		/* 3. SCHEDULING */
		/* NO CPU USE */
		if(CPU_USED == 0){ 
			scheduling();
		}
		/* PREEMTION */
		else {  
			if(CPU_TIME_SLICE == 0 && CPU_PRIORITY == 0){ /* RQ0 -> RQ1 */
				RQ1.push({CPU_PID, CPU_CPU_BT, CPU_IO_BT, 1, CPU_NUM_CYCLE, CPU_BT});
				scheduling();
			}
			else if(CPU_TIME_SLICE == 0 && CPU_PRIORITY == 1) { /* RQ1 -> RQ2 */
				RQ2.push({CPU_PID, -CPU_CPU_BT, CPU_IO_BT, 2, CPU_NUM_CYCLE, CPU_BT});
				scheduling();
			}
			else if(!RQ2.empty() && CPU_PRIORITY == 2){
				if(CPU_CPU_BT > -RQ2.top().CPU_BT){
					RQ2.push({CPU_PID, -CPU_CPU_BT, CPU_IO_BT, 2, CPU_NUM_CYCLE, CPU_BT});
					INSERT_TO_CPU(2);
				}
			}
		}
		
		/* let's consider 1s elapsed */
		/* 4. IO burst */ 
		if(!IOQ.empty()){
			for(int i = 0; i < IOQ.size(); i++){
				IOQ[i].IO_BT -= 1;
				if(IOQ[i].IO_BT == 0){
					int IO_PID = IOQ[i].PID;
					int IO_NUM_CYCLE = IOQ[i].NUM_CYCLE + 1; //NEXT CYCLE
					if(IOQ[i].PRIORITY == 0){ //PRIORITY 0 - > 0
						RQ0.push({IO_PID, info_ps[IO_PID][2 * IO_NUM_CYCLE + 4], info_ps[IO_PID][2 * IO_NUM_CYCLE + 5], 0, IO_NUM_CYCLE, IOQ[i].BT});
					}
					else if(IOQ[i].PRIORITY == 1){ //PRIORITY 1 -> 0
						RQ0.push({IO_PID, info_ps[IO_PID][2 * IO_NUM_CYCLE + 4], info_ps[IO_PID][2 * IO_NUM_CYCLE + 5], 0, IO_NUM_CYCLE, IOQ[i].BT});
					}
					else if(IOQ[i].PRIORITY == 2){ //PRIORITY 2 -> 1
						RQ1.push({IO_PID, info_ps[IO_PID][2 * IO_NUM_CYCLE + 4], info_ps[IO_PID][2 * IO_NUM_CYCLE + 5], 1, IO_NUM_CYCLE, IOQ[i].BT});
					}
					IOQ.erase(IOQ.begin() + i);
				}
			}
		}

		/* 5. CPU burst */ 
		if(CPU_USED == 1){
			CPU_CPU_BT -= 1;
			CPU_BT += 1;
			CPU_TIME_SLICE -= 1;
			CPU_FLAG[time] = CPU_PID;

			printf("%s%d%6s","|     ps",CPU_FLAG[time],"|");

			if(CPU_CPU_BT == 0 && CPU_IO_BT == 0){
				done_ps += 1;
				printf("->ps%d\n", CPU_PID);
				printf(" -------------\n");
				info_ps[CPU_PID][4] = time - info_ps[CPU_PID][1] + 1; //TT
				info_ps[CPU_PID][5] = info_ps[CPU_PID][4] - CPU_BT; //WT
				CPU_USED = 0;
			} 
			else if(CPU_CPU_BT == 0 && CPU_IO_BT != 0){
				IOQ.push_back({CPU_PID, CPU_CPU_BT, CPU_IO_BT, CPU_PRIORITY, CPU_NUM_CYCLE, CPU_BT});
				CPU_USED = 0;
				printf("\n");
				printf(" -------------\n");
			} 
			else{
				printf("\n");
				printf(" -------------\n");
			}
		}
		else{
			printf("|     IDLE    |\n");
			printf(" -------------\n");
		}
		time += 1;
	}
	printf("%s%3d%-1s%6s","|    ",time,"s","|");
	printf("\n -------------\n");


	printf("==================================================================\n");
	int TT_sum = 0;
	int WT_sum = 0;
	for(int i = 1; i <= num_ps; i++){
		printf("Process%d : TT %d , WT %d\n", i, info_ps[i][4], info_ps[i][5]);
		TT_sum += info_ps[i][4];
		WT_sum += info_ps[i][5];
	}
	printf("Avg TT = %.1f, Avg WT = %.1f\n", float(TT_sum)/num_ps, float(WT_sum)/num_ps);
	printf("==================================================================\n");

}
