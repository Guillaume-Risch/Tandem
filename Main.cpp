#include <iostream>
#include <sstream>
#include <vector>

#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition.hpp>
#include <boost/sync/semaphore.hpp>


using namespace std;
using namespace boost;

namespace{

    enum { KExcArg  = 253 };
    int riding = 0, waiting = 0;
    bool must_wait = false; // Indique qu'il n'y a pas de place libre sur le vélo 
    mutex mtx;
    condition_variable condition;
    semaphore block = semaphore(0);
    



    void search(){
        unique_lock <mutex> oneLock (mtx);
        condition.wait(oneLock);
            if (must_wait){
                ++ waiting;
                condition.notify_one();
                block.wait(oneLock);
            }
                    else{

                        ++ riding;
                        must_wait = (riding == 5);
                        condition.notify_one();

                    }      
    }


    void Ride(){

        unique_lock <mutex> oneLock (mtx);
        condition.wait(oneLock);
        riding -- ;
        if(riding == 0){
            int n = min(5, waiting);
            waiting = waiting - n;
            riding = riding - n; 
            must_wait = (riding == 5);
            block.notify_one(n);


        }

        condition.notify_one();
    }  
}//namesapce


int main (){

    thread_group groupe;
    groupe.create_thread(riding);
    groupe.create_thread(waiting);
    groupe.create_thread(block);
    groupe.join_all();

    return 0;
}

