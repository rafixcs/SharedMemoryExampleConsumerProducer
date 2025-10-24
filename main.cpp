#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>
#include <thread>
#include <chrono>

namespace bip = boost::interprocess;

struct SharedBuffer {
    bip::interprocess_mutex mutex;
    bip::interprocess_condition data_ready;
    bool has_data;
    char message[256];

    SharedBuffer() : has_data(false) {}
};

void producer() {
    bip::shared_memory_object::remove("SyncShm");

    bip::shared_memory_object shm(bip::create_only, "SyncShm", bip::read_write);
    shm.truncate(sizeof(SharedBuffer));
    bip::mapped_region region(shm, bip::read_write);

    SharedBuffer* buff = new (region.get_address()) SharedBuffer;

    std::cout << "Producer: preparing data..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    {
        bip::scoped_lock<bip::interprocess_mutex> lock(buff->mutex);
        std::strcpy(buff->message, "Hello from producer!");
        buff->has_data = true;
        std::cout << "Producer: Data ready, notifying consumer" << std::endl;
        buff->data_ready.notify_one();
    }

    std::cout << "Producer: finishing process.\n";
}

void consumer() {
    std::cout << "Consumer: waiting for shared memory..." << std::endl;

    bool connected = false;
    while (!connected) {
        try {
            bip::shared_memory_object shm(bip::open_only, "SyncShm", bip::read_write);
            connected = true;
            std::cout << "Consumer: connected to shared memory!" << std::endl;

            bip::mapped_region region(shm, bip::read_write);
            SharedBuffer* buff = (SharedBuffer*) region.get_address();

            {
                bip::scoped_lock<bip::interprocess_mutex> lock(buff->mutex);
                std::cout << "Consumer: waiting for data..." << std::endl;
                while (!buff->has_data) {
                    buff->data_ready.wait(lock);
                }

                std::cout << "Consumer: received data: " << buff->message << std::endl;
            }
        } catch (bip::interprocess_exception& e) {
            std::cout << "Consumer: exception caught: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    std::cout << "Consumer: finishing process.\n";
    bip::shared_memory_object::remove("SyncShm");
}

int main() {
    std::thread prod(producer);
    std::thread cons(consumer);

    prod.join();
    cons.join();

    return 0;
}