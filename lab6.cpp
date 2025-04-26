#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <list>

using namespace std;

struct Process
{
    char name;
    int arrivalt;
    int servicet;
    int initpriority;
    int curpriority;
    int finisht;
    int waitingt;
    int turnaroundt;
    int starttime;
    int remainingt; 
    bool finished;
    int currentQ;
    bool started;

    Process(char Name, int arrival, int service, int p) : name(Name), arrivalt(arrival), servicet(service), initpriority(p), curpriority(p), finisht(0), waitingt(0), turnaroundt(0), remainingt(servicet), finished(false), currentQ(0), started(false) {}
};

class PriorityQueue {
public:
    std::list<Process> processes;

    // Insert a new process into the queue, maintaining order
    void enqueue(const Process& p) {
        // Find the correct position to insert, ensuring stability
        auto index = processes.begin();
        while (index != processes.end() && index->curpriority >= p.curpriority) {
            ++index;
        }
        processes.insert(index, p);
    }

    Process dequeue() {
        if (processes.empty()) {
            throw std::out_of_range("Queue is empty");
        }
        Process firstProcess = processes.front();
        processes.pop_front(); // Remove the first process
        return firstProcess;
    }

    void incrementQueue() {
        for (auto& process : processes) {
            process.curpriority++;  // Increment the priority of each process by 1
        }
    }

    // Check if the queue is empty
    bool isEmpty() {
        return processes.empty();
    }
};

void parseinput(vector<Process> &processes, string &mode, vector<pair<int, int>> &policies, int &simulationEndTime)
{
    string line;
    string policy;
    getline(cin, mode);
    getline(cin, line);
    stringstream ss(line);

    while (getline(ss, policy, ','))
    {
        if (policy.find('-') != string::npos)
        {
            int algorithm = stoi(policy.substr(0, policy.find('-')));
            int quantum = stoi(policy.substr(policy.find('-') + 1));
            policies.emplace_back(algorithm, quantum);
        }
        else
            policies.emplace_back(stoi(policy), 0);
    }

    getline(cin, line);
    simulationEndTime = stoi(line);

    getline(cin, line);
    int processesNumber = stoi(line);

    for (int i = 0; i < processesNumber; i++)
    {
        getline(cin, line);
        stringstream ps(line);
        vector<string> parts;
        string part;
        while (getline(ps, part, ','))
        {
            parts.push_back(part);
        }

        char name = parts[0][0];
        int arrivaltime = stoi(parts[1]);
        int servicetime = stoi(parts[2]);
        int priority = 0;
        if (!policies.empty() && policies[0].first == 8) 
        {
            // If policy.first == 8, set priority to parts[2]
            priority = stoi(parts[2]);
            // servicetime = __INT_MAX__;
        }
        else if (parts.size() == 4) 
        {
            // Else, use the regular priority field (if available)
            priority = stoi(parts[3]);
        }
        // check if policy is 8, priority = stoi(parts[2])
        processes.emplace_back(name, arrivaltime, servicetime, priority);
    }
}

bool existinQ(queue<int> readyQ, int value)
{
    while (!readyQ.empty())
    {
        if (readyQ.front() == value)
        {
            return true;
        }
        readyQ.pop();
    }
    return false;
}

void tracePrint(const vector<Process> &processes, int endtime)
{
    for (int t = 0; t <= endtime; t++)
        cout << t % 10 << " ";

    cout << "\n------------------------------------------------\n";

    for (auto &process : processes)
    {
        cout << process.name << "     ";
        for (int i = 0; i <= endtime; ++i)
        {
            if (i < process.arrivalt || i > process.finisht)
                cout << "| ";

            else if (i >= process.arrivalt && i < process.finisht)
            {
                if (i < process.starttime)
                    cout << "|.";
                else
                    cout << "|*";
            }
        }

        cout << "| \n";
    }

    cout << "------------------------------------------------\n\n";
}

// fix
// prints each process in a row rather than column
// void statsPrint(const vector<Process> &processes)
// {
//     cout << "Process\tArrival\tService\tFinish\tTurnaround\tWaiting\n";
//     for (auto &process : processes)
//     {
//         cout << process.name << "\t" << process.arrivalt << "\t"
//              << process.servicet << "\t" << process.finisht << "\t"
//              << process.turnaroundt << "\t\t" << process.waitingt << "\n";
//     }
// }

