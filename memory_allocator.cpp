#include <cstddef>
#include<iostream>
#include<cstdint>

struct block{
    size_t size;
    bool free;
    block* next;
};

uint8_t heap[1024];

block* free_list = reinterpret_cast<block*>(heap);

void* allocate(size_t size_a){
    block* current{};
    current = free_list;
    bool found{false};
    size_t original_size;
    bool split{true};
    block* previous{};

    while (current!=nullptr) {
        if(current->free == 1 &&current->size>=size_a){
            found = true;
            break;
        }else {
            previous = current;
            current = current->next;

        }            
    }
    
    if(found==true){
        if(current->size-size_a<sizeof(block)){
            split = false;
        }
        
        original_size = current->size;
        current->free = false;
        if(split == true){
            current->size = size_a;
            uint8_t* address = reinterpret_cast<uint8_t*>(current);
            address = address+sizeof(block)+size_a;
            block* a = reinterpret_cast<block*>(address);
            a->size =original_size-size_a-sizeof(block);
            a->free = true;
            a->next = current->next;
           if (previous == nullptr) {
                free_list = a;
           }else {
                previous->next = a;
           }
        }else {
            current->size = original_size;
        }

    }
    else {
        return nullptr;
    }
    uint8_t* ncurrent = reinterpret_cast<uint8_t*>(current);
    ncurrent = ncurrent+sizeof(block);

    return ncurrent;
}

int main() {

    // =========================
    // TEST 1: SPLITTING
    // =========================

    free_list->size = sizeof(heap) - sizeof(block);
    free_list->free = true;
    free_list->next = nullptr;

    std::cout << "===== TEST 1: SPLIT =====\n";

    void* p1 = allocate(100);

    std::cout << "Allocated address: " << p1 << "\n";
    std::cout << "Free block size: " << free_list->size << "\n";
    std::cout << "Free block status: " << free_list->free << "\n";


    // =========================
    // RESET HEAP
    // =========================

    free_list = reinterpret_cast<block*>(heap);

    free_list->size = sizeof(heap) - sizeof(block);
    free_list->free = true;
    free_list->next = nullptr;


    // =========================
    // TEST 2: NO SPLITTING
    // =========================

    std::cout << "\n===== TEST 2: NO SPLIT =====\n";

    void* p2 = allocate(980);

    std::cout << "Allocated address: " << p2 << "\n";
    std::cout << "Block size: " << free_list->size << "\n";
    std::cout << "Block status: " << free_list->free << "\n";

    // =========================
// TEST 3: MIDDLE BLOCK
// =========================

free_list = reinterpret_cast<block*>(heap);

// Create A
free_list->size = 50;
free_list->free = true;

// Create B
block* B = reinterpret_cast<block*>(
    reinterpret_cast<uint8_t*>(free_list)
    + sizeof(block)
    + 50
);

B->size = 200;
B->free = true;

// Create C
block* C = reinterpret_cast<block*>(
    reinterpret_cast<uint8_t*>(B)
    + sizeof(block)
    + 200
);

C->size = 300;
C->free = true;

free_list->next = B;
B->next = C;
C->next = nullptr;

std::cout << "\n===== TEST 3: MIDDLE BLOCK =====\n";

void* p3 = allocate(100);

std::cout << "Allocated address: " << p3 << "\n";

std::cout << "A size: " << free_list->size << "\n";
std::cout << "A next: " << free_list->next << "\n";
std::cout << "B/remainder size: "
          << free_list->next->size << "\n";

std::cout << "B/remainder next: "
          << free_list->next->next << "\n";


    return 0;
}