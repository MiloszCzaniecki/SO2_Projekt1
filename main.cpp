#include <iostream>
#include <random>
#include <thread>
#include <mutex>
using namespace std;

int num_phil = 5;
std::mt19937 rng;
std::mutex mtx;
std::mutex waiter_mtx;
bool forks[5] = {true, true, true, true, true};

int left_phil(int id_phil){
    return (id_phil-1+num_phil)%num_phil;
}

int right_phil(int id_phil){
    return (id_phil+1)%num_phil;
}

void thinking(int& thinking_time, int id_phil){ // action_id = 0
    std::lock_guard<std::mutex> lock(mtx);
    cout << "Philosopher number " << id_phil << " is thinking for " << chrono::milliseconds(thinking_time).count() << endl;
}

void waiting(int& waiting_time, int id_phil){ // action_id = 1
    std::lock_guard<std::mutex> lock(mtx);
    cout << "Philosopher number " << id_phil << " is waiting for " << chrono::milliseconds(waiting_time).count() << endl;
}

void taking_forks(int& taking_time, int id_phil){ // action_id = 2
    std::lock_guard<std::mutex> lock(mtx);
    cout << "Philosopher number " << id_phil << " is picking up forks for " << chrono::milliseconds(taking_time).count() << endl;
}

void eating(int& eating_time, int id_phil){ // action_id = 3
    std::lock_guard<std::mutex> lock(mtx);
    cout << "Philosopher number " << id_phil << " is eating for " << chrono::milliseconds(eating_time).count() << endl;
}

void returning_forks(int& returning_time, int id_phil){ // action_id = 4
    std::lock_guard<std::mutex> lock(mtx);
    cout << "Philosopher number " << id_phil << " is putting forks away for " << chrono::milliseconds(returning_time).count() << endl;
}

void philosopher(int times_array[], int id_phil, mutex& mut, mutex& tex, const chrono::steady_clock::time_point& end_time){

    int thinking_time = times_array[0];
    int waiting_time = times_array[1];
    int taking_time = times_array[2];
    int eating_time = times_array[3];
    int returning_time = times_array[4];

    while(chrono::steady_clock::now() < end_time){
        // noone will forbid philosopher to think freely
        thinking_time = std::uniform_int_distribution<int>(400, 900)(rng);
        {
            //std::lock_guard<std::mutex> g_thinking(mtx);
            thinking(thinking_time, id_phil);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(thinking_time));

        // after thinking philosophers wait before eating
        waiting_time = std::uniform_int_distribution<int>(1000, 2000)(rng);
        //std::lock_guard<std::mutex> g_waiting(mtx);
        {
            waiting(waiting_time, id_phil);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time));

        int left = left_phil(id_phil);
        int right = right_phil(id_phil);

        {
            std::unique_lock<std::mutex> lock(waiter_mtx);
            while(!forks[left] || !forks[right]){
                lock.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                lock.lock();
            }
            forks[left] = false; // left fork unavailable
            forks[right] = false; // right fork unavailable
        }

        taking_time = std::uniform_int_distribution<int>(1000, 2000)(rng);
        //std::lock_guard<std::mutex> g_taking(mtx);
        {
            taking_forks(taking_time, id_phil);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(taking_time));

        eating_time = std::uniform_int_distribution<int>(1000, 2000)(rng);
        //std::lock_guard<std::mutex> g_eating(mtx);  uuuuseeleeeess
        {
            eating(eating_time, id_phil);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(eating_time));

        returning_time = std::uniform_int_distribution<int>(1000, 2000)(rng);
        //std::lock_guard<std::mutex> g_returning(mtx);
        {
            returning_forks(returning_time, id_phil);
            std::lock_guard<std::mutex> lock(waiter_mtx);
            forks[left] = true;
            forks[right] = true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(returning_time));


    }
    cout << "Philosopher number " << id_phil << " got bored and left the party" << endl;

}

int main() {
    auto end_time = chrono::steady_clock::now() + chrono::seconds(20); //program should last 20 seconds
    int table_state[num_phil];

    int thinking_time;  // time_id = 0
    int taking_time;    // time_id = 1
    int returning_time; // time_id = 2
    int eating_time;    // time_id = 3
    int waiting_time;   // time_id = 4

    int times_array[num_phil][5];
    // every philosopher [num_phil] has a list of times it takes to makes an action
    // on this basis (e.g. which philosopher is waiting the longest to eat) priority queue will be created
    //check_directions();
    ;
    mutex mut1, mut2, mut3, mut4, mut5;
    // c++ threads require user to add parameters after passing pointer to desired function
    thread t1([&] {philosopher(&times_array[0][5], 0, mut1, mut2, end_time);});
    thread t2([&] {philosopher(&times_array[1][5], 1, mut2, mut3, end_time);});
    thread t3([&] {philosopher(&times_array[2][5], 2, mut3, mut4, end_time);});
    thread t4([&] {philosopher(&times_array[3][5], 3, mut4, mut5, end_time);});
    thread t5([&] {philosopher(&times_array[4][5], 4, mut5, mut1, end_time);});

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    return 0;
}