void statsPrint(const vector<Process>& processes) {
    int count = 0;
    
    // Print the first row (Process names)
    cout << "Process    |";
    for (const auto& process : processes) {
        cout << "  " << process.name << "  |";
        count += 1;
    }
    cout << "\n";

    // Print the Arrival row
    cout << "Arrival    |";
    for (const auto& process : processes) {
        if (process.arrivalt < 10)
            cout << "  " << process.arrivalt << "  |";
        else
            cout << " " << process.arrivalt << "  |";
    }
    cout << "\n";

    // Print the Service row
    cout << "Service    |";
    for (const auto& process : processes) {
        if (process.servicet < 10)
            cout << "  " << process.servicet << "  |";
        else
            cout << " " << process.servicet << "  |";
    }
    cout << " Mean|\n";

    // Print the Finish row
    cout << "Finish     |";
    for (const auto& process : processes) {
        if (process.finisht < 10)
            cout << "  " << process.finisht << "  |";
        else
            cout << " " << process.finisht << "  |";
    }
    cout << "-----|\n";

    // Print the Turnaround row
    cout << "Turnaround |";
    double totalTurnaround = 0;
    for (const auto& process : processes) {
        if (process.turnaroundt < 10)
            cout << "  " << process.turnaroundt << "  |";
        else
            cout << " " << process.turnaroundt << "  |";

        totalTurnaround += process.turnaroundt;
    }

    double meanTurnaround = totalTurnaround / count;

    // cout << " " << meanTurnaround <<"\n";
    if (meanTurnaround < 10)
        printf(" %.2f|\n", meanTurnaround);
    else
        printf("%.2f|\n", meanTurnaround);

    double total = 0;
    double normTurnaround = 0;
    // Print the Normalized Turnaround row
    cout << "NormTurn   |";
    for (const auto& process : processes) {
        normTurnaround = static_cast<double>(process.turnaroundt) / process.servicet;
        cout << " " << fixed << setprecision(2) << normTurnaround << "|";

        total += normTurnaround;
    }
    if ((total / count) < 10)
        cout << " " << fixed << setprecision(2) << (total / count) << "|\n\n";
    else
        cout << fixed << setprecision(2) << (total / count) << "|\n\n";
}

void dynamicTracePrint(const vector<Process> &processes, const vector<vector<char>> &timeline, int endtime)
{
    int n = processes.size();
    // Print time headers
    for (int t = 0; t <= endtime; t++)
        cout << t % 10 << " ";

    cout << "\n------------------------------------------------\n";

    // Print the process timelines
    for (int i = 0; i < n; i++)
    {
        cout << processes[i].name << "     ";
        for (int t = 0; t < endtime; t++)
        {
            cout << "|" << timeline[i][t];
        }
        cout << "| \n";
    }

    cout << "------------------------------------------------\n\n";
}

// fix
// timeline?
void fcfs(vector<Process> &processes, int endtime, string printmode)
{
    sort(processes.begin(), processes.end(), [](Process &f, Process &k)
         { return f.arrivalt < k.arrivalt; });

    int currenttime = 0;

    for (auto &currentprocess : processes)
    {
        if (currenttime < currentprocess.arrivalt)
            currenttime = currentprocess.arrivalt;

        currentprocess.finisht = currenttime + currentprocess.servicet;
        currentprocess.turnaroundt = currentprocess.finisht - currentprocess.arrivalt;
        currentprocess.waitingt = currentprocess.turnaroundt - currentprocess.servicet;
        currentprocess.starttime = currentprocess.waitingt + currentprocess.arrivalt;

        currenttime = currenttime + currentprocess.servicet;
    }

    if (printmode == "stats")
    {
        cout << "FCFS\n";
        statsPrint(processes);
    }
    else if (printmode == "trace")
    {
        cout << "FCFS  ";
        tracePrint(processes, endtime);
    }
}

