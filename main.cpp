#include <iostream>
#include <cstdint>
#include <bitset>
#include <vector>
#include <string>
#include <iomanip>
using std::cout;
using std::endl;
using std::vector;
using std::string;

typedef struct State{
    uint64_t a;
    uint64_t b;
    uint64_t c;
    uint64_t d;
}State;

uint64_t getIthBit(uint64_t num, int i){
    return (num >> i) & 1ULL;
}

void shakeColumns(State* s){
    uint64_t a,b,c,d;
    a = s->b^s->c^s->d;
    b = s->a^s->c^s->d;
    c = s->a^s->b^s->d;
    d = s->a^s->b^s->c;
    s->a = a; 
    s->b = b; 
    s->c = c; 
    s->d = d;
}

void preShiftRows(State* s){
    uint64_t a,b,c,d;
    a = s->a;
    b = (s->b >> 1) | (s->b << 63);
    c = (s->c >> 3) | (s->c << 61);
    d = (s->d >> 9) | (s->d << 55);
    s->a = a; 
    s->b = b; 
    s->c = c; 
    s->d = d;
}

void stirColumns(State* s){
    s->a ^= (s->b&s->c);
    s->b ^= (s->c&s->d);
    s->c ^= (s->d&s->a);
    s->d ^= (s->a&s->b);
}

void postShiftRows(State* s){
    uint64_t a,b,c,d;
    a = s->a;
    b = (s->b >> 1) | (s->b << 63);
    c = (s->c >> 5) | (s->c << 59);
    d = (s->d >> 25) | (s->d << 39);
    s->a = a; 
    s->b = b; 
    s->c = c; 
    s->d = d; 
}

void addRoundConstant(State* s, uint8_t r){
    uint64_t mask = (1ULL << (r + 1))-1;
    s->a ^= mask;
}

void ABCDPermutation(State* s) {
    for (int i=0; i<13; i++) {
        shakeColumns(s);
        preShiftRows(s);
        stirColumns(s);
        postShiftRows(s);
        addRoundConstant(s, i);
    }
}

void printState(State* s){
    cout << "State: " << std::hex << s->a << " " << s->b << " " << s->c << " " << s->d << endl;
}


vector<uint64_t> extractMessage(const string &input) {
    std::vector<uint64_t> message = {};
    uint8_t nbOfBytes = 0;
    while (message.size() < (uint64_t) ceil((double) input.size() / 8)) {
        uint64_t block = 0;
        nbOfBytes = 0;
        for (int i = 0; i < 8; i++) {
            if ((message.size() * 8) + i >= input.size()) {
                break;
            }
            uint64_t temp = input[(message.size() * 8) + i];
            temp <<= i * 8;
            block |= temp;
            nbOfBytes++;
        }
        message.push_back(block);
    }

    if (nbOfBytes < 8) {
        uint64_t padding = (1ULL << ((8 - nbOfBytes - 1) * 8 + 7)) + 1;
        padding <<= (nbOfBytes * 8);
        if (message.empty()) {
            message.push_back(padding);
        } else {
            message.back() |= padding;
        }
    }

    return message;
}

string ABCDSponge(const string& input) {
    vector<uint64_t> message = extractMessage(input);

    State s = {0, 0, 0, 0};
    // Absorbing phase
    for (uint64_t i : message) {
        cout << "Absorbing: " << std::hex << i << endl;
        printState(&s);
        s.a ^= i;
        ABCDPermutation(&s);
    }

    uint64_t output[3];
    //Squeezing phase
    for (uint64_t &i : output) {
        cout << "Squeezing: " << std::hex << s.a << endl;
        printState(&s);
        i = s.a;
        ABCDPermutation(&s);
    }

    string outputBytes;
    for (uint64_t & i : output) {
        for (int j = 0; j < 8; j++) {
            outputBytes.push_back((char) (i & 0xFF));
            i >>= 8;
        }
    }

    return outputBytes;

}

void printStringInHex(string &output) {
    for (unsigned char c : output) {
        std::cout << std::hex << std::setfill('0') << std::setw(2)
                  << static_cast<int>(c) << " ";
    }
    std::cout << std::endl;
}

int main(){
    uint64_t a,b,c,d;
    //a = 0x0123456789ABCDEF, b = 0x2222222222222222, c = 0x1231231231231231, d = 0xABCDABCDABCDABCD;
    a = 0x0000000000000000, b = 0x0000000000000000, c = 0x0000000000000000, d = 0x0000000000000000;
    State s = {a, b, c, d};
    ABCDPermutation(&s);

    string output = ABCDSponge("0123456789");
    printStringInHex(output);

    return 0;

}