/*void SPN (vector<Process> &processes, int endtime, string printmode){
    int currenttime =0;
    while (true){
        vector<Process*> obtainable;
        for (auto &p : processes){
            if (p.arrivalt <= currenttime && !p.finished)
            obtainable.push_back(&p);
        }

        if (obtainable.empty())
            currenttime = currenttime +1;
        else{
            Process* shortest = obtainable[0];
            for (auto &p : obtainable){
                if (p->servicet < shortest->servicet )
                shortest = p;
            }

            currenttime = currenttime + shortest->servicet;
            shortest->finisht = currenttime;
            shortest->finished = true;


            shortest->turnaroundt = shortest->finisht - shortest->arrivalt;
            shortest->waitingt = shortest->turnaroundt - shortest->servicet;
            shortest->starttime = shortest->waitingt + shortest->arrivalt;



        }


        bool done = true;
        for (const auto &p : processes){
            if (!p.finished){
                done = false;
                break;
            }

        }


        if (done)
        break;
    }


    if (printmode == "stats"){
        cout << "SPN  \n";
        statsPrint(processes);
    }
    else if (printmode == "trace"){
         cout << "SPN  ";
         tracePrint(processes, endtime);
    }

}
*/

// fix
// timeline?
void SPN(vector<Process> &processes, int endtime, string printmode)
{
    int currenttime = 0;
    int completed = 0;
    int n = processes.size();

    while (completed < n)
    {
        int shortestindex = -1;
        int shortesttime = __INT_MAX__;

        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrivalt <= currenttime && !processes[i].finished && processes[i].servicet < shortesttime)
            {
                shortesttime = processes[i].servicet;
                shortestindex = i;
            }
        }

        if (shortestindex == -1)
            currenttime++;
        else
        {
            currenttime = currenttime + processes[shortestindex].servicet;
            processes[shortestindex].finished = true;
            processes[shortestindex].finisht = currenttime;
            processes[shortestindex].turnaroundt = processes[shortestindex].finisht - processes[shortestindex].arrivalt;
            processes[shortestindex].waitingt = processes[shortestindex].turnaroundt - processes[shortestindex].servicet;
            processes[shortestindex].starttime = processes[shortestindex].waitingt + processes[shortestindex].arrivalt;

            completed++;
        }
    }
    if (printmode == "stats")
    {
        cout << "SPN\n";
        statsPrint(processes);
    }
    else if (printmode == "trace")
    {
        cout << "SPN   ";
        tracePrint(processes, endtime);
    }
}

void SRT(vector<Process> &processes, int endtime, string printmode)
{
    int currenttime = 0;
    int completed = 0;
    int n = processes.size();

    // Initialize timeline for each process
    vector<vector<char>> timeline(n, vector<char>(endtime + 1, ' '));

    while (completed < n)
    {
        int shortestindex = -1;
        int shortesttime = __INT_MAX__;

        // Find the process with the shortest remaining time that has arrived
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrivalt <= currenttime && !processes[i].finished && processes[i].remainingt < shortesttime)
            {
                shortesttime = processes[i].remainingt;
                shortestindex = i;
            }
        }

        if (shortestindex == -1)
        {
            // No process available; increment time
            currenttime++;
        }
        else
        {
            // Mark running process
            timeline[shortestindex][currenttime] = '*';

            // Mark waiting processes
            for (int i = 0; i < n; i++)
            {
                if (i != shortestindex && processes[i].arrivalt <= currenttime && !processes[i].finished)
                {
                    timeline[i][currenttime] = '.';
                }
            }

            // Execute process for 1 unit of time
            processes[shortestindex].remainingt--;

            if (processes[shortestindex].remainingt == 0)
            {
                // Process finishes execution
                processes[shortestindex].finished = true;
                processes[shortestindex].finisht = currenttime + 1;
                processes[shortestindex].turnaroundt = processes[shortestindex].finisht - processes[shortestindex].arrivalt;
                processes[shortestindex].waitingt = processes[shortestindex].turnaroundt - processes[shortestindex].servicet;
                completed++;
            }

            // Increment current time
            currenttime++;
        }
    }

    // Print results
    if (printmode == "stats")
    {
        cout << "SRT\n";
        statsPrint(processes);
    }
    else if (printmode == "trace")
    {
        cout << "SRT   ";
        dynamicTracePrint(processes, timeline, endtime);
    }
}

void roundrobin(vector<Process> &processes, int endtime, int quantum, string printmode)
{
    int currenttime = 0;
    int completed = 0;
    int n = processes.size();

    // Initialize timeline for each process
    vector<vector<char>> timeline(n, vector<char>(endtime + 1, ' '));

    queue<int> readyqueue;

    for (int i = 0; i < n; i++)
        if (processes[i].arrivalt <= currenttime)
            readyqueue.push(i);

    while (!readyqueue.empty()) // what if current process finished, but no process in queue yet? while currenttime < endtime?
    {
        int index = readyqueue.front();
        readyqueue.pop(); // ??

        int executiontime = min(quantum, processes[index].remainingt);

        for (int t = 0; t < executiontime; t++)
        {
            processes[index].remainingt--;
            timeline[index][currenttime] = '*';

            for (int i = 0; i < n; i++)
            {
                if (i != index && processes[i].arrivalt <= currenttime && processes[i].remainingt > 0)
                    timeline[i][currenttime] = '.';
            }

            currenttime++;
            for (int i = 0; i < n; i++)
            {
                if (processes[i].arrivalt == currenttime && processes[i].remainingt > 0 && !processes[i].finished)
                {
                    readyqueue.push(i); // Add new arrival to the ready queue
                }
            }
        }

        if (processes[index].remainingt == 0)
        {
            processes[index].finished = true;
            completed++;
            processes[index].finisht = currenttime;
            processes[index].turnaroundt = processes[index].finisht - processes[index].arrivalt;
            processes[index].waitingt = processes[index].turnaroundt - processes[index].servicet;
        }
        else
            readyqueue.push(index);
    }
    if (printmode == "stats")
    {
        cout << "RR-" << quantum<<"\n";
        statsPrint(processes);
    }
    else if (printmode == "trace")
    {
        cout << "RR-" << quantum<< "  ";
        dynamicTracePrint(processes, timeline, endtime);
    }
}

void HRRN(vector<Process> &processes, int endtime, string printmode)
{
    int currenttime = 0;
    int completed = 0;
    int n = processes.size();

    // Initialize timeline for each process
    vector<vector<char>> timeline(n, vector<char>(endtime + 1, ' '));

    while (completed < n)
    {
        int highestindex = -1;
        double highestratio = -1.0; // why not 0?

        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrivalt <= currenttime && !processes[i].finished)
            {
                double waitingtime = currenttime - processes[i].arrivalt;
                double ratio = (waitingtime + processes[i].servicet) / processes[i].servicet;

                if (ratio > highestratio)
                {
                    highestratio = ratio;
                    highestindex = i;
                }
            }
        }

        if (highestindex == -1)
        {
            currenttime++;
            continue;
        }

        Process &picked = processes[highestindex];

        for (int t = 0; t < picked.servicet; t++)
        {
            timeline[highestindex][currenttime] = '*';

            for (int i = 0; i < n; i++)
            {
                if (i != highestindex && processes[i].arrivalt <= currenttime && !processes[i].finished)
                    timeline[i][currenttime] = '.';
            }

            currenttime++;
        }

        picked.finished = true;
        picked.finisht = currenttime;
        picked.turnaroundt = picked.finisht - picked.arrivalt;
        picked.waitingt = picked.turnaroundt - picked.servicet;
        completed++;
    }
    if (printmode == "stats")
    {
        cout << "HRRN\n";
        statsPrint(processes);
    }
    else if (printmode == "trace")
    {
        cout << "HRRN  ";
        dynamicTracePrint(processes, timeline, endtime);
    }
}

void FeedbackFixed(vector<Process> &processes, int endtime, string printmode) {
    int n = processes.size();
    int currenttime = 0, completed = 0;
    const int numQueues = 5;

    // Initialize queues for feedback scheduling
    vector<queue<int>> queues(numQueues);

    // Timeline for each process
    vector<vector<char>> timeline(n, vector<char>(endtime + 1, ' '));

    // Enqueue all processes in the first queue
    for (int i = 0; i < n; i++) {
        if (processes[i].arrivalt <= currenttime) {
            queues[0].push(i);
        }
    }

    while (completed < n) {
        bool processFound = false;

        // Iterate over queues
        for (int q = 0; q < numQueues; q++) {
            if (!queues[q].empty()) {
                processFound = true;
                int index = queues[q].front();
                queues[q].pop();

                // Execute process for 1 unit of time (fixed quantum)
                processes[index].remainingt--;
                timeline[index][currenttime] = '*';

                for (int i = 0; i < n; i++) {
                    if (i != index && processes[i].arrivalt <= currenttime && !processes[i].finished) {
                        timeline[i][currenttime] = '.';
                    }
                }

                currenttime++;

                // Check for new arrivals
                for (int i = 0; i < n; i++) {
                    if (processes[i].arrivalt == currenttime && !processes[i].finished) {
                        queues[0].push(i);
                    }
                }

                bool emptyqueues = true;
                for (int i = q; i < numQueues; i++){
                    if (!queues[i].empty()){
                        emptyqueues = false;
                        break;
                    }
                }

                if (processes[index].remainingt == 0) {
                    // Process finished
                    processes[index].finished = true;
                    completed++;
                    processes[index].finisht = currenttime;
                    processes[index].turnaroundt = processes[index].finisht - processes[index].arrivalt;
                    processes[index].waitingt = processes[index].turnaroundt - processes[index].servicet;
                } else {
                    // Move to next queue or stay in the last queue
                    if (emptyqueues){
                        queues[q].push(index);
                    }
                    else if (q + 1 < numQueues) {
                        queues[q + 1].push(index);
                    } else {
                        queues[q].push(index);
                    }
                }
                break;
            }
        }

        if (!processFound) {
            currenttime++;
        }
    }

    if (printmode == "stats") {
        cout << "FB-1\n";
        statsPrint(processes);
    } else if (printmode == "trace") {
        cout << "FB-1  ";
        dynamicTracePrint(processes, timeline, endtime);
    }
}

void FeedbackVariable(vector<Process> &processes, int endtime, string printmode) {
    int n = processes.size();
    int currenttime = 0, completed = 0;
    const int numQueues = 5;

    // Initialize queues for feedback scheduling
    vector<queue<int>> queues(numQueues);

    // Timeline for each process
    vector<vector<char>> timeline(n, vector<char>(endtime + 1, ' '));

    // Enqueue all processes in the first queue
    for (int i = 0; i < n; i++) {
        if (processes[i].arrivalt <= currenttime) {
            queues[0].push(i);
        }
    }

    while (completed < n) {
        bool processFound = false;

        // Iterate over queues
        for (int q = 0; q < numQueues; q++) {
            if (!queues[q].empty()) {
                processFound = true;
                int index = queues[q].front();
                queues[q].pop();

                // Calculate quantum time (2^q)
                int quantum = pow(2, q);
                int executionTime = min(quantum, processes[index].remainingt);

                for (int t = 0; t < executionTime; t++) {
                    processes[index].remainingt--;
                    timeline[index][currenttime] = '*';

                    for (int i = 0; i < n; i++) {
                        if (i != index && processes[i].arrivalt <= currenttime && !processes[i].finished) {
                            timeline[i][currenttime] = '.';
                        }
                    }

                    currenttime++;

                    // Check for new arrivals
                    for (int i = 0; i < n; i++) {
                        if (processes[i].arrivalt == currenttime && !processes[i].finished) {
                            queues[0].push(i);
                        }
                    }
                }

                bool emptyqueues = true;
                for (int i = q; i < numQueues; i++){
                    if (!queues[i].empty()){
                        emptyqueues = false;
                        break;
                    }
                }

                if (processes[index].remainingt == 0) {
                    // Process finished
                    processes[index].finished = true;
                    completed++;
                    processes[index].finisht = currenttime;
                    processes[index].turnaroundt = processes[index].finisht - processes[index].arrivalt;
                    processes[index].waitingt = processes[index].turnaroundt - processes[index].servicet;
                } else {
                    // Move to next queue or stay in the last queue
                    if (emptyqueues){
                        queues[q].push(index);
                    }
                    else if (q + 1 < numQueues) {
                        queues[q + 1].push(index);
                    } else {
                        queues[q].push(index);
                    }
                }
                break;
            }
        }

        if (!processFound) {
            currenttime++;
        }
    }

    if (printmode == "stats") {
        cout << "FB-2i\n";
        statsPrint(processes);
    } else if (printmode == "trace") {
        cout << "FB-2i ";
        dynamicTracePrint(processes, timeline, endtime);
    }
}

// fix
// implement the roundrobin for similiar priorities, when to timeout?

// tyb bos, e7na dlwa2ty bygelna process, bn7otaha f priority queue 3la asas el priority bta3etha gameeel.
// b3dein 7an dequeue ml priority queue da, elmafrood ykoon bysta3mel stable sorting 3shan ykon fifo lw 
// kaza process leehom nafs el priority. ba run el process el 3amaltelha pop l min of quantum w remaining time
// w ba increment kol priority fl queue. lama akhalas el run tb3n ba check lw el process khelset wla la w 
// baraga3laha el priority el asleya w a enqueue.

void aging(vector<Process> &processes, int endtime, int quantum,string printmode)
{
    int currenttime = 0;
   // int completed = 0;
    int n = processes.size();

    // Initialize timeline for each process
    vector<vector<char>> timeline(n, vector<char>(endtime + 1, ' '));

    PriorityQueue pq;
    int index = -1;
    int count = 0;

    for (int i = 0; i < n; i++)
    {
        if (processes[i].arrivalt <= currenttime && !processes[i].started && !processes[i].finished)
        {
            pq.enqueue(processes[i]);
            processes[i].started = true;
            count += 1;
        }
    }

    while (currenttime <= endtime)
    {
        
        // for (int i = 0; i < n; i++)
        // {
        //     if (processes[i].started)
        //         std::cout << "Process " << processes[i].name << ", Priority: " << processes[i].curpriority << std::endl;
        // }

        if (pq.isEmpty()){
            currenttime++;

            for (int i = 0; i < n; i++)
            {
                if (processes[i].arrivalt <= currenttime && !processes[i].started && !processes[i].finished)
                {
                    pq.enqueue(processes[i]);
                    processes[i].started = true;
                    count += 1;
                }
            }
        }
        else{
            //mylist = pq->processes;

            Process current = pq.dequeue();
            // std::cout << "Popped Process: " << current.name << std::endl;

            // was here

            index = -1;

            for (int i = 0; i < n; i++)
            {
                if (processes[i].name == current.name)
                {
                    index = i;
                }
            }

            // from here
            for (int t = 0; t < quantum; t++){

            timeline[index][currenttime] = '*';

            // Mark waiting processes
            for (int i = 0; i < n; i++)
            {
                if (i != index && processes[i].arrivalt <= currenttime && !processes[i].finished)
                {
                    timeline[i][currenttime] = '.';
                }
            }

            // Increment current time
            currenttime++;

            for (int i = 0; i < n; i++)
            {
                if (processes[i].arrivalt <= currenttime && !processes[i].started && !processes[i].finished)
                {
                    pq.enqueue(processes[i]);
                    processes[i].started = true;
                    count += 1;
                }
            }
            
            pq.incrementQueue();
            }
            // till here

            if (processes[index].remainingt != 0)
            {
                processes[index].curpriority = processes[index].initpriority;
                pq.enqueue(processes[index]);
            }   
        }
    }

    // Print results
    if (printmode == "stats")
    {
        cout << "Aging\n";
        statsPrint(processes);
    }
    else if (printmode == "trace")
    {
        cout << "Aging ";
        dynamicTracePrint(processes, timeline, endtime);
    }
}

int main()
{
    vector<Process> processes;
    string mode;
    vector<pair<int, int>> policies;
    int simulationEndTime;
   // int numPolicies =  policies.size();
    //cout << "NUm of policies" <<numPolicies <<"\n";
    parseinput(processes, mode, policies, simulationEndTime);
    
    for (auto &policy : policies)
    //for (int i ; i < numPolicies ; ++i)
    {
        //cout << "Starting simulation for policy" << policy.first << endl;
        vector<Process>processescopy = processes;
       // auto &policy = policies[i];
        if (policy.first == 1)
            fcfs(processescopy, simulationEndTime, mode);
        else if (policy.first == 2)
            roundrobin(processescopy, simulationEndTime, policy.second, mode);
        else if (policy.first == 3)
            SPN(processescopy, simulationEndTime, mode);
        else if (policy.first == 4)
            SRT(processescopy, simulationEndTime, mode);
        else if (policy.first == 5)
            HRRN(processescopy, simulationEndTime, mode);
        else if (policy.first == 6)
            FeedbackFixed(processescopy, simulationEndTime, mode);
        else if (policy.first == 7)
            FeedbackVariable(processescopy, simulationEndTime, mode);
        else if (policy.first == 8)
            aging(processescopy, simulationEndTime, policy.second, mode);
        else{
            cout << "Error in main";
            continue;}
    }
}